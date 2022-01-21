/*
        STARFLIGHT - THE LOST COLONY
        ModuleTopGUI.cpp
        Author: Keith Patch
        Date: April 2008
*/

#ifndef _MODULETOPGUI_H
#define _MODULETOPGUI_H

#include "Button.h"
#include "Module.h"

class ModuleTopGUI : public Module {
  private:
    ALLEGRO_BITMAP *img_gauges;
    ALLEGRO_BITMAP *img_fuel_gauge;
    ALLEGRO_BITMAP *img_hull_gauge;
    ALLEGRO_BITMAP *img_shield_gauge;
    ALLEGRO_BITMAP *img_armor_gauge;

  public:
    ModuleTopGUI();
    ~ModuleTopGUI();
    virtual bool Init() override;
    virtual void Update() override;
    void UpdateInjector();
    virtual void Draw() override;
    virtual void Close() override;
};

#endif
