#ifndef SCROLLED_MODULE_H
#define SCROLLED_MODULE_H

#include <list>
#include <memory>
#include <string>
#include <unordered_set>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "Button.h"
#include "Module.h"

class Scrollbar : public Module {
  public:
    Scrollbar(
        int x,
        int y,
        int height,
        ALLEGRO_COLOR color,
        ALLEGRO_COLOR background_color,
        ALLEGRO_COLOR border_color,
        ALLEGRO_COLOR hover_color);

    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *target) override;

  private:
    ALLEGRO_BITMAP *create_triangle(
        ALLEGRO_COLOR color,
        ALLEGRO_COLOR background,
        ALLEGRO_COLOR border);

    std::shared_ptr<Bitmap> m_scrollbar_background;
    std::shared_ptr<Button> m_up_button;
    std::shared_ptr<Button> m_down_button;
};

template <class T> class ScrolledModule : public T {
  public:
    template <typename... Args>
    ScrolledModule(
        int x,
        int y,
        int width,
        int height,
        int scroll_jump,
        ALLEGRO_COLOR color,
        ALLEGRO_COLOR background_color,
        ALLEGRO_COLOR border_color,
        ALLEGRO_COLOR hover_color,
        ALLEGRO_COLOR content_background_color,
        Args &&...args)
        : T(x, y, width + 16, height, args...),
          m_scrollbar(std::make_shared<Scrollbar>(
              x + width,
              y,
              height,
              color,
              background_color,
              border_color,
              hover_color)),
          m_scroll_position(0), m_scroll_jump(scroll_jump), m_drawing(false),
          m_background_color(background_color),
          m_content_background_color(content_background_color) {
        T::set_clip_width(width);
        T::set_clip_height(height);
        T::add_child_module(m_scrollbar);
    }
    virtual ~ScrolledModule() {}

    void scroll_to_top() { m_scroll_position = 0; }
    void scroll_to_bottom() {
        m_scroll_position = T::get_height() - T::get_clip_height();
    }
    void scroll_up() { scroll(-m_scroll_jump); }
    void scroll_up(int amount) { scroll(-amount); }
    void scroll_down(int amount) { scroll(amount); }
    void scroll_down() { scroll(m_scroll_jump); }
    void scroll(int amount) {
        int max_scroll = T::get_height() - T::get_clip_height();
        m_scroll_position += amount;

        if (m_scroll_position < 0) {
            m_scroll_position = 0;
        } else if (m_scroll_position > max_scroll) {
            m_scroll_position = max_scroll;
        }
    }

    virtual bool draw(ALLEGRO_BITMAP *target) override {
        if (T::get_active()) {
            return draw_scrolled(target);
        }
        return true;
    }

  protected:
    bool draw_scrolled(ALLEGRO_BITMAP *target) {
        int x = T::get_clip_x();
        int y = T::get_clip_y();

        al_set_target_bitmap(target);
        al_draw_filled_rectangle(
            x,
            y,
            x + T::get_clip_width(),
            y + T::get_clip_height(),
            m_content_background_color);
        m_scrollbar->draw(target);

        auto [w, h] = this->get_size();

        ALLEGRO_BITMAP *scratch = al_create_bitmap(w, h);
        ALLEGRO_TRANSFORM translate;
        al_identity_transform(&translate);
        al_translate_transform(&translate, -x, -y);

        al_set_target_bitmap(scratch);
        al_clear_to_color(m_content_background_color);
        al_use_transform(&translate);
        T::draw(scratch);

        al_set_target_bitmap(target);
        al_draw_bitmap_region(
            scratch,
            0,
            m_scroll_position,
            T::get_clip_width(),
            T::get_clip_height(),
            x,
            y,
            0);
        al_destroy_bitmap(scratch);

        return true;
    }
    virtual bool mouse_move(ALLEGRO_MOUSE_EVENT *event) override {
        if (!m_scrollbar->mouse_move(event)) {
            return false;
        }
        if (T::event_within_module(event)) {
            if (T::is_mouse_wheel_up(event)) {
                scroll_up();
            } else if (T::is_mouse_wheel_down(event)) {
                scroll_down();
            }
        }
        MaskModule masked(m_scrollbar);
        ALLEGRO_MOUSE_EVENT e = *event;
        if (event->y > T::get_y() + T::get_clip_height()
            || event->x > T::get_x() + T::get_width()) {
            e.x = -1;
            e.y = -1;
        } else {
            e.y += m_scroll_position;
        }

        auto ret = T::mouse_move(&e);

        return ret;
    }
    virtual bool mouse_button_down(ALLEGRO_MOUSE_EVENT *event) override {
        if (!m_scrollbar->mouse_button_down(event)) {
            return false;
        }
        MaskModule masked(m_scrollbar);

        ALLEGRO_MOUSE_EVENT e = *event;
        if (event->y > T::get_y() + T::get_clip_height()
            || event->x > T::get_x() + T::get_width()) {
            e.x = -1;
            e.y = -1;
        } else {
            e.y += m_scroll_position;
        }

        auto ret = T::mouse_button_down(&e);

        return ret;
    }
    virtual bool mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override {
        if (!m_scrollbar->mouse_button_up(event)) {
            return false;
        }

        MaskModule masked(m_scrollbar);

        ALLEGRO_MOUSE_EVENT e = *event;
        if (event->y > T::get_y() + T::get_clip_height()
            || event->x > T::get_x() + T::get_width()) {
            e.x = -1;
            e.y = -1;
        } else {
            e.y += m_scroll_position;
        }

        auto ret = T::mouse_button_up(&e);

        return ret;
    }
    virtual bool on_event(ALLEGRO_EVENT *event) override {
        switch (event->type) {
        case EVENT_SCROLL_UP:
            if (event->user.data1 == m_scrollbar->get_id()) {
                scroll_up();
                return false;
            }
            break;
        case EVENT_SCROLL_DOWN:
            if (event->user.data1 == m_scrollbar->get_id()) {
                scroll_down();
                return false;
            }
            break;
        default:
            return T::on_event(event);
        }
        return true;
    }

  private:
    struct MaskModule {
        explicit MaskModule(std::shared_ptr<Module> m)
            : m_module(m), m_state(m_module->get_active()) {
            m_module->set_active(false);
        }
        ~MaskModule() { m_module->set_active(m_state); }
        std::shared_ptr<Module> m_module;
        bool m_state;
    };
    std::shared_ptr<Scrollbar> m_scrollbar;
    int m_scroll_position;
    int m_scroll_jump;
    bool m_drawing;
    ALLEGRO_COLOR m_background_color;
    ALLEGRO_COLOR m_content_background_color;
};

#endif /* SCROLLED_MODULE_H */
// vi: ft=cpp
