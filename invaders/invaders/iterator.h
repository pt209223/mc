#ifndef INVADERS_ITERATOR_H_
#define INVADERS_ITERATOR_H_

#include <types.h>

// Wymagane: 
//  idx_t::begin_idx(), 
//  idx_t::end_idx(), 
//  idx_t::operator++(), 
//  idx_t::operator--()

template <typename idx_t>
class Iterator {
  public: 
    Iterator(void) { }
    Iterator(idx_t i) : idx_(i) { }
    ~Iterator(void) { }

    const idx_t &operator*(void) const { return idx_; }
    const idx_t *operator->(void) const { return &idx_; }
    Iterator &operator++(void) { ++idx_; return *this; }
    Iterator &operator--(void) { --idx_; return *this; }
    Iterator operator++(int) { Iterator i(idx_); ++idx_; return i; }
    Iterator operator--(int) { Iterator i(idx_); --idx_; return i; }

    bool operator==(const Iterator &i) const { return idx_ == i.idx_; }
    bool operator!=(const Iterator &i) const { return idx_ != i.idx_; }

    static Iterator begin(void) { return Iterator(idx_t::begin_idx()); }
    static Iterator end(void) { return Iterator(idx_t::end_idx()); }

    operator idx_t(void) const { return idx_; }
  private:
    idx_t idx_;
};

#endif

