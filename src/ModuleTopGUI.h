/*
        STARFLIGHT - THE LOST COLONY
        ModuleTopGUI.cpp
        Author: Keith Patch
        Date: April 2008
*/

#ifndef MODULETOPGUI_H
#define MODULETOPGUI_H

#include <allegro5/allegro.h>

#include "Module.h"

class ModuleTopGUI : public Module {
  public:
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
};

#endif
