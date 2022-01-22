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

ModuleTitleScreen::ModuleTitleScreen() : m_resources(TITLESCREEN_IMAGES) {
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
ModuleTitleScreen::Init() {
    const int mainmenu_x = 392;
    const int mainmenu_y = 450;

    // game-time frozen in this module. Call is necessary since there are
    // multiple paths to this module in the game:
    g_game->SetTimePaused(true);

    // create title button
    btnTitle = new Button(m_resources[I_TITLE_NORMAL],
                          m_resources[I_TITLE_OVER],
                          NULL,
                          0,
                          100,
                          0,
                          0);

    // create new game button
    btnNewGame = new Button(m_resources[I_TITLE_NEWGAME_NORMAL],
                            m_resources[I_TITLE_NEWGAME_OVER],
                            NULL,
                            mainmenu_x,
                            mainmenu_y,
                            0,
                            700);

    // create load button
    btnLoadGame = new Button(m_resources[I_TITLE_LOADGAME_NORMAL],
                             m_resources[I_TITLE_LOADGAME_OVER],
                             NULL,
                             mainmenu_x,
                             mainmenu_y + 60,
                             0,
                             701);

    // create settings button
    btnSettings = new Button(m_resources[I_TITLE_SETTINGS_NORMAL],
                             m_resources[I_TITLE_SETTINGS_OVER],
                             NULL,
                             mainmenu_x,
                             mainmenu_y + 120,
                             0,
                             702);

    // create credits button
    btnCredits = new Button(m_resources[I_TITLE_CREDITS_NORMAL],
                            m_resources[I_TITLE_CREDITS_OVER],
                            NULL,
                            mainmenu_x,
                            mainmenu_y + 180,
                            0,
                            703);

    // create quit button
    btnQuit = new Button(m_resources[I_TITLE_QUIT_NORMAL],
                         m_resources[I_TITLE_QUIT_OVER],
                         NULL,
                         mainmenu_x,
                         mainmenu_y + 240,
                         0,
                         704);

    return true;
}

void
ModuleTitleScreen::Update() {}

void
ModuleTitleScreen::Draw() {
    al_set_target_bitmap(g_game->GetBackBuffer());
    al_draw_scaled_bitmap(m_resources[I_TITLE_BACKGROUND],
                          0,
                          0,
                          al_get_bitmap_width(m_resources[I_TITLE_BACKGROUND]),
                          al_get_bitmap_height(m_resources[I_TITLE_BACKGROUND]),
                          0,
                          0,
                          SCREEN_WIDTH,
                          SCREEN_HEIGHT,
                          0);
    btnTitle->Run(g_game->GetBackBuffer());
    btnNewGame->Run(g_game->GetBackBuffer());
    btnLoadGame->Run(g_game->GetBackBuffer());
    btnSettings->Run(g_game->GetBackBuffer());
    btnCredits->Run(g_game->GetBackBuffer());
    btnQuit->Run(g_game->GetBackBuffer());

    switch (title_mode) {
    case 690: // welcome message if needed
        title_mode = 1;
        break;

    case 700: // clicked New Game
        title_mode = 1;
        g_game->LoadModule(MODULE_CAPTAINCREATION);
        return;
        break;

    case 701: // clicked Load Game
        title_mode = 1;
        g_game->LoadModule(MODULE_CAPTAINSLOUNGE);
        return;
        break;

    case 702: // clicked Settings
        title_mode = 1;
        g_game->LoadModule(MODULE_SETTINGS);
        return;
        break;

    case 703: // clicked Credits
        title_mode = 1;
        g_game->LoadModule(MODULE_CREDITS);
        return;
        break;

    case 704: // QUIT GAME
        title_mode = 705;
        break;

    case 705:
        g_game->shutdown();
        break;
    }
}

void
ModuleTitleScreen::OnKeyReleased(int keyCode) {
    if (keyCode == ALLEGRO_KEY_ESCAPE)
        title_mode = 704;
}

void
ModuleTitleScreen::OnMouseMove(int x, int y) {
    btnTitle->OnMouseMove(x, y);
    btnNewGame->OnMouseMove(x, y);
    btnLoadGame->OnMouseMove(x, y);
    btnSettings->OnMouseMove(x, y);
    btnCredits->OnMouseMove(x, y);
    btnQuit->OnMouseMove(x, y);
}

void
ModuleTitleScreen::OnMouseReleased(int button, int x, int y) {
    btnTitle->OnMouseReleased(button, x, y);
    btnNewGame->OnMouseReleased(button, x, y);
    btnLoadGame->OnMouseReleased(button, x, y);
    btnSettings->OnMouseReleased(button, x, y);
    btnCredits->OnMouseReleased(button, x, y);
    btnQuit->OnMouseReleased(button, x, y);
}

void
ModuleTitleScreen::OnEvent(Event *event) {
    Module::OnEvent(event);

    switch (event->getEventType()) {
    case 700: // new game
        title_mode = 700;
        break;
    case 701: // load game
        title_mode = 701;
        break;
    case 702: // settings
        title_mode = 702;
        break;
    case 703: // credits
        title_mode = 703;
        break;
    case 704: // exit game
        title_mode = 704;
        break;
    }
}

void
ModuleTitleScreen::Close() {
    delete btnTitle;
    delete btnNewGame;
    delete btnLoadGame;
    delete btnSettings;
    delete btnCredits;
    delete btnQuit;
}
