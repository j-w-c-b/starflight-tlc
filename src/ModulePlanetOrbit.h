/*
        STARFLIGHT - THE LOST COLONY
        ModulePlanetOrbit.h - Handles planet orbiting
        Author: J.Harbour
        Date: March, 2007
*/

#ifndef _PLANETORBIT_H
#define _PLANETORBIT_H

#include "AudioSystem.h"
#include "DataMgr.h"
#include "Module.h"
#include "ResourceManager.h"
#include "ScrollBox.h"
#include "TexturedSphere.h"
#include <allegro5/allegro.h>

const int HOMEWORLD_ID = 8;

class ModulePlanetOrbit : public Module {
  private:
    ~ModulePlanetOrbit(void);

    bool CreatePlanetTexture();

    std::shared_ptr<Sample> audio_scan;

    // shortcuts to crew last names to simplify code
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
    int starid;
    int planetid;
    int planetRadius;
    int lightmapOffsetX, lightmapOffsetY;
    bool flag_DoDock;
    double planetRotationSpeed, planetRotation;
    bool gui_viewer_sliding;
    ALLEGRO_BITMAP *planet_topography, *planet_scanner_map, *planet_texture;

    int planetScan;
    int planetAnalysis;

    PlanetType planetType;
    Planet *planet;

    ALLEGRO_BITMAP *lightmap_overlay;

    TexturedSphere *texsphere;
    ResourceManager<ALLEGRO_BITMAP> m_resources;

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
