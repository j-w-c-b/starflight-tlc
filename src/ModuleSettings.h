#pragma once

#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "Module.h"
#include "ResourceManager.h"
#include "Stardate.h"
#include <allegro5/allegro.h>

/*
current problem is that the default button has some overlapping problems
It isn't showing up, or if the order is changed another button doesn't show.
*/
class ModuleSettings : public Module {
  public:
    ModuleSettings();
    virtual ~ModuleSettings();
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_down(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

    bool SaveConfigurationFile();

  private:
    Button *btn_exit, *btn_fullscreen, *btn_controls[11], *btn_defaults,
        *btn_save;

    ScrollBox::ScrollBox *resScrollbox;
    std::string chosenResolution;
    int button_selected;
    ResourceManager<ALLEGRO_BITMAP> m_resources;
};
