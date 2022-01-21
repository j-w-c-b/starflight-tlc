/*
        STARFLIGHT - THE LOST COLONY
        main.cpp - main function that launches the game class
        Author: Coder
        Date:
*/

#include "Game.h"
#include <allegro5/allegro.h>
#include <iostream>

// global engine object
Game *g_game;

int
main(int, char *[]) {
    g_game = new Game();
    g_game->Run();
    delete g_game;

    return 0;
}
