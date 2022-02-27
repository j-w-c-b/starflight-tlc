#ifndef UNEMPLOYED_SLOT_BUTTON_H
#define UNEMPLOYED_SLOT_BUTTON_H

#include <memory>
#include <optional>

#include <allegro5/allegro.h>

#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "Label.h"
#include "PersonnelSlotButton.h"

class UnemployedSlotButton : public Button {
  public:
    UnemployedSlotButton(
        int x,
        int y,
        const Officer *officer,
        EventType event_type);
    static constexpr int spacing = 30;

    bool has_officer() const { return m_officer != nullptr; }

    std::optional<OfficerType> get_preferred_profession() const {
        if (m_officer) {
            return m_officer->get_preferred_profession();
        } else {
            return std::nullopt;
        }
    }
    const Officer *get_officer() const { return m_officer; }
    const Officer *set_officer(const Officer *officer);
    const Officer *fire_officer();

  private:
    EventType m_event_type;
    std::shared_ptr<Label> m_name_label;
    std::shared_ptr<Label> m_preferred_role_label;

    const Officer *m_officer;
};

#endif // UNEMPLOYED_SLOT_BUTTON_H
// vi: ft=cpp
