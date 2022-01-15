/*
	STARFLIGHT - THE LOST COLONY
	ModeMgr.cpp - ?
	Author: ?
	Date: ?
*/

#include <allegro5/allegro.h>

#include "ModeMgr.h"
#include "Module.h"
#include "GameState.h"
#include "Game.h"
#include "MessageBoxWindow.h"
#include "PauseMenu.h"
#include "AudioSystem.h"

using namespace std;

ALLEGRO_DEBUG_CHANNEL("ModeMgr")

Mode::Mode(Module *module, const string &path) : rootModule(module), musicPath(path)
{}

ModeMgr::ModeMgr(Game * /* game*/):
	m_activeRootModule(nullptr),
	currentMusic(nullptr),
        prevModeName(""),
        currentModeName("")
{
}

ModeMgr::~ModeMgr()
{
	ALLEGRO_DEBUG("[DESTROYING MODULES]\n");
	try {
		map<string,Mode *>::iterator i;
		i = m_modes.begin(); 

		bool isOperationsRoom, operationsRoomDeleted= false; //needed!! (cannot delete same object 3x)
		while ( i != m_modes.end() )
		{
			if (i->first.length() > 0) {
				if (  (strcmp(i->first.c_str(), "CANTINA") == 0)
					||(strcmp(i->first.c_str(), "RESEARCHLAB") == 0)
					||(strcmp(i->first.c_str(), "MILITARYOPS") == 0))
					isOperationsRoom= true;
				else
					isOperationsRoom= false;
					
				if ((!isOperationsRoom) || (!operationsRoomDeleted)) {
					if (isOperationsRoom) operationsRoomDeleted= true;
					ALLEGRO_DEBUG("  Destroying %s\n", i->first.c_str());
					delete i->second;
				}
				else {
					ALLEGRO_DEBUG("  Module %s was previously deleted (object assigned 3x)\n", i->first.c_str());
				}
			}
			++i;
		}
	}
	catch(std::exception e) {
		ALLEGRO_DEBUG("%s\n", e.what());
	}
	catch(...) {
		ALLEGRO_DEBUG("Unhandled exception in ~ModeMgr\n");
	}
}

void ModeMgr::AddMode(const string &modeName, Module *rootModule, const string &musicPath) 
{
	if ( musicPath.compare("") != 0) {
		ALLEGRO_FS_ENTRY *entry = al_create_fs_entry(musicPath.c_str());
		if (!al_fs_entry_exists(entry)) {
			std::string error = "ModeMgr::AddMode: [ERROR] file " + musicPath + " does not exist";
			g_game->fatalerror(error);
		}
		al_destroy_fs_entry(entry);
	}

	Mode *newmode = new Mode(rootModule, musicPath);
	m_modes[modeName] = newmode;
}

void ModeMgr::CloseCurrentModule()
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if ( m_activeRootModule == NULL) return;

	m_activeRootModule->Close();
	m_activeRootModule = NULL;
}

