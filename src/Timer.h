
#ifndef _TIMER_H
#define _TIMER_H 1

#if defined(_POSIX_SOURCE) || defined(__APPLE__)
#include <sys/time.h>
#endif

class Timer {
  private:
    long timer_start;
    long stopwatch_start;
#if defined(_POSIX_SOURCE) || defined(__APPLE__)
    timeval initial;
#endif

  public:
    Timer(void);
    ~Timer(void);
    long getTimer();

    long getStartTimeMillis();

    void reset();
    bool stopwatch(long ms);
};

#endif
