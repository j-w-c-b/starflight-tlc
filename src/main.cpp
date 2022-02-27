/*
        STARFLIGHT - THE LOST COLONY
        main.cpp - main function that launches the game class
        Author: Coder
        Date:
*/

#include <allegro5/allegro.h>

#include "Game.h"
#include "ResourceManager.h"

// global engine object
Game *g_game;
ImageManager images;
SampleManager samples;

int
main(int, char *[]) {
    al_init();
    g_game = new Game();
    g_game->Run();

    delete g_game;
    images.clear();
    samples.clear();

    return 0;
}
