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

    typedef enum
    {
        WP_NONE = 0,
        WP_PROFESSION_CHOICE = 1,
        WP_DETAILS = 2
    } WizPage;

    WizPage m_wizPage;

    ALLEGRO_BITMAP *m_professionChoiceBackground;

    ALLEGRO_BITMAP *m_scientificBtn;
    ALLEGRO_BITMAP *m_scientificBtnMouseOver;
    ALLEGRO_BITMAP *m_freelanceBtn;
    ALLEGRO_BITMAP *m_freelanceBtnMouseOver;
    ALLEGRO_BITMAP *m_militaryBtn;
    ALLEGRO_BITMAP *m_militaryBtnMouseOver;

    Label *m_prof_info_label;

    static const std::string c_prof_info_scientific_text;
    static const std::string c_prof_info_freelance_text;
    static const std::string c_prof_info_military_text;

    ALLEGRO_BITMAP *m_detailsBackground;

    ALLEGRO_BITMAP *m_plusBtn;
    ALLEGRO_BITMAP *m_plusBtnMouseOver;

    ALLEGRO_BITMAP *m_resetBtn;
    ALLEGRO_BITMAP *m_resetBtnMouseOver;

    Button *m_finishBtn;

    ALLEGRO_BITMAP *m_cursor[2];
    int m_cursorIdx;
    int m_cursorIdxDelay;

    ALLEGRO_BITMAP *m_backBtn;
    ALLEGRO_BITMAP *m_backBtnMouseOver;

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

    Button *m_minusBtns[8];
    ResourceManager<ALLEGRO_BITMAP> m_resources;
};

#endif
