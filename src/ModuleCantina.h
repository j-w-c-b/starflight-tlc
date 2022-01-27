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
    virtual ~ModuleCantina();
    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

  private:
    ALLEGRO_BITMAP *m_background;
    Button *m_exitBtn;
    Button *m_turninBtn;

    Label *questTitle;
    Label *questLong;
    Label *questReward;
    Label *m_title_label;
    Label *m_description_label;
    Label *m_reward_label;
    Label *m_requirement_label;

    bool selectedQuestCompleted;
    ResourceManager<ALLEGRO_BITMAP> resources;
};

#endif
