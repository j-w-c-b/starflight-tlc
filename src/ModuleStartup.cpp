/*
	STARFLIGHT - THE LOST COLONY
	ModuleStartup.cpp - Handles opening sequences, videos, copyrights, prior to titlescreen.
	The purpose of this module is to free up resources and reduce the logic in titlescreen
	which was having to deal with the startup sequence. This is just easier.
	Author: J.Harbour
	Date: Jan,2008
*/

#include <exception>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "Game.h"
#include "ModeMgr.h"
#include "DataMgr.h"
#include "Events.h"
#include "Util.h"
#include "ModuleStartup.h"
#include <string>

void showOpeningStory(int page);
int storypage = 0;

ALLEGRO_DEBUG_CHANNEL("ModuleStartup")

ModuleStartup::ModuleStartup()
{
	display_mode = 3;
}

ModuleStartup::~ModuleStartup(){}


bool ModuleStartup::Init()
{
    m_background = al_load_bitmap("data/startup/space_1280.bmp");


	//load copyright screen
	copyright = al_load_bitmap("data/startup/startup_copyrights.bmp");
	if (!copyright) {
		g_game->message("Startup: Error loading startup_copyrights");
		return false;
	}

	return true;
}

void ModuleStartup::Close()
{
	try {
        al_destroy_bitmap(copyright);
        al_destroy_bitmap(m_background);
	}
	catch(std::exception e) {
		ALLEGRO_DEBUG("%s\n", e.what());
	}
	catch(...) {
		ALLEGRO_DEBUG("Unhandled exception in Startup::Close\n");
	}
}

int ModuleStartup::fadein(ALLEGRO_BITMAP *dest, ALLEGRO_BITMAP *source, int speed)
{
	int retval = 0;
	static int loop = 0;

	al_set_target_bitmap(dest);
	if (loop < 256-speed)
	{
		loop += speed;
		al_draw_filled_rectangle( 0,0, al_get_bitmap_width(source), al_get_bitmap_height(source), al_map_rgba(0,0,0,255 - loop));
	}
	else {
		loop = 0;
		retval = 1;
	}

	return retval;
}

int ModuleStartup::fadeout(ALLEGRO_BITMAP *dest, ALLEGRO_BITMAP *source, int speed)
{
	int retval = 0;
	static int loop = 255;

	al_set_target_bitmap(dest);
	if (loop > speed)
	{
		loop -= speed;
		al_draw_filled_rectangle( 0,0, al_get_bitmap_width(source), al_get_bitmap_height(source), al_map_rgba(0,0,0,loop));
	}
	else {
		al_draw_filled_rectangle( 0,0, al_get_bitmap_width(source), al_get_bitmap_height(source), al_map_rgba(0,0,0,255));
		loop = 255;
		retval = 1;
	}

	return retval;
}

void ModuleStartup::Update(){}

void ModuleStartup::Draw()
{
	static bool title_done = false;
        al_set_target_bitmap(g_game->GetBackBuffer());

    al_draw_bitmap(m_background, 0, 0, 0);

	switch (display_mode) {

    case 0: //initial blank period to slow down the intro
        if (Util::ReentrantDelay(4000)) 
            display_mode = 1;
        break;

	case 1: //copyright fadein
		if (!title_done) {
			if (fadein(g_game->GetBackBuffer(), copyright, 1)) {
				title_done = true;
			}

		} else {
		al_draw_bitmap(copyright, 0, 0, 0);
			if (Util::ReentrantDelay(4000))
				display_mode = 2;
		}
		break;
		
	case 2: //copyright fadeout
		title_done = false;
		if (fadeout(g_game->GetBackBuffer(), copyright, 2)) {
			display_mode = 3;
		}
		break;


    case 3: //opening story
        showOpeningStory(storypage);
        break;

	case 100: //done, transition to TitleScreen
		if (Util::ReentrantDelay(1000))
        {
		    g_game->LoadModule(MODULE_TITLESCREEN);
		    return;
        }
		break;
		
	}
}

#pragma region INPUT

void ModuleStartup::OnKeyReleased(int /*keyCode*/)
{
	switch (display_mode) {
	//pressing any key will fast forward the slideshow
	case 1: //fade in copyright
		display_mode = 2;
		break;

	case 2: //fade out copyright
		display_mode = 3;
		break;

    case 3: //opening story
        storypage++;
        if (storypage > 4)
            display_mode = 100;
        break;

	case 100: //done
		break;
	}
}

#pragma endregion

void showOpeningStory(int page)
{
 /*   g_game->Print18(g_game->GetBackBuffer(), 50, SCREEN_HEIGHT-40,
        "PAGE " + Util::ToString(page), YELLOW, true);
    g_game->Print18( g_game->GetBackBuffer(), SCREEN_WIDTH-350, SCREEN_HEIGHT-40, 
        "Press any key or mouse button to continue...", YELLOW, true);*/

    int y=50,x=100,spacing=28;
    for (int j=0; j<lines; j++) 
    {
        g_game->Print24( g_game->GetBackBuffer(), x, y, story[page][j] );
        y+=spacing;
    }

}
