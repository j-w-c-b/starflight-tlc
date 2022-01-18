/*
	STARFLIGHT - THE LOST COLONY
	ModuleInterplanetaryTravel.h - Handles space travel in a star system among the planets
	Author: J.Harbour
	Date: Feb, 2007
*/

#ifndef INTERPLANETARYTRAVEL_H
#define INTERPLANETARYTRAVEL_H

#include <math.h>
#include <string>
#include <allegro5/allegro_font.h>
#include "Module.h"
#include "TileScroller.h"
#include "ScrollBox.h"
#include "PlayerShipSprite.h"
#include "GameState.h"
#include "DataMgr.h"
#include "Label.h"
#include "ResourceManager.h"

class ModuleInterPlanetaryTravel : public Module
{
private:
	PlayerShipSprite		*ship;
	TileScroller	*scroller;
	float			acceleration;

	int				planetFound;
	bool			distressSignal;

	ALLEGRO_BITMAP *miniMap; 
	ScrollBox::ScrollBox *text;

	int loadStarSystem(int id);
	void updateMiniMap();
	void checkShipPosition();
	bool checkSystemBoundary(int x,int y);
	void Print(std::string str, ALLEGRO_COLOR color, int delay);

	//this struct helps to simplify searches when ship moves over a planet
	struct planet_t {
		int tilex;
		int tiley;
		int tilenum;
		int planetid;
		int radius;
	};

	planet_t planets[10];

	int tilex, tiley, tilenum;
	bool flag_DoOrbit;
	bool flag_DoDock;
	bool flag_DoHyperspace;
	int burning;
	bool m_bOver_Planet;
	int asx, asy;
	Star *star;
	Planet *planet;
	Label* planet_label;
	bool flag_nav;
	bool flag_thrusting;
	int flag_rotation;

	//shortcuts to crew last names to simplify code
	std::string com;
	std::string sci;
	std::string nav;
	std::string tac;
	std::string eng;
	std::string doc;
	ResourceManager<ALLEGRO_BITMAP> resources;

public:
	ModuleInterPlanetaryTravel();
	~ModuleInterPlanetaryTravel();
	virtual bool Init() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void OnKeyPress(int keyCode) override;
	virtual void OnKeyReleased(int keyCode) override;
	virtual void OnMouseMove(int x, int y) override;
	virtual void OnMouseClick(int button, int x, int y) override;
	virtual void OnMousePressed(int button, int x, int y) override;
	virtual void OnMouseReleased(int button, int x, int y) override;
	virtual void OnMouseWheelUp(int x, int y) override;
	virtual void OnMouseWheelDown(int x, int y) override;
	virtual void OnEvent(Event *event) override;
	virtual void Close() override;
};

#endif
