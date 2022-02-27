#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "Game.h"
#include "ItemStackButtonList.h"

using namespace std;

ItemStackButton::ItemStackButton(
    const Item *item,
    int count,
    EventType click_event,
    std::shared_ptr<ALLEGRO_FONT> font,
    int width,
    ALLEGRO_COLOR color,
    ALLEGRO_COLOR background_color,
    ALLEGRO_COLOR border_color,
    ALLEGRO_COLOR hover_color)
    : Module(0, 0, width, al_get_font_line_height(font.get())), m_item(item),
      m_count(count), m_click_event(click_event), m_color(color),
      m_background_color(background_color), m_border_color(border_color),
      m_hover_color(hover_color), m_font(font) {
    m_normal_button_image = shared_ptr<ALLEGRO_BITMAP>(
        al_create_bitmap(get_width(), get_height()), al_destroy_bitmap);

    al_set_target_bitmap(m_normal_button_image.get());
    al_clear_to_color(m_background_color);
    draw_button_contents();

    m_selected_button_image = shared_ptr<ALLEGRO_BITMAP>(
        al_create_bitmap(get_width(), get_height()), al_destroy_bitmap);
    al_set_target_bitmap(m_selected_button_image.get());
    al_clear_to_color(m_hover_color);
    draw_button_contents();

    m_item_button = make_shared<Button>(
        get_x(),
        get_y(),
        EVENT_NONE,
        click_event,
        m_normal_button_image,
        m_selected_button_image);
    add_child_module(m_item_button);
}

bool
ItemStackButton::on_event(ALLEGRO_EVENT *event) {
    if (event->type == m_click_event
        && static_cast<int>(event->user.data1) == m_item_button->get_id()) {
        ALLEGRO_EVENT e = make_event(m_click_event);
        g_game->broadcast_event(&e);
        return false;
    }
    return true;
}

void
ItemStackButton::set_count(int count) {
    if (count != m_count) {
        m_count = count;

        al_set_target_bitmap(m_normal_button_image.get());
        al_clear_to_color(m_background_color);
        draw_button_contents();

        al_set_target_bitmap(m_selected_button_image.get());
        al_clear_to_color(m_hover_color);
        draw_button_contents();
    }
}

void
ItemStackButton::draw_button_contents() {
    auto [width, height] = get_size();
    int name_width = static_cast<int>(width * .67);
    int num_items_width = static_cast<int>(width * .13);
    int value_width = width - name_width - num_items_width;

    al_draw_rectangle(0, 0, width, height, m_border_color, 1);
    al_draw_rectangle(0, 0, name_width, height, m_border_color, 1);
    al_draw_rectangle(
        0, 0, name_width + num_items_width, height, m_border_color, 1);
    al_draw_rectangle(
        0,
        0,
        name_width + num_items_width + value_width,
        height,
        m_border_color,
        1);
    al_draw_text(
        m_font.get(),
        m_color,
        0,
        0,
        ALLEGRO_ALIGN_LEFT,
        (string(" ") + m_item->name).c_str());
    al_draw_text(
        m_font.get(),
        m_color,
        name_width,
        0,
        ALLEGRO_ALIGN_LEFT,
        (string(" ") + to_string(m_count)).c_str());
    al_draw_text(
        m_font.get(),
        m_color,
        name_width + num_items_width,
        0,
        ALLEGRO_ALIGN_LEFT,
        (string(" ") + to_string(static_cast<int>(m_item->value))).c_str());
}

ItemStackButtonList::ItemStackButtonList(
    int x,
    int y,
    int width,
    int height,
    Items &items,
    ItemType filter,
    EventType click_event,
    shared_ptr<ALLEGRO_FONT> font,
    ALLEGRO_COLOR color,
    ALLEGRO_COLOR background_color,
    ALLEGRO_COLOR border_color,
    ALLEGRO_COLOR hover_color)
    : Module(x, y, width, height),
      m_canvas(make_shared<Module>(x, y, width, height)), m_items(items),
      m_filter(filter), m_click_event(click_event), m_selected_item(-1),
      m_color(color), m_background_color(background_color),
      m_border_color(border_color), m_hover_color(hover_color), m_font(font) {
    update_items();
    add_child_module(m_canvas);
}

void
ItemStackButtonList::set_filter(ItemType filter) {
    if (filter != m_filter) {
        for (auto &[id, button] : m_item_buttons) {
            if (button->get_active()) {
                m_canvas->remove_child_module(button);
            }
        }
        m_filter = filter;
        filter_items();
    }
}

bool
ItemStackButtonList::on_event(ALLEGRO_EVENT *event) {
    bool ret = true;
    if (event->type == EVENT_ITEM_STACK_BUTTON_LIST) {
        for (auto &[id, button] : m_item_buttons) {
            if (button->get_id() == static_cast<int>(event->user.data1)) {
                m_selected_item = id;
                button->set_highlight(true);
                ALLEGRO_EVENT e = make_event(m_click_event);
                g_game->broadcast_event(&e);
                ret = false;
            } else {
                button->set_highlight(false);
            }
        }
    }
    return ret;
}

void
ItemStackButtonList::update_items() {
    int width = get_width();
    int button_h = 0;
    for (const auto &i : m_items) {
        Item *item = g_game->dataMgr->GetItemByID(i.first);

        auto bp = m_item_buttons.find(i.first);
        shared_ptr<ItemStackButton> b;

        if (bp != m_item_buttons.end()) {
            b = bp->second;
            b->set_count(i.second);
        } else {
            b = make_shared<ItemStackButton>(
                item,
                i.second,
                EVENT_ITEM_STACK_BUTTON_LIST,
                m_font,
                width,
                m_color,
                m_background_color,
                m_border_color,
                m_hover_color);
        }
        if (button_h == 0) {
            button_h = b->get_height();
        }
        m_item_buttons[i.first] = b;
    }
    filter_items();
}

void
ItemStackButtonList::filter_items() {
    int width = get_width();
    int y = 0;
    int rows = 0;
    for (auto &[id, button] : m_item_buttons) {
        int x = get_x();
        const Item *i = g_game->dataMgr->GetItemByID(id);
        int count = m_items.get_count(id);
        if (count > 0 && (m_filter == IT_INVALID || m_filter == i->itemType)) {
            button->move(x, get_y() + y);
            y += button->get_height();
            button->set_active(true);
            rows++;
        } else {
            button->set_active(false);
        }
    }
    resize(width, y);
    m_canvas->resize(width, y);
    m_canvas->clear_child_modules();
    for (auto &i : m_item_buttons) {
        if (i.second->get_active()) {
            m_canvas->add_child_module(i.second);
        }
    }
    m_rows = rows;
}
// vi: ft=cpp
