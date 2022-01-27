/*
        STARFLIGHT - THE LOST COLONY
        ModuleInterstellarTravel.h - Handles interstellar travel on the main
   viewscreen Author: J.Harbour Date: January, 2007

        Flux system: Keith Patch
*/

#ifndef INTERSTELLARTRAVEL_H
#define INTERSTELLARTRAVEL_H

#include "DataMgr.h"
#include "Flux.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "Module.h"
#include "PlayerShipSprite.h"
#include "ResourceManager.h"
#include "ScrollBox.h"
#include "Sprite.h"
#include "TileScroller.h"
#include "Timer.h"
#include "Util.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <math.h>
#include <typeinfo>
#include <vector>

class ModuleInterstellarTravel : public Module {
  private:
    TileScroller *scroller;

    int controlKey;
    int shiftKey;
    int starFound;

    Officer *tempOfficer;

    const Flux *flux;
    Sprite *shield;

    bool flag_Shields;
    bool flag_Weapons;
    bool flag_Engaged;
    Timer timerEngaged;
    std::string alienRaceText, alienRaceTextPlural, depth;
    AlienRaces alienRace;
    double roll, proximity, odds;
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

    // shortcuts to crew last names to simplify code
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
    int getFleetSizeByRace(bool small_fleet);
    void load_flux();
    bool enter_flux();
    void identify_flux();
    void AugmentFuel(float percentage);
    bool RollEncounter(AlienRaces forceThisRace = ALIEN_NONE);
    void EnterStarSystem();
    double getPlayerGalacticX();
    double getPlayerGalacticY();
    double Distance(double x1, double y1, double x2, double y2);
    ResourceManager<ALLEGRO_BITMAP> resources;

  public:
    ModuleInterstellarTravel();
    ~ModuleInterstellarTravel();
    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_key_down(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_key_up(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_down(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;
};

#endif
