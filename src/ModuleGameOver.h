/*
        STARFLIGHT - THE LOST COLONY
        ModuleGameOver.h
        Date: October, 2007
*/

#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "Module.h"

class ModuleGameOver : public Module {
  private:
  public:
    ModuleGameOver();
    virtual ~ModuleGameOver();
    virtual bool on_update() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;

    bool bQuickShutdown;
};

#endif
