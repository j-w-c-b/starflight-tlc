#include "PauseMenu.h"
#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "pausemenu_resources.h"

using namespace std;
using namespace pausemenu_resources;

static const int PAUSEMENU_W = 400;
static const int PAUSEMENU_H = 400;
static const int PAUSEMENU_X = (SCREEN_WIDTH - PAUSEMENU_W) / 2;
static const int PAUSEMENU_Y = (SCREEN_HEIGHT - PAUSEMENU_H) / 2;
static const int PAUSEMENU_BUTTON_W = 264;
static const int PAUSEMENU_BUTTON_H = 74;

PauseMenu::PauseMenu()
    : Module(PAUSEMENU_X, PAUSEMENU_Y, PAUSEMENU_W, PAUSEMENU_H),
      m_resources(PAUSEMENU_IMAGES), m_background(make_shared<Bitmap>(
                                         m_resources[I_PAUSEMENU_BG],
                                         PAUSEMENU_X,
                                         PAUSEMENU_Y)),
      m_save_button(make_shared<TextButton>(
          make_shared<Label>(
              "SAVE GAME",
              m_x + 70,
              m_y + 45
                  + (PAUSEMENU_BUTTON_H
                     - al_get_font_line_height(g_game->font20))
                        / 2,
              PAUSEMENU_BUTTON_W,
              PAUSEMENU_BUTTON_H,
              false,
              ALLEGRO_ALIGN_CENTER,
              g_game->font20,
              WHITE),
          m_x + 70,
          m_y + 45,
          PAUSEMENU_BUTTON_W,
          PAUSEMENU_BUTTON_H,
          EVENT_NONE,
          EVENT_SAVE_GAME,
          m_resources[I_BUTTON],
          m_resources[I_BUTTON_OVER],
          m_resources[I_BUTTON_DIS])),
      m_load_button(make_shared<TextButton>(
          make_shared<Label>(
              "LOAD GAME",
              m_x + 70,
              m_y + 130
                  + (PAUSEMENU_BUTTON_H
                     - al_get_font_line_height(g_game->font20))
                        / 2,
              PAUSEMENU_BUTTON_W,
              PAUSEMENU_BUTTON_H,
              false,
              ALLEGRO_ALIGN_CENTER,
              g_game->font20,
              WHITE),
          m_x + 70,
          m_y + 130,
          PAUSEMENU_BUTTON_W,
          PAUSEMENU_BUTTON_H,
          EVENT_NONE,
          EVENT_LOAD_GAME,
          m_resources[I_BUTTON],
          m_resources[I_BUTTON_OVER],
          m_resources[I_BUTTON_DIS])),
      m_quit_button(make_shared<TextButton>(
          make_shared<Label>(
              "QUIT GAME",
              m_x + 70,
              m_y + 215
                  + (PAUSEMENU_BUTTON_H
                     - al_get_font_line_height(g_game->font20))
                        / 2,
              PAUSEMENU_BUTTON_W,
              PAUSEMENU_BUTTON_H,
              false,
              ALLEGRO_ALIGN_CENTER,
              g_game->font20,
              WHITE),
          m_x + 70,
          m_y + 215,
          PAUSEMENU_BUTTON_W,
          PAUSEMENU_BUTTON_H,
          EVENT_NONE,
          EVENT_QUIT_GAME,
          m_resources[I_BUTTON],
          m_resources[I_BUTTON_OVER],
          m_resources[I_BUTTON_DIS])),
      m_return_button(make_shared<TextButton>(
          make_shared<Label>(
              "RETURN",
              m_x + 70,
              m_y + 300
                  + (PAUSEMENU_BUTTON_H
                     - al_get_font_line_height(g_game->font20))
                        / 2,
              PAUSEMENU_BUTTON_W,
              PAUSEMENU_BUTTON_H,
              false,
              ALLEGRO_ALIGN_CENTER,
              g_game->font20,
              WHITE),
          m_x + 70,
          m_y + 300,
          PAUSEMENU_BUTTON_W,
          PAUSEMENU_BUTTON_H,
          EVENT_NONE,
          EVENT_CLOSE,
          m_resources[I_BUTTON],
          m_resources[I_BUTTON_OVER],
          m_resources[I_BUTTON_DIS])),
      m_enabled(false) {
    add_child_module(m_background);
    add_child_module(m_save_button);
    add_child_module(m_load_button);
    add_child_module(m_quit_button);
    add_child_module(m_return_button);
}

void
PauseMenu::set_enabled(bool enabled) {
    m_enabled = enabled;

    string module_name = g_game->gameState->getCurrentModule();

    if (enabled
        && (module_name == MODULE_HYPERSPACE
            || module_name == MODULE_INTERPLANETARY
            || module_name == MODULE_ORBIT || module_name == MODULE_STARPORT)) {
        m_save_button->set_active(true);
        m_load_button->set_active(true);
    } else {
        m_save_button->set_active(false);
        m_load_button->set_active(false);
    }
}
// vi: ft=cpp
