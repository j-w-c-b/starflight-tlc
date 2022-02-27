#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "DialogButtonPanel.h"
#include "Game.h"
#include "ResourceManager.h"
#include "encounter_resources.h"

using namespace std;
using namespace encounter;

DialogButtonPanel::DialogButtonPanel(
    int x,
    int y,
    int w,
    int h,
    shared_ptr<ALLEGRO_FONT> font)
    : Bitmap(images[I_DIALOG_BG], x, y, w, h), m_font(font),
      m_line_size(al_get_font_line_height(font.get())), m_next_index(0),
      m_selection(-1) {}

bool
DialogButtonPanel::on_event(ALLEGRO_EVENT *event) {
    EventType type = static_cast<EventType>(event->type);

    switch (type) {
    case EVENT_DIALOG_BUTTON_PANEL_CLICK:
        {
            auto index = static_cast<int>(event->user.data2);

            m_selection = index;
            ALLEGRO_EVENT e = make_event(EVENT_ENCOUNTER_DIALOGUE);
            g_game->broadcast_event(&e);
            cout << "Clicked on item " << index << endl;

            return false;
        }
    default:
        return true;
    }
}

void
DialogButtonPanel::clear() {
    for (auto &i : m_options) {
        remove_child_module(i.second);
    }
    m_options.clear();
    m_next_index = 0;
    m_selection = -1;
}

void
DialogButtonPanel::add_option(const RichText &text, std::optional<int> index) {
    int next_index = m_next_index;
    if (index) {
        next_index = *index;
    }

    auto button = make_shared<DialogButton>(
        next_index,
        text.m_text,
        m_font,
        text.m_color,
        ALLEGRO_ALIGN_LEFT,
        get_x(),
        get_y() + next_index * m_line_size,
        EVENT_NONE,
        EVENT_DIALOG_BUTTON_PANEL_CLICK,
        images[I_DIALOG_NORM],
        images[I_DIALOG_HOV],
        images[I_DIALOG_DIS]);
    add_child_module(button);
    m_options[next_index] = button;
    m_next_index = next_index + 1;
}

int
DialogButtonPanel::get_selection() const {
    return m_selection;
}
// vi: ft=cpp
