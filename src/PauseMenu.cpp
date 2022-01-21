#include "PauseMenu.h"
#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"

using namespace std;

ALLEGRO_BITMAP *PauseMenu::bg = NULL;
Button *PauseMenu::button1 = NULL;
Button *PauseMenu::button2 = NULL;
Button *PauseMenu::button3 = NULL;
Button *PauseMenu::button4 = NULL;

// ctors
PauseMenu::PauseMenu() {
    display = false;
    enabled = false;

    if (bg == NULL)
        bg = al_load_bitmap("data/pausemenu/pausemenu_bg.tga");

    // get location of dialog
    x = 512 - al_get_bitmap_width(bg) / 2;
    y = 384 - al_get_bitmap_height(bg) / 2;

    // SAVE GAME button
    if (button1 == NULL) {
        button1 = new Button("data/pausemenu/button.tga",
                             "data/pausemenu/button_over.tga",
                             "data/pausemenu/button_dis.tga",
                             x + 70,
                             y + 45,
                             EVENT_MOUSEOVER,
                             EVENT_SAVE_GAME,
                             g_game->font20,
                             "SAVE GAME",
                             WHITE);
    }

    // LOAD GAME button
    if (button2 == NULL) {
        button2 = new Button("data/pausemenu/button.tga",
                             "data/pausemenu/button_over.tga",
                             "data/pausemenu/button_dis.tga",
                             x + 70,
                             y + 130,
                             EVENT_MOUSEOVER,
                             EVENT_LOAD_GAME,
                             g_game->font20,
                             "LOAD GAME",
                             WHITE);
    }

    // QUIT GAME button
    if (button3 == NULL) {
        button3 = new Button("data/pausemenu/button.tga",
                             "data/pausemenu/button_over.tga",
                             "data/pausemenu/button_dis.tga",
                             x + 70,
                             y + 215,
                             EVENT_MOUSEOVER,
                             EVENT_QUIT_GAME,
                             g_game->font20,
                             "QUIT GAME",
                             WHITE);
    }

    // RETURN button
    if (button4 == NULL) {
        button4 = new Button("data/pausemenu/button.tga",
                             "data/pausemenu/button_over.tga",
                             "data/pausemenu/button_dis.tga",
                             x + 70,
                             y + 300,
                             EVENT_MOUSEOVER,
                             EVENT_CLOSE,
                             g_game->font20,
                             "RETURN",
                             WHITE);
    }
}

PauseMenu::~PauseMenu() {
    display = false;
    if (bg != NULL)
        al_destroy_bitmap(bg);
    if (button1 != NULL)
        delete button1;
    if (button2 != NULL)
        delete button2;
    if (button3 != NULL)
        delete button3;
    if (button4 != NULL)
        delete button4;
}

// other funcs
bool
PauseMenu::OnMouseMove(int x, int y) {
    if (!enabled)
        return false;
    bool result = false;
    result = button1->OnMouseMove(x, y);
    if (!result)
        result = button2->OnMouseMove(x, y);
    if (!result)
        result = button3->OnMouseMove(x, y);
    if (!result)
        result = button4->OnMouseMove(x, y);
    return result;
}
bool
PauseMenu::OnMouseReleased(int button, int x, int y) {
    if (!enabled)
        return false;
    bool result = false;
    result = button1->OnMouseReleased(button, x, y);
    if (!result)
        result = button2->OnMouseReleased(button, x, y);
    if (!result)
        result = button3->OnMouseReleased(button, x, y);
    if (!result)
        result = button4->OnMouseReleased(button, x, y);
    return result;
}
void
PauseMenu::Draw() {
    if (!enabled)
        return;
    al_set_target_bitmap(g_game->GetBackBuffer());

    // draw background
    al_draw_bitmap(bg, x, y, 0);

    // save/load only available in certain modules
    string module = g_game->gameState->getCurrentModule();
    if (module == MODULE_HYPERSPACE || module == MODULE_INTERPLANETARY ||
        module == MODULE_ORBIT ||
        // module == MODULE_ENCOUNTER ||
        module == MODULE_STARPORT) {
        button1->SetEnabled(true);
        button2->SetEnabled(true);
    } else {
        button1->SetEnabled(false);
        button2->SetEnabled(false);
    }

    // let buttons run
    if (button1)
        button1->Run(g_game->GetBackBuffer());
    if (button2)
        button2->Run(g_game->GetBackBuffer());
    if (button3)
        button3->Run(g_game->GetBackBuffer());
    if (button4)
        button4->Run(g_game->GetBackBuffer());
}
