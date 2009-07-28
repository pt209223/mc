#ifndef INVADERS_MOVE_H_
#define INVADERS_MOVE_H_

#include <pawn.h>
#include <player.h>
#include <vertex.h>

class Move {
  public:
    // typ ruchu
    enum Type { Any, Place, Mov, Pass, Resign };

    Move(void) : type_(Any) { }
    Move(Player pl, Type t)
      : type_(t), player_(pl), from_(Vertex::any()), to_(Vertex::any()), block_(Vertex::any()) { }
    Move(Player pl, Vertex t)
      : type_(Place), player_(pl), from_(Vertex::any()), to_(t), block_(Vertex::any()) { }
    Move(Player pl, Vertex f, Vertex t, Vertex b)
      : type_(Mov), player_(pl), from_(f), to_(t), block_(b) { }
    ~Move(void) { }

    Type type(void) const { return type_; }
    Player player(void) const { return player_; }
    Vertex from(void) const { return from_; }
    Vertex to(void) const { return to_; }
    Vertex block(void) const { return block_; }

    // kto spasowal, kto zrezygnowal? jak Player::any() nikt nie pasowa/rezygnowal
    Player who_pass(void) const { return (type_ == Pass) ? player_:Player::any(); }
    Player who_resign(void) const { return (type_ == Resign) ? player_:Player::any(); }

  private:
    Type type_;                   //<- typ ruchu
    Player player_;               //<- kto wykonuje ruch
    Vertex from_, to_, block_;    //<- skad, dokad, co blokowane
};

#endif

