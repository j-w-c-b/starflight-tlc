/*
        STARFLIGHT - THE LOST COLONY
        ModuleTitleScreen.cpp
        Author: J.Harbour
        Date: Dec,2007

    menu: 311,461
*/

#include <allegro5/allegro.h>

#include "Button.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "ModuleTitleScreen.h"
#include "Util.h"
#include "titlescreen_resources.h"

using namespace std;
using namespace titlescreen;

ALLEGRO_DEBUG_CHANNEL("ModuleTitleScreen")

ModuleTitleScreen::ModuleTitleScreen()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT) {}

bool
ModuleTitleScreen::on_init() {
    const int mainmenu_x = 392;
    const int mainmenu_y = 450;

    // game-time frozen in this module. Call is necessary since there are
    // multiple paths to this module in the game:
    g_game->SetTimePaused(true);

    auto background = make_shared<Bitmap>(images[I_TITLE_BACKGROUND]);
    add_child_module(background);

    auto title = make_shared<Bitmap>(images[I_TITLE_NORMAL], 0, 100);
    add_child_module(title);

    // create new game button
    auto new_game = make_shared<Button>(
        mainmenu_x,
        mainmenu_y,
        -1,
        -1,
        EVENT_NONE,
        EVENT_NEW_GAME,
        images[I_TITLE_NEWGAME_NORMAL],
        images[I_TITLE_NEWGAME_OVER]);
    add_child_module(new_game);

    // create load button
    auto load_game = make_shared<Button>(
        mainmenu_x,
        mainmenu_y + 60,
        -1,
        -1,
        EVENT_NONE,
        EVENT_LOAD_GAME,
        images[I_TITLE_LOADGAME_NORMAL],
        images[I_TITLE_LOADGAME_OVER]);
    add_child_module(load_game);

    // create settings button
    auto settings = make_shared<Button>(
        mainmenu_x,
        mainmenu_y + 120,
        -1,
        -1,
        EVENT_NONE,
        EVENT_SETTINGS,
        images[I_TITLE_SETTINGS_NORMAL],
        images[I_TITLE_SETTINGS_OVER]);
    add_child_module(settings);

    // create credits button
    auto credits = make_shared<Button>(
        mainmenu_x,
        mainmenu_y + 180,
        -1,
        -1,
        EVENT_NONE,
        EVENT_CREDITS,
        images[I_TITLE_CREDITS_NORMAL],
        images[I_TITLE_CREDITS_OVER]);
    add_child_module(credits);

    // create quit button
    auto quit = make_shared<Button>(
        mainmenu_x,
        mainmenu_y + 240,
        -1,
        -1,
        EVENT_NONE,
        EVENT_QUIT_GAME,
        images[I_TITLE_QUIT_NORMAL],
        images[I_TITLE_QUIT_OVER]);
    add_child_module(quit);

    return true;
}

bool
ModuleTitleScreen::on_close() {
    clear_child_modules();

    return true;
}

bool
ModuleTitleScreen::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    ALLEGRO_EVENT e = make_event(EVENT_NONE);
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
