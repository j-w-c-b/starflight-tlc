/*
        STARFLIGHT - THE LOST COLONY
        ModuleInterplanetaryTravel.h - Handles space travel in a star system
   among the planets Author: J.Harbour Date: Feb, 2007
*/

#ifndef INTERPLANETARYTRAVEL_H
#define INTERPLANETARYTRAVEL_H

#include <string>

#include <allegro5/allegro_font.h>

#include "DataMgr.h"
#include "GameState.h"
#include "Label.h"
#include "Module.h"
#include "PlayerShipSprite.h"
#include "RichTextLabel.h"
#include "ScrolledModule.h"
#include "TileScroller.h"

class ModuleInterPlanetaryTravel : public Module {
  private:
    PlayerShipSprite *ship;
    std::shared_ptr<TileScroller> m_scroller;
    float acceleration;

    bool distressSignal;

    ALLEGRO_BITMAP *miniMap;

    int loadStarSystem(const Star *star);
    void updateMiniMap(ALLEGRO_BITMAP *target);
    void checkShipPosition();
    bool checkSystemBoundary(int x, int y);
    void Print(std::string str, ALLEGRO_COLOR color, int delay);
    void enter_system();

    // this struct helps to simplify searches when ship moves over a planet
    struct planet_t {
        int tilex;
        int tiley;
        int tilenum;
        int planetid;
        int radius;
        ALLEGRO_COLOR color;
    };

    planet_t planets[10];

    int tilex, tiley, tilenum;
    bool flag_DoOrbit;
    bool flag_DoDock;
    bool flag_DoHyperspace;
    int burning;
    bool m_bOver_Planet;
    int asx, asy;
    const Star *star;
    const Planet *planet;
    std::shared_ptr<Label> m_planet_label;
    bool flag_nav;
    bool flag_thrusting;
    int flag_rotation;

  public:
    ModuleInterPlanetaryTravel();
    ~ModuleInterPlanetaryTravel();
    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_key_down(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_key_up(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;
};

#endif
