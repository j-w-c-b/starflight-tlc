#ifndef ITEM_STACK_BUTTON_LIST_H
#define ITEM_STACK_BUTTON_LIST_H

#include <memory>

#include <allegro5/allegro.h>

#include "Button.h"
#include "GameState.h"
#include "Module.h"

class ItemStackButton : public Module {
  public:
    ItemStackButton(
        const Item *item,
        int count,
        EventType click_event,
        std::shared_ptr<ALLEGRO_FONT> font,
        int width,
        ALLEGRO_COLOR color,
        ALLEGRO_COLOR background_color,
        ALLEGRO_COLOR border_color,
        ALLEGRO_COLOR hover_color);
    virtual ~ItemStackButton() {}

    void set_highlight(bool highlight) {
        m_item_button->set_highlight(highlight);
    }
    void set_count(int count);

  protected:
    bool on_event(ALLEGRO_EVENT *event) override;

  private:
    void draw_button_contents();

    const Item *m_item;
    int m_count;
    std::shared_ptr<Button> m_item_button;
    EventType m_click_event;

    ALLEGRO_COLOR m_color;
    ALLEGRO_COLOR m_background_color;
    ALLEGRO_COLOR m_border_color;
    ALLEGRO_COLOR m_hover_color;

    std::shared_ptr<ALLEGRO_BITMAP> m_normal_button_image;
    std::shared_ptr<ALLEGRO_BITMAP> m_selected_button_image;
    std::shared_ptr<ALLEGRO_FONT> m_font;
};

class ItemStackButtonList : public Module {
  public:
    ItemStackButtonList(
        int x,
        int y,
        int width,
        int height,
        Items &items,
        ItemType filter,
        EventType click_event,
        std::shared_ptr<ALLEGRO_FONT> font,
        ALLEGRO_COLOR color,
        ALLEGRO_COLOR background_color,
        ALLEGRO_COLOR border_color,
        ALLEGRO_COLOR hover_color);
    ItemStackButtonList() = delete;
    virtual ~ItemStackButtonList() {}

    void set_filter(ItemType filter);
    void clear_selected() { m_selected_item = -1; }

    ID get_selected() const { return m_selected_item; }

    int get_count(ID item_id) const { return m_items.get_count(item_id); }

    int get_num_rows() const { return m_rows; }
    void update_items();
    void filter_items();

  protected:
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    std::shared_ptr<Module> m_canvas;
    std::map<ID, std::shared_ptr<ItemStackButton>> m_item_buttons;
    Items &m_items;
    ItemType m_filter;
    EventType m_click_event;
    ID m_selected_item;
    ALLEGRO_COLOR m_color;
    ALLEGRO_COLOR m_background_color;
    ALLEGRO_COLOR m_border_color;
    ALLEGRO_COLOR m_hover_color;
    std::shared_ptr<ALLEGRO_FONT> m_font;
    int m_rows;
};

#endif // ITEM_STACK_BUTTON_LIST_H
// vi: ft=cpp
