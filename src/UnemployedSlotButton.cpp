#include <utility>

#include "Game.h"
#include "UnemployedSlotButton.h"
#include "crewhire_resources.h"

using namespace std;
using namespace crewhire;

UnemployedSlotButton::UnemployedSlotButton(
    int x,
    int y,
    const Officer *officer,
    EventType click_event)
    : Button(
        x,
        y,
        EVENT_NONE,
        click_event,
        images[I_PERSONNEL_CATBTN_UNASSIGNED],
        images[I_PERSONNEL_CATBTN_UNASSIGNED_HOV],
        images[I_PERSONNEL_CATBTN_UNASSIGNED_DIS]),
      m_officer(officer) {
    int w = get_width();
    int h = get_height();

    m_name_label = make_shared<Label>(
        officer->get_name(),
        x,
        y,
        w,
        h,
        false,
        ALLEGRO_ALIGN_LEFT,
        g_game->font20,
        YELLOW);
    add_child_module(m_name_label);

    m_preferred_role_label = make_shared<Label>(
        to_string(officer->get_preferred_profession()),
        x + w * 2 / 3,
        y,
        w,
        h,
        false,
        ALLEGRO_ALIGN_LEFT,
        g_game->font20,
        YELLOW);
    add_child_module(m_preferred_role_label);
}

const Officer *
UnemployedSlotButton::set_officer(const Officer *officer) {
    auto tmp = m_officer;
    m_officer = officer;
    if (officer) {
        m_name_label->set_text(officer->get_name());
        m_preferred_role_label->set_text(
            to_string(officer->get_preferred_profession()));
    } else {
        m_name_label->set_text("");
        m_preferred_role_label->set_text("");
    }

    return tmp;
}

const Officer *
UnemployedSlotButton::fire_officer() {
    auto tmp = m_officer;
    g_game->gameState->fire_officer(m_officer);
    m_officer = nullptr;

    return tmp;
}
// vi: ft=cpp
