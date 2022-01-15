/*
	STARFLIGHT - THE LOST COLONY
	ModuleEngineer.h - The Engineering module.
	Author: Keith "Daikaze" Patch
	Date: 5-27-2008
*/	

#ifndef MODULEENGINEER_H
#define MODULEENGINEER_H

#include "Module.h"
#include "Button.h"
#include "ScrollBox.h"
#include "GameState.h"
#include "ResourceManager.h"

class ModuleEngineer : public Module
{
public:
	ModuleEngineer();
	virtual ~ModuleEngineer();
	virtual bool Init() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void OnMouseMove(int x, int y) override;
	virtual void OnMouseReleased(int button, int x, int y) override;
	virtual void OnEvent(Event *event) override;
	virtual void Close() override;
private:
	bool useMineral(Ship &ship);
	bool module_active;
	ALLEGRO_BITMAP* img_window;
	ALLEGRO_BITMAP* img_bar_base;
	ALLEGRO_BITMAP* text;
	ALLEGRO_BITMAP* img_bar_laser ;
	ALLEGRO_BITMAP* img_bar_missile;
	ALLEGRO_BITMAP* img_bar_hull;
	ALLEGRO_BITMAP* img_bar_armor;
	ALLEGRO_BITMAP* img_bar_shield;
	ALLEGRO_BITMAP* img_bar_engine;
	ALLEGRO_BITMAP* img_ship;
	ALLEGRO_BITMAP* img_button_repair;
	ALLEGRO_BITMAP* img_button_repair_over;
	Button* button[5];
	int	 VIEWER_WIDTH,
		 VIEWER_HEIGHT,
		 VIEWER_TARGET_OFFSET,
		 VIEWER_MOVE_RATE,
		 viewer_offset_y;
	ResourceManager<ALLEGRO_BITMAP> resources;
};

#endif
