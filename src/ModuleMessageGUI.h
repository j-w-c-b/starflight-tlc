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
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_close() override;
};

#endif
