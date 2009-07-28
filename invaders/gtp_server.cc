#include "gtp_server.h"

#include <cassert>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

GtpServer::GtpServer(const char *first, const char *second) : players_(2)
{
  players_cmd_[0] = first;
  players_cmd_[1] = second;

  signal(SIGPIPE, SIG_IGN); // ignorowanie sygnalu broken pipe

  int ppr[players_][2], ppw[players_][2], ppe[players_][2]; // 0-read, 1-write

  for (uint i = 0; i < players_; ++i) {
    if (pipe(ppr[i]) || pipe(ppw[i]) || pipe(ppe[i])) {
      std::cerr << "Fatal: error in pipe: " << errno << ", " << strerror(errno) << std::endl;
      exit(1);
    }
  }

  for (uint i = 0; i < players_; ++i) {
    switch ((pids_[i] = fork())) {
      case -1:
        std::cerr << "Fatal: error in fork: " << errno << ", " << strerror(errno) << std::endl;
        exit(1);
      
      case 0:
        // Nowemu procesowi podmieniamy we/wy 
        // 0 = read, 1 = write, 2 = write
        close(0); close(1); close(2);
        dup(ppr[i][0]); dup(ppw[i][1]); dup(ppe[i][1]); 

        close(ppr[i][0]); close(ppw[i][1]); close(ppe[i][1]);
        close(ppr[i][1]); close(ppw[i][0]); close(ppe[i][0]);
        execlp(players_cmd_[i].c_str(), players_cmd_[i].c_str(), NULL);

        std::cerr << "Fatal: error in execlp: " << errno << ", " << strerror(errno) << std::endl;
        exit(1);
     
      default:
        // Macierzystemu podpinamy we/wy potomka
        // 0 = write, 1 = read, 2 = read
        close(ppr[i][0]); close(ppw[i][1]); close(ppe[i][1]);

        if (!(in_[i]  = fdopen(ppr[i][1], "w")) ||
            !(out_[i] = fdopen(ppw[i][0], "r")) ||
            !(err_[i] = fdopen(ppe[i][0], "r"))) {
          std::cerr << "Fatal: error in fdopen: " << errno << ", " << strerror(errno) << std::endl;
          exit(1);
        }
    }
  }



}

GtpServer::~GtpServer(void) 
{
  for (uint i = 0; i < players_; ++i) {
    if (kill(pids_[i], SIGINT))
      std::cerr << "Error: cannot kill(SIGINT) process: " << players_cmd_[i] 
        << " (pid: " << pids_[i] << "): " << errno << ", " << strerror(errno) << std::endl;
  }

  sleep(1);

  for (uint i = 0; i < players_; ++i) {
    int ret = waitpid(pids_[i], 0, WNOHANG);
    if (ret == pids_[i])
      std::cerr << "Process " << players_cmd_[i] << " (pid: " << pids_[i] << ") killed" << std::endl;
    else {
      switch (ret) {
        case -1:
          std::cerr << "Error: cannot wait for process: " << players_cmd_[i] 
            << " (pid: " << pids_[i] << "): " << errno << ", " << strerror(errno) << std::endl;
          break;
        case 0:
          std::cerr << "Process " << players_cmd_[i] << " (pid: " 
            << pids_[i] << ") still running" << std::endl;
          break;
        default:
          std::cerr << "Error: unknown pid returned for process: " 
            << players_cmd_[i] << " (pid: " << pids_[i] << "): " << ret << std::endl;
      }
      
      if (kill(pids_[i], SIGKILL))
        std::cerr << "Error: cannot kill(SIGKILL) process: " << players_cmd_[i]
          << " (pid: " << pids_[i] << "): " << errno << ", " << strerror(errno) << std::endl;
    }
  }

  for (uint i = 0; i < players_; ++i) {
    fclose(in_[i]); fclose(out_[i]); fclose(err_[i]);
  }
}

