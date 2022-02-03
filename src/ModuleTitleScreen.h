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
    std::shared_ptr<Bitmap> m_title;
    std::shared_ptr<NewButton> m_new_game;
    std::shared_ptr<NewButton> m_load_game;
    std::shared_ptr<NewButton> m_settings;
    std::shared_ptr<NewButton> m_credits;
    std::shared_ptr<NewButton> m_quit;
};

#endif
