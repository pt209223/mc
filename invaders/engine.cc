#include <iostream>

#include <invaders.h>

using namespace std;

class Engine {
  public: 
    Engine(Board &board, uint &time_left) 
      : board_(board), time_left_(time_left) { }
    ~Engine(void) { }

    Move genplace(Player p) 
    { 
      Vertex v[BoardSize*BoardSize];
      uint len = board_.get_legal_places(v, p);

      if (!len) return Move(p, Move::Pass);
      else return Move(p, v[rand() % len]);
    }
    
    Move genmove(Player p) 
    { 
      Move ms[320];
      uint len = board_.get_legal_moves(ms, p);

      if (!len) return Move(p, Move::Pass);
      else return ms[rand() % len];
    }

  private:
    Board &board_;
    uint &time_left_;
};

int main(void)
{
  Board board;                       //<- plansza z rozgrywka
  uint time_left;                    //<- ile czasu zostalo
  Engine engine(board, time_left);   //<- silnik gry

  Gtp gtp(cin, cout);
  GtpCommands gtp_commands_cmd(gtp);
  GtpGenplace<Engine> gtp_gen_place_cmd(gtp, engine, board);
  GtpGenmove<Engine> gtp_gen_move_cmd(gtp, engine, board);
  GtpPlace gtp_place_cmd(gtp, board);
  GtpMove gtp_move_cmd(gtp, board);
  GtpTimeLeft gtp_time_left_cmd(gtp, time_left);
  GtpSetBoard gtp_set_board_cmd(gtp, board);
  GtpShowBoard gtp_show_board_cmd(gtp, board);
  GtpLegalMoves gtp_legal_moves(gtp, board);
  GtpVersion gtp_version_cmd(gtp, "0.0.0");
  GtpAuthor gtp_author_cmd(gtp, "Piotr Truszkowski");
  GtpName gtp_name_cmd(gtp, "invaders-pt");
 
  board.clear();
  srand(Timer::now() % 1000000);

  gtp.loop();
  return 0;
}

