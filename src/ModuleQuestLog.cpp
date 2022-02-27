/*
        STARFLIGHT - THE LOST COLONY
        ModuleStarmap.cpp - The QuestLog Module
        Author: Justin Sargent
        Date: Nov-24-2007
*/

#include <allegro5/allegro.h>

#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "Label.h"
#include "ModuleControlPanel.h"
#include "ModuleQuestLog.h"
#include "QuestMgr.h"
#include "questviewer_resources.h"

using namespace std;
using namespace questviewer;

#define VIEWER_X 588
#define VIEWER_Y 114
#define VIEWER_W 436
#define VIEWER_H 334

#define NAME_X VIEWER_X + 36
#define NAME_Y VIEWER_Y + 36
#define NAME_H 48
#define NAME_W 290

#define DESC_X NAME_X
#define DESC_Y NAME_Y + NAME_H + 8
#define DESC_H 190
#define DESC_W NAME_W

#define STATUS_X NAME_X
#define STATUS_Y DESC_Y + DESC_H
#define STATUS_H NAME_H
#define STATUS_W NAME_W

ALLEGRO_DEBUG_CHANNEL("ModuleQuestLog")

ModuleQuestLog::ModuleQuestLog()
    : Module(VIEWER_X, VIEWER_Y, VIEWER_W, VIEWER_H),
      m_viewer(make_shared<SlidingModule<Bitmap>>(
          SLIDE_FROM_RIGHT,
          EVENT_NONE,
          0.6,
          images[I_QUEST_VIEWER],
          VIEWER_X,
          VIEWER_Y)),
      m_quest_name(make_shared<Label>(
          "",
          NAME_X,
          NAME_Y,
          NAME_W,
          NAME_H,
          false,
          0,
          g_game->font22,
          al_map_rgb(255, 84, 0))),
      m_quest_desc(make_shared<Label>(
          "",
          DESC_X,
          DESC_Y,
          DESC_W,
          DESC_H,
          true,
          0,
          g_game->font22,
          WHITE)),
      m_quest_status(make_shared<Label>(
          "",
          STATUS_X,
          STATUS_Y,
          STATUS_W,
          STATUS_H,
          false,
          0,
          g_game->font20,
          BLACK)) {
    add_child_module(m_viewer);
    m_viewer->add_child_module(m_quest_name);
    m_viewer->add_child_module(m_quest_desc);
    m_viewer->add_child_module(m_quest_status);
}

bool
ModuleQuestLog::on_event(ALLEGRO_EVENT *event) {
    EventType ev = static_cast<EventType>(event->type);

    switch (ev) {
    case EVENT_CAPTAIN_QUESTLOG:
        m_viewer->toggle();
        if (!m_is_active) {
            m_is_active = true;
        }
        return true;
    case EVENT_CAPTAIN_LAUNCH:
        [[fallthrough]];
    case EVENT_CAPTAIN_DESCEND:
        [[fallthrough]];
    case EVENT_CAPTAIN_CARGO:
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
ModuleQuestLog::on_update() {
    auto current_officer = g_game->gameState->getCurrentSelectedOfficer();

    if (current_officer != OFFICER_CAPTAIN) {
        if (!m_viewer->is_closed()) {
            m_viewer->toggle();
        }
        m_is_active = false;
    } else if (m_is_active && !m_viewer->is_visible()) {
        ALLEGRO_EVENT e = make_event(EVENT_COMMAND_COMPLETE);
        m_is_active = false;
        g_game->broadcast_event(&e);
    }
    m_quest_name->set_text(g_game->questMgr->getName());
    m_quest_desc->set_text(g_game->questMgr->getShort());

    if (g_game->gameState->getQuestCompleted()) {
        m_quest_status->set_text("(COMPLETE)");
        m_quest_status->set_color(GREEN);
    } else {
        m_quest_status->set_text("(INCOMPLETE)");
        m_quest_status->set_color(RED);
    }
    return true;
}
