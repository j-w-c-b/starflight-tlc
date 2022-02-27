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

bool
ModuleMessageGUI::on_init() {
    gmx = static_cast<int>(g_game->getGlobalNumber("GUI_MESSAGE_POS_X"));
    gmy = static_cast<int>(g_game->getGlobalNumber("GUI_MESSAGE_POS_Y"));
    gmw = static_cast<int>(g_game->getGlobalNumber("GUI_MESSAGE_WIDTH"));
    gmh = static_cast<int>(g_game->getGlobalNumber("GUI_MESSAGE_HEIGHT"));
    gsx = static_cast<int>(g_game->getGlobalNumber("GUI_SOCKET_POS_X"));
    gsy = static_cast<int>(g_game->getGlobalNumber("GUI_SOCKET_POS_Y"));

    m_messagewindow =
        make_shared<Bitmap>(images[I_GUI_MESSAGEWINDOW], gmx, gmy);
    add_child_module(m_messagewindow);

    m_gui_socket = make_shared<Bitmap>(images[I_GUI_SOCKET], gsx, gsy);
    add_child_module(m_gui_socket);

    m_stardate = make_shared<Label>(
        "",
        gsx + 140,
        gsy + 24,
        gmw,
        gmh,
        false,
        ALLEGRO_ALIGN_LEFT,
        g_game->font22,
        STEEL);
    add_child_module(m_stardate);

    if (!m_text) {
        m_text = make_shared<ScrolledModule<RichTextLabel>>(
            gmx + 38,
            gmy + 18,
            gmw - 54,
            gmh - 32,
            al_get_font_line_height(g_game->font20.get()),
            al_map_rgb(64, 64, 64),
            al_map_rgb(32, 32, 32),
            al_map_rgb(48, 48, 128),
            al_map_rgb(96, 96, 128),
            al_map_rgb(32, 32, 32),
            true,
            ALLEGRO_ALIGN_LEFT,
            g_game->font20,
            WHITE,
            al_map_rgb(32, 32, 32));
        add_child_module(m_text);
    }

    return true;
}

bool
ModuleMessageGUI::on_close() {
    remove_child_module(m_messagewindow);
    m_messagewindow = nullptr;

    remove_child_module(m_gui_socket);
    m_gui_socket = nullptr;

    remove_child_module(m_stardate);
    m_stardate = nullptr;

    clear_printout();

    return true;
}

bool
ModuleMessageGUI::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);

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
    m_stardate->set_text(datestr);

    return true;
}

void
ModuleMessageGUI::clear_printout() {
    m_text->set_text(vector<RichText>({}));
}

void
ModuleMessageGUI::printout(
    const std::string &text,
    ALLEGRO_COLOR color,
    long delay) {
    printout(RichText{text, color}, delay);
}

void
ModuleMessageGUI::printout(const RichText &text, long delay) {
    bool found = false;

    TimedText message = {text, g_game->globalTimer.getTimer() + delay};

    // do we care about repeating messages? -1 = one-time only, 0 = always, n =
    // ms delay
    if (delay == 0) {
        // just print it without remembering the message
        m_text->add_line({message.text});
        m_text->scroll_to_bottom();
        found = true;
    } else {
        // scan timestamps of printed messages to see if ready to print again
        for (auto &mess : m_messages) {
            // text found in vector?
            if (mess.text.m_text == message.text.m_text) {
                found = true;

                // print-once code
                if (delay == -1) {
                    mess.delay = -1;
                } else if (g_game->globalTimer.getTimer() > mess.delay) {
                    // ready to print again?
                    m_text->add_line(message.text);
                    m_text->scroll_to_bottom();
                    // reset delay timer
                    mess.delay = g_game->globalTimer.getTimer() + delay;
                }
                break;
            }
        }
    }

    // text not found, add to vector and print
    if (!found) {
        if (delay == -1) {
            message.delay = -1;
        }
        m_messages.push_back(message);
        m_text->add_line(message.text);
        m_text->scroll_to_bottom();
    }
}
