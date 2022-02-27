/*
        STARFLIGHT - THE LOST COLONY
        ModuleTitleScreen.h -
        Author: J.Harbour
        Date: Dec,2007
*/

#ifndef MODULETITLESCREEN_H
#define MODULETITLESCREEN_H

#include <memory>

#include <allegro5/allegro.h>

#include "Bitmap.h"
#include "Button.h"
#include "Module.h"

class ModuleTitleScreen : public Module {
  public:
    ModuleTitleScreen();

    bool on_init() override;
    bool on_close() override;
    bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    bool on_event(ALLEGRO_EVENT *event) override;
};

#endif
