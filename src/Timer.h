
#ifndef _TIMER_H
#define _TIMER_H 1

#if defined(__linux__) || defined(__APPLE__)
#include <sys/time.h>
#endif

class Timer {
  private:
    long timer_start;
    long stopwatch_start;
#if defined(__linux__) || defined(__APPLE__)
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
