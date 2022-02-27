#ifndef PERSONNEL_SLOT_BUTTON_H
#define PERSONNEL_SLOT_BUTTON_H

#include <memory>
#include <optional>

#include <allegro5/allegro.h>

#include "Button.h"
#include "GameState.h"

class PersonnelSlotButton : public Button {
  public:
    PersonnelSlotButton(int x, int y, OfficerType type);
    static constexpr int spacing = 59;

    OfficerType get_officer_type() const { return m_officer_type; }

    bool has_officer() { return m_officer != nullptr; }

    const Officer *get_officer() { return m_officer; }
    const Officer *set_officer(const Officer *officer);
    const Officer *fire_officer();
    const Officer *unassign_officer();

  private:
    std::shared_ptr<ALLEGRO_BITMAP>
    make_normal_bitmap(OfficerType officer_type);
    std::shared_ptr<ALLEGRO_BITMAP> make_over_bitmap(OfficerType officer_type);
    std::shared_ptr<ALLEGRO_BITMAP>
    make_disabled_bitmap(OfficerType officer_type);
    std::shared_ptr<Label> m_name_label;

    OfficerType m_officer_type;
    const Officer *m_officer;
};

#endif // PERSONNEL_SLOT_BUTTON_H
// vi: ft=cpp
