/*
	STARFLIGHT - THE LOST COLONY
	ModuleTopGUI.cpp 
	Author: Keith Patch
	Date: April 2008
*/

#include <allegro5/allegro.h>
#include "Util.h"
#include "GameState.h"
#include "Game.h"
#include "Events.h"
#include "Script.h"
#include "ModuleTopGUI.h"
#include "DataMgr.h"
#include "ModeMgr.h"

int ggx = 0;
int ggy = 0;

ALLEGRO_DEBUG_CHANNEL("ModuleTopGUI")

ModuleTopGUI::ModuleTopGUI() {}
ModuleTopGUI::~ModuleTopGUI(){}

bool ModuleTopGUI::Init()
{
	ggx = (int)g_game->getGlobalNumber("GUI_GAUGES_POS_X");
	ggy = (int)g_game->getGlobalNumber("GUI_GAUGES_POS_Y");

	//load the gauges gui
	img_gauges = al_load_bitmap("data/topgui/topgauge.tga");
	img_fuel_gauge = al_load_bitmap("data/topgui/Element_Gauge_Orange.bmp");
	img_hull_gauge = al_load_bitmap("data/topgui/Element_Gauge_Green.bmp");
	img_shield_gauge = al_load_bitmap("data/topgui/Element_Gauge_Blue.bmp");
	img_armor_gauge = al_load_bitmap("data/topgui/Element_Gauge_Red.bmp");

	return true;
}

void ModuleTopGUI::Close()
{
	try {
		al_destroy_bitmap(img_gauges);
		al_destroy_bitmap(img_fuel_gauge);
		al_destroy_bitmap(img_armor_gauge);
		al_destroy_bitmap(img_hull_gauge);
		al_destroy_bitmap(img_shield_gauge);
	}
	catch(std::exception e) {
		ALLEGRO_DEBUG("%s\n", e.what());
	}
	catch(...) {
		ALLEGRO_DEBUG("Unhandled exception in ModuleTopGUI::Close\n");
	}
}
	
void ModuleTopGUI::Update(){}

void ModuleTopGUI::Draw()
{
	float fuel_percent = g_game->gameState->getShip().getFuel();
	float hull_percent = g_game->gameState->getShip().getHullIntegrity() / 100;
	float armor_percent = 0; 
	float shield_percent = 0;
        al_set_target_bitmap(g_game->GetBackBuffer());
	
	if(g_game->gameState->getShip().getMaxArmorIntegrity() <= 0)
	{
		armor_percent = 0;
	}
	else{
		armor_percent = g_game->gameState->getShip().getArmorIntegrity() / g_game->gameState->getShip().getMaxArmorIntegrity();
	}

	if (g_game->gameState->getShip().getMaxShieldCapacity() <= 0)
	{
		shield_percent = 0;
	}
	else{
		shield_percent = g_game->gameState->getShip().getShieldCapacity() / g_game->gameState->getShip().getMaxShieldCapacity();
	}
	/*
	 * draw top gauge gui
	 */
	al_draw_bitmap(img_gauges, ggx, ggy, 0);
	al_draw_bitmap_region(img_hull_gauge, 0, 0, al_get_bitmap_width(img_hull_gauge) * hull_percent, al_get_bitmap_height(img_hull_gauge), ggx+89, ggy+15, 0);
	al_draw_bitmap_region(img_armor_gauge, 0, 0, al_get_bitmap_width(img_armor_gauge) * armor_percent, al_get_bitmap_height(img_armor_gauge),  ggx+273, ggy+15, 0);
	al_draw_bitmap_region(img_shield_gauge, 0, 0, al_get_bitmap_width(img_shield_gauge) * shield_percent, al_get_bitmap_height(img_shield_gauge), ggx+464, ggy+15, 0);
	al_draw_bitmap_region(img_fuel_gauge, 0, 0, al_get_bitmap_width(img_fuel_gauge) * fuel_percent, al_get_bitmap_height(img_fuel_gauge), ggx+630, ggy+14, 0);

}
