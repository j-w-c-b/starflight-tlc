/*
        ModuleCargoWindow.cpp
        By Dave Calkins
        Extensive cleanup by Vincent Cappe

        Changelog:
        *	2009/10/08: vcap: Removed old changelog since it is not
   meaningful anymore.

*/

#include <sstream>
#include <string>

#include "AudioSystem.h"
#include "Button.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "Label.h"
#include "ModeMgr.h"
#include "ModuleCargoWindow.h"
#include "ScrolledModule.h"
#include "Util.h"
#include "cargohold_resources.h"

using namespace std;
using namespace cargohold;

// gui elements positioning, fonts settings...
#define PLAYERLIST_X 108
#define PLAYERLIST_Y 52
#define PLAYERLIST_WIDTH 294
#define PLAYERLIST_HEIGHT 335

#define CARGO_JETTISON_X 315
#define CARGO_JETTISON_Y 390
#define CARGO_EXIT_X 115
#define CARGO_EXIT_Y 390

#define CARGO_SPACESTATUS_X 230
#define CARGO_SPACESTATUS_Y 390
#define CARGO_SPACESTATUS_HEIGHT 31
#define CARGO_SPACESTATUS_WIDTH 87

ALLEGRO_DEBUG_CHANNEL("ModuleCargoWindow")

ModuleCargoWindow::ModuleCargoWindow() : Module(0, 10, 440, 454) {}

ModuleCargoWindow::~ModuleCargoWindow() {}

bool
ModuleCargoWindow::on_init() {
    ALLEGRO_DEBUG("  ModuleCargoWindow: initializing...\n");

    m_viewer = make_shared<SlidingModule<Bitmap>>(
        SLIDE_FROM_LEFT,
        EVENT_NONE,
        0.6,
        images[I_GUI_VIEWER],
        get_x(),
        get_y());
    add_child_module(m_viewer);

    // jettison button
    m_jettisonButton = make_shared<TextButton>(
        "JETTISON",
        g_game->font12,
        LTGREEN,
        ALLEGRO_ALIGN_CENTER,
        CARGO_JETTISON_X,
        CARGO_JETTISON_Y,
        EVENT_NONE,
        EVENT_CARGO_JETTISON,
        images[I_CARGO_BTN],
        images[I_CARGO_BTN_MO]);
    m_jettisonButton->set_active(false);
    m_viewer->add_child_module(m_jettisonButton);

    // cargo capacity indicator
    m_space_status = make_shared<Label>(
        "",
        CARGO_SPACESTATUS_X,
        m_jettisonButton->get_y()
            + (m_jettisonButton->get_height()
               - al_get_font_line_height(g_game->font12.get()))
                  / 2,
        CARGO_SPACESTATUS_WIDTH,
        CARGO_SPACESTATUS_HEIGHT,
        false,
        0,
        g_game->font12,
        LTGREEN);
    m_viewer->add_child_module(m_space_status);

    m_player_items = make_shared<ScrolledItemStackButtonList>(
        PLAYERLIST_X,
        PLAYERLIST_Y,
        PLAYERLIST_WIDTH,
        PLAYERLIST_HEIGHT,
        al_get_font_line_height(g_game->font18.get()),
        al_map_rgb(64, 64, 64),
        al_map_rgb(32, 32, 32),
        al_map_rgb(48, 48, 128),
        al_map_rgb(96, 96, 128),
        al_map_rgb(32, 32, 32),
        g_game->gameState->m_items,
        IT_INVALID,
        EVENT_CARGO_LIST_CLICK,
        g_game->font18,
        WHITE,
        al_map_rgb(32, 32, 32),
        al_map_rgb(48, 48, 128),
        al_map_rgb(96, 96, 128));
    m_viewer->add_child_module(m_player_items);

    update_inventory();
    m_is_active = false;

    ALLEGRO_DEBUG("  ModuleCargoWindow: initialized\n");

    return true;
}

void
ModuleCargoWindow::update_inventory() {
    if (m_player_items) {
        m_player_items->update_items();
    }

    if (m_space_status) {
        m_space_status->set_text(
            to_string(g_game->gameState->m_ship.getOccupiedSpace()) + "/"
            + to_string(g_game->gameState->m_ship.getTotalSpace()));
    }
}

bool
ModuleCargoWindow::on_event(ALLEGRO_EVENT *event) {
    EventType ev = static_cast<EventType>(event->type);

    switch (ev) {
    // player clicked on an item line
    case EVENT_CARGO_LIST_CLICK:
        g_game->audioSystem->Play(samples[S_BUTTONCLICK]);

        m_jettisonButton->set_active(m_player_items->get_selected() >= 0);
        return false;

    // player clicked on the jettison button
    case EVENT_CARGO_JETTISON:
        {
            g_game->audioSystem->Play(samples[S_BUTTONCLICK]);
            m_jettisonButton->set_active(false);

            int selected_item = m_player_items->get_selected();
            const Item *item = g_game->dataMgr->GetItemByID(selected_item);
            int num_items = m_player_items->get_count(selected_item);

            m_player_items->clear_selected();
            g_game->gameState->m_items.RemoveItems(item->id, num_items);

            // notify everybody (include ourselves) that the inventory
            // changed
            ALLEGRO_EVENT e = {
                .type = static_cast<unsigned int>(EVENT_CARGO_UPDATE)};
            g_game->broadcast_event(&e);
            update_inventory();
            return false;
        }

    // inventory changed due to either the jettison button or an external
    // factor
    case EVENT_CARGO_UPDATE:
        update_inventory();
        return true;
    case EVENT_CAPTAIN_CARGO:
        m_viewer->toggle();
        if (!m_is_active) {
            m_is_active = true;
        }
        return true;
    case EVENT_CAPTAIN_LAUNCH:
        [[fallthrough]];
    case EVENT_CAPTAIN_DESCEND:
        [[fallthrough]];
    case EVENT_CAPTAIN_QUESTLOG:
        if (!m_viewer->is_closed()) {
            m_is_active = false;
            m_viewer->toggle();
        }
        return true;
    default:
        return true;
    }
}

bool
ModuleCargoWindow::on_update() {
    auto current_officer = g_game->gameState->getCurrentSelectedOfficer();

    if (current_officer != OFFICER_CAPTAIN) {
        if (!m_viewer->is_closed()) {
            m_viewer->toggle();
        }
        m_is_active = false;
    } else if (m_is_active && !m_viewer->is_visible()) {
        m_is_active = false;
    }
    return true;
}

bool
ModuleCargoWindow::on_close() {
    ALLEGRO_DEBUG("*** ModuleCargoWindow: closing...\n");

    remove_child_module(m_viewer);
    m_viewer = nullptr;
    m_space_status = nullptr;
    m_player_items = nullptr;
    m_jettisonButton = nullptr;

    ALLEGRO_DEBUG("*** ModuleCargoWindow: closed\n");
    return true;
}
