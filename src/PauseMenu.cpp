#include "PauseMenu.h"
#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "pausemenu_resources.h"

using namespace std;
using namespace pausemenu_resources;

PauseMenu::PauseMenu() : m_resources(PAUSEMENU_IMAGES) {
    display = false;
    enabled = false;

    // get location of dialog
    x = (SCREEN_WIDTH - al_get_bitmap_width(m_resources[I_PAUSEMENU_BG])) / 2;
    y = (SCREEN_HEIGHT - al_get_bitmap_height(m_resources[I_PAUSEMENU_BG])) / 2;

    // SAVE GAME button
    m_save_button = new Button(
        m_resources[I_BUTTON],
        m_resources[I_BUTTON_OVER],
        m_resources[I_BUTTON_DIS],
        x + 70,
        y + 45,
        EVENT_MOUSEOVER,
        EVENT_SAVE_GAME,
        g_game->font20,
        "SAVE GAME",
        WHITE);

    // LOAD GAME button
    m_load_button = new Button(
        m_resources[I_BUTTON],
        m_resources[I_BUTTON_OVER],
        m_resources[I_BUTTON_DIS],
        x + 70,
        y + 130,
        EVENT_MOUSEOVER,
        EVENT_LOAD_GAME,
        g_game->font20,
        "LOAD GAME",
        WHITE);

    // QUIT GAME button
    m_quit_button = new Button(
        m_resources[I_BUTTON],
        m_resources[I_BUTTON_OVER],
        m_resources[I_BUTTON_DIS],
        x + 70,
        y + 215,
        EVENT_MOUSEOVER,
        EVENT_QUIT_GAME,
        g_game->font20,
        "QUIT GAME",
        WHITE);

    // RETURN button
    m_return_button = new Button(
        m_resources[I_BUTTON],
        m_resources[I_BUTTON_OVER],
        m_resources[I_BUTTON_DIS],
        x + 70,
        y + 300,
        EVENT_MOUSEOVER,
        EVENT_CLOSE,
        g_game->font20,
        "RETURN",
        WHITE);
}

PauseMenu::~PauseMenu() {
    display = false;
    if (m_save_button != NULL)
        delete m_save_button;
    if (m_load_button != NULL)
        delete m_load_button;
    if (m_quit_button != NULL)
        delete m_quit_button;
    if (m_return_button != NULL)
        delete m_return_button;
}

// other funcs
bool
PauseMenu::OnMouseMove(int x, int y) {
    if (!enabled)
        return false;
    bool result = false;
    result = m_save_button->OnMouseMove(x, y);
    if (!result)
        result = m_load_button->OnMouseMove(x, y);
    if (!result)
        result = m_quit_button->OnMouseMove(x, y);
    if (!result)
        result = m_return_button->OnMouseMove(x, y);
    return result;
}
bool
PauseMenu::OnMouseReleased(int button, int x, int y) {
    if (!enabled)
        return false;
    bool result = false;
    result = m_save_button->OnMouseReleased(button, x, y);
    if (!result)
        result = m_load_button->OnMouseReleased(button, x, y);
    if (!result)
        result = m_quit_button->OnMouseReleased(button, x, y);
    if (!result)
        result = m_return_button->OnMouseReleased(button, x, y);
    return result;
}
void
PauseMenu::Draw() {
    if (!enabled)
        return;
    al_set_target_bitmap(g_game->GetBackBuffer());

    // draw background
    al_draw_bitmap(m_resources[I_PAUSEMENU_BG], x, y, 0);

    // save/load only available in certain modules
    string module = g_game->gameState->getCurrentModule();
    if (module == MODULE_HYPERSPACE || module == MODULE_INTERPLANETARY
        || module == MODULE_ORBIT ||
        // module == MODULE_ENCOUNTER ||
        module == MODULE_STARPORT) {
        m_save_button->SetEnabled(true);
        m_load_button->SetEnabled(true);
    } else {
        m_save_button->SetEnabled(false);
        m_load_button->SetEnabled(false);
    }

    // let buttons run
    if (m_save_button)
        m_save_button->Run(g_game->GetBackBuffer());
    if (m_load_button)
        m_load_button->Run(g_game->GetBackBuffer());
    if (m_quit_button)
        m_quit_button->Run(g_game->GetBackBuffer());
    if (m_return_button)
        m_return_button->Run(g_game->GetBackBuffer());
}
