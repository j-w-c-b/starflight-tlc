/*
	STARFLIGHT - THE LOST COLONY
	ModuleSettings.cpp 
	Author: Keith Patch
	Date: October 2008
*/
#include <sstream>
#include <fstream>
#include <allegro5/allegro.h>
#include "Util.h"
#include "GameState.h"
#include "Game.h"
#include "Events.h"
#include "Script.h"
#include "DataMgr.h"
#include "ModeMgr.h"
#include "AudioSystem.h"
#include "ModuleSettings.h"

using namespace std;

const int EVENT_EXIT							= -1;

const int EVENT_CONTROLS						= 100;
const int EVENT_SAVESETTINGS					= -200;
const int EVENT_RES_CHANGE  = 9900;
const int EVENT_FULLSCREEN_BUTTON = 9901;

ALLEGRO_DEBUG_CHANNEL("ModuleSettings")

std::string old_resolution="";

ModuleSettings::ModuleSettings(void)
{
    background = NULL;
	btn_exit = NULL;
    btn_fullscreen = NULL;
	btn_defaults = NULL;
	btn_save = NULL;
	for(int i=0; i<11; i++)
        btn_controls[i] = NULL;
}

ModuleSettings::~ModuleSettings(void){}

bool ModuleSettings::Init()
{
	ALLEGRO_DEBUG("  ModuleSettings Initialize\n");

	cmd_selected = 0;
	button_selected = 0;

	background = al_load_bitmap("data/settings/background.tga");
	if (!background) {
		g_game->message("Settings: Error loading background");
		return false;
	}

	ALLEGRO_BITMAP *imgNorm=NULL, *imgOver=NULL; 
	g_game->audioSystem->Load("data/cantina/buttonclick.ogg", "click");

    //exit button
	imgNorm = al_load_bitmap("data/settings/button1.tga");
	imgOver = al_load_bitmap("data/settings/button.tga");
	btn_exit = new Button(
		imgNorm, imgOver, NULL, 
		10, SCREEN_HEIGHT-al_get_bitmap_height(imgNorm)-10, 
        0, EVENT_EXIT, g_game->font22, "EXIT", GOLD, "click", true, true);
	if(!btn_exit) return false;
	if(!btn_exit->IsInitialized()) return false;

    //save button
	btn_save = new Button(
		imgNorm, imgOver, NULL, 
		140, SCREEN_HEIGHT-al_get_bitmap_height(imgNorm)-10,  
        0, EVENT_SAVESETTINGS, g_game->font22, "SAVE", GOLD, "click", true, true);
	if(!btn_save) return false;
	if(!btn_save->IsInitialized()) return false;

    //key name buttons
    int x,y;
	for(int i=0; i<11; i++)
    {
        x = 680;
        y = 110+i*40;
		btn_controls[i] = new Button(
			imgNorm, imgOver, NULL, 
			x, y, 0, EVENT_CONTROLS+i, g_game->font22, "", GOLD, "click", true, true);
		if(!btn_controls[i]) return false;
		if(!btn_controls[i]->IsInitialized()) return false;
	}

	btn_controls[0]->SetButtonText("UP or W");
    btn_controls[1]->SetButtonText("LEFT or A");
    btn_controls[2]->SetButtonText("RIGHT or D");
    btn_controls[3]->SetButtonText("DOWN or S");
    btn_controls[4]->SetButtonText("Q");
    btn_controls[5]->SetButtonText("E");
    btn_controls[6]->SetButtonText("ALT or X");
    btn_controls[7]->SetButtonText("CTRL or Z");
    btn_controls[8]->SetButtonText("PGUP");
    btn_controls[9]->SetButtonText("PGDN");
    btn_controls[10]->SetButtonText("F1-F7");



    //create the resolution list scrollbox
    resScrollbox = new ScrollBox::ScrollBox(g_game->font18, ScrollBox::SB_LIST,
    	40, 100, 200, 230, EVENT_RES_CHANGE);
    resScrollbox->setLines(g_game->videomodes.size());
	resScrollbox->DrawScrollBar(true);
    resScrollbox->SetColorBackground(al_map_rgb(30,30,30));
    resScrollbox->SetColorItemBorder(al_map_rgb(40,40,40));
    resScrollbox->SetColorControls(al_map_rgb(130,130,130));
    resScrollbox->PaintNormalImage();
    resScrollbox->SetColorHover(al_map_rgb(160,160,160));
    resScrollbox->PaintHoverImage();
    resScrollbox->SetColorSelectedBackground(al_map_rgb(80,80,160));
    resScrollbox->SetColorSelectedHighlight(al_map_rgb(160,160,255));
    resScrollbox->PaintSelectedImage();

    chosenResolution = g_game->getGlobalString("RESOLUTION");

    for (auto mode = g_game->videomodes.begin(); mode != g_game->videomodes.end(); ++mode)
	{
        std::ostringstream os;
        os << mode->width << " x " << mode->height;
        ScrollBox::ColoredString item;
        item.Color = WHITE; item.String = os.str();
        resScrollbox->Write(item);
    }


    //create fullscreen toggle
    ALLEGRO_BITMAP *toggleImage=NULL;
    toggleImage = al_load_bitmap("data/settings/button32_normal.bmp");
    if (!toggleImage) {
        g_game->fatalerror("Settings: Error loading toggle image\n");
        return false;
    }
    ALLEGRO_BITMAP *toggleImageOver=NULL;
    toggleImageOver = al_load_bitmap("data/settings/button32_over.bmp");
    if (!toggleImageOver) {
        g_game->fatalerror("Settings: Error loading toggle image\n");
        return false;
    }

    btn_fullscreen = new Button( toggleImage, toggleImageOver, NULL, 
        40, 360, 0, EVENT_FULLSCREEN_BUTTON, g_game->font24, "", GOLD, "click", true, true);
    if (!btn_fullscreen) return false;
    if (!btn_fullscreen->IsInitialized()) return false;

    string fullscreen="";
    try {
        if (g_game->getGlobalBoolean("FULLSCREEN"))
            fullscreen="X";
    }
    catch(...)    {    }
    btn_fullscreen->SetButtonText(fullscreen);
   
	
	return true;
}

