/*
STARFLIGHT - THE LOST COLONY
Module.h
Author: Dave Calkins
Date: Dec, 06

* used to provide a hierarchical set of modules which can each perform their
* own processing/rendering to local surfaces which then get rendered back to
* their parent surfaces. this makes the various game visual and computational
* modules independent and allows easily working on one or creating a new
* one without having to understand all the others.
*
* to create a new module, derive from this class and then override the
* virtual methods, making sure to call the base class methods from your
* derived class.
*
* for a visual module, you'll want to call InitCanvas from InitModule to
* ensure a rendering canvas is setup. for a non-visual computational only
* module, don't setup the canvas.
*
* NOTE: non-visual computational only modules can not have any modules
* beneath (children or descendants) them which are visual.
*/

#ifndef MODULE_H
#define MODULE_H

#include <vector>

#include "Events.h"
#include "Game.h"

struct ALLEGRO_BITMAP;
class GameState;
class MessageBoxWindow;

class Module {
  public:
    Module(
        int x = 0,
        int y = 0,
        int width = SCREEN_WIDTH,
        int height = SCREEN_HEIGHT,
        bool active = true)
        : m_x(x), m_y(y), m_width(width), m_height(height), m_active(active),
          m_last_mouse_move_event({.x = -1, .y = -1}),
          m_draw_after_children(false), m_modal_child(nullptr) {}
    virtual ~Module();

    void add_child_module(std::shared_ptr<Module> m);
    // Removes the module from the child list if present and returns it.
    std::shared_ptr<Module> remove_child_module(std::shared_ptr<Module> m);

    virtual bool init() final;
    virtual bool update() final;
    virtual bool draw(ALLEGRO_BITMAP *target) final;
    virtual bool close() final;
    virtual bool key_pressed(ALLEGRO_KEYBOARD_EVENT *event) final;
    virtual bool key_down(ALLEGRO_KEYBOARD_EVENT *event) final;
    virtual bool key_up(ALLEGRO_KEYBOARD_EVENT *event) final;
    virtual bool mouse_move(ALLEGRO_MOUSE_EVENT *event) final;
    virtual bool mouse_button_down(ALLEGRO_MOUSE_EVENT *event) final;
    virtual bool mouse_button_up(ALLEGRO_MOUSE_EVENT *event) final;
    virtual bool event(ALLEGRO_EVENT *event) final;

    void set_draw_after_children(bool draw_after_children) {
        m_draw_after_children = draw_after_children;
    }

    void move(int x, int y) {
        int deltax = x - m_x;
        int deltay = y - m_y;

        m_x = m_x + deltax;
        m_y = m_y + deltay;

        for (auto &i : m_modules) {
            i->move(i->m_x + deltax, i->m_y + deltay);
        }
    }

    void resize(int width, int height) {
        m_width = width;
        m_height = height;
    }

    void get_location(int &x, int &y) {
        x = m_x;
        y = m_y;
    }

    void get_size(int &width, int &height) {
        width = m_width;
        height = m_height;
    }

    bool get_active() { return m_active; }
    void set_active(bool active) { m_active = active; }

    std::shared_ptr<Module>
    set_modal_child(std::shared_ptr<Module> modal_child);

