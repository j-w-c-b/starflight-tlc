/*
        STARFLIGHT - THE LOST COLONY
        ModuleMessageGUI.cpp
        Author:
        Date:
*/

#include <allegro5/allegro.h>

#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "ModuleMessageGUI.h"
#include "Script.h"
#include "Util.h"
#include "messagegui_resources.h"

using namespace std;
using namespace messagegui;

int gmx, gmy, gmw, gmh, gsx, gsy;

ALLEGRO_DEBUG_CHANNEL("ModuleMessageGUI")

ModuleMessageGUI::ModuleMessageGUI() : Module() {}
ModuleMessageGUI::~ModuleMessageGUI() {}

bool
ModuleMessageGUI::on_init() {
    gmx = static_cast<int>(g_game->getGlobalNumber("GUI_MESSAGE_POS_X"));
    gmy = static_cast<int>(g_game->getGlobalNumber("GUI_MESSAGE_POS_Y"));
    gmw = static_cast<int>(g_game->getGlobalNumber("GUI_MESSAGE_WIDTH"));
    gmh = static_cast<int>(g_game->getGlobalNumber("GUI_MESSAGE_HEIGHT"));
    gsx = static_cast<int>(g_game->getGlobalNumber("GUI_SOCKET_POS_X"));
    gsy = static_cast<int>(g_game->getGlobalNumber("GUI_SOCKET_POS_Y"));

    return true;
}

bool
ModuleMessageGUI::on_close() {
    return true;
}

bool
ModuleMessageGUI::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);

    // draw message gui
    al_draw_bitmap(images[I_GUI_MESSAGEWINDOW].get(), gmx, gmy, 0);

    // draw socket gui
    al_draw_bitmap(images[I_GUI_SOCKET].get(), gsx, gsy, 0);

    // print stardate
    Stardate date = g_game->gameState->stardate;
    int hour = date.GetHour();
    int day = date.GetDay();
    int month = date.GetMonth();
    int year = date.GetYear();
    string datestr = Util::ToString(year) + "-" + Util::ToString(month, 2) + "-"
                     + Util::ToString(day, 2) + " " + Util::ToString(hour % 12);
    if (hour < 12)
        datestr += " AM";
    else
        datestr += " PM";
    g_game->Print22(target, gsx + 140, gsy + 24, datestr, STEEL);

    return true;
}
