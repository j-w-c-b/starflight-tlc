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
#include "ScrollBox.h"
#include "Util.h"
#include "cargohold_resources.h"

using namespace std;
using namespace cargohold_resources;

// gui elements positioning, fonts settings...
#define PLAYERLIST_X 108
#define PLAYERLIST_Y 52
#define PLAYERLIST_WIDTH 294
#define PLAYERLIST_HEIGHT 335
#define PLAYERLIST_NUMITEMS_WIDTH 50
#define PLAYERLIST_VALUE_WIDTH 80

#define CARGO_LIST_FONT_HEIGHT 18
#define CARGO_BUTTONS_FONT_HEIGHT 20
#define CARGO_JETTISON_X 315
#define CARGO_JETTISON_Y 390
#define CARGO_EXIT_X 115
#define CARGO_EXIT_Y 390

#define CARGO_SPACESTATUS_X 230
#define CARGO_SPACESTATUS_Y 390
#define CARGO_SPACESTATUS_HEIGHT 31
#define CARGO_SPACESTATUS_WIDTH 87

// events we generate (and handle)
#define EVENT_CARGO_LIST_CLICK 501 /* player clicked on an item line */
#define EVENT_CARGO_JETTISON 502   /* player clicked on the jettison button */

ALLEGRO_DEBUG_CHANNEL("ModuleCargoWindow")

ModuleCargoWindow::ModuleCargoWindow()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), m_resources(CARGOHOLD_IMAGES) {
    // from data/globals.lua (left=-440 right=-40 speed=12)
    gui_viewer_left = (int)g_game->getGlobalNumber("GUI_VIEWER_LEFT");
    gui_viewer_right = (int)g_game->getGlobalNumber("GUI_VIEWER_RIGHT");
    gui_viewer_speed = (int)g_game->getGlobalNumber("GUI_VIEWER_SPEED");

    initialized = false;

    img_viewer = nullptr;

    m_items = nullptr;
    m_playerItemsFiltered = nullptr;
    m_playerList = nullptr;
    m_playerListNumItems = nullptr;
    m_playerListValue = nullptr;

    m_jettisonButton = nullptr;
    m_sndButtonClick = nullptr;

    // cargo capacity indicator
    m_space_status = make_shared<Label>(
        "",
        CARGO_SPACESTATUS_X,
        CARGO_SPACESTATUS_Y,
        CARGO_SPACESTATUS_WIDTH,
        CARGO_SPACESTATUS_HEIGHT,
        false,
        0,
        g_game->font20,
        LTGREEN);
    m_space_status->set_active(false);
    add_child_module(m_space_status);
}

ModuleCargoWindow::~ModuleCargoWindow() {}

