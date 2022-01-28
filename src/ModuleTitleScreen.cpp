/*
        STARFLIGHT - THE LOST COLONY
        ModuleTitleScreen.cpp
        Author: J.Harbour
        Date: Dec,2007

    menu: 311,461
*/

#include "ModuleTitleScreen.h"
#include "Button.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "Util.h"
#include "titlescreen_resources.h"
#include <allegro5/allegro.h>

using namespace std;
using namespace titlescreen_resources;

ALLEGRO_DEBUG_CHANNEL("ModuleTitleScreen")

ModuleTitleScreen::ModuleTitleScreen()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
      m_resources(TITLESCREEN_IMAGES) {
    const int mainmenu_x = 392;
    const int mainmenu_y = 450;

    m_title = new Bitmap(m_resources[I_TITLE_NORMAL], 0, 100);
    add_child_module(m_title);

    // create new game button
    m_new_game = new NewButton(
        mainmenu_x,
        mainmenu_y,
        -1,
        -1,
        EVENT_NONE,
        EVENT_NEW_GAME,
        m_resources[I_TITLE_NEWGAME_NORMAL],
        m_resources[I_TITLE_NEWGAME_OVER]);
    add_child_module(m_new_game);

    // create load button
    m_load_game = new NewButton(
        mainmenu_x,
        mainmenu_y + 60,
        -1,
        -1,
        EVENT_NONE,
        EVENT_LOAD_GAME,
        m_resources[I_TITLE_LOADGAME_NORMAL],
        m_resources[I_TITLE_LOADGAME_OVER]);
    add_child_module(m_load_game);

    // create settings button
    m_settings = new NewButton(
        mainmenu_x,
        mainmenu_y + 120,
        -1,
        -1,
        EVENT_NONE,
        EVENT_SETTINGS,
        m_resources[I_TITLE_SETTINGS_NORMAL],
        m_resources[I_TITLE_SETTINGS_OVER]);
    add_child_module(m_settings);

    // create credits button
    m_credits = new NewButton(
        mainmenu_x,
        mainmenu_y + 180,
        -1,
        -1,
        EVENT_NONE,
        EVENT_CREDITS,
        m_resources[I_TITLE_CREDITS_NORMAL],
        m_resources[I_TITLE_CREDITS_OVER]);
    add_child_module(m_credits);

    // create quit button
    m_quit = new NewButton(
        mainmenu_x,
        mainmenu_y + 240,
        -1,
        -1,
        EVENT_NONE,
        EVENT_QUIT_GAME,
        m_resources[I_TITLE_QUIT_NORMAL],
        m_resources[I_TITLE_QUIT_OVER]);
    add_child_module(m_quit);
}

ModuleTitleScreen::~ModuleTitleScreen() {}

bool
ModuleTitleScreen::on_init() {

    // game-time frozen in this module. Call is necessary since there are
    // multiple paths to this module in the game:
    g_game->SetTimePaused(true);

    return true;
}

bool
ModuleTitleScreen::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);
    al_draw_scaled_bitmap(
        m_resources[I_TITLE_BACKGROUND],
        0,
        0,
        al_get_bitmap_width(m_resources[I_TITLE_BACKGROUND]),
        al_get_bitmap_height(m_resources[I_TITLE_BACKGROUND]),
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0);

    return true;
}

bool
ModuleTitleScreen::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    ALLEGRO_EVENT e = {.type = EVENT_NONE};
    switch (event->keycode) {
    case ALLEGRO_KEY_N:
        e.type = EVENT_NEW_GAME;
        break;
    case ALLEGRO_KEY_L:
        e.type = EVENT_LOAD_GAME;
        break;
    case ALLEGRO_KEY_S:
        e.type = EVENT_SETTINGS;
        break;
    case ALLEGRO_KEY_C:
        e.type = EVENT_CREDITS;
        break;
    case ALLEGRO_KEY_ESCAPE:
    case ALLEGRO_KEY_Q:
        e.type = EVENT_QUIT_GAME;
        break;
    default:
        break;
    }
    if (e.type != EVENT_NONE) {
        g_game->broadcast_event(&e);
        return false;
    } else {
        return true;
    }
}

bool
ModuleTitleScreen::on_event(ALLEGRO_EVENT *event) {
    switch (event->type) {
    case EVENT_NEW_GAME:
        g_game->LoadModule(MODULE_CAPTAINCREATION);
        return false;
    case EVENT_LOAD_GAME:
        g_game->LoadModule(MODULE_CAPTAINSLOUNGE);
        return false;
    case EVENT_SETTINGS:
        g_game->LoadModule(MODULE_SETTINGS);
        return false;
    case EVENT_CREDITS:
        g_game->LoadModule(MODULE_CREDITS);
        return false;
    case EVENT_QUIT_GAME:
        g_game->shutdown();
        return false;
    }
    return true;
}

bool
ModuleTitleScreen::on_close() {
    return true;
}
