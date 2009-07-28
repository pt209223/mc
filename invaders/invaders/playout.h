#ifndef INVADERS_PLAYOUT_H_
#define INVADERS_PLAYOUT_H_

template <typename policy_t>
class Playout {
  public:
    Playout(Board &board, policy_t &policy) 
      : board_(board), policy_(policy) { points_.fill(0); }
    
    void do_playout(void)
    {
      while (board_.history_len() < 10 && 
          (!board_.passed(Player::black()) || 
           !board_.passed(Player::white()))) {

        Move mv = board_.passed(board_.player_move()) ?
          Move(board_.player_move(), Move::Pass) : 
          policy_.genplace(board_);

        switch (mv.type()) {
          case Move::Pass:
          case Move::Resign:
          case Move::Place:
            board_.move(mv);
            break;
          default:
            abort();
        }
      }

      while (!board_.passed(Player::white()) || 
          !board_.passed(Player::black())) {

        Move mv = board_.passed(board_.player_move()) ? 
          Move(board_.player_move(), Move::Pass) :
          policy_.genmove(board_);

        switch (mv.type()) {
          case Move::Pass:
          case Move::Resign:
          case Move::Mov:
            board_.move(mv);
            break;
          default:
            abort();
        }
      }

      assert(board_.history_len() > 0);

      for (uint i = 0; i < board_.history_len() - 1; ++i)
        if (board_.history()[i].type() == Move::Pass ||
            board_.history()[i].type() == Move::Resign) 
          ++points_[board_.history()[i].player().other()];

      assert(points_[0] != points_[1]);
    }

    Player winner(void) const 
    {
      return points_[0] > points_[1] ? Player::black() : Player::white();
    }

  private:
    Board &board_;
    policy_t &policy_;
    FastMap<Player, uint> points_;
};

class RandomPolicy { 
  public:
    RandomPolicy(void) { }
    ~RandomPolicy(void) { }
    
    Move genplace(Board &board)
    {
      Vertex v[BoardSize*BoardSize];
      uint len = board.get_legal_places(v, board.player_move());

      if (!len) return Move(board.player_move(), Move::Pass);
      else return Move(board.player_move(), v[rand() % len]);
    }

    Move genmove(Board &board)
    {
      Move ms[320];
      uint len = board.get_legal_moves(ms, board.player_move());

      if (!len) return Move(board.player_move(), Move::Pass);
      else return ms[rand() % len];
    }
};

#endif

