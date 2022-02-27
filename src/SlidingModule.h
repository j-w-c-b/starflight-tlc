#ifndef SLIDER_H
#define SLIDER_H

#include <memory>

#include <allegro5/allegro.h>

#include "Module.h"

enum SlideOrigin
{
    SLIDE_FROM_LEFT,
    SLIDE_FROM_RIGHT,
    SLIDE_FROM_TOP,
    SLIDE_FROM_BOTTOM
};

template <class T> class SlidingModule : public T {
  public:
    SlidingModule(SlideOrigin origin, EventType event, double slide_time = 0.6)
        : T(), m_origin(origin), m_slide_time(slide_time), m_slide_start(0),
          m_slide_event(event), m_slide_state(SLIDE_CLOSED),
          m_scratch(std::shared_ptr<ALLEGRO_BITMAP>(
              al_create_bitmap(T::get_width(), T::get_height()),
              al_destroy_bitmap)) {}
    template <typename... Args>
    SlidingModule(
        SlideOrigin origin,
        EventType event,
        double slide_time,
        Args &&...args)
        : T(args...), m_origin(origin), m_slide_time(slide_time),
          m_slide_start(0), m_slide_event(event), m_slide_state(SLIDE_CLOSED),
          m_scratch(std::shared_ptr<ALLEGRO_BITMAP>(
              al_create_bitmap(T::get_width(), T::get_height()),
              al_destroy_bitmap)) {}

    virtual bool draw(ALLEGRO_BITMAP *target) override {
        bool result = true;
        if (T::get_active()) {
            result = draw_to_slider(target);
        }
        return result;
    }
    virtual bool on_update() override {
        if (al_get_time() > (m_slide_start + m_slide_time)) {
            if (m_slide_state == SLIDE_OPENING) {
                m_slide_state = SLIDE_OPEN;
            } else if (m_slide_state == SLIDE_CLOSING) {
                m_slide_state = SLIDE_CLOSED;
            }
        }
        return T::on_update();
    }

    bool draw_to_slider(ALLEGRO_BITMAP *target) {
        if (m_slide_state == SLIDE_CLOSED) {
            return true;
        } else if (m_slide_state == SLIDE_OPEN) {
            auto ret = T::draw(target);
            return ret;
        }

        auto [x, y] = T::get_position();
        auto [w, h] = T::get_size();

        if (m_scratch
            && (al_get_bitmap_width(m_scratch.get()) != x + w
                || al_get_bitmap_height(m_scratch.get()) != y + h)) {
            m_scratch = nullptr;
        }

        if (!m_scratch) {
            m_scratch = std::shared_ptr<ALLEGRO_BITMAP>(
                al_create_bitmap(x + w, y + h), al_destroy_bitmap);
        }

        // render content to a scratch bitmap
        al_set_target_bitmap(m_scratch.get());
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        T::draw(m_scratch.get());

        double now = al_get_time();
        double elapsed = now - m_slide_start;
        double partial = elapsed / m_slide_time;
        if (partial > 1) {
            partial = 1;
        } else if (partial < 0) {
            partial = 0;
        }
        if (m_slide_state == SLIDE_CLOSING) {
            partial = 1 - partial;
        }
        al_set_target_bitmap(target);

        float source_x, source_y;
        float dest_x, dest_y;
        float source_w, source_h;

        switch (m_origin) {
        case SLIDE_FROM_LEFT:
            source_x = x + (1.0 - partial) * w;
            source_y = y;
            source_w = partial * w;
            source_h = h;
            dest_x = x;
            dest_y = y;
            break;
        case SLIDE_FROM_RIGHT:
            source_x = x;
            source_y = y;
            source_w = partial * w;
            source_h = h;
            dest_x = x + (1 - partial) * w;
            dest_y = y;
            break;
        case SLIDE_FROM_TOP:
            source_x = x;
            source_y = y + (1.0 - partial) * h;
            source_w = w;
            source_h = partial * h;
            dest_x = x;
            dest_y = y;
            break;
        case SLIDE_FROM_BOTTOM:
            source_x = x;
            source_y = y;
            source_w = w;
            source_h = partial * h;
            dest_x = x;
            dest_y = y + (1 - partial) * h;
            break;
        }
        al_draw_bitmap_region(
            m_scratch.get(),
            source_x,
            source_y,
            source_w,
            source_h,
            dest_x,
            dest_y,
            0);
        return true;
    }

    virtual bool mouse_move(ALLEGRO_MOUSE_EVENT *event) override {
        ALLEGRO_MOUSE_EVENT translated = translate_mouse_coords(event);
        return T::mouse_move(&translated);
    }
    virtual bool mouse_button_down(ALLEGRO_MOUSE_EVENT *event) override {
        ALLEGRO_MOUSE_EVENT translated = translate_mouse_coords(event);
        return T::mouse_button_down(&translated);
    }
    virtual bool mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override {
        ALLEGRO_MOUSE_EVENT translated = translate_mouse_coords(event);
        return T::mouse_button_up(&translated);
    }
    virtual bool on_event(ALLEGRO_EVENT *event) override {
        EventType t = static_cast<EventType>(event->type);

        if (t == m_slide_event) {
            toggle();
            return true;
        }
        bool ret = T::on_event(event);
        return ret;
    }

    void toggle() {
        if (m_slide_state == SLIDE_OPEN || m_slide_state == SLIDE_OPENING) {
            begin_close();
        } else {
            begin_open();
        }
    }

    void close_slider() {
        m_slide_start = 0;
        m_slide_state = SLIDE_CLOSED;
    }

    bool is_open() {
        return m_slide_state == SLIDE_OPEN || m_slide_state == SLIDE_OPENING;
    }

    bool is_closed() {
        return m_slide_state == SLIDE_CLOSED || m_slide_state == SLIDE_CLOSING;
    }
    bool is_visible() { return m_slide_state != SLIDE_CLOSED; }

  private:
    ALLEGRO_MOUSE_EVENT
    translate_mouse_coords(const ALLEGRO_MOUSE_EVENT *event) const {
        double partial = (al_get_time() - m_slide_start) / m_slide_time;

        ALLEGRO_MOUSE_EVENT translated = *event;

        switch (m_slide_state) {
        case SLIDE_OPEN:
            return translated;
        case SLIDE_CLOSED:
            translated.x = -1;
            translated.y = -1;
            return translated;
        case SLIDE_OPENING:
        case SLIDE_CLOSING:
            break;
        }

        switch (m_origin) {
        case SLIDE_FROM_LEFT:
            translated.x +=
                T::get_width() - static_cast<int>(T::get_width() * partial);
            break;
        case SLIDE_FROM_RIGHT:
            translated.x -= static_cast<int>(T::get_width() * partial);
            break;
        case SLIDE_FROM_TOP:
            translated.y +=
                T::get_height() - static_cast<int>(T::get_height() * partial);
            break;
        case SLIDE_FROM_BOTTOM:
            translated.y -= static_cast<int>(T::get_height() * partial);
        }
        return translated;
    }
    void begin_open() {
        double now = al_get_time();
        double elapsed;

        switch (m_slide_state) {
        case SLIDE_OPENING:
        case SLIDE_OPEN:
            return;
        case SLIDE_CLOSING:
            elapsed = m_slide_time - (now - m_slide_start);
            break;
        case SLIDE_CLOSED:
            elapsed = 0;
        }

        double partial = elapsed / m_slide_time;
        if (partial > 1) {
            partial = 1;
        } else if (partial < 0) {
            partial = 0;
        }
        m_slide_start = now - m_slide_time * partial;
        m_slide_state = SLIDE_OPENING;
    }
    void begin_close() {
        double now = al_get_time();
        double elapsed;

        switch (m_slide_state) {
        case SLIDE_CLOSING:
        case SLIDE_CLOSED:
            return;
        case SLIDE_OPENING:
            elapsed = m_slide_time - (now - m_slide_start);
            break;
        case SLIDE_OPEN:
            elapsed = 0;
        }

        double partial = elapsed / m_slide_time;
        if (partial > 1) {
            partial = 1;
        } else if (partial < 0) {
            partial = 0;
        }
        m_slide_start = now - m_slide_time * partial;
        m_slide_state = SLIDE_CLOSING;
    }

    SlideOrigin m_origin;

    double m_slide_time;
    double m_slide_start;
    EventType m_slide_event;

    enum SlideState
    {
        SLIDE_CLOSED,
        SLIDE_OPENING,
        SLIDE_OPEN,
        SLIDE_CLOSING
    } m_slide_state;

    std::shared_ptr<ALLEGRO_BITMAP> m_scratch;
};

#endif
// vi: ft=cpp
