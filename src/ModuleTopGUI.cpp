/*
        STARFLIGHT - THE LOST COLONY
        ModuleTopGUI.cpp
        Author: Keith Patch
        Date: April 2008
*/

#include <allegro5/allegro.h>

#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "ModuleTopGUI.h"
#include "Script.h"
#include "Util.h"
#include "topgui_resources.h"

using namespace topgui_resources;

int ggx = 0;
int ggy = 0;

ALLEGRO_DEBUG_CHANNEL("ModuleTopGUI")

ModuleTopGUI::ModuleTopGUI() : m_resources(TOPGUI_IMAGES) {}
ModuleTopGUI::~ModuleTopGUI() {}

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
    al_draw_bitmap(m_resources[I_TOPGAUGE], ggx, ggy, 0);
    al_draw_bitmap_region(
        m_resources[I_ELEMENT_GAUGE_GREEN],
        0,
        0,
        al_get_bitmap_width(m_resources[I_ELEMENT_GAUGE_GREEN]) * hull_percent,
        al_get_bitmap_height(m_resources[I_ELEMENT_GAUGE_GREEN]),
        ggx + 89,
        ggy + 15,
        0);
    al_draw_bitmap_region(
        m_resources[I_ELEMENT_GAUGE_RED],
        0,
        0,
        al_get_bitmap_width(m_resources[I_ELEMENT_GAUGE_RED]) * armor_percent,
        al_get_bitmap_height(m_resources[I_ELEMENT_GAUGE_RED]),
        ggx + 273,
        ggy + 15,
        0);
    al_draw_bitmap_region(
        m_resources[I_ELEMENT_GAUGE_BLUE],
        0,
        0,
        al_get_bitmap_width(m_resources[I_ELEMENT_GAUGE_BLUE]) * shield_percent,
        al_get_bitmap_height(m_resources[I_ELEMENT_GAUGE_BLUE]),
        ggx + 464,
        ggy + 15,
        0);
    al_draw_bitmap_region(
        m_resources[I_ELEMENT_GAUGE_ORANGE],
        0,
        0,
        al_get_bitmap_width(m_resources[I_ELEMENT_GAUGE_ORANGE]) * fuel_percent,
        al_get_bitmap_height(m_resources[I_ELEMENT_GAUGE_ORANGE]),
        ggx + 630,
        ggy + 14,
        0);
    return true;
}
