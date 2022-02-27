/*
        STARFLIGHT - THE LOST COLONY
        ModuleGameOver.cpp
        Date: October, 2007
*/

#include "ModuleGameOver.h"
#include "Game.h"
#include "ModeMgr.h"

ModuleGameOver::ModuleGameOver() : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT) {
    bQuickShutdown = false;
}

ModuleGameOver::~ModuleGameOver() {}

bool
ModuleGameOver::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    if (event->keycode == ALLEGRO_KEY_ESCAPE) {
        al_rest(500 * 0.001);
        g_game->LoadModule(MODULE_TITLESCREEN);
        return false;
    }

    return true;
}

bool
ModuleGameOver::on_update() {
    if (bQuickShutdown) {
        g_game->shutdown();
        return false;
    }

    return true;
}

bool
ModuleGameOver::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);

    al_clear_to_color(BLACK);

    g_game->Print32(target, 400, 300, "G A M E  O V E R", RED);

    return true;
}
