/*
        STARFLIGHT - THE LOST COLONY
        ModulePlanetOrbit.h - Handles planet orbiting
        Author: J.Harbour
        Date: March, 2007
*/

#ifndef _PLANETORBIT_H
#define _PLANETORBIT_H

#include <allegro5/allegro.h>

#include "AudioSystem.h"
#include "DataMgr.h"
#include "Module.h"
#include "RichTextLabel.h"
#include "ScrolledModule.h"
#include "TexturedSphere.h"

const int HOMEWORLD_ID = 8;

class ModulePlanetOrbit : public Module {
  public:
    ModulePlanetOrbit();
    ~ModulePlanetOrbit();

    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

    void doorbit();
    void dosurface();
    void scanplanet();
    void analyzeplanet();

  private:
    bool CreatePlanetTexture();

    std::shared_ptr<ScrolledModule<RichTextLabel>> m_text;

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
    const Planet *planet;

    std::shared_ptr<ALLEGRO_BITMAP> lightmap_overlay;

    TexturedSphere *texsphere;
};

#endif
