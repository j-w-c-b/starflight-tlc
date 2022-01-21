#pragma once

#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "Module.h"
#include "Stardate.h"
#include <allegro5/allegro.h>

/*
current problem is that the default button has some overlapping problems
It isn't showing up, or if the order is changed another button doesn't show.
*/
class ModuleSettings : public Module {
  public:
    ModuleSettings(void);
    virtual ~ModuleSettings(void);
    virtual bool Init() override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void OnKeyReleased(int keyCode) override;
    virtual void OnMouseMove(int x, int y) override;
    virtual void OnMouseClick(int button, int x, int y) override;
    virtual void OnMousePressed(int button, int x, int y) override;
    virtual void OnMouseReleased(int button, int x, int y) override;
    virtual void OnMouseWheelUp(int x, int y) override;
    virtual void OnMouseWheelDown(int x, int y) override;
    virtual void OnEvent(Event *event) override;
    virtual void Close() override;
    bool SaveConfigurationFile();

  private:
    ALLEGRO_BITMAP *background;
    Button *btn_exit, *btn_fullscreen, *btn_controls[11], *btn_defaults,
        *btn_save;

    ScrollBox::ScrollBox *resScrollbox;
    std::string chosenResolution;
    int cmd_selected, button_selected;
};
