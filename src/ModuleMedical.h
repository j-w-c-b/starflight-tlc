/*
        STARFLIGHT - THE LOST COLONY
        ModuleMedical.cpp
        Author: Keith Patch
        Date: April 2008
*/

#ifndef _MODULEMEDICAL_H
#define _MODULEMEDICAL_H

#include <map>
#include <memory>
#include <optional>

#include <allegro5/allegro.h>

#include "Button.h"
#include "GameState.h"
#include "Label.h"
#include "Module.h"
#include "RichTextLabel.h"
#include "SlidingModule.h"

class ModuleMedical : public Module {
  public:
    ModuleMedical();
    virtual ~ModuleMedical();
    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

  private:
    void update_treat_view(OfficerType officer_type);
    enum
    {
        MEDICAL_MODE_NONE,
        MEDICAL_MODE_TREAT,
        MEDICAL_MODE_EXAMINE
    } m_mode;

    std::map<OfficerType, std::shared_ptr<TextButton>> m_officer_buttons;

    std::optional<OfficerType> m_selected_officer;

    std::shared_ptr<SlidingModule<class MedicalTreatView>> m_viewer_treat;
    std::shared_ptr<SlidingModule<class MedicalExamineView>> m_viewer_examine;
    std::shared_ptr<SlidingModule<Module>> m_viewer_crewlist;
};

#endif
