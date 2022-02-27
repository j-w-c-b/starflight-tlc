/*
        STARFLIGHT - THE LOST COLONY
        ModuleCrewHire.h - ?
        Author: ?
        Date: 9/21/07
*/

#ifndef MODULECANTINA_H
#define MODULECANTINA_H

#include <allegro5/allegro.h>

#include "Bitmap.h"
#include "Button.h"
#include "Label.h"
#include "Module.h"

class ModuleCantina : public Module {
  public:
    ModuleCantina();
    virtual ~ModuleCantina() {}
    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
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

    enum quest_status
    {
        QUEST_ACTIVE,
        QUEST_COMPLETE,
        QUEST_DEBRIEF,
        QUEST_DEBRIEF_COMPLETE,
        QUEST_REWARD,
        QUEST_REWARD_COMPLETE,
    };

    quest_status m_quest_status;
};

#endif
// vi: ft=cpp
