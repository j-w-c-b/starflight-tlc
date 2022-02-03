/*
        STARFLIGHT - THE LOST COLONY
        ModuleCrewHire.h - ?
        Author: ?
        Date: 9/21/07
*/

#ifndef MODULECANTINA_H
#define MODULECANTINA_H

#include <allegro5/allegro.h>

#include "Button.h"
#include "GameState.h"
#include "Label.h"
#include "Module.h"
#include "ResourceManager.h"
#include "ScrollBox.h"

class ModuleCantina : public Module {
  public:
    ModuleCantina();
    virtual ~ModuleCantina() {}
    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    ResourceManager<ALLEGRO_BITMAP> resources;
    bool m_button_click_loaded;

    std::shared_ptr<Bitmap> m_background;

    std::shared_ptr<Label> m_title_heading_label;
    std::shared_ptr<Label> m_title_label;
    std::shared_ptr<Label> m_status_label;
    std::shared_ptr<Label> m_description_title_label;
    std::shared_ptr<Label> m_description_label;
    std::shared_ptr<Label> m_reward_title_label;
    std::shared_ptr<Label> m_reward_label;

    std::shared_ptr<TextButton> m_exit_button;
    std::shared_ptr<TextButton> m_turn_in_button;

    bool m_quest_complete;
    int m_debrief_status;
};

#endif
// vi: ft=cpp
