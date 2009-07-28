#ifndef GTP_SERVER_H_
#define GTP_SERVER_H_

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <stdarg.h>
#include <string>
#include <vector>

class GtpServer { 
  public:    
    enum State { Ok, Time, Eof, Error };

    GtpServer(const char *first, const char *second);
    ~GtpServer(void);

    // Odczyt od gracza 'nr' timeout w milisek
    State read_from(uint nr, uint timeout = 0);

    // Odpowiedz od gracza 'nr'
    const std::vector<std::string> &get_reply(uint nr) const;
    const std::string &get_reply(uint nr, uint i) const;

    // Zapis do gracza 'nr'
    State write_to(uint nr, const char *fmt, ...);

  private:
    static const uint MaxPlayers = 2;
    const uint players_;

    FILE *in_[MaxPlayers], *out_[MaxPlayers], *err_[MaxPlayers];
    int pids_[MaxPlayers];
    
    std::vector<std::string> reply_[MaxPlayers];
    std::string players_cmd_[MaxPlayers];
    
};

#endif

