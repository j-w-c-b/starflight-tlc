#include "Timer.h"

Timer::Timer() {
    timer_start = stopwatch_start = initial = al_get_time() * 1000;
}

long
Timer::getTimer() {
    return al_get_time() * 1000;
}

double
Timer::getStartTimeMillis() {
    return getTimer() - timer_start;
}

void
Timer::reset() {
    stopwatch_start = timer_start = getTimer();
}

bool
Timer::stopwatch(double ms) {
    if (getTimer() > stopwatch_start + ms) {
        stopwatch_start = getTimer();
        return true;
    } else {
        return false;
    }
}
// vi: ft=cpp
