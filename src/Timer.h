#ifndef STLC_TIMER_H
#define STLC_TIMER_H 1

#include <allegro5/allegro.h>

class Timer {
  public:
    Timer();
    long getTimer();
    double getStartTimeMillis();

    void reset();
    bool stopwatch(double ms);

  private:
    double timer_start;
    double stopwatch_start;
    double initial;
};

#endif // STLC_TIMER_H
// vi: ft=cpp
