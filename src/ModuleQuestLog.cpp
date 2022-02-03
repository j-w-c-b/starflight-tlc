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
using namespace questviewer_resources;

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
    : Module(), m_resources(QUESTVIEWER_IMAGES),
      m_background(new Bitmap(
          m_resources[I_QUEST_VIEWER],
          VIEWER_X,
          VIEWER_Y,
          VIEWER_W,
          VIEWER_H)),
      m_quest_name(new Label(
          "",
          NAME_X,
          NAME_Y,
          NAME_W,
          NAME_H,
          false,
          0,
          g_game->font22,
          al_map_rgb(255, 84, 0))),
      m_quest_desc(new Label(
          "",
          DESC_X,
          DESC_Y,
          DESC_W,
          DESC_H,
          true,
          0,
          g_game->font22,
          WHITE)),
      m_quest_status(new Label(
          "",
          STATUS_X,
          STATUS_Y,
          STATUS_W,
          STATUS_H,
          false,
          0,
          g_game->font20,
          BLACK)) {
    add_child_module(m_background);
    add_child_module(m_quest_name);
    add_child_module(m_quest_desc);
    add_child_module(m_quest_status);
}

ModuleQuestLog::~ModuleQuestLog() {}

bool
ModuleQuestLog::on_update() {
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
