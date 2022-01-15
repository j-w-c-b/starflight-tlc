/*
	STARFLIGHT - THE LOST COLONY
	ModuleStarmap.cpp - The QuestLog Module
	Author: Justin Sargent
	Date: Nov-24-2007
*/

#include <allegro5/allegro.h>
#include "ModuleQuestLog.h"
#include "GameState.h"
#include "Game.h"
#include "QuestMgr.h"
#include "Events.h"
#include "DataMgr.h"
#include "Label.h"
#include "ModuleControlPanel.h"
#include "questviewer_resources.h"

using namespace std;

#define VIEWER_MOVE_RATE 16

#define NAME_X 36
#define NAME_Y 36
#define NAME_H 48
#define NAME_W 290

#define DESC_X NAME_X
#define DESC_Y NAME_Y + NAME_H + 8
#define DESC_H 190
#define DESC_W NAME_W

ALLEGRO_DEBUG_CHANNEL("ModuleQuestLog")

ModuleQuestLog::ModuleQuestLog() : resources(QUESTVIEWER_IMAGES)
{
	log_active = false;
}

ModuleQuestLog::~ModuleQuestLog()
{
}

void ModuleQuestLog::OnEvent(Event *event)
{
	switch(event->getEventType()) 
	{
		case EVENT_CAPTAIN_QUESTLOG:
			if(!log_active){
				log_active = true;
			}else{
				log_active = false;
			}
			break;
	}
}

void ModuleQuestLog::Close()
{
	try {
		resources.unload();
	}
	catch(std::exception e) {
		ALLEGRO_DEBUG("%s\n", e.what());
	}
	catch(...) {
		ALLEGRO_DEBUG("Unhandled exception in QuestLog::Close\n");
	}
}

bool ModuleQuestLog::Init()
{
	ALLEGRO_DEBUG("ModuleQuestLog Initialize\n");
	
	if (!resources.load()) {
		g_game->message("QuestLog: Error loading resources");	
		return false;
	}
	

	viewer_offset_x = SCREEN_WIDTH;
	viewer_offset_y = 90;

	log_active = false;

	//create quest name label
	ALLEGRO_COLOR questTitleColor = al_map_rgb(255,84,0);
	questName = new Label(g_game->questMgr->getName(), 
		viewer_offset_x+NAME_X, viewer_offset_y+NAME_Y, NAME_W, NAME_H, questTitleColor, g_game->font22);
	questName->Refresh();

	//create quest description label
	ALLEGRO_COLOR questTextColor = al_map_rgb(255,255,255);
	questDesc = new Label(g_game->questMgr->getShort(), 
		viewer_offset_x+DESC_X, viewer_offset_y+DESC_Y, DESC_W, DESC_H, questTextColor, g_game->font22);
	questDesc->Refresh();


	//load window GUI
	window = resources[QUEST_VIEWER];

	return true;
}

void ModuleQuestLog::Update()
{
	if(log_active){
		if(g_game->gameState->getCurrentSelectedOfficer() != OFFICER_CAPTAIN){
			log_active = false;
		}
	}
}


void ModuleQuestLog::Draw()
{
	//is quest viewer visible?
	if(viewer_offset_x < SCREEN_WIDTH)
	{
		//draw background
		al_draw_bitmap(window, viewer_offset_x, viewer_offset_y, 0);

		//draw quest title
		questName->SetX(NAME_X + viewer_offset_x);
		questName->Draw(g_game->GetBackBuffer());

		//draw quest description
		questDesc->SetX(DESC_X + viewer_offset_x);
		questDesc->Draw(g_game->GetBackBuffer());	


		//display quest completion status
		string metstr;
		ALLEGRO_COLOR metcolor;
		if (g_game->gameState->getQuestCompleted()) {
			metstr = "(COMPLETE)";
			metcolor = GREEN;
		}
		else {
			metstr = "(INCOMPLETE)";
			metcolor = RED;
		}
		g_game->Print20(g_game->GetBackBuffer(), viewer_offset_x + NAME_X, viewer_offset_y+DESC_Y+DESC_H, metstr, metcolor);


	}

	if(log_active)
	{
		if(viewer_offset_x > 620) {
			viewer_offset_x -= VIEWER_MOVE_RATE;
		}
	}else{
		if(viewer_offset_x < SCREEN_WIDTH) {
			viewer_offset_x += VIEWER_MOVE_RATE;
		}
	}
}

