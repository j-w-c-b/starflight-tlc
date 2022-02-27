/*
        STARFLIGHT - THE LOST COLONY
        module.cpp - ?
        Author: ?
        Date: ?
*/

#include <allegro5/allegro.h>

#include "Game.h"
#include "GameState.h"
#include "Module.h"

using namespace std;

int Module::c_next_id = 0;

Module::~Module() {}

void
Module::add_child_module(std::shared_ptr<Module> m, bool add_first) {
    if (add_first) {
        m_modules.insert(m_modules.begin(), m);
    } else {
        m_modules.push_back(m);
    }
}

shared_ptr<Module>
Module::remove_child_module(shared_ptr<Module> m) {
    auto i = find(m_modules.begin(), m_modules.end(), m);

    ALLEGRO_ASSERT(i != m_modules.end());

    if (i != m_modules.end()) {
        auto res = *i;
        m_modules.erase(i);
        return res;
    }
    return nullptr;
}

shared_ptr<Module>
Module::remove_child_module(int id) {
    auto i =
        find_if(m_modules.begin(), m_modules.end(), [id](shared_ptr<Module> m) {
            return m->get_id() == id;
        });

    ALLEGRO_ASSERT(i != m_modules.end());

    if (i != m_modules.end()) {
        auto res = *i;
        m_modules.erase(i);
        return res;
    }
    return nullptr;
}

shared_ptr<Module>
Module::get_child_module(int id) const {
    auto i =
        find_if(m_modules.begin(), m_modules.end(), [id](shared_ptr<Module> m) {
            return m->get_id() == id;
        });

    ALLEGRO_ASSERT(i != m_modules.end());

    if (i != m_modules.end()) {
        return *i;
    }
    return nullptr;
}

bool
Module::init() {
    bool result = on_init();

    for (auto &i : m_modules) {
        if (!result) {
            break;
        }
        result = i->init();
    }
    if (result && m_modal_child) {
        result = m_modal_child->init();
    }

    return result;
}

bool
Module::update() {
    bool result = true;

    if (m_active) {
        result = on_update();

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->update();
        }
        if (result && m_modal_child) {
            result = m_modal_child->update();
        }
    }
    return result;
}

bool
Module::draw(ALLEGRO_BITMAP *target) {
    bool result = true;
    if (m_active) {
        if (!m_draw_after_children) {
            result = on_draw(target);
        }

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->draw(target);
        }

        if (result && m_draw_after_children) {
            result = on_draw(target);
        }

        if (result && m_modal_child) {
            result = m_modal_child->draw(target);
        }
    }
    return result;
}

bool
Module::close() {
    bool result = true;

    if (m_active) {
        result = on_close();

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->close();
        }
        if (m_modal_child) {
            m_modal_child->close();
        }
    }
    return result;
}

bool
Module::key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    bool result = true;
    if (m_active) {
        if (m_modal_child) {
            return m_modal_child->key_pressed(event);
        }
        result = on_key_pressed(event);

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->key_pressed(event);
        }
    }
    return result;
}

bool
Module::key_down(ALLEGRO_KEYBOARD_EVENT *event) {
    bool result = true;
    if (m_active) {
        if (m_modal_child) {
            return m_modal_child->key_down(event);
        }
        result = on_key_down(event);

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->key_down(event);
        }
    }
    return result;
}

bool
Module::key_up(ALLEGRO_KEYBOARD_EVENT *event) {
    bool result = true;
    if (m_active) {
        if (m_modal_child) {
            return m_modal_child->key_up(event);
        }
        result = on_key_up(event);

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->key_up(event);
        }
    }
    return result;
}

bool
Module::mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    bool result = true;
    bool in_module = event_within_module(event);
    bool was_in_module = event_within_module(&m_last_mouse_move_event);

    if (m_active && m_modal_child) {
        return m_modal_child->mouse_move(event);
    }
    if (m_active && (in_module || was_in_module)) {
        result = on_mouse_move(event);

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->mouse_move(event);
        }
        if (result && in_module && !was_in_module) {
            result = on_mouse_enter(event);

            for (auto &i : m_modules) {
                if (!result) {
                    break;
                }
                result = i->on_mouse_enter(event);
            }
        }
        if (result && !in_module && was_in_module) {
            result = on_mouse_leave(event);

            for (auto &i : m_modules) {
                if (!result) {
                    break;
                }
                result = i->on_mouse_leave(event);
            }
        }
    }
    m_last_mouse_move_event = *event;

    return result;
}

bool
Module::mouse_button_down(ALLEGRO_MOUSE_EVENT *event) {
    bool result = true;
    if (m_active && m_modal_child) {
        return m_modal_child->mouse_button_down(event);
    }
    ALLEGRO_MOUSE_EVENT last_event = get_last_button_event(event);

    if (m_active
        && (event_within_module(event) || event_within_module(&last_event))) {
        result = on_mouse_button_down(event);

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->mouse_button_down(event);
        }
    }
    set_last_button_event(event);

    return result;
}

bool
Module::mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    bool result = true;
    ALLEGRO_MOUSE_EVENT last_event = get_last_button_event(event);

    if (m_active && m_modal_child) {
        return m_modal_child->mouse_button_up(event);
    }

    if (m_active
        && (event_within_module(event) || event_within_module(&last_event))) {
        result = on_mouse_button_up(event);

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->mouse_button_up(event);
        }
        if (result && last_event.x == event->x && last_event.y == event->y) {
            result = on_mouse_button_click(event);
        }
    }
    clear_last_button_event(event);

    return result;
}

bool
Module::event(ALLEGRO_EVENT *event) {
    bool result = true;
    if (m_active) {
        result = on_event(event);
        if (m_active && result && m_modal_child) {
            return m_modal_child->event(event);
        }

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->event(event);
        }
    }

    return result;
}

shared_ptr<Module>
Module::set_modal_child(shared_ptr<Module> modal_child) {
    shared_ptr<Module> old_value = m_modal_child;

    m_modal_child = nullptr;

    clear_mouse_events();

    m_modal_child = modal_child;

    return old_value;
}

void
Module::clear_mouse_events() {
    ALLEGRO_MOUSE_EVENT e = m_last_mouse_move_event;
    e.x = -1;
    e.y = -1;

    mouse_move(&e);

    for (auto &e : m_last_mouse_button_events) {
        e.x = -1;
        e.y = -1;
    }

    for (auto &m : m_modules) {
        m->clear_mouse_events();
    }
}
