#pragma once
#include <string>

#include <allegro5/allegro.h>

#include "Button.h"
#include "Label.h"
#include "ResourceManager.h"

class MessageBoxWindow {
  public:
    // ctors
    MessageBoxWindow(
        const std::string &heading,
        const std::string &text,
        int x,
        int y,
        int width,
        int height,
        ALLEGRO_COLOR text_color,
        bool centered);

    ~MessageBoxWindow();

    // accessors
    bool is_active() const { return m_active; }

    // mutators
    void set_text(const std::string &text) { m_label_text->set_text(text); }
    void move(int x, int y) {
        m_x = x;
        m_y = y;
    }
    void set_active(bool active) { m_active = active; }

    // other funcs
    bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event);
    bool on_mouse_button_down(ALLEGRO_MOUSE_EVENT *event);
    bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event);
    bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event);

    bool on_draw(ALLEGRO_BITMAP *target);

  private:
    Button *m_ok_button;

    int m_x;
    int m_y;
    int m_width;
    int m_height;
    bool m_active;
    Label *m_label_heading;
    Label *m_label_text;
    ResourceManager<ALLEGRO_BITMAP> m_resources;
};
