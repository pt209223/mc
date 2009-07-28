#include <iostream>

#include <invaders.h>

#include <gtp_server.h>

using namespace std;

struct player_info { 
  player_info(void) { }

  std::string name;                    //<- nazwa programu
  std::string author;                  //<- autor programu
  std::string version;                 //<- wersja programu
  std::vector<std::string> supports;   //<- lista obslugiwanych komend
  std::string path;                    //<- sciezka do programu
  bool playing;                        //<- czy w grze
};

player_info players[2];
GtpServer *srv;
Board board;
uint points = 1;

bool get_info(void)
{
  players[0].playing = true;
  players[1].playing = true;

  // Pobieranie nazwy programu

  for (uint i = 0; i < 2; ++i) {
    if (srv->write_to(i, "name\n") != GtpServer::Ok) {
      cout << "# player " << (Player(i).to_string()) << " not responding" << endl;
      players[i].playing = false;
      continue;
    }
    if (srv->read_from(i) != GtpServer::Ok) {
      cout << "# player " << (Player(i).to_string()) << " not responding" << endl;
      players[i].playing = false;
      continue;
    }
    if (srv->get_reply(i, 0) == "?") {
      cout << "# player " << (Player(i).to_string()) << " not supports 'name'" << endl;
      players[i].name = "?";
      continue;
    }

    for (uint j = 1; j < srv->get_reply(i).size(); ++j) 
      players[i].name += (j > 1) ? (" " + srv->get_reply(i, j)) : srv->get_reply(i, j);
  }

  if (!players[0].playing || !players[1].playing) return false;

  // Pobieranie autora programu

  for (uint i = 0; i < 2; ++i) {
    if (srv->write_to(i, "author\n") != GtpServer::Ok) {
      cout << "# player " << (Player(i).to_string()) << " not responding" << endl;
      players[i].playing = false;
      continue;
    }
    if (srv->read_from(i) != GtpServer::Ok) {
      cout << "# player " << (Player(i).to_string()) << " not responding" << endl;
      players[i].playing = false;
      continue;
    }
    if (srv->get_reply(i, 0) == "?") {
      cout << "# player " << (Player(i).to_string()) << " not supports 'author'" << endl;
      players[i].author = "?";
      continue;
    }

    for (uint j = 1; j < srv->get_reply(i).size(); ++j) 
      players[i].author += (j > 1) ? (" " + srv->get_reply(i, j)) : srv->get_reply(i, j);
  }

  if (!players[0].playing || !players[1].playing) return false;
  
  // Pobieranie wersji programu

  for (uint i = 0; i < 2; ++i) {
    if (srv->write_to(i, "version\n") != GtpServer::Ok) {
      cout << "# player " << (Player(i).to_string()) << " not responding" << endl;
      players[i].playing = false;
      continue;
    }
    if (srv->read_from(i) != GtpServer::Ok) {
      cout << "# player " << (Player(i).to_string()) << " not responding" << endl;
      players[i].playing = false;
      continue;
    }
    if (srv->get_reply(i, 0) == "?") {
      cout << "# player " << (Player(i).to_string()) << " not supports 'version'" << endl;
      players[i].version = "?";
      continue;
    }

    for (uint j = 1; j < srv->get_reply(i).size(); ++j) 
      players[i].version += (j > 1) ? (" " + srv->get_reply(i, j)) : srv->get_reply(i, j);
  }

  if (!players[0].playing || !players[1].playing) return false;
  
  // Pobieranie wspomaganych komend
  
  for (uint i = 0; i < 2; ++i) {
    if (srv->write_to(i, "commands\n") != GtpServer::Ok) {
      cout << "# player " << (Player(i).to_string()) << " not responding" << endl;
      players[i].playing = false;
      continue;
    }
    if (srv->read_from(i) != GtpServer::Ok) {
      cout << "# player " << (Player(i).to_string()) << " not responding" << endl;
      players[i].playing = false;
      continue;
    }
    
    if (srv->get_reply(i, 0) == "?") {
      cout << "# player " << (Player(i).to_string()) << " not supports 'commands'" << endl;
      players[i].supports.clear();
      continue;
    }

    for (uint j = 1; j < srv->get_reply(i).size(); ++j) 
      players[i].supports.push_back(srv->get_reply(i, j));
  }

  if (!players[0].playing || !players[1].playing) return false;

  for (uint i = 0; i < 2; ++i) {
    cout << "# player : " << (Player(i).to_string()) << " : name        : " << players[i].name << std::endl;
    cout << "# player : " << (Player(i).to_string()) << " : author      : " << players[i].author << std::endl;
    cout << "# player : " << (Player(i).to_string()) << " : version     : " << players[i].version << std::endl;
    cout << "# player : " << (Player(i).to_string()) << " : supports    :"; 
    
    if (players[i].supports.size() > 0) {
      for (uint j = 0; j < players[i].supports.size(); ++j) 
        cout << " " << players[i].supports[j];
    } else
      cout << " ?";
    
    cout << endl;
  }

  return true;
}

