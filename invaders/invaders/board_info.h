#ifndef INVADERS_BOARD_INFO_H_
#define INVADERS_BOARD_INFO_H_

#include <board.h>

class BoardInfo : public Board {
  public:
    BoardInfo(void) : ending_(false) { }
    BoardInfo(const Board &b) : Board(b), ending_(false) { }

    // analiza sytuacji
    void look(void)
    {
      dist_[Player::black()].fill(-1);
      dist_[Player::white()].fill(-1);
      ending_ = false;

      for (Player::Iterator pl = Player::begin(); pl != Player::end(); ++pl) {
        Vertex queue[Vertex::MaxCount];
        uint pos = 0, len = 0;

        for (Pawn::Iterator i = Pawn::begin(); i != Pawn::end(); ++i) {
          dist_[pl][pawns_[pl][i]] = 0;
          queue[len++] = pawns_[pl][i];
        }

        while (pos < len) {
          Vertex v = queue[pos++];
          vertex_for_each_8_nbr(v, vv, {
            if (dist_[pl][vv] != (uint)(-1) && 
                Field::outof() != fields_[v] && 
                Field::block() != fields_[v]) {
              dist_[pl][vv] = dist_[pl][v]+1;
              queue[len++] = vv;
            }
          });
        }

        count_[pl] = len;
      }

      for (Pawn::Iterator i = Pawn::begin(); i != Pawn::end(); ++i)
        if (dist_[Player::black()][pawns_[Player::white()][i]] != (uint)(-1))
        { ending_ = false; break; }

    }

  protected:
    FastMap<Player, FastMap<Vertex, uint> > dist_;  //<- odleglosci od najblizszego pionka
    FastMap<Player, uint> count_;                   //<- liczba pol, do ktorych mozna dojsc
    bool ending_;                                   //<- czy koncowka gry
};

#endif

