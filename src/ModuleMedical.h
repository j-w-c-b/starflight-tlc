/*
        STARFLIGHT - THE LOST COLONY
        ModuleMedical.cpp
        Author: Keith Patch
        Date: April 2008
*/

#ifndef _MODULEMEDICAL_H
#define _MODULEMEDICAL_H

#include "Button.h"
#include "GameState.h"
#include "Module.h"
#include "ResourceManager.h"

class ModuleMedical : public Module {
  private:
    ALLEGRO_BITMAP *img_right_viewer;
    ALLEGRO_BITMAP *img_right_bg;
    ALLEGRO_BITMAP *img_left_viewer;  // shows stat bars for examining officers
    ALLEGRO_BITMAP *img_left_viewer2; // shows health readout.
    ALLEGRO_BITMAP *img_left_bg;
    ALLEGRO_BITMAP *img_health_bar, *img_science_bar, *img_nav_bar,
        *img_medical_bar, *img_engineer_bar, *img_dur_bar, *img_learn_bar,
        *img_comm_bar, *img_tac_bar;
    ALLEGRO_BITMAP *img_button_crew, *img_button_crew_hov, *img_button_crew_dis;
    ALLEGRO_BITMAP *img_treat, *img_treat_hov, *img_treat_dis;
    bool viewer_active, b_examine;
    Button *HealBtns[7];
    Button *OfficerBtns[7];

    Officer *selected_officer;
    ResourceManager<ALLEGRO_BITMAP> resources;

    void disable_others(int officer);
    void cease_healing();

  public:
    ModuleMedical();
    virtual ~ModuleMedical();
    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;
};

#endif
