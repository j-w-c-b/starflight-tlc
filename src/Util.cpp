/*
        STARFLIGHT - THE LOST COLONY
        util.cpp - ?
        Author: ?
        Date: ?
*/

#include "Util.h"
#include "Game.h"

#include <algorithm>
#include <string>

using namespace std;

void
Util::Init() {
    srand((unsigned int)time(NULL));
}

int
Util::ReentrantDelay(int ms) {
    static int start = 0;
    int retval = 0;

    if (start == 0) {
        start = g_game->globalTimer.getTimer();
    } else {
        if (g_game->globalTimer.getTimer() > start + ms) {
            start = 0;
            retval = 1;
        }
    }

    return retval;
}

std::string
Util::ToUpper(std::string &str) {
    std::string converted(str);
    transform(str.begin(), str.end(), converted.begin(), ::toupper);
    return converted;
}
