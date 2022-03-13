/*
        STARFLIGHT - THE LOST COLONY
        ModuleCrewHire.h - ?
        Author: ?
        Date: 9/21/07
*/

#ifndef MODULECREWHIRE_H
#define MODULECREWHIRE_H

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <allegro5/allegro.h>

#include "Button.h"
#include "GameState.h"
#include "Label.h"
#include "Module.h"
#include "OfficerInfo.h"
#include "PersonnelSlotButton.h"
#include "UnemployedPanel.h"
#include "UnemployedSlotButton.h"

class ModuleCrewHire : public Module {
  public:
    virtual bool on_init() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

  private:
    enum crew_slot_type
    {
        CREW,
        UNASSIGNED
    };
    struct selected_slot {
        crew_slot_type slot_type;
        int slot;
    };
    static std::string c_directions;
    static std::string c_hire_more_directions;
    static std::string c_title;
    static std::string c_statistics_title;

    std::optional<selected_slot> m_selected_slot;

    std::shared_ptr<Label> m_title;
    std::shared_ptr<Label> m_slogan;
    std::shared_ptr<Label> m_directions;

    std::shared_ptr<TextButton> m_exit_button;

    std::shared_ptr<Module> m_personnel_panel;
    std::shared_ptr<TextButton> m_personnel_fire;
    std::shared_ptr<TextButton> m_personnel_unassign;
    std::map<int, std::shared_ptr<PersonnelSlotButton>>
        m_personnel_slot_buttons;
    std::shared_ptr<OfficerInfo> m_officer_info;

    std::shared_ptr<Module> m_personnel_unassigned_panel;
    std::map<int, std::shared_ptr<UnemployedSlotButton>>
        m_personnel_unassigned_slot_buttons;

    std::shared_ptr<UnemployedPanel> m_unemployed_panel;
};

#endif
