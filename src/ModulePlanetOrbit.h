/*
	STARFLIGHT - THE LOST COLONY
	ModulePlanetOrbit.h - Handles planet orbiting
	Author: J.Harbour
	Date: March, 2007
*/

#ifndef _PLANETORBIT_H
#define _PLANETORBIT_H

#include <allegro5/allegro.h>
#include "ScrollBox.h"
#include "Module.h"
#include "DataMgr.h"
#include "AudioSystem.h"
#include "TexturedSphere.h"

const int HOMEWORLD_ID = 8;

class ModulePlanetOrbit : public Module
{
private:
	~ModulePlanetOrbit(void);

	bool CreatePlanetTexture();
	
	ALLEGRO_BITMAP *background;
	std::shared_ptr<Sample> audio_scan;

	//shortcuts to crew last names to simplify code
	std::string com;
	std::string sci;
	std::string nav;
	std::string tac;
	std::string eng;
	std::string doc;

	ScrollBox::ScrollBox *text;

	int gui_viewer_x;
	int gui_viewer_y;
	int gui_viewer_dir;
	bool gui_viewer_sliding;

	int planetScan;
	int planetAnalysis;

	PlanetType planetType;
	Planet *planet;

    ALLEGRO_BITMAP *lightmap_overlay;

    TexturedSphere *texsphere;


public:
	ModulePlanetOrbit(void);
	virtual bool Init() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void OnMouseMove(int x, int y) override;
	virtual void OnMouseClick(int button, int x, int y) override;
	virtual void OnMousePressed(int button, int x, int y) override;
	virtual void OnMouseReleased(int button, int x, int y) override;
	virtual void OnMouseWheelUp(int x, int y) override;
	virtual void OnMouseWheelDown(int x, int y) override;
	virtual void OnEvent(Event *event) override;
	virtual void Close() override;

	void doorbit();
	void dosurface();
	void scanplanet();
	void analyzeplanet();
};

#endif
