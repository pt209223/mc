#ifndef INVADERS_FAST_POOL_H_
#define INVADERS_FAST_POOL_H_

#include <types.h>

template <typename elt_t>
class FastPool {
  public:
    FastPool(uint pool_size) : size_(pool_size) {
      memory_ = new elt_t[size_];
      free_ = new elt_t*[size_];
    }
    ~FastPool(void) 
    {
      delete[] memory_;
      delete[] free_;
    }

    // wyczyszczenie pamieci
    void clear(void)
    {
      free_count_ = size_;
      for (uint i = 0; i < size_; ++i)
        free_[i] = memory_+i;
    }

    // pobranie elementu
    elt_t *alloc(void)
    {
      assert(free_count_ > 0);
      return free_[--free_count_];
    }

    // oddanie elementu
    void free(elt_t *elt)
    {
      free_[free_count_++] = elt;
    }

  private:
    uint size_;         //<- liczba elementow
    uint free_count_;   //<- liczba wolnych elementow
    elt_t *memory_;     //<- zaalokowana pamiec
    elt_t **free_;      //<- wskazniki do wolnych elementow
};

#endif
