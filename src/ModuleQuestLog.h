/*
	STARFLIGHT - THE LOST COLONY
	ModuleStarmap.h - The QuestLog Module
	Author: Justin Sargent
	Date: Nov-24-2007
*/	

#ifndef MODULEQUESTLOG_H
#define MODULEQUESTLOG_H

#include "Module.h"
#include <allegro5/allegro.h>
#include "GameState.h"
#include "EventMgr.h"
#include "Label.h"
#include "ResourceManager.h"

class ModuleQuestLog : public Module
{
public:
	ModuleQuestLog();
	virtual ~ModuleQuestLog();
	virtual bool Init() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void OnEvent(Event *event) override;
	virtual void Close() override;

private:
	bool log_active;
	int viewer_offset_x, viewer_offset_y;

	ALLEGRO_BITMAP *window;

	Label *questName;
	Label *questDesc;
	ResourceManager<ALLEGRO_BITMAP> resources;

};

#endif
