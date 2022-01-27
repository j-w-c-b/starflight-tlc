/*
        STARFLIGHT - THE LOST COLONY
        ModuleEngineer.h - The Engineering module.
        Author: Keith "Daikaze" Patch
        Date: 5-27-2008
*/

#ifndef MODULEENGINEER_H
#define MODULEENGINEER_H

#include "Button.h"
#include "GameState.h"
#include "Module.h"
#include "ResourceManager.h"
#include "ScrollBox.h"

class ModuleEngineer : public Module {
  public:
    ModuleEngineer();
    virtual ~ModuleEngineer();
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

  private:
    bool useMineral(Ship &ship);
    bool module_active;
    ALLEGRO_BITMAP *img_window;
    ALLEGRO_BITMAP *img_bar_base;
    ALLEGRO_BITMAP *text;
    ALLEGRO_BITMAP *img_bar_laser;
    ALLEGRO_BITMAP *img_bar_missile;
    ALLEGRO_BITMAP *img_bar_hull;
    ALLEGRO_BITMAP *img_bar_armor;
    ALLEGRO_BITMAP *img_bar_shield;
    ALLEGRO_BITMAP *img_bar_engine;
    ALLEGRO_BITMAP *img_ship;
    ALLEGRO_BITMAP *img_button_repair;
    ALLEGRO_BITMAP *img_button_repair_over;
    Button *button[5];
    int VIEWER_WIDTH, VIEWER_HEIGHT, VIEWER_TARGET_OFFSET, VIEWER_MOVE_RATE,
        viewer_offset_y;
    ResourceManager<ALLEGRO_BITMAP> resources;
};

#endif