void ModuleSettings::Close()
{
	ALLEGRO_DEBUG("*** ModuleSettings closing\n");
	try {
        if (btn_fullscreen!=NULL)
        {
            btn_fullscreen->Destroy();
            btn_fullscreen=NULL;
        }
		if(btn_exit != NULL){
			btn_exit->Destroy();
			btn_exit = NULL;
		}
		for(int i=0; i<10; i++){
			if(btn_controls[i] != NULL){
				btn_controls[i]->Destroy();
				btn_controls[i] = NULL;
			}
		}
		if(btn_save != NULL){
			btn_save->Destroy();
			btn_save = NULL;
		}
        if (btn_fullscreen) {
            btn_fullscreen->Destroy();
            btn_fullscreen=NULL;
        }
	}
	catch(std::exception e) {
		ALLEGRO_DEBUG("%s\n", e.what());
	}
	catch(...) {
		ALLEGRO_DEBUG("Unhandled exception in ModuleSettings::Close\n");
	}
}
	
void ModuleSettings::Draw()
{
	//draw background
    al_set_target_bitmap(g_game->GetBackBuffer());
    al_draw_bitmap(background, 0, 0, 0);

    btn_save->Run(g_game->GetBackBuffer());
	btn_exit->Run(g_game->GetBackBuffer());


    //Show screen resolutions
    g_game->Print32(g_game->GetBackBuffer(), 40, 30, "VIDEO MODES");
    g_game->Print24(g_game->GetBackBuffer(), 40, 70, "MODE: " + chosenResolution);
    resScrollbox->SetX(40);
    resScrollbox->SetY(110);
    resScrollbox->Draw(g_game->GetBackBuffer());

	//text
    g_game->Print32(g_game->GetBackBuffer(), 680, 30, "CONTROLS");
	int x = btn_controls[0]->GetX() + btn_controls[0]->GetWidth() + 10;
    int y = btn_controls[0]->GetY()+5;

	g_game->Print20(g_game->GetBackBuffer(), x, y, "Forward", WHITE); 	y+=40;
	g_game->Print20(g_game->GetBackBuffer(), x, y, "Turn left", WHITE);	y+=40;
	g_game->Print20(g_game->GetBackBuffer(), x, y, "Turn right", WHITE); 	y+=40;
	g_game->Print20(g_game->GetBackBuffer(), x, y, "Reverse", WHITE); 	y+=40;
	g_game->Print20(g_game->GetBackBuffer(), x, y, "Strafe left", WHITE);   	y+=40;
	g_game->Print20(g_game->GetBackBuffer(), x, y, "Strafe right", WHITE);  	y+=40;
	g_game->Print20(g_game->GetBackBuffer(), x, y, "Fire laser or stunner", WHITE);    	y+=40;
	g_game->Print20(g_game->GetBackBuffer(), x, y, "Fire missile", WHITE);  	y+=40;
	g_game->Print20(g_game->GetBackBuffer(), x, y, "Toggle shield", WHITE); 	y+=40;
	g_game->Print20(g_game->GetBackBuffer(), x, y, "Toggle weapons", WHITE);    y+=40;
    g_game->Print20(g_game->GetBackBuffer(), x, y, "Select bridge station", WHITE);

	//draw buttons for each command
	for(int i=0; i<11; i++)
    {
		btn_controls[i]->Run(g_game->GetBackBuffer());
	}

    //draw fullscreen toggle
    btn_fullscreen->Run(g_game->GetBackBuffer());
    x = btn_fullscreen->GetX()+60;
    y = btn_fullscreen->GetY();
    g_game->Print24(g_game->GetBackBuffer(), x, y, "FULLSCREEN", WHITE);
}

