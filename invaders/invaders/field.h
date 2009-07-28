#ifndef INVADERS_FIELD_H_
#define INVADERS_FIELD_H_

#include <types.h>

class Field {
  public:
    Field(void) : idx_(0) { }
    ~Field(void) { }

    Field(uint idx) : idx_(idx) { }
    uint idx(void) const { return idx_; }

    static Field black(void) { return 0; }
    static Field white(void) { return 1; }
    static Field outof(void) { return 2; }
    static Field empty(void) { return 3; }
    static Field block(void) { return 4; }

    bool operator==(const Field &f) const { return idx_ == f.idx_; }
    bool operator!=(const Field &f) const { return idx_ != f.idx_; }

    operator uint(void) const { return idx_; }
  private:
    uint idx_;
};

#endif

