#pragma once

#include <string.h>

#include <allegro5/allegro.h>

#include "Bitmap.h"
#include "Button.h"
#include "Label.h"
#include "Module.h"
#include "ResourceManager.h"

class ModuleProfessionChoice : public Module {
  public:
    explicit ModuleProfessionChoice(ResourceManager<ALLEGRO_BITMAP> &resources);
    virtual ~ModuleProfessionChoice() {}

    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    Bitmap *m_background;
    NewButton *m_scientific_button;
    NewButton *m_freelance_button;
    NewButton *m_military_button;

    Label *m_prof_info_label;

    static const std::string c_prof_info_scientific_text;
    static const std::string c_prof_info_freelance_text;
    static const std::string c_prof_info_military_text;
};

// vi: ft=cpp
