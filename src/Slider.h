#ifndef SLIDER_H
#define SLIDER_H

#include <memory>

#include <allegro5/allegro.h>

#include "Module.h"

class Slider : public Module {
  public:
    enum SlideOrigin
    {
        SLIDE_FROM_LEFT,
        SLIDE_FROM_RIGHT,
        SLIDE_FROM_TOP,
        SLIDE_FROM_BOTTOM
    };
    Slider(
        std::shared_ptr<Module> content,
        SlideOrigin origin,
        EventType event,
        double slide_time = 0.7);
    virtual ~Slider();

    virtual bool on_init() override { return m_content->init(); }
    virtual bool on_update() override;
    virtual bool on_draw(ALLEGRO_BITMAP *) override;
    virtual bool on_close() override { return m_content->close(); }

    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override {
        return m_content->key_pressed(event);
    }
    virtual bool on_key_down(ALLEGRO_KEYBOARD_EVENT *event) override {
        return m_content->key_down(event);
    }
    virtual bool on_key_up(ALLEGRO_KEYBOARD_EVENT *event) override {
        return m_content->key_up(event);
    }
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override {
        ALLEGRO_MOUSE_EVENT translated = translate_mouse_coords(event);
        return m_content->mouse_move(&translated);
    }
    virtual bool on_mouse_button_down(ALLEGRO_MOUSE_EVENT *event) override {
        ALLEGRO_MOUSE_EVENT translated = translate_mouse_coords(event);
        return m_content->mouse_button_down(&translated);
    }
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override {
        ALLEGRO_MOUSE_EVENT translated = translate_mouse_coords(event);
        return m_content->mouse_button_up(&translated);
    }
    virtual bool on_event(ALLEGRO_EVENT *event) override {
        EventType t = static_cast<EventType>(event->type);

        if (t == m_slide_event) {
            toggle();
            return false;
        }
        return m_content->event(event);
    }

    void toggle();

  private:
    ALLEGRO_MOUSE_EVENT
    translate_mouse_coords(const ALLEGRO_MOUSE_EVENT *event) const;
    void begin_open();
    void begin_close();

    std::shared_ptr<Module> m_content;
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

    ALLEGRO_BITMAP *m_scratch;
};

#endif
// vi: ft=cpp
