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

using namespace std;
using namespace topgui;

ALLEGRO_DEBUG_CHANNEL("ModuleTopGUI")

bool
ModuleTopGUI::on_init() {
    int ggx = static_cast<int>(g_game->getGlobalNumber("GUI_GAUGES_POS_X"));
    int ggy = static_cast<int>(g_game->getGlobalNumber("GUI_GAUGES_POS_Y"));

    m_top_gauge = make_shared<Bitmap>(images[I_TOPGAUGE], ggx, ggy);
    add_child_module(m_top_gauge);

    m_hull_gauge =
        make_shared<Bitmap>(images[I_ELEMENT_GAUGE_GREEN], ggx + 89, ggy + 15);
    add_child_module(m_hull_gauge);
    m_armor_gauge =
        make_shared<Bitmap>(images[I_ELEMENT_GAUGE_RED], ggx + 273, ggy + 15);
    add_child_module(m_armor_gauge);

    m_shields_gauge =
        make_shared<Bitmap>(images[I_ELEMENT_GAUGE_BLUE], ggx + 464, ggy + 15);
    add_child_module(m_shields_gauge);

    m_fuel_gauge = make_shared<Bitmap>(
        images[I_ELEMENT_GAUGE_ORANGE], ggx + 630, ggy + 14);
    add_child_module(m_fuel_gauge);

    return true;
}

bool
ModuleTopGUI::on_close() {
    remove_child_module(m_fuel_gauge);
    remove_child_module(m_shields_gauge);
    remove_child_module(m_armor_gauge);
    remove_child_module(m_hull_gauge);
    remove_child_module(m_top_gauge);

    m_top_gauge = nullptr;
    m_hull_gauge = nullptr;
    m_armor_gauge = nullptr;
    m_shields_gauge = nullptr;
    m_fuel_gauge = nullptr;

    return true;
}

bool
ModuleTopGUI::on_draw(ALLEGRO_BITMAP * /*target*/) {
    float fuel_percent = g_game->gameState->getShip().getFuel();
    float hull_percent = g_game->gameState->getShip().getHullIntegrity() / 100;
    float armor_percent = 0;
    float shield_percent = 0;

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
    m_hull_gauge->set_clip_width(m_hull_gauge->get_width() * hull_percent);
    m_armor_gauge->set_clip_width(m_armor_gauge->get_width() * armor_percent);
    m_shields_gauge->set_clip_width(
        m_shields_gauge->get_width() * shield_percent);
    m_fuel_gauge->set_clip_width(m_fuel_gauge->get_width() * fuel_percent);

    return true;
}
