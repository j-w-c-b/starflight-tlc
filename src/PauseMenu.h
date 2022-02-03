#pragma once

#include <allegro5/allegro.h>

#include "Bitmap.h"
#include "Button.h"
#include "ResourceManager.h"

class PauseMenu : public Module {
  public:
    PauseMenu();
    ~PauseMenu() {}

    bool is_enabled() const { return m_enabled; }
    void set_enabled(bool enabled);

  private:
    ResourceManager<ALLEGRO_BITMAP> m_resources;

    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<NewButton> m_save_button;
    std::shared_ptr<NewButton> m_load_button;
    std::shared_ptr<NewButton> m_quit_button;
    std::shared_ptr<NewButton> m_return_button;

    bool m_enabled;
};
// vi: ft=cpp
