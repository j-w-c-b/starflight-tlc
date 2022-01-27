/*
        STARFLIGHT - THE LOST COLONY
        ModuleStarmap.cpp - The QuestLog Module
        Author: Justin Sargent
        Date: Nov-24-2007
*/

#include "ModuleQuestLog.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "Label.h"
#include "ModuleControlPanel.h"
#include "QuestMgr.h"
#include "questviewer_resources.h"
#include <allegro5/allegro.h>

using namespace std;
using namespace questviewer_resources;

#define VIEWER_MOVE_RATE 16
#define VIEWER_WIDTH 620

#define NAME_X 36
#define NAME_Y 36
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

ModuleQuestLog::ModuleQuestLog() : resources(QUESTVIEWER_IMAGES) {
    log_active = false;

    // create quest name label
    ALLEGRO_COLOR questTitleColor = al_map_rgb(255, 84, 0);
    m_quest_name = new Label(
        "",
        NAME_X,
        NAME_Y,
        NAME_W,
        NAME_H,
        false,
        0,
        g_game->font22,
        questTitleColor);
    add_child_module(m_quest_name);

    // create quest description label
    ALLEGRO_COLOR questTextColor = al_map_rgb(255, 255, 255);
    m_quest_desc = new Label(
        "",
        DESC_X,
        DESC_Y,
        DESC_W,
        DESC_H,
        true,
        0,
        g_game->font22,
        questTextColor);
    add_child_module(m_quest_desc);

    m_quest_status = new Label(
        "",
        STATUS_X,
        STATUS_Y,
        STATUS_W,
        STATUS_H,
        false,
        0,
        g_game->font20,
        BLACK);
    add_child_module(m_quest_status);
}

ModuleQuestLog::~ModuleQuestLog() {}

bool
ModuleQuestLog::on_event(ALLEGRO_EVENT *event) {
    switch (event->type) {
    case EVENT_CAPTAIN_QUESTLOG:
        if (!log_active) {
            log_active = true;
        } else {
            log_active = false;
        }
        break;
    }
    return true;
}

bool
ModuleQuestLog::on_init() {
    ALLEGRO_DEBUG("ModuleQuestLog Initialize\n");

    if (!resources.load()) {
        g_game->message("QuestLog: Error loading resources");
        return false;
    }

    viewer_offset_x = SCREEN_WIDTH;
    viewer_offset_y = 90;

    log_active = false;

    // load window GUI
    window = resources[I_QUEST_VIEWER];

    return true;
}

bool
ModuleQuestLog::on_update() {
    if (log_active) {
        if (g_game->gameState->getCurrentSelectedOfficer() != OFFICER_CAPTAIN) {
            log_active = false;
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
        if (viewer_offset_x > VIEWER_WIDTH) {
            viewer_offset_x -= VIEWER_MOVE_RATE;
        }
    } else {
        if (viewer_offset_x < SCREEN_WIDTH) {
            viewer_offset_x += VIEWER_MOVE_RATE;
        }
    }
    return true;
}

bool
ModuleQuestLog::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);

    // is quest viewer visible?
    if (viewer_offset_x < SCREEN_WIDTH) {
        // draw background
        al_draw_bitmap(window, viewer_offset_x, viewer_offset_y, 0);

        // set quest label positions
        m_quest_name->move(NAME_X + viewer_offset_x, NAME_Y + viewer_offset_y);
        m_quest_desc->move(DESC_X + viewer_offset_x, DESC_Y + viewer_offset_y);

        // display quest completion status
        m_quest_status->move(
            viewer_offset_x + NAME_X, viewer_offset_y + DESC_Y + DESC_H);
    }

    return true;
}