void do_placing(void)
{
  if (!players[0].playing || !players[1].playing) return;
  
  for (uint count = 0; count < 5; ++count) {
    for (uint i = 0; i < 2; ++i) {
      if (srv->write_to(i, "genplace %s\n", i?"w":"b") != GtpServer::Ok || 
          srv->read_from(i) != GtpServer::Ok) {
        cout << "# player " << (Player(i).to_string()) << " not responding" << endl;
        players[i].playing = false; 
        break;
      }
      if (srv->get_reply(i, 0) == "?") {
        cout << "# player " << (Player(i).to_string()) << " not supports 'genplace'" << endl;
        players[i].playing = false;
        break;
      }
      if (srv->get_reply(i).size() != 2) {
        cout << "# player " << (Player(i).to_string()) << " returns wrong number of parameters" << endl;
        players[i].playing = false;
        break;
      }
      
      Vertex v = Vertex::scan(srv->get_reply(i, 1));
      if (Vertex::any() == v) {
        if (srv->get_reply(i,1) == "pass" || srv->get_reply(i,1) == "resign") {
          cout << "# player " << (Player(i).to_string()) << " passes/resigns" << endl;
          players[i].playing = false;
          break;
        }

        cout << "# player " << (Player(i).to_string()) << " returns wrong vertex coordinates" << endl;
        players[i].playing = false;
        break;
      }

      if (!board.is_legal_move(Move(i, v))) {
        cout << "# player " << (Player(i).to_string()) << " returns illegal move: " << v.to_string() << endl;
        players[i].playing = false;
        break;
      }

      board.move(Move(i, v));
      cout << "place " << Player(i).to_string() << " " << v.to_string() << endl;

      if (srv->write_to(!i, "place %s %s\n", i?"w":"b", v.to_string().c_str()) != GtpServer::Ok || 
          srv->read_from(!i) != GtpServer::Ok) {
        cout << "# player " << (!i) << " not responding" << endl;
        players[!i].playing = false;
        break;
      }

      if (srv->get_reply(!i).size() != 1 || srv->get_reply(!i, 0) != "=") {
        cout << "# player " << (!i) << " returns unexpected output" << endl;
        players[!i].playing = false;
        break;
      }
    }
  
    if (!players[0].playing || !players[1].playing) return;
  }
}

void do_moving(void)
{
  if (!players[0].playing || !players[1].playing) return;
  
  while (true) {
    for (uint i = 0; i < 2; ++i) {
      if (srv->write_to(i, "genmove %s\n", i?"w":"b") != GtpServer::Ok || 
          srv->read_from(i) != GtpServer::Ok) {
        cout << "# player " << (Player(i).to_string()) << " not responding" << endl;
        players[i].playing = false; 
        break;
      }
      if (srv->get_reply(i, 0) == "?") {
        cout << "# player " << (Player(i).to_string()) << " not supports 'genmove'" << endl;
        players[i].playing = false;
        break;
      }
      if (srv->get_reply(i).size() != 4) {
        if (srv->get_reply(i).size() == 2 &&
            (srv->get_reply(i,1) == "pass" || srv->get_reply(i,1) == "resign")) {
          cout << "# player " << (Player(i).to_string()) << " passes/resigns" << endl;
          players[i].playing = false;
          break;
        }
        cout << "# player " << (Player(i).to_string()) << " returns wrong number of params" << endl;
        players[i].playing = false;
        break;
      }
      
      Vertex vf = Vertex::scan(srv->get_reply(i, 1)),
             vt = Vertex::scan(srv->get_reply(i, 2)),
             vb = Vertex::scan(srv->get_reply(i, 3));

      if (Vertex::any() == vf || Vertex::any() == vt || Vertex::any() == vb) {
        cout << "# player " << (Player(i).to_string()) << " returns wrong vertex coordinates: " 
          << vf.to_string() << ", " << vt.to_string() << ", " << vb.to_string() << endl;
        players[i].playing = false;
        break;
      }

      if (!board.is_legal_move(Move(i, vf, vt, vb))) {
        cout << "# player " << (Player(i).to_string()) << " returns illegal move: " 
          << vf.to_string() << ", " << vt.to_string() << ", " << vb.to_string() << endl;
        players[i].playing = false;
        break;
      }

      board.move(Move(i, vf, vt, vb));
      cout << "move " << Player(i).to_string() << " " 
        << vf.to_string() << " " << vt.to_string() << " " << vb.to_string() << endl;

      if (srv->write_to(!i, "move %s %s %s %s\n", i?"w":"b", 
            vf.to_string().c_str(), vt.to_string().c_str(), vb.to_string().c_str()) != GtpServer::Ok || 
          srv->read_from(!i) != GtpServer::Ok) {
        cout << "# player " << (!i) << " not responding" << endl;
        players[!i].playing = false;
        break;
      }

      if (srv->get_reply(!i).size() != 1 || srv->get_reply(!i, 0) != "=") {
        cout << "# player " << (!i) << " returns unexpected output" << endl; // TODO
        players[!i].playing = false;
        break;
      }
    }

    if (!players[0].playing || !players[1].playing) return;
  }
}

