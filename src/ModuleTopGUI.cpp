/*
        STARFLIGHT - THE LOST COLONY
        ModuleTopGUI.cpp
        Author: Keith Patch
        Date: April 2008
*/

#include <allegro5/allegro.h>

#include "Game.h"
#include "ModuleTopGUI.h"
#include "topgui_resources.h"

using namespace topgui;

int ggx = 0;
int ggy = 0;

ALLEGRO_DEBUG_CHANNEL("ModuleTopGUI")

bool
ModuleTopGUI::on_init() {
    ggx = static_cast<int>(g_game->getGlobalNumber("GUI_GAUGES_POS_X"));
    ggy = static_cast<int>(g_game->getGlobalNumber("GUI_GAUGES_POS_Y"));

    return true;
}

bool
ModuleTopGUI::on_draw(ALLEGRO_BITMAP *target) {
    float fuel_percent = g_game->gameState->getShip().getFuel();
    float hull_percent = g_game->gameState->getShip().getHullIntegrity() / 100;
    float armor_percent = 0;
    float shield_percent = 0;
    al_set_target_bitmap(target);

    if (g_game->gameState->getShip().getMaxArmorIntegrity() <= 0) {
        armor_percent = 0;
    } else {
        armor_percent = g_game->gameState->getShip().getArmorIntegrity()
                        / g_game->gameState->getShip().getMaxArmorIntegrity();
    }

    if (g_game->gameState->getShip().getMaxShieldCapacity() <= 0) {
        shield_percent = 0;
    } else {
        shield_percent = g_game->gameState->getShip().getShieldCapacity()
                         / g_game->gameState->getShip().getMaxShieldCapacity();
    }
    /*
     * draw top gauge gui
     */
    al_draw_bitmap(images[I_TOPGAUGE].get(), ggx, ggy, 0);
    al_draw_bitmap_region(
        images[I_ELEMENT_GAUGE_GREEN].get(),
        0,
        0,
        al_get_bitmap_width(images[I_ELEMENT_GAUGE_GREEN].get()) * hull_percent,
        al_get_bitmap_height(images[I_ELEMENT_GAUGE_GREEN].get()),
        ggx + 89,
        ggy + 15,
        0);
    al_draw_bitmap_region(
        images[I_ELEMENT_GAUGE_RED].get(),
        0,
        0,
        al_get_bitmap_width(images[I_ELEMENT_GAUGE_RED].get()) * armor_percent,
        al_get_bitmap_height(images[I_ELEMENT_GAUGE_RED].get()),
        ggx + 273,
        ggy + 15,
        0);
    al_draw_bitmap_region(
        images[I_ELEMENT_GAUGE_BLUE].get(),
        0,
        0,
        al_get_bitmap_width(images[I_ELEMENT_GAUGE_BLUE].get())
            * shield_percent,
        al_get_bitmap_height(images[I_ELEMENT_GAUGE_BLUE].get()),
        ggx + 464,
        ggy + 15,
        0);
    al_draw_bitmap_region(
        images[I_ELEMENT_GAUGE_ORANGE].get(),
        0,
        0,
        al_get_bitmap_width(images[I_ELEMENT_GAUGE_ORANGE].get())
            * fuel_percent,
        al_get_bitmap_height(images[I_ELEMENT_GAUGE_ORANGE].get()),
        ggx + 630,
        ggy + 14,
        0);
    return true;
}
