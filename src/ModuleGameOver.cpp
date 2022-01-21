/*
        STARFLIGHT - THE LOST COLONY
        ModuleGameOver.cpp
        Date: October, 2007
*/

#include "ModuleGameOver.h"
#include "Game.h"
#include "ModeMgr.h"

ModuleGameOver::ModuleGameOver(void) { bQuickShutdown = false; }

ModuleGameOver::~ModuleGameOver(void) {}

void
ModuleGameOver::OnKeyReleased(int keyCode) {
    if (keyCode == ALLEGRO_KEY_ESCAPE) {
        al_rest(500 * 0.001);
        g_game->LoadModule(MODULE_TITLESCREEN);
        return;
    }
}

void
ModuleGameOver::Update() {
    if (bQuickShutdown) {
        g_game->shutdown();
    }
}

void
ModuleGameOver::Draw() {
    al_set_target_bitmap(g_game->GetBackBuffer());

    al_clear_to_color(BLACK);

    g_game->Print32(g_game->GetBackBuffer(), 400, 300, "G A M E  O V E R", RED);
}
