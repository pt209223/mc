#ifndef INVADERS_VERTEX_H_
#define INVADERS_VERTEX_H_

#include <string>

#include <types.h>
#include <config.h>
#include <iterator.h>

class Vertex {
  public:
    Vertex(void) : idx_(0) { }
    ~Vertex(void) { }

    Vertex(uint idx) : idx_(idx) { }
    Vertex(uint x, uint y) : idx_((x+1)*(BoardSize+2)+y+1) { }
    static Vertex any(void) { return -1; }

    uint idx(void) const { return idx_; }
    uint get_x(void) const { return (idx_ / (BoardSize+2)) - 1; }
    uint get_y(void) const { return (idx_ % (BoardSize+2)) - 1; }

    bool is_on_board(void) const 
    { 
      return get_x() < BoardSize && get_y() < BoardSize;
    }

    Vertex N(void) const { return idx_ - BoardSize - 2; }
    Vertex S(void) const { return idx_ + BoardSize + 2; }
    Vertex W(void) const { return idx_ - 1; }
    Vertex E(void) const { return idx_ + 1; }

    Vertex NN(void) const { return idx_ - 2*BoardSize - 4; }
    Vertex SS(void) const { return idx_ + 2*BoardSize + 4; }
    Vertex WW(void) const { return idx_ - 2; }
    Vertex EE(void) const { return idx_ + 2; }

    Vertex NE(void) const { return idx_ - BoardSize - 1; }
    Vertex NW(void) const { return idx_ - BoardSize - 3; }
    Vertex SE(void) const { return idx_ + BoardSize + 3; }
    Vertex SW(void) const { return idx_ + BoardSize + 1; }

    static const uint MaxCount = (BoardSize+2)*(BoardSize+2); //<- max wierzcholkow
    
    typedef Iterator<Vertex> Iterator; //<- iterator dla wierzcholkow
    
    Vertex &operator++(void) { ++idx_; return *this; }
    Vertex &operator--(void) { --idx_; return *this; }

    static uint begin_idx(void) { return 0; }
    static uint end_idx(void) { return MaxCount; }

    static Iterator begin(void) { return Iterator::begin(); }
    static Iterator end(void) { return Iterator::end(); }

    static Vertex to_vertex(const std::string &str)
    {
      uint x, y;
      y = str[0] - 'a';
      x = atoi(str.c_str()+1);
      return Vertex(x, y);
    }

    std::string to_string(void) const
    {
      if (idx_ >= MaxCount) return "any";

      char buf[4];
      snprintf(buf, 4, "%c%u", get_y()+'a', get_x()+1);

      return buf;
    }

    static Vertex scan(const std::string &str)
    {
      if (str.length() != 2 && str.length() != 3) return any();
      if ((str[0] < 'a' || str[0] >= 'a'+(char)BoardSize) &&
          (str[0] < 'A' || str[0] >= 'A'+(char)BoardSize)) return any();

      uint x = atoi(str.c_str()+1) - 1, y = str[0];
      if (x > BoardSize) return any();

      if (y >= 'a' && y < 'a' + BoardSize) y -= 'a';
      else if (y >= 'A' && y < 'A' + BoardSize) y -= 'A';
      else return any();
      
      return Vertex(x, y);
    }

    operator uint(void) const { return idx_; }
  private:
    uint idx_;
};

// Petla po wszystkich polach
#define vertex_for_each_all(v, code)            \
do {                                            \
  Vertex v;                                     \
  for (uint i = 0; i < BoardSize; ++i)          \
    for (uint j = 0; j < BoardSize; ++j) {      \
      v = Vertex(i,j);                          \
      code;                                     \
    }                                           \
} while (false)

// Petla po wszystkich sasiadach(na boki, na ukos, centrum)
#define vertex_for_each_9_nbr(center, v, code)  \
do {                                            \
  Vertex v;                                     \
  v = (center);      { code; };                 \
  v = (center).N();  { code; };                 \
  v = (center).NE(); { code; };                 \
  v = (center).E();  { code; };                 \
  v = (center).SE(); { code; };                 \
  v = (center).S();  { code; };                 \
  v = (center).SW(); { code; };                 \
  v = (center).W();  { code; };                 \
  v = (center).NW(); { code; };                 \
} while (false)

// Petla po wszystkich sasiadach(na boki, na ukos)
#define vertex_for_each_8_nbr(center, v, code)  \
do {                                            \
  Vertex v;                                     \
  v = (center).N();  { code; };                 \
  v = (center).NE(); { code; };                 \
  v = (center).E();  { code; };                 \
  v = (center).SE(); { code; };                 \
  v = (center).S();  { code; };                 \
  v = (center).SW(); { code; };                 \
  v = (center).W();  { code; };                 \
  v = (center).NW(); { code; };                 \
} while (false)

// Petla po wszystkich sasiadach(na boki, centrum)
#define vertex_for_each_5_nbr(center, v, code)  \
do {                                            \
  Vertex v;                                     \
  v = (center);     { code; };                  \
  v = (center).N(); { code; };                  \
  v = (center).E(); { code; };                  \
  v = (center).S(); { code; };                  \
  v = (center).W(); { code; };                  \
} while (false)

// Petla po wszystkich sasiadach(na boki)
#define vertex_for_each_4_nbr(center, v, code)  \
do {                                            \
  Vertex v;                                     \
  v = (center).N(); { code; };                  \
  v = (center).E(); { code; };                  \
  v = (center).S(); { code; };                  \
  v = (center).W(); { code; };                  \
} while (false)

#endif

