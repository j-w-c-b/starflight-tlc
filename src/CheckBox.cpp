#include "CheckBox.h"
#include "Game.h"
#include "settings_resources.h"

using namespace settings;
using namespace std;

CheckBox::CheckBox(
    int x,
    int y,
    int width,
    int height,
    const std::string &text,
    std::shared_ptr<ALLEGRO_FONT> font,
    ALLEGRO_COLOR color,
    bool checked)
    : Module(x, y, width, height), m_checked(checked),
      m_label(make_shared<Label>(
          text,
          x,
          y,
          width - al_get_bitmap_width(images[I_BUTTON32_NORMAL].get()),
          height,
          false,
          ALLEGRO_ALIGN_LEFT,
          font,
          color)),
      m_button(make_shared<TextButton>(
          checked ? "X" : "",
          font,
          GOLD,
          ALLEGRO_ALIGN_CENTER,
          x + width - al_get_bitmap_width(images[I_BUTTON32_NORMAL].get()),
          y,
          EVENT_NONE,
          EVENT_CHECKBOX_CLICK,
          images[I_BUTTON32_NORMAL],
          images[I_BUTTON32_OVER])) {
    add_child_module(m_label);
    add_child_module(m_button);
}

CheckBox::CheckBox(
    int x,
    int y,
    int width,
    const std::string &text,
    std::shared_ptr<ALLEGRO_FONT> font,
    ALLEGRO_COLOR color,
    bool checked)
    : CheckBox(
        x,
        y,
        width,
        al_get_bitmap_height(images[I_BUTTON32_NORMAL].get()),
        text,
        font,
        color,
        checked) {}

void
CheckBox::set_checked(bool checked) {
    m_checked = checked;
    m_button->set_text(m_checked ? "X" : "");
}

bool
CheckBox::on_event(ALLEGRO_EVENT *event) {
    EventType t = static_cast<EventType>(event->type);

    switch (t) {
    case EVENT_CHECKBOX_CLICK:
        if (static_cast<int>(event->user.data1) == m_button->get_id()) {
            m_checked = !m_checked;
            m_button->set_text(m_checked ? "X" : "");
            return false;
        }
    default:
        return true;
    }
}
// vi: ft=cpp