bool
ModuleCargoWindow::on_init() {
    ALLEGRO_DEBUG("  ModuleCargoWindow: initializing...\n");

    // load the window "skin"
    if (!m_resources.load()) {
        g_game->message("CargoWindow: Error loading resources");
        return false;
    }

    img_viewer = m_resources[I_GUI_VIEWER];

    // jettison button
    ALLEGRO_BITMAP *btnNorm, *btnOver;
    btnNorm = m_resources[I_CARGO_BTN];
    btnOver = m_resources[I_CARGO_BTN_MO];

    m_jettisonButton = make_shared<Button>(
        btnNorm,
        btnOver,
        nullptr,
        CARGO_JETTISON_X,
        CARGO_JETTISON_Y,
        EVENT_NONE,
        EVENT_CARGO_JETTISON,
        g_game->font20,
        "JETTISON",
        LTGREEN,
        "",
        true,
        false);

    if (!m_jettisonButton || !m_jettisonButton->IsInitialized())
        return false;

    maxSpace = g_game->gameState->m_ship.getTotalSpace();

    // the items list
    // NOTE: these three scrollboxes are linked. what this means among other
    // things
    //  is that calling the OnMouse* of one will call the other two. in our case
    //  we are only interested in getting one and only one
    //  EVENT_CARGO_LIST_CLICK when the user click on an item line, hence the
    //  need to pass EVENT_NONE for the other two.
    m_playerList = new ScrollBox::ScrollBox(
        g_game->font20,
        ScrollBox::SB_LIST,
        0,
        0,
        PLAYERLIST_WIDTH,
        PLAYERLIST_HEIGHT,
        EVENT_CARGO_LIST_CLICK);
    if (m_playerList == nullptr)
        return false;

    m_playerListNumItems = new ScrollBox::ScrollBox(
        g_game->font20,
        ScrollBox::SB_LIST,
        0,
        0,
        PLAYERLIST_VALUE_WIDTH + PLAYERLIST_NUMITEMS_WIDTH,
        PLAYERLIST_HEIGHT,
        EVENT_NONE);
    if (m_playerListNumItems == nullptr)
        return false;

    m_playerListValue = new ScrollBox::ScrollBox(
        g_game->font20,
        ScrollBox::SB_LIST,
        0,
        0,
        PLAYERLIST_VALUE_WIDTH,
        PLAYERLIST_HEIGHT,
        EVENT_NONE);
    if (m_playerListValue == nullptr)
        return false;

    m_playerListNumItems->LinkBox(m_playerList);
    m_playerListValue->LinkBox(m_playerListNumItems);

    m_items = &g_game->gameState->m_items;
    m_playerItemsFiltered = new Items;

    // load audio files
    m_sndButtonClick =
        g_game->audioSystem->Load("data/cargohold/buttonclick.ogg");
    if (!m_sndButtonClick) {
        g_game->message("CargoWindow: Error loading buttonclick");
        return false;
    }

    // NOTE: initialized==true will tell UpdateLists() its data are properly set
    // up.
    //  This protection is necessary because some inventory changes can happen
    //  before CargoWindow is initialized (e.g. encounter scripts Initialize
    //  function)
    initialized = true;

    // NOTE: This will call UpdateLists() among others, so all must be properly
    //  initialized when calling this.
    this->InitViewer();

    ALLEGRO_DEBUG("  ModuleCargoWindow: initialized\n");

    return true;
}

// NOTE: Right now, InitViewer() do the same thing as ResetViewer().
//  The difference is that InitViewer() is private and intented to be called
//  as part of our own Init(), while ResetViewer() is public and intented for
//  use by others (e.g. our parent module).
void
ModuleCargoWindow::InitViewer() {
    this->ResetViewer();
}

/* Force the window into its starting (hidden) state */
void
ModuleCargoWindow::ResetViewer() {
    // start hidden
    m_x = gui_viewer_left;
    m_y = 10;
    sliding = false;
    sliding_offset = -gui_viewer_speed;

    if (initialized) {
        // update the content of the scrollbox to match current gamestate
        this->UpdateLists();
        // unselect previously selected line (if any)
        m_playerList->SetSelectedIndex(-1);
    }

    // hide the jettison button
    if (m_jettisonButton && m_jettisonButton->IsInitialized())
        m_jettisonButton->SetVisible(false);
}

/* Update the content of the scrollbox to match current gamestate */
void
ModuleCargoWindow::UpdateLists() {
    // update the lists only if between Init() and Close()
    if (!initialized)
        return;

    m_playerItemsFiltered->Reset();
    for (int i = 0; i < m_items->GetNumStacks(); i++) {
        Item item;
        int numItems;
        m_items->GetStack(i, item, numItems);
        m_playerItemsFiltered->AddItems(item.id, numItems);
    }

    m_playerList->Clear();
    m_playerList->setLines(m_playerItemsFiltered->GetNumStacks());
    m_playerListNumItems->Clear();
    m_playerListNumItems->setLines(m_playerItemsFiltered->GetNumStacks());
    m_playerListValue->Clear();
    m_playerListValue->setLines(m_playerItemsFiltered->GetNumStacks());
    for (int i = 0; i < m_playerItemsFiltered->GetNumStacks(); i++) {
        Item item;
        int numItems;
        m_playerItemsFiltered->GetStack(i, item, numItems);

        m_playerList->Write(item.name);
        m_playerListNumItems->Write(Util::ToString(numItems));
        m_playerListValue->Write(Util::ToString((int)item.value));
    }

    std::string space;
    int occupiedSpace = g_game->gameState->m_ship.getOccupiedSpace();
    space = to_string(occupiedSpace) + "/" + to_string(maxSpace);

    m_space_status->set_text(space);
}

