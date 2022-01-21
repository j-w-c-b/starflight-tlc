/*
        STARFLIGHT - THE LOST COLONY
        ModuleMessageGUI.cpp
        Author:
        Date:
*/

#ifndef _ModuleMessageGUI_H
#define _ModuleMessageGUI_H

#include "Button.h"
#include "Module.h"
#include "ResourceManager.h"

class ModuleMessageGUI : public Module {
  private:
    ALLEGRO_BITMAP *img_message;
    ALLEGRO_BITMAP *img_socket;

    ResourceManager<ALLEGRO_BITMAP> resources;

  public:
    ModuleMessageGUI();
    virtual ~ModuleMessageGUI();
    virtual bool Init() override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void Close() override;
};

#endif
