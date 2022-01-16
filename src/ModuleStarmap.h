/*
	STARFLIGHT - THE LOST COLONY
	ModuleStarmap.h - The Starmap module.
	Author: Keith "Daikaze" Patch
	Date: ??-??-2007
*/	

#ifndef MODULESTARMAP_H
#define MODULESTARMAP_H

#include "Module.h"
#include "Sprite.h"
#include "Flux.h"
#include "Label.h"
#include "DataMgr.h"
#include "ResourceManager.h"

class ModuleStarmap : public Module
{
public:
	ModuleStarmap();
	virtual ~ModuleStarmap();
	virtual bool Init() override;
	void Update() override;
	virtual void Draw() override;
	virtual void OnKeyPress( int keyCode ) override;
	virtual void OnKeyPressed(int keyCode) override;
	virtual void OnKeyReleased(int keyCode) override;
	virtual void OnMouseMove(int x, int y) override;
	virtual void OnMouseClick(int button, int x, int y) override;
	virtual void OnMousePressed(int button, int x, int y) override;
	virtual void OnMouseReleased(int button, int x, int y) override;
	virtual void OnMouseWheelUp(int x, int y) override;
	virtual void OnMouseWheelDown(int x, int y) override;
	virtual void OnEvent(Event *event) override;
	virtual void Close() override;

private:
	bool map_active, dest_active, m_bOver_Star;
	float FUEL_PER_UNIT, ratioX, ratioY;
	int	 VIEWER_WIDTH,
		 VIEWER_HEIGHT,
		 MAP_WIDTH,
		 MAP_HEIGHT,
		 X_OFFSET,
		 Y_OFFSET,
		 MAP_POS_X,
		 MAP_POS_Y,
		 VIEWER_TARGET_OFFSET,
		 VIEWER_MOVE_RATE,
		 viewer_offset_y;

	ALLEGRO_BITMAP *starview;
	ALLEGRO_BITMAP *gui_starmap;
	ALLEGRO_BITMAP *text;
	ALLEGRO_BITMAP *flux_view;

	Point2D cursorPos;
	Point2D m_destPos;

	Label* star_label;
	CoordValue star_x;
	CoordValue star_y;
	ResourceManager<ALLEGRO_BITMAP> resources;
};

#endif
