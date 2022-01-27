#include "MessageBoxWindow.h"
#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "Label.h"
#include "ModeMgr.h"
#include "gui_resources.h"

using namespace std;
using namespace gui_resources;

MessageBoxWindow::MessageBoxWindow(
    const string &heading,
    const string &text,
    int x,
    int y,
    int width,
    int height,
    ALLEGRO_COLOR text_color,
    bool centered)
    : m_x(centered ? x - height / 2 : x), m_y(centered ? y - height / 2 : y),
      m_width(width), m_height(height), m_active(true),
      m_resources(GUI_IMAGES) {
    int flags = 0;

    m_ok_button = new Button(
        m_resources[I_GENERIC_EXIT_BTN_NORM],
        m_resources[I_GENERIC_EXIT_BTN_OVER],
        m_resources[I_GENERIC_EXIT_BTN_OVER],
        m_x,
        m_y,
        EVENT_MOUSEOVER,
        EVENT_CLOSE,
        g_game->font24,
        "Ok",
        WHITE);

    if (centered) {
        flags = ALLEGRO_ALIGN_CENTER;
    }
    m_label_heading = new Label(
        heading,
        m_x + 20,
        m_y + 10,
        width - 34,
        height - 20,
        false,
        flags,
        g_game->font20,
        text_color);
    m_label_text = new Label(
        text,
        m_x + 20,
        m_y + 30,
        width - 34,
        height - 20,
        true,
        flags,
        g_game->font20,
        text_color);
    m_ok_button->SetX((m_x + width / 2) - (m_ok_button->GetWidth() / 2));
    m_ok_button->SetY((m_y + height) - (m_ok_button->GetHeight() + 7));
}

MessageBoxWindow::~MessageBoxWindow() {
    delete m_ok_button;
    delete m_label_heading;
    delete m_label_text;
}

bool
MessageBoxWindow::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;
    bool result = false;

    if (m_ok_button)
        result = !m_ok_button->OnMouseMove(x, y);

    return result;
}

bool
MessageBoxWindow::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    int button = event->button - 1;
    int x = event->x;
    int y = event->y;

    bool result = false;

    if (m_ok_button) {
        result = !m_ok_button->OnMouseReleased(button, x, y);
    }

    return result;
}

bool
MessageBoxWindow::on_mouse_button_down(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    bool result = false;

    if (m_ok_button)
        result = !m_ok_button->PtInBtn(x, y);

    return result;
}

bool
MessageBoxWindow::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    if (event->keycode == ALLEGRO_KEY_ENTER) {
        ALLEGRO_EVENT e = {.type = static_cast<unsigned int>(EVENT_CLOSE)};
        g_game->broadcast_event(&e);
        return false;
    }
    return true;
}

bool
MessageBoxWindow::on_draw(ALLEGRO_BITMAP *target) {
    int left = m_x;
    int top = m_y;

    ALLEGRO_BITMAP *temp = al_create_bitmap(m_width, m_height);
    al_set_target_bitmap(temp);
    al_draw_scaled_bitmap(
        m_resources[I_TRANS_BG],
        0,
        0,
        al_get_bitmap_width(m_resources[I_TRANS_BG]),
        al_get_bitmap_height(m_resources[I_TRANS_BG]),
        0,
        0,
        m_width,
        m_height,
        0);
    al_set_target_bitmap(target);
    al_draw_bitmap(temp, left, top, 0);

    if (m_ok_button)
        m_ok_button->Run(target);

    al_set_target_bitmap(target);
    al_draw_scaled_bitmap(
        m_resources[I_MESSAGEBOX_BAR],
        0,
        0,
        al_get_bitmap_width(m_resources[I_MESSAGEBOX_BAR]),
        al_get_bitmap_height(m_resources[I_MESSAGEBOX_BAR]),
        left,
        top,
        al_get_bitmap_width(temp),
        al_get_bitmap_height(m_resources[I_MESSAGEBOX_BAR]),
        0);
    al_draw_scaled_bitmap(
        m_resources[I_MESSAGEBOX_BAR],
        0,
        0,
        al_get_bitmap_width(m_resources[I_MESSAGEBOX_BAR]),
        al_get_bitmap_height(m_resources[I_MESSAGEBOX_BAR]),
        left,
        top + al_get_bitmap_height(temp)
            - al_get_bitmap_height(m_resources[I_MESSAGEBOX_BAR]),
        al_get_bitmap_width(temp),
        al_get_bitmap_height(m_resources[I_MESSAGEBOX_BAR]),
        0);

    al_destroy_bitmap(temp);

    m_label_heading->on_draw(target);
    m_label_text->on_draw(target);

    return true;
}