  protected:
    virtual bool on_init() { return true; }
    virtual bool on_update() { return true; }
    virtual bool on_draw(ALLEGRO_BITMAP *) { return true; }
    virtual bool on_close() { return true; }

    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *) { return true; }
    virtual bool on_key_down(ALLEGRO_KEYBOARD_EVENT *) { return true; }
    virtual bool on_key_up(ALLEGRO_KEYBOARD_EVENT *) { return true; }
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *) { return true; }
    virtual bool on_mouse_button_down(ALLEGRO_MOUSE_EVENT *) { return true; }
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *) { return true; }
    virtual bool on_event(ALLEGRO_EVENT *) { return true; }

    // higher-level events--you probably don't need to deal with mouse up/down
    // or move if you just need to detect click and enter/exit
    virtual bool on_mouse_enter(ALLEGRO_MOUSE_EVENT *) { return true; }
    virtual bool on_mouse_leave(ALLEGRO_MOUSE_EVENT *) { return true; }
    virtual bool on_mouse_button_click(ALLEGRO_MOUSE_EVENT *) { return true; }

    /* helpers */
    bool point_within_module(int x, int y) const {
        return (
            x >= m_x && x < (m_x + m_width) && y >= m_y
            && y < (m_y + m_height));
    }
    bool is_mouse_enter_event(ALLEGRO_MOUSE_EVENT *event) const {
        return event->type == ALLEGRO_EVENT_MOUSE_AXES
               && point_within_module(event->x, event->y)
               && !point_within_module(
                   m_last_mouse_move_event.x, m_last_mouse_move_event.y);
    }
    bool is_mouse_leave_event(ALLEGRO_MOUSE_EVENT *event) const {
        return event->type == ALLEGRO_EVENT_MOUSE_AXES
               && !point_within_module(event->x, event->y)
               && point_within_module(
                   m_last_mouse_move_event.x, m_last_mouse_move_event.y);
    }
    bool is_mouse_wheel_up(ALLEGRO_MOUSE_EVENT *event) const {
        return (
            event->type == ALLEGRO_EVENT_MOUSE_AXES
            && event->z < m_last_mouse_move_event.z);
    }
    bool is_mouse_wheel_down(ALLEGRO_MOUSE_EVENT *event) const {
        return (
            event->type == ALLEGRO_EVENT_MOUSE_AXES
            && event->z > m_last_mouse_move_event.z);
    }
    bool is_mouse_click(ALLEGRO_MOUSE_EVENT *event) const {
        ALLEGRO_MOUSE_EVENT last_event = get_last_button_event(event);

        return (
            event->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP
            && event->button == last_event.button && event->x == last_event.x
            && event->y == last_event.y);
    }
    bool is_mouse_over() const {
        return point_within_module(
            m_last_mouse_move_event.x, m_last_mouse_move_event.y);
    }

    int m_x;
    int m_y;
    int m_width;
    int m_height;
    int m_active;

  private:
    void clear_mouse_events();

    ALLEGRO_MOUSE_EVENT
    get_last_button_event(ALLEGRO_MOUSE_EVENT *event) const {
        if (event->button > 0
            && event->button <= m_last_mouse_button_events.size()) {
            return m_last_mouse_button_events[event->button - 1];
        } else {
            ALLEGRO_MOUSE_EVENT last_event = *event;
            last_event.x = -1;
            last_event.y = -1;
            return last_event;
        }
    }

    void clear_last_button_event(ALLEGRO_MOUSE_EVENT *event) {
        ALLEGRO_MOUSE_EVENT def = *event;
        def.x = -1;
        def.y = -1;

        if (event->button > m_last_mouse_button_events.size()) {
            m_last_mouse_button_events.resize(event->button, def);
        } else {
            m_last_mouse_button_events[event->button - 1] = def;
        }
    }

    void set_last_button_event(ALLEGRO_MOUSE_EVENT *event) {
        if (event->button > m_last_mouse_button_events.size()) {
            ALLEGRO_MOUSE_EVENT def = *event;
            def.x = -1;
            def.y = -1;
            m_last_mouse_button_events.resize(event->button, def);
        }
        m_last_mouse_button_events[event->button - 1] = *event;
    }

    std::vector<std::shared_ptr<Module>> m_modules;
    ALLEGRO_MOUSE_EVENT m_last_mouse_move_event;
    std::vector<ALLEGRO_MOUSE_EVENT> m_last_mouse_button_events;
    bool m_draw_after_children;
    // If set, I/O events (mouse, keyboard) will only be sent to this child,
    // but drawing will be done on all.
    std::shared_ptr<Module> m_modal_child;
};

#endif
