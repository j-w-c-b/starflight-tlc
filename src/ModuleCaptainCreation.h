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

class ModuleCaptainCreation : public Module {
  public:
    ModuleCaptainCreation();
    virtual ~ModuleCaptainCreation();
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    void chooseFreelance();
    void chooseMilitary();
    void chooseScience();

    enum WizPage
    {
        WP_NONE = 0,
        WP_PROFESSION_CHOICE = 1,
        WP_DETAILS = 2
    };

    WizPage m_wizPage;

    std::shared_ptr<ModuleProfessionChoice> m_profession_choice;
    std::shared_ptr<ModuleCaptainDetails> m_captain_details;

    std::shared_ptr<Button> m_back_button;

    using Attributes = std::map<Skill, int>;
    // intermediate captain vars used while creating the captain
    Attributes m_attributesMax;
    Attributes m_attributesInitial;
};

#endif
// vi: ft=cpp