bool
ModuleCargoWindow::on_event(ALLEGRO_EVENT *event) {
    int ev = event->type;

    switch (ev) {
    // player asked the window to show on/hide away
    case EVENT_CAPTAIN_CARGO:
        {
            if (!sliding)
                sliding = true;
            sliding_offset = -sliding_offset;
            break;
        }

    // player clicked on an item line
    case EVENT_CARGO_LIST_CLICK:
        {
            g_game->audioSystem->Play(m_sndButtonClick);

            if (m_playerList->GetSelectedIndex() >= 0)
                m_jettisonButton->SetVisible(true);
            else
                m_jettisonButton->SetVisible(false);
            break;
        }

    // player clicked on the jettison button
    case EVENT_CARGO_JETTISON:
        {
            g_game->audioSystem->Play(m_sndButtonClick);
            m_jettisonButton->SetVisible(false);

            int itemIdx = m_playerList->GetSelectedIndex();
            m_playerList->SetSelectedIndex(-1);
            Item item;
            int numItems;
            m_playerItemsFiltered->GetStack(itemIdx, item, numItems);
            m_items->RemoveItems(item.id, numItems);

            // notify everybody (include ourselves) that the inventory
            // changed
            ALLEGRO_EVENT e = {
                .type = static_cast<unsigned int>(EVENT_CARGO_UPDATE)};
            g_game->broadcast_event(&e);
            break;
        }

    // inventory changed due to either the jettison button or an external
    // factor
    case EVENT_CARGO_UPDATE:
        this->UpdateLists();
        break;
    }
    return true;
}

bool
ModuleCargoWindow::on_update() {
    // shut off the window if not in "Captain mode"
    if (isVisible()
        && g_game->gameState->getCurrentSelectedOfficer() != OFFICER_CAPTAIN) {
        if (!sliding)
            sliding = true;
        if (sliding_offset != -gui_viewer_speed)
            sliding_offset = -gui_viewer_speed;
    }
    return true;
}

bool
ModuleCargoWindow::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);

    // sliding the window
    if (sliding) {
        m_x += sliding_offset;

        // hitting the edges
        if (m_x < gui_viewer_left) {
            m_x = gui_viewer_left;
            sliding = false;
        } else if (m_x > gui_viewer_right) {
            m_x = gui_viewer_right;
            sliding = false;
        }
    }

    // return early since we are not visible
    if (!isVisible()) {
        m_space_status->set_active(false);
        return true;
    }

    m_space_status->set_active(true);

    // drawing the window
    al_draw_bitmap(img_viewer, m_x, m_y, 0);

    // draw items list header
    g_game->Print20(target, 108 + m_x, 32 + m_y, "ITEM", LTGREEN, true);
    g_game->Print20(target, 273 + m_x, 32 + m_y, "QTY", LTGREEN, true);
    g_game->Print20(target, 324 + m_x, 32 + m_y, "VALUE", LTGREEN, true);

    // draw items list content
    m_playerList->SetX(PLAYERLIST_X + m_x);
    m_playerList->SetY(PLAYERLIST_Y + m_y);
    m_playerListNumItems->SetX(
        PLAYERLIST_X + m_x + PLAYERLIST_WIDTH - PLAYERLIST_VALUE_WIDTH
        - PLAYERLIST_NUMITEMS_WIDTH);
    m_playerListNumItems->SetY(PLAYERLIST_Y + m_y);
    m_playerListValue->SetX(
        PLAYERLIST_X + m_x + PLAYERLIST_WIDTH - PLAYERLIST_VALUE_WIDTH);
    m_playerListValue->SetY(PLAYERLIST_Y + m_y);
    m_playerListValue->Draw(target);

    // draw jettison button
    int relX = m_jettisonButton->GetX();
    int relY = m_jettisonButton->GetY();
    m_jettisonButton->SetX(m_x + relX);
    m_jettisonButton->SetY(m_y + relY);
    m_jettisonButton->Run(target);
    m_jettisonButton->SetX(relX);
    m_jettisonButton->SetY(relY);

    // draw capacity indicator
    m_space_status->move(m_x + CARGO_SPACESTATUS_X, m_y + CARGO_SPACESTATUS_Y);

    return true;
}

