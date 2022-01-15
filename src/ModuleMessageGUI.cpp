/*
	STARFLIGHT - THE LOST COLONY
	ModuleMessageGUI.cpp 
	Author: 
	Date: 
*/

#include <allegro5/allegro.h>
#include "Util.h"
#include "GameState.h"
#include "Game.h"
#include "Events.h"
#include "Script.h"
#include "ModuleMessageGUI.h"
#include "DataMgr.h"
#include "ModeMgr.h"
#include "messagegui_resources.h"

using namespace std;

int gmx,gmy,gmw,gmh,gsx,gsy;

ALLEGRO_DEBUG_CHANNEL("ModuleMessageGUI")

ModuleMessageGUI::ModuleMessageGUI() : resources(MESSAGEGUI_IMAGES) {}
ModuleMessageGUI::~ModuleMessageGUI(){}

bool ModuleMessageGUI::Init()
{
	//load the datafile
	if (!resources.load()) {
		g_game->message("MessageGUI: Error loading resources");
		return false;
	}

	//load the gauges gui
	img_message = resources[GUI_MESSAGEWINDOW];
	img_socket = resources[GUI_SOCKET];

	gmx = (int)g_game->getGlobalNumber("GUI_MESSAGE_POS_X");
	gmy = (int)g_game->getGlobalNumber("GUI_MESSAGE_POS_Y");
	gmw = (int)g_game->getGlobalNumber("GUI_MESSAGE_WIDTH");
	gmh = (int)g_game->getGlobalNumber("GUI_MESSAGE_HEIGHT");
	gsx = (int)g_game->getGlobalNumber("GUI_SOCKET_POS_X");
	gsy = (int)g_game->getGlobalNumber("GUI_SOCKET_POS_Y");

	return true;
}

void ModuleMessageGUI::Close()
{
	try {
		resources.unload();
	}
	catch(std::exception e) {
		ALLEGRO_DEBUG("%s\n", e.what());
	}
	catch(...) {
		ALLEGRO_DEBUG("Unhandled exception in ModuleMessageGUI::Close\n");
	}
}
	
void ModuleMessageGUI::Update()
{
}

void ModuleMessageGUI::Draw()
{
	//draw message gui
	al_draw_bitmap(img_message, gmx, gmy, 0);

	//draw socket gui
	al_draw_bitmap(img_socket, gsx, gsy, 0);

	//print stardate
	Stardate date = g_game->gameState->stardate;
	int hour = date.GetHour();
	int day = date.GetDay();
	int month = date.GetMonth();
	int year = date.GetYear();
	string datestr = Util::ToString(year) + "-" + Util::ToString(month,2) + "-" + Util::ToString(day,2)
		+ " " + Util::ToString(hour%12);
	if (hour < 12) datestr += " AM"; else datestr += " PM";
	g_game->Print22(g_game->GetBackBuffer(), gsx+140, gsy+24, datestr, STEEL);

}