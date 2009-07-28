#ifndef INVADERS_FAST_MAP_H_
#define INVADERS_FAST_MAP_H_

#include <types.h>

// Wymagane: 
//  idx_t.idx(),
//  idx_t::MaxCount

template <typename idx_t, typename elt_t>
class FastMap {
  public:
    FastMap(void) { }
    ~FastMap(void) { }

    // operator []
    const elt_t &operator[](idx_t i) const { return tbl_[i.idx()]; }
    elt_t &operator[](idx_t i) { return tbl_[i.idx()]; }

    void fill(elt_t e) 
    { 
      for (uint i = 0; i < idx_t::MaxCount; ++i) tbl_[i] = e;
    }

  private:
    elt_t tbl_[idx_t::MaxCount]; //<- tablica elementow
};

#endif