GtpServer::State GtpServer::read_from(uint nr, uint timeout)
{
  assert(nr < players_);
  
  reply_[nr].clear();
  uint max = 10240;
  char buffer[max], param[max];
  uint lines = 0;

  int fd = fileno(out_[nr]);
  if (fd < 0) {
    std::cerr << "Error: error in fileno() for " << players_cmd_[nr] 
      << " (pid: " << pids_[nr] << "): " << errno << ", " << strerror(errno) << std::endl;
    return Error;
  }

  while (true) {
  
    // sprawdzamy czy jest co czytac...
    if (timeout > 0) {
      fd_set rdfs[1];
      FD_ZERO(rdfs);
      FD_SET(fd, rdfs);

      struct timeval tv = { timeout / 1000, (timeout % 1000) * 1000 };
      int ret = select(fd + 1, rdfs, NULL, NULL, &tv);
      
      switch (ret) {
        case 0:
          return Time;
        case 1:
          break;
        default:
          std::cerr << "Error: error in select() for " << players_cmd_[nr] 
            << " (pid: " << pids_[nr] << "): " << errno << ", " << strerror(errno) << std::endl;
          return Error;
      }
    }
  
    // ... i czytamy
    if (!fgets(buffer, max, out_[nr])) {
      if (errno) {
        std::cerr << "Error: cannot read from " << players_cmd_[nr] 
          << " (pid: " << pids_[nr] << "): " << errno << ", " << strerror(errno) << std::endl;
        return Time;
      }
      return Eof;
    }

    // analizujemy dane...

    uint pos = 0, len = strlen(buffer);

    if (len > 0 && buffer[len - 1] != '\n') {
      std::cerr << "Error: reply from " << players_cmd_[nr] 
        << " (pid: " << pids_[nr] << ") is too long" << std::endl;
      return Error;
    }

    ++lines;

    if (len == 0) {
      std::cerr << "Fatal: read zero bytes from " << players_cmd_[nr] 
        << " (pid: " << pids_[nr] << ")? how?" << std::endl;
      abort();
    }

    if (len == 1) {
      if (lines == 1) {
        std::cerr << "Error: empty reply from: " << players_cmd_[nr]
          << " (pid: " << pids_[nr] << ")" << std::endl;
        return Error;
      } else
        return Ok;
    }

    //std::cerr << "buffer = '" << buffer << "'" << std::endl;

    while (pos < len) {
      size_t param_len = 0;

      while (buffer[pos] == ' ' || buffer[pos] == '\t' || buffer[pos] == '\n') ++pos;
      while (buffer[pos] != ' ' && buffer[pos] != '\t' && buffer[pos] != '\n' && buffer[pos] != '\0') {
        param[param_len] = buffer[pos]; ++param_len; ++pos;
      }

      param[param_len] = '\0';

      if (!param_len) break; // brak slow do wczytania
      
      if (!reply_[nr].size()) {
        if (param[0] != '=' && param[0] != '?') {
          std::cerr << "Error: wrong reply from " << players_cmd_[nr]
            << " (pid: " << pids_[nr] << "): first param = '" << param << "'" << std::endl;
          return Error;
        }
        
        if (param[1] != '\0') { // '=' lub '?' zlepione z pierwszym parametrem
          reply_[nr].push_back(param[0]=='=' ? "=":"?");
          reply_[nr].push_back(param+1);
        } else // jest odstep miedzy '=' lub '?' a reszta parametrow...
          reply_[nr].push_back(param);
      } else
        reply_[nr].push_back(param);
    }
  }

  if (!reply_[nr].size()) {
    std::cerr << "Error: no reply from " << players_cmd_[nr] 
      << " (pid: " << pids_[nr] << ")" << std::endl;
    return Error;
  }

  // TODO: czytanie z err

  return Ok;
}

const std::vector<std::string> &GtpServer::get_reply(uint nr) const 
{
  assert(nr < players_);
  return reply_[nr];
}

const std::string &GtpServer::get_reply(uint nr, uint i) const
{
  assert(nr < players_);
  return reply_[nr][i];
}

GtpServer::State GtpServer::write_to(uint nr, const char *fmt, ...)
{
  assert(nr < players_);

  va_list args;
  va_start(args, fmt);
  int ret = vfprintf(in_[nr], fmt, args);
  va_end(args);


  if (ret < 0) {
    std::cerr << "Error: cannot write to " << players_cmd_[nr] 
      << " (pid: " << pids_[nr] << "): " << errno << ", " << strerror(errno) << std::endl;
  }

  if (fflush(in_[nr])) {
    std::cerr << "Error: cannot flush data to " << players_cmd_[nr] 
      << " (pid: " << pids_[nr] << "): " << errno << ", " << strerror(errno) << std::endl;

    return Error;
  }

  return (ret >= 0) ? Ok : Error;
}




