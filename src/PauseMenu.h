#pragma once
#include <allegro5/allegro.h>
#include <string>

#include "Button.h"
#include "ResourceManager.h"

class PauseMenu {
  public:
    // ctors
    PauseMenu();
    ~PauseMenu();

    bool
    isShowing() const {
        return display;
    }
    void
    setShowing(bool value) {
        display = value;
    }

    bool
    isEnabled() {
        return enabled;
    }
    void
    setEnabled(bool value) {
        enabled = value;
    }

    bool OnMouseMove(int x, int y);
    bool OnMouseReleased(int button, int x, int y);
    bool OnKeyReleased(int keyCode);
    void Update();
    void Draw();

  private:
    Button *m_save_button;
    Button *m_load_button;
    Button *m_quit_button;
    Button *m_return_button;
    ResourceManager<ALLEGRO_BITMAP> m_resources;
    bool display;
    bool enabled;

    int x, y;
};
