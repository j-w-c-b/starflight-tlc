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
    ~ModuleTopGUI();
    virtual bool Init() override;
    virtual void Update() override;
    void UpdateInjector();
    virtual void Draw() override;
    virtual void Close() override;

  private:
    ResourceManager<ALLEGRO_BITMAP> m_resources;
};

#endif
