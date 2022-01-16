/*
	STARFLIGHT - THE LOST COLONY
	ModuleInterstellarTravel.h - Handles interstellar travel on the main viewscreen
	Author: J.Harbour
	Date: January, 2007

	Flux system: Keith Patch
*/

#ifndef INTERSTELLARTRAVEL_H
#define INTERSTELLARTRAVEL_H

#include <vector>
#include <typeinfo>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <math.h>
#include "Module.h"
#include "TileScroller.h"
#include "Flux.h"
#include "Sprite.h"
#include "Timer.h"
#include "Util.h"
#include "GameState.h"
#include "DataMgr.h"
#include "PlayerShipSprite.h"
#include "ScrollBox.h"
#include "ModeMgr.h"
#include "ResourceManager.h"


class ModuleInterstellarTravel : public Module
{
private:

	TileScroller	*scroller;

	int				controlKey;
	int				shiftKey;
	int				starFound;

	Officer* tempOfficer;

	Flux* flux;
	Sprite *shield;

	bool flag_Shields;
	bool flag_Weapons;
	bool flag_Engaged;
	Timer timerEngaged;
	std::string alienRaceText,alienRaceTextPlural,depth;
	AlienRaces alienRace;
	double roll,proximity,odds;
	int movement_counter;

	int currentStar;
	float ratiox;
	float ratioy;
	Star *starSystem;
	PlayerShipSprite *ship;
	ScrollBox::ScrollBox *text;
	bool flag_DoNormalSpace;
	bool flag_FoundFlux;
	bool flag_nav;
	bool flag_thrusting;
	int flag_rotation;
	bool flag_launchEncounter;


	//shortcuts to crew last names to simplify code
	std::string cap;
	std::string com;
	std::string sci;
	std::string nav;
	std::string tac;
	std::string eng;
	std::string doc;

	ALLEGRO_BITMAP *img_gui;

	void loadGalaxyData();
	void createGalaxy();
	void identifyStar();
	void calculateEnemyFleetSize();
	int getFleetSizeByRace( bool small_fleet );
	void load_flux();
	void place_flux_exits();
	void identify_flux();
	void AugmentFuel(float percentage);
	bool RollEncounter(AlienRaces forceThisRace = ALIEN_NONE);
	void EnterStarSystem();
	double getPlayerGalacticX();
	double getPlayerGalacticY();
	double Distance( double x1,double y1,double x2,double y2 );
	ResourceManager<ALLEGRO_BITMAP> resources;

public:
	ModuleInterstellarTravel(void);
	~ModuleInterstellarTravel(void);
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