void do_ending(void)
{
  if (!players[0].playing && !players[1].playing) return;
  
  assert(! (players[0].playing && players[1].playing) );

  uint p = players[0].playing ? 0:1;

  cout << "move " + (Player(!p).to_string()) + " pass" << endl;

  while (true) {
    if (srv->write_to(p, "genmove %s\n", p?"w":"b") != GtpServer::Ok || 
        srv->read_from(p) != GtpServer::Ok) {
      cout << "# player " << (Player(p).to_string()) << " not responding" << endl;
      players[p].playing = false; 
      break;
    }
    if (srv->get_reply(p, 0) == "?") {
      cout << "# player " << (Player(p).to_string()) << " not supports 'genmove'" << endl;
      players[p].playing = false;
      break;
    }
    if (srv->get_reply(p).size() != 4) {
      if (srv->get_reply(p).size() == 2 &&
          (srv->get_reply(p,1) == "pass" || srv->get_reply(p,1) == "resign")) {
        cout << "# player " << (Player(p).to_string()) << " passes/resigns" << endl;
        players[p].playing = false;
        break;
      }
      cout << "# player " << (Player(p).to_string()) << " returns wrong number of params" << endl;
      players[p].playing = false;
      break;
    }
      
    Vertex vf = Vertex::scan(srv->get_reply(p, 1)),
           vt = Vertex::scan(srv->get_reply(p, 2)),
           vb = Vertex::scan(srv->get_reply(p, 3));

    if (Vertex::any() == vf || Vertex::any() == vt || Vertex::any() == vb) {
      cout << "# player " << (Player(p).to_string()) << " returns wrong vertex coordinates: " 
        << vf.to_string() << ", " << vt.to_string() << ", " << vb.to_string() << endl;
      players[p].playing = false;
      break;
    }

    if (!board.is_legal_move(Move(p, vf, vt, vb))) {
      cout << "# player " << (Player(p).to_string()) << " returns illegal move: " 
        << vf.to_string() << ", " << vt.to_string() << ", " << vb.to_string() << endl;
      players[p].playing = false;
      break;
    }

    board.move(Move(p, vf, vt, vb));
    cout << "move " << Player(p).to_string() << " " 
      << vf.to_string() << " " << vt.to_string() << " " << vb.to_string() << endl;

    ++points;
  }

  cout << "move " << Player(p).to_string() << " pass" << endl;
  cout << "# " << Player(p).to_string() << " wins " << points << " points advantage" << endl;
}

bool usage(int argc, char **argv)
{
  if (argc != 3) return false;

  players[0].path = argv[1];
  players[1].path = argv[2];

  return true;
}

int main(int argc, char **argv)
{
  // Sprawdzenie parametorow sedziego
  
  if (!usage(argc, argv)) {
    cerr 
      << "Usage: " << argv[0] << " <first-player> <second-player>\n"
      << endl;
    
    return 1;
  }

  // Odpalenie serwera gtp

  srv = new GtpServer(players[0].path.c_str(), players[1].path.c_str());
  board.clear();

  // Pobranie informacji od graczy

  if (!get_info()) {
    if (!players[0].playing) cout << "# player black crash" << endl;
    if (!players[1].playing) cout << "# player white crash" << endl;
    delete srv; 
    return 0; 
  }

  do_placing();   //<- umieszczanie pionow na planszy
  do_moving();    //<- rozgrywka dwoch graczy
  do_ending();    //<- koncowka - jeden wypelnia plansze

  cerr << endl << board.to_string() << endl;
  cerr << endl << "-- Bye" << endl << endl;

  delete srv;
  return 0;
}
