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
    bool on_event(ALLEGRO_EVENT *event) override;
    bool on_close() override;

  private:
    ResourceManager<ALLEGRO_BITMAP> m_resources;
    Bitmap *m_title;
    NewButton *m_new_game;
    NewButton *m_load_game;
    NewButton *m_settings;
    NewButton *m_credits;
    NewButton *m_quit;
};

#endif
