/*
        STARFLIGHT - THE LOST COLONY
        ModuleTopGUI.cpp
        Author: Keith Patch
        Date: April 2008
*/

#ifndef _MODULETOPGUI_H
#define _MODULETOPGUI_H

#include "Module.h"
#include "ResourceManager.h"

class ModuleTopGUI : public Module {
  public:
    ModuleTopGUI();
    virtual ~ModuleTopGUI();
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;

  private:
    ResourceManager<ALLEGRO_BITMAP> m_resources;
    void UpdateInjector();
};

#endif
