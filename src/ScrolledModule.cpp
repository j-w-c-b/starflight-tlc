#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "Events.h"
#include "Game.h"
#include "ModeMgr.h"
#include "ScrolledModule.h"

using namespace std;

ALLEGRO_DEBUG_CHANNEL("ScrolledModule")

static const int SCROLL_BAR_WIDTH = 16;

Scrollbar::Scrollbar(
    int x,
    int y,
    int height,
    ALLEGRO_COLOR color,
    ALLEGRO_COLOR background_color,
    ALLEGRO_COLOR border_color,
    ALLEGRO_COLOR hover_color)
    : Module(x, y, SCROLL_BAR_WIDTH, height) {

    auto bg = shared_ptr<ALLEGRO_BITMAP>(
        al_create_bitmap(get_width(), get_height()), al_destroy_bitmap);
    al_set_target_bitmap(bg.get());
    al_clear_to_color(background_color);
    al_draw_rectangle(0, 0, get_width(), get_height(), border_color, 1);
    m_scrollbar_background = make_shared<Bitmap>(bg, x, y);
    add_child_module(m_scrollbar_background);

    auto up_arrow = shared_ptr<ALLEGRO_BITMAP>(
        create_triangle(color, background_color, border_color),
        al_destroy_bitmap);

    auto up_arrow_hover = shared_ptr<ALLEGRO_BITMAP>(
        create_triangle(hover_color, background_color, border_color),
        al_destroy_bitmap);
    auto up_arrow_disabled = shared_ptr<ALLEGRO_BITMAP>(
        create_triangle(background_color, background_color, border_color),
        al_destroy_bitmap);

    m_up_button = make_shared<Button>(
        x,
        y,
        EVENT_NONE,
        EVENT_SCROLL_UP,
        up_arrow,
        up_arrow_hover,
        up_arrow_disabled);
    add_child_module(m_up_button);

    auto down_arrow = shared_ptr<ALLEGRO_BITMAP>(
        al_create_bitmap(get_width(), get_width()), al_destroy_bitmap);
    al_set_target_bitmap(down_arrow.get());
    al_draw_bitmap(up_arrow.get(), 0, 0, ALLEGRO_FLIP_VERTICAL);

    auto down_arrow_hover = shared_ptr<ALLEGRO_BITMAP>(
        al_create_bitmap(get_width(), get_width()), al_destroy_bitmap);
    al_set_target_bitmap(down_arrow_hover.get());
    al_draw_bitmap(up_arrow_hover.get(), 0, 0, ALLEGRO_FLIP_VERTICAL);

    auto down_arrow_disabled = shared_ptr<ALLEGRO_BITMAP>(
        al_create_bitmap(get_width(), get_width()), al_destroy_bitmap);
    al_set_target_bitmap(down_arrow_disabled.get());
    al_draw_bitmap(up_arrow_disabled.get(), 0, 0, ALLEGRO_FLIP_VERTICAL);

    m_down_button = make_shared<Button>(
        x,
        y + height - get_width(),
        EVENT_NONE,
        EVENT_SCROLL_DOWN,
        down_arrow,
        down_arrow_hover,
        down_arrow_disabled);
    add_child_module(m_down_button);
}

bool
Scrollbar::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    if (is_mouse_wheel_up(event)) {
        ALLEGRO_EVENT e = make_event(EVENT_SCROLL_UP);
        g_game->broadcast_event(&e);
        return false;
    } else if (is_mouse_wheel_down(event)) {
        ALLEGRO_EVENT e = make_event(EVENT_SCROLL_DOWN);
        g_game->broadcast_event(&e);
        return false;
    }
    return true;
}

bool
Scrollbar::on_event(ALLEGRO_EVENT *event) {
    switch (event->type) {
    case EVENT_SCROLL_UP:
        if (event->user.data1 == m_up_button->get_id()) {
            ALLEGRO_EVENT e = make_event(EVENT_SCROLL_UP);
            g_game->broadcast_event(&e);
            return false;
        }
        break;
    case EVENT_SCROLL_DOWN:
        if (event->user.data1 == m_down_button->get_id()) {
            ALLEGRO_EVENT e = make_event(EVENT_SCROLL_DOWN);
            g_game->broadcast_event(&e);
            return false;
        }
        break;
    }
    return true;
}

ALLEGRO_BITMAP *
Scrollbar::create_triangle(
    ALLEGRO_COLOR color,
    ALLEGRO_COLOR background,
    ALLEGRO_COLOR border) {
    ALLEGRO_BITMAP *b = al_create_bitmap(get_width(), get_width());
    al_set_target_bitmap(b);
    al_clear_to_color(background);
    al_draw_rectangle(0, 0, get_width(), get_width(), border, 1);
    al_draw_filled_triangle(
        static_cast<float>(get_width()) / 2,
        0,
        0,
        get_width(),
        get_width(),
        get_width(),
        color);
    al_draw_triangle(
        static_cast<float>(get_width()) / 2,
        0,
        0,
        get_width(),
        get_width(),
        get_width(),
        border,
        1);
    return b;
}

// vi: ft=cpp
