#ifndef UNEMPLOYED_PANEL_H
#define UNEMPLOYED_PANEL_H

#include <map>
#include <memory>

#include "Button.h"
#include "Module.h"
#include "UnemployedSlotButton.h"

class UnemployedPanel : public Module {
  public:
    UnemployedPanel();

    int add(const Officer *officer);
    const Officer *hire();

    const Officer *get_selected() {
        if (m_selected_slot) {
            return m_slots[*m_selected_slot]->get_officer();
        }
        return nullptr;
    }

    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    std::map<int, std::shared_ptr<UnemployedSlotButton>> m_slots;
    std::shared_ptr<Module> m_officer_panel;
    std::shared_ptr<TextButton> m_hire_button;
    std::shared_ptr<TextButton> m_back_button;
    std::optional<int> m_selected_slot;
};
#endif // UNEMPLOYED_PANEL_H
// vi: ft=cpp