#pragma region INPUT

void ModuleSettings::OnKeyReleased(int keyCode)
{
	if (keyCode == ALLEGRO_KEY_ESCAPE)
    { 
		g_game->modeMgr->LoadModule(MODULE_TITLESCREEN);
		return;
	}

	if(keyCode != 0 && cmd_selected != 0)
    {
		cmd_selected = 0;
	}
}

void ModuleSettings::OnMouseMove(int x, int y)
{
    resScrollbox->OnMouseMove(x,y);
	btn_exit->OnMouseMove(x, y);
	btn_save->OnMouseMove(x, y);
    btn_fullscreen->OnMouseMove(x, y);
}

void ModuleSettings::OnMouseClick(int button, int x, int y)
{
    resScrollbox->OnMouseClick(button, x, y);
}

void ModuleSettings::OnMousePressed(int button, int x, int y)
{
    resScrollbox->OnMousePressed(button, x, y);
}

void ModuleSettings::OnMouseReleased(int button, int x, int y)
{
    resScrollbox->OnMouseReleased(button, x, y);
	btn_exit->OnMouseReleased(button, x, y);
	btn_save->OnMouseReleased(button, x, y);
    btn_fullscreen->OnMouseReleased(button, x, y);
}
void ModuleSettings::OnMouseWheelUp(int x, int y)
{
    resScrollbox->OnMouseWheelUp(x, y);
}

void ModuleSettings::OnMouseWheelDown(int x, int y)
{
    resScrollbox->OnMouseWheelDown(x, y);
}

#pragma endregion

//Save configuration settings back INTO the Config.lua file
bool ModuleSettings::SaveConfigurationFile()
{
    string audio_global="false";
    if (g_game->getGlobalBoolean("AUDIO_GLOBAL"))
        audio_global="true";

    string audio_music="false";
    if (g_game->getGlobalBoolean("AUDIO_MUSIC"))
        audio_music="true";

    string fullscreen="false";
    if (btn_fullscreen->GetButtonText() == "X")
        fullscreen="true";

    

    std::ofstream configfile;
    configfile.open("data/config.lua", std::ofstream::out | std::ofstream::trunc);
    if (!configfile.is_open())
    {
        ALLEGRO_DEBUG("ModuleSettings: error opening config.lua\n");
        return false;
    }
    
    configfile << "--CONFIGURATION SETTINGS" << std::endl;
    configfile << "--This file is overwritten by the settings screen but can be manually edited." << std::endl;
    configfile << std::endl;
    configfile << "RESOLUTION = \"" << chosenResolution << "\"" << std::endl;
    configfile << "FULLSCREEN = " << fullscreen << std::endl;
    configfile << "AUDIO_GLOBAL = " << audio_global <<  std::endl;
    configfile << "AUDIO_MUSIC = " << audio_music << std::endl;
    configfile.close();

    //use new video mode
    int cx = SCREEN_WIDTH/2;
    int cy = SCREEN_HEIGHT/2;
    g_game->ShowMessageBoxWindow("SETTINGS SAVED","Please restart the program for changes to take effect...", 
        400, 250, WHITE, cx, cy, true);

    return true;
}


void ModuleSettings::OnEvent(Event *event)
{
    string text;
	switch(event->getEventType()) 
    {
        case EVENT_RES_CHANGE:
            if (resScrollbox->GetSelectedItem().length() > 0)
            {
                old_resolution = chosenResolution;
                chosenResolution = resScrollbox->GetSelectedItem();
                ALLEGRO_DEBUG("Resolution change: %s\n", chosenResolution.c_str());
            }
            break;

        case EVENT_FULLSCREEN_BUTTON:
            text = btn_fullscreen->GetButtonText();
            if (text == "X") text = ""; else text = "X";
            btn_fullscreen->SetButtonText(text);
            break;

		case EVENT_SAVESETTINGS:
            //save resolution
            if (chosenResolution != old_resolution)
            {
                old_resolution = chosenResolution;
                SaveConfigurationFile();
            }
			break;

        case EVENT_EXIT:
			g_game->modeMgr->LoadModule(MODULE_TITLESCREEN);
			break;

	}
}

void ModuleSettings::Update() { }
