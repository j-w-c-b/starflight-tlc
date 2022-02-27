#include "UnemployedPanel.h"
#include "crewhire_resources.h"

using namespace std;
using namespace crewhire;

static const int UNEMPLOYED_X = 564;
static const int UNEMPLOYED_Y = 68;
static const int UNEMPLOYED_HEIGHT = 594;
static const int UNEMPLOYED_WIDTH = 460;
static const int HIREBTN_X = 815;
static const int HIREBTN_Y = 698;

static const int EXITBTN_X = 16;
static const int EXITBTN_Y = 698;

UnemployedPanel::UnemployedPanel()
    : Module(), m_officer_panel(make_shared<ScrolledModule<Module>>(
                    UNEMPLOYED_X,
                    UNEMPLOYED_Y,
                    UNEMPLOYED_WIDTH - 16,
                    UNEMPLOYED_HEIGHT,
                    al_get_font_line_height(g_game->font20.get()),
                    al_map_rgb(128, 128, 128),
                    al_map_rgb(32, 32, 32),
                    al_map_rgb(48, 48, 128),
                    al_map_rgb(96, 96, 128),
                    al_map_rgba(0, 0, 0, 0))),

      m_hire_button(make_shared<TextButton>(
          "Hire",
          g_game->font24,
          al_map_rgb(0, 255, 255),
          ALLEGRO_ALIGN_CENTER,
          HIREBTN_X,
          HIREBTN_Y,
          EVENT_NONE,
          EVENT_CREWHIRE_HIRE,
          images[I_PERSONNEL_BTN],
          images[I_PERSONNEL_BTN_HOV],
          images[I_PERSONNEL_BTN_DIS],
          samples[S_BUTTONCLICK])),
      m_back_button(make_shared<TextButton>(
          "Back",
          g_game->font24,
          al_map_rgb(255, 0, 0),
          ALLEGRO_ALIGN_CENTER,
          EXITBTN_X,
          EXITBTN_Y,
          EVENT_NONE,
          EVENT_CREWHIRE_BACK,
          images[I_GENERIC_EXIT_BTN_NORM],
          images[I_GENERIC_EXIT_BTN_OVER],
          nullptr,
          samples[S_BUTTONCLICK])) {
    int y = UNEMPLOYED_Y;
    for (auto i = g_game->gameState->unemployed_begin(),
              e = g_game->gameState->unemployed_end();
         i != e;
         ++i) {
        auto b = make_shared<UnemployedSlotButton>(
            UNEMPLOYED_X, y, *i, EVENT_CREWHIRE_UNEMPLOYED_CLICK);

        y += b->get_height();
        m_slots[b->get_id()] = b;
        m_officer_panel->add_child_module(b);
    }
    m_officer_panel->resize(UNEMPLOYED_WIDTH - 16, y - UNEMPLOYED_Y);

    add_child_module(m_officer_panel);
    add_child_module(m_hire_button);
    add_child_module(m_back_button);

    m_hire_button->set_enabled(false);
}

int
UnemployedPanel::add(const Officer *officer) {
    int y = UNEMPLOYED_Y + m_slots.size() * UnemployedSlotButton::spacing;
    auto b = make_shared<UnemployedSlotButton>(
        UNEMPLOYED_X, y, officer, EVENT_CREWHIRE_UNEMPLOYED_CLICK);
    int id = b->get_id();

    y += UnemployedSlotButton::spacing;
    m_slots[id] = b;
    m_officer_panel->add_child_module(b);
    m_officer_panel->resize(UNEMPLOYED_WIDTH - 16, y - UNEMPLOYED_Y);

    return id;
}

const Officer *
UnemployedPanel::hire() {
    if (!m_selected_slot) {
        return nullptr;
    }
    auto button = m_slots.at(*m_selected_slot);

    int y = button->get_y();
    auto officer = button->get_officer();

    for (auto &[id, button] : m_slots) {
        if (button->get_y() > y) {
            button->move(
                button->get_x(),
                button->get_y() - UnemployedSlotButton::spacing);
        }
    }
    m_officer_panel->resize(
        m_officer_panel->get_width(),
        m_slots.size() * UnemployedSlotButton::spacing);

    m_officer_panel->remove_child_module(button);
    m_slots.erase(*m_selected_slot);

    m_selected_slot = nullopt;
    m_hire_button->set_enabled(false);

    g_game->gameState->hire_officer(officer);

    return officer;
}

bool
UnemployedPanel::on_event(ALLEGRO_EVENT *event) {
    EventType type = static_cast<EventType>(event->type);

    switch (type) {
    case EVENT_CREWHIRE_UNEMPLOYED_CLICK:
        {
            int slot_id = static_cast<int>(event->user.data1);
            auto button = m_slots[slot_id];
            if (m_selected_slot && slot_id == *m_selected_slot) {
                // Select the same slot, clear highlight and selection
                button->set_highlight(false);
                m_hire_button->set_enabled(false);
                m_selected_slot = nullopt;
            } else {
                if (m_selected_slot) {
                    m_slots[*m_selected_slot]->set_highlight(false);
                }
                button->set_highlight(true);
                m_hire_button->set_enabled(true);
                m_selected_slot = slot_id;
            }
            return false;
        }
        [[fallthrough]];
    default:
        return true;
    }
}
