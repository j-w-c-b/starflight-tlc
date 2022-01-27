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
    m_rotationAngle = 0;
    btnTitle = NULL;
    btnNewGame = NULL;
    btnLoadGame = NULL;
    btnSettings = NULL;
    btnCredits = NULL;
    btnQuit = NULL;
    title_mode = 690;
}

ModuleTitleScreen::~ModuleTitleScreen() {}

bool
ModuleTitleScreen::on_init() {
    const int mainmenu_x = 392;
    const int mainmenu_y = 450;

    // game-time frozen in this module. Call is necessary since there are
    // multiple paths to this module in the game:
    g_game->SetTimePaused(true);

    // create title button
    btnTitle = new Button(
        m_resources[I_TITLE_NORMAL],
        m_resources[I_TITLE_OVER],
        NULL,
        0,
        100,
        0,
        0);

    // create new game button
    btnNewGame = new Button(
        m_resources[I_TITLE_NEWGAME_NORMAL],
        m_resources[I_TITLE_NEWGAME_OVER],
        NULL,
        mainmenu_x,
        mainmenu_y,
        0,
        EVENT_NEW_GAME);

    // create load button
    btnLoadGame = new Button(
        m_resources[I_TITLE_LOADGAME_NORMAL],
        m_resources[I_TITLE_LOADGAME_OVER],
        NULL,
        mainmenu_x,
        mainmenu_y + 60,
        0,
        EVENT_LOAD_GAME);

    // create settings button
    btnSettings = new Button(
        m_resources[I_TITLE_SETTINGS_NORMAL],
        m_resources[I_TITLE_SETTINGS_OVER],
        NULL,
        mainmenu_x,
        mainmenu_y + 120,
        0,
        EVENT_SETTINGS);

    // create credits button
    btnCredits = new Button(
        m_resources[I_TITLE_CREDITS_NORMAL],
        m_resources[I_TITLE_CREDITS_OVER],
        NULL,
        mainmenu_x,
        mainmenu_y + 180,
        0,
        EVENT_CREDITS);

    // create quit button
    btnQuit = new Button(
        m_resources[I_TITLE_QUIT_NORMAL],
        m_resources[I_TITLE_QUIT_OVER],
        NULL,
        mainmenu_x,
        mainmenu_y + 240,
        0,
        EVENT_QUIT_GAME);

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
    btnTitle->Run(target);
    btnNewGame->Run(target);
    btnLoadGame->Run(target);
    btnSettings->Run(target);
    btnCredits->Run(target);
    btnQuit->Run(target);

    switch (title_mode) {
    case 690: // welcome message if needed
        title_mode = 1;
        break;

    case EVENT_NEW_GAME: // clicked New Game
        title_mode = 1;
        g_game->LoadModule(MODULE_CAPTAINCREATION);
        return false;

    case EVENT_LOAD_GAME: // clicked Load Game
        title_mode = 1;
        g_game->LoadModule(MODULE_CAPTAINSLOUNGE);
        return false;

    case EVENT_SETTINGS: // clicked Settings
        title_mode = 1;
        g_game->LoadModule(MODULE_SETTINGS);
        return false;

    case EVENT_CREDITS: // clicked Credits
        title_mode = 1;
        g_game->LoadModule(MODULE_CREDITS);
        return false;

    case EVENT_QUIT_GAME: // QUIT GAME
        title_mode = 705;
        break;

    case 705:
        g_game->shutdown();
        break;
    }
    return true;
}

bool
ModuleTitleScreen::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    if (event->keycode == ALLEGRO_KEY_ESCAPE) {
        title_mode = EVENT_QUIT_GAME;
    }
    return true;
}

bool
ModuleTitleScreen::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    btnTitle->OnMouseMove(x, y);
    btnNewGame->OnMouseMove(x, y);
    btnLoadGame->OnMouseMove(x, y);
    btnSettings->OnMouseMove(x, y);
    btnCredits->OnMouseMove(x, y);
    btnQuit->OnMouseMove(x, y);

    return true;
}

bool
ModuleTitleScreen::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    int button = event->button - 1;
    int x = event->x;
    int y = event->y;

    if (!is_mouse_click(event)) {
        return true;
    }

    btnTitle->OnMouseReleased(button, x, y);
    btnNewGame->OnMouseReleased(button, x, y);
    btnLoadGame->OnMouseReleased(button, x, y);
    btnSettings->OnMouseReleased(button, x, y);
    btnCredits->OnMouseReleased(button, x, y);
    btnQuit->OnMouseReleased(button, x, y);

    return true;
}

bool
ModuleTitleScreen::on_event(ALLEGRO_EVENT *event) {
    switch (event->type) {
    case EVENT_NEW_GAME: // new game
        title_mode = EVENT_NEW_GAME;
        break;
    case EVENT_LOAD_GAME: // load game
        title_mode = EVENT_LOAD_GAME;
        break;
    case EVENT_SETTINGS: // settings
        title_mode = EVENT_SETTINGS;
        break;
    case EVENT_CREDITS: // credits
        title_mode = EVENT_CREDITS;
        break;
    case EVENT_QUIT_GAME: // exit game
        title_mode = EVENT_QUIT_GAME;
        break;
    }
    return true;
}

bool
ModuleTitleScreen::on_close() {
    delete btnTitle;
    delete btnNewGame;
    delete btnLoadGame;
    delete btnSettings;
    delete btnCredits;
    delete btnQuit;

    return true;
}
