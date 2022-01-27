/*
        STARFLIGHT - THE LOST COLONY
        ModuleTitleScreen.h -
        Author: J.Harbour
        Date: Dec,2007
*/

#ifndef MODULETITLESCREEN_H
#define MODULETITLESCREEN_H

#include "Button.h"
#include "Module.h"
#include "ResourceManager.h"

class ModuleTitleScreen : public Module {
  public:
    ModuleTitleScreen();
    virtual ~ModuleTitleScreen();

    bool on_init() override;
    bool on_draw(ALLEGRO_BITMAP *target) override;
    bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    bool on_event(ALLEGRO_EVENT *event) override;
    bool on_close() override;

  private:
    float m_rotationAngle;
    ResourceManager<ALLEGRO_BITMAP> m_resources;
    Button *btnTitle;
    Button *btnNewGame;
    Button *btnLoadGame;
    Button *btnSettings;
    Button *btnCredits;
    Button *btnQuit;
    int title_mode;
};

#endif
