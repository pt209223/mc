#ifndef INVADERS_BOARD_H_
#define INVADERS_BOARD_H_

#include <config.h>
#include <fast_map.h>
#include <field.h>
#include <iterator.h>
#include <move.h>
#include <types.h>
#include <vertex.h>

class Board {
  public:
    Board(void) { }
    virtual ~Board(void) { }

    // ustawienie pustej planszy
    virtual void clear(void)
    {
      for (Vertex::Iterator i = Vertex::begin(); i != Vertex::end(); ++i) 
        fields_[i] = i->is_on_board() ? Field::empty():Field::outof();

      for (Player::Iterator i = Player::begin(); i != Player::end(); ++i)
        for (Pawn::Iterator j = Pawn::begin(); j != Pawn::end(); ++j)
          pawns_[i][j] = Vertex::any();
      
      for (Player::Iterator i = Player::begin(); i != Player::end(); ++i)
        pawnc_[i] = 0;
    
      pass.fill((uint)(-1));
      history_len_ = 0;
    }
    
    // wygenerowanie legalnych umieszczen pionow, tablica v musi miec najmiej BoardSize*BoardSize el.
    virtual uint get_legal_places(Vertex *v, Player pl)
    {
      (void)pl; // nie jest istotne jaki gracz

      uint len = 0;

      vertex_for_each_all(vv, {
        if (Field::empty() == fields_[vv]) v[len++] = vv;
      });

      return len;
    }

    // wygenerowanie legalnych posuniec, tablica ms musi miec najmniej 320 el.
    virtual uint get_legal_moves(Move *ms, Player pl)
    {
      uint len = 0;

      for (Pawn::Iterator i = Pawn::begin(); i != Pawn::end(); ++i) {
        vertex_for_each_8_nbr(pawns_[pl][i], v1, {
          if (Field::empty() == fields_[v1]) {
            vertex_for_each_8_nbr(v1, v2, {
              if (Field::empty() == fields_[v2] || pawns_[pl][i] == v2)
                ms[len++] = Move(pl, pawns_[pl][i], v1, v2);
            });
          }
        });
      }

      return len;
    }

    // czy posuniecie jest dozwolone
    virtual bool is_legal_move(Move mv) const
    {
      switch (mv.type()) {
        case Move::Resign:
        case Move::Pass:
          return true;
        case Move::Mov:
          if (Field::empty() != fields_[mv.to()]) return false;

          {
            bool found_from = false, found_block = false;

            vertex_for_each_8_nbr(mv.to(), v, {
              if (mv.from() == v && Field(mv.player()) == fields_[v]) found_from = true;
              if (mv.block() == v && (Field::empty() == fields_[v] || v == mv.from())) found_block = true;
            });

            return found_from && found_block;
          }
        case Move::Place:
          if (Field::empty() != fields_[mv.to()]) return false;

          return pawnc_[mv.player()] < PawnSize;
        default:
          return false;
      }
    }

    // wykonanie ruchu
    virtual void move(Move mv)
    {
      history_[history_len_++] = mv;

      switch (mv.type()) {
        case Move::Pass:
        case Move::Resign:
          if (pass[mv.player()] == (uint)(-1))
            pass[mv.player()] = history_len_-1;
          break;
        case Move::Place: // umieszczenie piona
          assert(Field::empty() == fields_[mv.to()]);
          assert(PawnSize > pawnc_[mv.player()]);
      
          fields_[mv.to()] = Field(mv.player());
          pawns_[mv.player()][pawnc_[mv.player()]++] = mv.to();
          break;
        case Move::Mov: // ruch pionem
          for (Pawn::Iterator i = Pawn::begin(); i != Pawn::end(); ++i) {
            if (mv.from() == pawns_[mv.player()][*i]) {
              pawns_[mv.player()][*i] = mv.to();
              fields_[mv.to()]    = fields_[mv.from()];
              fields_[mv.from()]  = Field::empty();
              fields_[mv.block()] = Field::block();
              return;
            }
          }
          abort();
        default:
          abort();
      }
    }

    // cofniecie ostatniego ruchu
    virtual void undo(void)
    {
      assert(history_len_ > 0);
      --history_len_;

      //assert(Field(history_[history_len_].player) == fields_[]);
      //assert(pawns_[pl][pawnc_[pl]-1] == v);
      Player pl = history_[history_len_].player();

      switch (history_[history_len_].type()) {
        case Move::Resign:
        case Move::Pass:
          if (pass[history_[history_len_].player()] >= history_len_) 
            pass[history_[history_len_].player()] = (uint)(-1);
          break;
        case Move::Mov:
          assert(Field(pl) == fields_[history_[history_len_].to()]);
          assert(Field::block() == fields_[history_[history_len_].block()]);
          
          fields_[history_[history_len_].from()] = fields_[history_[history_len_].to()];
          fields_[history_[history_len_].to()] = Field::empty();
          fields_[history_[history_len_].block()] = Field::empty();
          break;
        case Move::Place:
          assert(pawnc_[pl] > 0);
          assert(Field(pl) == fields_[history_[history_len_].to()]);
          assert(pawns_[pl][pawnc_[pl]-1] == history_[history_len_].to());
          
          fields_[history_[history_len_].to()] = Field::empty();
          --pawnc_[pl];
          break;
        default:
          abort();
      }
    }

    // ustawienie pola
    virtual void set_field(Vertex v, Field f) { fields_[v] = f; }

    // jaki pion na wierzcholku v
    virtual Pawn pawn_at(Player pl, Vertex v) const
    { 
      for (Pawn::Iterator pw = Pawn::begin(); *pw != pawnc_[pl]; ++pw)
        if (pawns_[pl][pw] == v) return *pw;
        
      return Pawn::any();
    }

    // gdzie jest zadany pionek
    virtual Vertex vertex_for(Player pl, Pawn pw) const
    {
      return pawns_[pl][pw];
    }

    // pole na pozycji v
    virtual Field at(Vertex v) const { return fields_[v]; }

    // opis planszy
    virtual std::string to_string(void) const 
    {
      std::string out;

      for (uint i = 0; i < BoardSize; ++i) {
        for (uint j = 0; j < BoardSize; ++j) {
          Field f = at(Vertex(i, j));
          if (Field::empty() == f) out += ". ";
          else if (Field::black() == f) out += "# ";
          else if (Field::white() == f) out += "O ";
          else if (Field::block() == f) out += "X ";
          else abort();
        }

        out += "\n";
      }

      return out;
    }

    // historia
    virtual const Move *history(void) const { return history_; }
    
    // liczba ruchow w historii
    virtual uint history_len(void) const { return history_len_; }

    // kto ma ruch
    virtual Player player_move(void) const 
    { 
      return history_len_? history_[history_len_-1].player().other() : Player::black(); 
    }

    // czy gracz pasowal
    virtual bool passed(Player pl) const { return pass[pl] != (uint)(-1); }

  protected:
    FastMap<Vertex, Field> fields_;                    //<- pola na planszy
    FastMap<Player, FastMap<Pawn, Vertex> > pawns_;    //<- pionki na planszy
    FastMap<Player, uint> pawnc_;                      //<- liczba pionkow

    static const uint HistoryMaxLen = 2*BoardSize*BoardSize; 
    Move history_[HistoryMaxLen];                      //<- historia gry
    uint history_len_;
    FastMap<Player, uint> pass;
};

#endif

