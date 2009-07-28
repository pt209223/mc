#ifndef INVADERS_PLAYER_H_
#define INVADERS_PLAYER_H_

#include <string>

#include <config.h>
#include <iterator.h>
#include <types.h>

class Player {
  public:
    Player(void) : idx_(0) { }
    ~Player(void) { }

    Player(uint idx) : idx_(idx) { }
    static Player black(void) { return  0; }
    static Player white(void) { return  1; }
    static Player any(void)   { return -1; }
   
    Player other(void) const { return *this == any() ? idx_ : !idx_; }

    uint idx(void) const { return idx_; }

    static const uint MaxCount = 2; //<- max graczy

    typedef Iterator<Player> Iterator; //<- iterator dla graczy

    Player &operator++(void) { ++idx_; return *this; }
    Player &operator--(void) { --idx_; return *this; }

    static uint begin_idx(void) { return 0; }
    static uint end_idx(void) { return MaxCount; }

    static Iterator begin(void) { return Iterator::begin(); }
    static Iterator end(void) { return Iterator::end(); }

    std::string to_string(void) const 
    {
      if (black() == *this) return "black";
      if (white() == *this) return "white";
      return "any";
    }

    static Player scan(const std::string &str) 
    {
      if (str == "black" || str == "b") return black();
      if (str == "white" || str == "w") return white();
      return any(); 
    }
    
    operator uint(void) const { return idx_; }
  private:
    uint idx_;
};

#endif
