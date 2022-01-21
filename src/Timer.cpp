#include "Timer.h"
#include <allegro5/allegro.h>

Timer::Timer(void) {
#if defined(_POSIX_SOURCE) || defined(__APPLE__)
    gettimeofday(&initial, NULL);
#endif

    reset();
}

Timer::~Timer(void) {}

long
Timer::getTimer() {
#if defined(_MSC_VER) || defined(WIN32)
    return (long)clock();

#elif defined(_POSIX_SOURCE) || defined(__APPLE__)
    timeval current, delta;
    gettimeofday(&current, NULL);
    timersub(&current, &initial, &delta);
    return (long)(delta.tv_sec * 1000 + delta.tv_usec / 1000);

#else
#error Could not determine the function to get wall-clock time

#endif
}

long
Timer::getStartTimeMillis() {
    return getTimer() - timer_start;
}

void
Timer::reset() {
    timer_start = getTimer();
    stopwatch_start = timer_start;
}

bool
Timer::stopwatch(long ms) {
    if (getTimer() > stopwatch_start + ms) {
        stopwatch_start = getTimer();
        return true;
    } else
        return false;
}
