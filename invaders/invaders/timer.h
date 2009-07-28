#ifndef INVADERS_TIMER_H_
#define INVADERS_TIMER_H_

#include <types.h>
#include <ctime>
#include <sys/time.h>

class Timer {
  public:
    Timer(void) { tic(); }
    ~Timer(void) { }

    // poczatek mierzenia czasu
    void tic(void)
    {
      gettimeofday(&t_, 0);
    }

    // w milisek, ile czasu minelu
    uint toc(void) const 
    {
      struct timeval t2;
      gettimeofday(&t2, 0);

      return (1000000*(t2.tv_sec - t_.tv_sec) + t2.tv_usec - t_.tv_usec)/1000;
    }

    // teraz, w usec
    static unsigned long long now(void)
    {
      struct timeval t;
      gettimeofday(&t, 0);
      return (1000000ULL*t.tv_sec) + t.tv_usec;
    }

  private:
    struct timeval t_;
};

#endif

