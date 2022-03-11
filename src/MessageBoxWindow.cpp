#include "MessageBoxWindow.h"
#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "Label.h"
#include "ModeMgr.h"
#include "ScrolledModule.h"
#include "gui_resources.h"

using namespace std;
using namespace gui;

MessageBoxWindow::MessageBoxWindow(
    const string &heading,
    const string &text,
    int x,
    int y,
    int width,
    int height,
    ALLEGRO_COLOR text_color,
    bool centered,
    bool scrollbar)
    : Module(
        centered ? x - width / 2 : x,
        centered ? y - height / 2 : y,
        width,
        height) {
    create_background();
    add_child_module(m_background);

    m_label_heading = make_shared<Label>(
        heading,
        get_x() + 20,
        get_y() + 10,
        get_width() - 34,
        get_height() - 25,
        false,
        centered ? ALLEGRO_ALIGN_CENTER : ALLEGRO_ALIGN_LEFT,
        g_game->font20,
        text_color);
    add_child_module(m_label_heading);

    if (!scrollbar) {
        m_label_text = make_shared<RichTextLabel>(
            get_x() + 20,
            get_y() + 40,
            get_width() - 34,
            get_height() - 55 - 74,
            true,
            centered ? ALLEGRO_ALIGN_CENTER : ALLEGRO_ALIGN_LEFT,
            g_game->font20,
            text_color,
            al_map_rgba(0, 0, 0, 0));
    } else {
        m_label_text = make_shared<ScrolledModule<RichTextLabel>>(
            get_x() + 20,
            get_y() + 40,
            get_width() - 55,
            get_height() - 55 - 74,
            al_get_font_line_height(g_game->font20.get()),
            al_map_rgb(128, 128, 128),
            al_map_rgb(32, 32, 32),
            al_map_rgb(48, 48, 128),
            al_map_rgb(96, 96, 128),
            al_map_rgba(0, 0, 0, 0),
            true,
            centered ? ALLEGRO_ALIGN_CENTER : ALLEGRO_ALIGN_LEFT,
            g_game->font20,
            text_color,
            al_map_rgba(0, 0, 0, 0));
    }
    if (text != "") {
        m_label_text->set_text(text);
    }
    add_child_module(m_label_text);

    m_ok_button = make_shared<TextButton>(
        "Ok",
        g_game->font24,
        WHITE,
        ALLEGRO_ALIGN_CENTER,
        get_x(),
        get_y(),
        EVENT_MOUSEOVER,
        EVENT_CLOSE,
        images[I_GENERIC_EXIT_BTN_NORM],
        images[I_GENERIC_EXIT_BTN_OVER],
        images[I_GENERIC_EXIT_BTN_OVER]);
    add_child_module(m_ok_button);
    auto [w, h] = m_ok_button->get_size();
    m_ok_button->move(
        (get_x() + width / 2) - (w / 2), (get_y() + height) - (h + 15));
    m_pause_state = g_game->getTimePaused();
}

MessageBoxWindow::~MessageBoxWindow() { g_game->SetTimePaused(m_pause_state); }

bool
MessageBoxWindow::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    if (event->keycode == ALLEGRO_KEY_ENTER) {
        ALLEGRO_EVENT e = {.type = static_cast<unsigned int>(EVENT_CLOSE)};
        g_game->broadcast_event(&e);
        return false;
    }
    return true;
}

void
MessageBoxWindow::create_background() {
    ALLEGRO_BITMAP *temp = al_create_bitmap(get_width(), get_height());
    al_set_target_bitmap(temp);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_draw_scaled_bitmap(
        images[I_TRANS_BG].get(),
        0,
        0,
        al_get_bitmap_width(images[I_TRANS_BG].get()),
        al_get_bitmap_height(images[I_TRANS_BG].get()),
        0,
        0,
        get_width(),
        get_height(),
        0);

    al_draw_scaled_bitmap(
        images[I_MESSAGEBOX_BAR].get(),
        0,
        0,
        al_get_bitmap_width(images[I_MESSAGEBOX_BAR].get()),
        al_get_bitmap_height(images[I_MESSAGEBOX_BAR].get()),
        0,
        0,
        al_get_bitmap_width(temp),
        al_get_bitmap_height(images[I_MESSAGEBOX_BAR].get()),
        0);

    al_draw_scaled_bitmap(
        images[I_MESSAGEBOX_BAR].get(),
        0,
        0,
        al_get_bitmap_width(images[I_MESSAGEBOX_BAR].get()),
        al_get_bitmap_height(images[I_MESSAGEBOX_BAR].get()),
        0,
        al_get_bitmap_height(temp)
            - al_get_bitmap_height(images[I_MESSAGEBOX_BAR].get()),
        al_get_bitmap_width(temp),
        al_get_bitmap_height(images[I_MESSAGEBOX_BAR].get()),
        0);

    m_background = make_shared<Bitmap>(
        shared_ptr<ALLEGRO_BITMAP>(temp, al_destroy_bitmap), get_x(), get_y());
}
// vi: ft=cpp