bool
ModuleCargoWindow::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    if (!isVisible())
        return false;

    m_playerListValue->OnMouseMove(x + m_x, y + m_y);

    int relX = m_jettisonButton->GetX();
    int relY = m_jettisonButton->GetY();
    m_jettisonButton->SetX(m_x + relX);
    m_jettisonButton->SetY(m_y + relY);
    m_jettisonButton->OnMouseMove(x + m_x, y + m_y);
    m_jettisonButton->SetX(relX);
    m_jettisonButton->SetY(relY);

    return true;
}

bool
ModuleCargoWindow::on_mouse_button_down(ALLEGRO_MOUSE_EVENT *event) {
    int button = event->button - 1;
    int x = event->x;
    int y = event->y;

    if (!isVisible())
        return false;

    m_playerListValue->OnMousePressed(button, x + m_x, y + m_y);
    return true;
}

bool
ModuleCargoWindow::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    int button = event->button - 1;
    int x = event->x;
    int y = event->y;

    if (!isVisible())
        return true;

    m_playerListValue->OnMouseReleased(button, x + m_x, y + m_y);

    int relX = m_jettisonButton->GetX();
    int relY = m_jettisonButton->GetY();
    m_jettisonButton->SetX(m_x + relX);
    m_jettisonButton->SetY(m_y + relY);
    m_jettisonButton->OnMouseReleased(button, x + m_x, y + m_y);
    m_jettisonButton->SetX(relX);
    m_jettisonButton->SetY(relY);

    if (is_mouse_click(event)) {
        m_playerListValue->OnMouseClick(button, x + m_x, y + m_y);
    }
    return true;
}

bool
ModuleCargoWindow::on_close() {
    ALLEGRO_DEBUG("*** ModuleCargoWindow: closing...\n");

    // NOTE: this is needed to prevent some sort of race condition which will
    //  cause the game to crash when leaving PlanetSurface for PlanetOrbit when
    //  the CargoWindow is shown. I did not dig into it much yet but it probably
    //  is related to Draw() (not event related in any case).
    //  strictly speaking this is not needed anymore. the check for
    //  `initialized' in IsVisible() will protect us against it, but i let this
    //  for documentation.
    this->ResetViewer();

    // will tell UpdateLists() the data it needs are not available.
    initialized = false;

    // unload the resources
    m_resources.unload();

    m_items = nullptr;

    if (m_playerItemsFiltered != nullptr) {
        delete m_playerItemsFiltered;
        m_playerItemsFiltered = nullptr;
    }

    if (m_playerListValue != nullptr) {
        // this will destroy the other m_playerList* too, since they are linked.
        delete m_playerListValue;
        m_playerListValue = nullptr;
    }

    m_jettisonButton.reset();

    if (m_sndButtonClick != nullptr) {
        m_sndButtonClick.reset();
    }

    ALLEGRO_DEBUG("*** ModuleCargoWindow: closed\n");
    return true;
}
