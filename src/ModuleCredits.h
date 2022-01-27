/*
        STARFLIGHT - THE LOST COLONY
        ModuleCredits.h - ?
        Author: Justin Sargent
        Date: 9/21/07
*/

#ifndef MODULECREDITS_H
#define MODULECREDITS_H

#include "GameState.h"
#include "Module.h"
#include "ResourceManager.h"

class ModuleCredits : public Module {
  public:
    ModuleCredits();
    virtual ~ModuleCredits();
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_close() override;

  private:
    ALLEGRO_BITMAP *background;
    ResourceManager<ALLEGRO_BITMAP> resources;
};

#endif
