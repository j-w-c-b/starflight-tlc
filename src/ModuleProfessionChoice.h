#pragma once

#include <memory>
#include <string>

#include <allegro5/allegro.h>

#include "Bitmap.h"
#include "Button.h"
#include "Label.h"
#include "Module.h"

class ModuleProfessionChoice : public Module {
  public:
    ModuleProfessionChoice();
    virtual ~ModuleProfessionChoice() {}

    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<Button> m_scientific_button;
    std::shared_ptr<Button> m_freelance_button;
    std::shared_ptr<Button> m_military_button;

    std::shared_ptr<Label> m_prof_info_label;

    static const std::string c_prof_info_scientific_text;
    static const std::string c_prof_info_freelance_text;
    static const std::string c_prof_info_military_text;
};

// vi: ft=cpp