bool ModeMgr::LoadModule(const string &newModeName)
{
	bool result = false;

	//disable the Pause Menu
	g_game->pauseMenu->setEnabled(false);

	// search the current and new modes in the m_modes associative array
	map<string,Mode*>::iterator icurr = m_modes.find(currentModeName);
	map<string,Mode*>::iterator inew = m_modes.find(newModeName);

	if ( inew == m_modes.end() ) {
		g_game->message( "Error '" + newModeName + "' is not a valid mode name");
		return false;
	}

	//save module name
	this->prevModeName = this->currentModeName;
	this->currentModeName = newModeName;

	//store module name in gamestate
	g_game->gameState->setCurrentModule(newModeName);

	//the following will always be true except exactly once, at game start
	if ( icurr != m_modes.end() ){
		//close active module
		ALLEGRO_DEBUG("ModeMgr: closing module '%s'\n", this->prevModeName.c_str());
		CloseCurrentModule();
		ALLEGRO_DEBUG("ModeMgr: module '%s' closed\n\n", this->prevModeName.c_str());
	}

	//launch new module 
	ALLEGRO_DEBUG("ModeMgr: initializing module '%s'\n", newModeName.c_str());
	m_activeRootModule = inew->second->rootModule;
	result = m_activeRootModule->Init();
	ALLEGRO_DEBUG("ModeMgr: module '%s' Init(): %s\n", newModeName.c_str(), result? "SUCCESS" : "FAILURE");

	if (!result) return false;


	//handle background music

	//if we don't want music, we are done
	if ( !g_game->getGlobalBoolean("AUDIO_MUSIC") ) return true;

	std::string currentMusicPath = (icurr==m_modes.end())? "" : icurr->second->musicPath;
	std::string newMusicPath = inew->second->musicPath;


	//if new music == current music, we do nothing (iow: we let it play)
	if ( newMusicPath.compare(currentMusicPath) == 0 ) return true;
	//if new music == "", we do nothing either (iow: we let the new module handle it all by itself)
	if ( newMusicPath.compare("") == 0 ) return true;


	//stop the current music, unless we were told not to deal with it.
	if ( currentMusicPath.compare("") != 0 && currentMusic != NULL ){ 
		ALLEGRO_DEBUG("ModeMgr: stop playing music %s\n", currentMusicPath.c_str());
		g_game->audioSystem->Stop(currentMusic);
		currentMusic.reset();
	}


	ALLEGRO_DEBUG("ModeMgr: start playing music %s\n", newMusicPath.c_str());
	currentMusic = g_game->audioSystem->LoadMusic(newMusicPath);
	if (currentMusic == nullptr) {
		g_game->message("Error loading music from " + newMusicPath);
		return false;
	}

	if ( !g_game->audioSystem->PlayMusic(currentMusic) ){
		g_game->message("Error playing music from " + newMusicPath);
		return false;
	}

	return true;
}

void ModeMgr::Update()
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if (m_activeRootModule==NULL) return;

	m_activeRootModule->Update();
}

void ModeMgr::Draw()
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if (m_activeRootModule==NULL) return;

	m_activeRootModule->Draw();
}

void ModeMgr::BroadcastEvent(Event *event)
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());

	//determine if messagebox or pausemenu need to be removed
	//These are events related to these pop-up controls
	//EVENT_CLOSE is always a close event for the control
	int evtype = event->getEventType();
        switch (evtype)
        {
		case EVENT_CLOSE:
		case EVENT_SAVE_GAME:
		case EVENT_LOAD_GAME:
		case EVENT_QUIT_GAME:
		//hide the pause menu
		if ( g_game->pauseMenu->isShowing() )
			g_game->TogglePauseMenu();

		//hide the messagebox
		if (g_game->messageBox != NULL)
			g_game->messageBox->SetVisible(false);
		default:;
	}

	if (m_activeRootModule==NULL) return;

	//if this is not a close event, pass it on
	if ( (unsigned int) evtype != EVENT_CLOSE)
		m_activeRootModule->OnEvent(event);
}
 
void ModeMgr::OnKeyPress(int keyCode) 
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if (m_activeRootModule==NULL) return;

	m_activeRootModule->OnKeyPress(keyCode);
}

void ModeMgr::OnKeyPressed(int keyCode)
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if (m_activeRootModule==NULL) return;

	m_activeRootModule->OnKeyPressed(keyCode);
}

void ModeMgr::OnKeyReleased(int keyCode)
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if (m_activeRootModule==NULL) return;

	m_activeRootModule->OnKeyReleased(keyCode);
}

void ModeMgr::OnMouseMove(int x, int y)
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if (m_activeRootModule==NULL) return;

	m_activeRootModule->OnMouseMove(x,y);
}

void ModeMgr::OnMouseClick(int button, int x, int y)
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if (m_activeRootModule==NULL) return;

	m_activeRootModule->OnMouseClick(button,x,y);
}

void ModeMgr::OnMousePressed(int button, int x, int y)
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if (m_activeRootModule==NULL) return;

	m_activeRootModule->OnMousePressed(button, x, y);
}

void ModeMgr::OnMouseReleased(int button, int x, int y)
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if (m_activeRootModule==NULL) return;

	m_activeRootModule->OnMouseReleased(button, x, y);
}

void ModeMgr::OnMouseWheelUp(int x, int y)
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if (m_activeRootModule==NULL) return;

	m_activeRootModule->OnMouseWheelUp(x, y);
}

void ModeMgr::OnMouseWheelDown(int x, int y)
{
	ALLEGRO_ASSERT(m_activeRootModule || !g_game->IsRunning());
	if (m_activeRootModule==NULL) return;

	m_activeRootModule->OnMouseWheelDown(x, y);
}
