#ifndef MODULECAPTAINCREATION_H
#define MODULECAPTAINCREATION_H
#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#
#include "AudioSystem.h"
#include "Button.h"
#include "GameState.h"
#include "Label.h"
#include "Module.h"
#include "ModuleCaptainDetails.h"
#include "ModuleProfessionChoice.h"
#include "ResourceManager.h"

class ModuleCaptainCreation : public Module {
  public:
    ModuleCaptainCreation();
    virtual ~ModuleCaptainCreation();
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

  private:
    void chooseFreelance();
    void chooseMilitary();
    void chooseScience();

    ResourceManager<ALLEGRO_BITMAP> m_resources;
    bool m_click_loaded;
    typedef enum
    {
        WP_NONE = 0,
        WP_PROFESSION_CHOICE = 1,
        WP_DETAILS = 2
    } WizPage;

    WizPage m_wizPage;

    ModuleProfessionChoice *m_profession_choice;
    ModuleCaptainDetails *m_captain_details;

    NewButton *m_back_button;
    Button *m_finishBtn;

    ALLEGRO_BITMAP *m_cursor[2];
    int m_cursorIdx;
    int m_cursorIdxDelay;

    ALLEGRO_BITMAP *m_mouseOverImg;
    int m_mouseOverImgX;
    int m_mouseOverImgY;

    std::shared_ptr<Sample> m_sndBtnClick;
    std::shared_ptr<Sample> m_sndClick;
    std::shared_ptr<Sample> m_sndErr;

    // in progress captain vars; once finished with creation, these
    // get stored to the game state
    ProfessionType m_profession;
    std::string m_name;
    Attributes m_attributes;

    // intermediate captain vars used while creating the captain
    Attributes m_attributesMax;
    Attributes m_attributesInitial;
    int m_availPts;
    int m_availProfPts;
};

#endif
// vi: ft=cpp
