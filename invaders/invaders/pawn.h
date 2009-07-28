#ifndef INVADERS_PAWN_H_
#define INVADERS_PAWN_H_

#include <types.h>
#include <config.h>
#include <iterator.h>

class Pawn {
  public:
    Pawn(void) : idx_(0) { }
    ~Pawn(void) { }

    Pawn(uint i) : idx_(i) { }
    static Pawn any(void)  { return -1; }
    
    uint idx(void) const { return idx_; }
    
    static const uint MaxCount = PawnSize; //<- Max pionkow

    typedef Iterator<Pawn> Iterator; //<- iterator dla pionkow

    Pawn &operator++(void) { ++idx_; return *this; }
    Pawn &operator--(void) { --idx_; return *this; }

    static uint begin_idx(void) { return 0; }
    static uint end_idx(void) { return MaxCount; }

    static Iterator begin(void) { return Iterator::begin(); }
    static Iterator end(void) { return Iterator::end(); }

    operator uint(void) const { return idx_; }
  private:
    uint idx_;
};

#endif

