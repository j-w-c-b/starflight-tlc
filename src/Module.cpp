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

Module::~Module() {
    while (!m_modules.empty()) {
        auto i = m_modules.end() - 1;
        delete *i;
        m_modules.pop_back();
    }
}

void
Module::add_child_module(Module *m) {
    m_modules.push_back(m);
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
    if (result) {
        m_active = true;
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
    }
    return result;
}

bool
Module::draw(ALLEGRO_BITMAP *target) {
    bool result = true;
    if (m_active) {
        result = on_draw(target);

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->draw(target);
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
        m_active = false;
    }
    return result;
}

bool
Module::key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    bool result = true;
    if (m_active) {
        result = on_key_pressed(event);

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->on_key_pressed(event);
        }
    }
    return result;
}

bool
Module::key_down(ALLEGRO_KEYBOARD_EVENT *event) {
    bool result = true;
    if (m_active) {
        result = on_key_down(event);

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->on_key_down(event);
        }
    }
    return result;
}

bool
Module::key_up(ALLEGRO_KEYBOARD_EVENT *event) {
    bool result = true;
    if (m_active) {
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
    if (m_active
        && (point_within_module(event->x, event->y)
            || point_within_module(
                m_last_mouse_move_event.x, m_last_mouse_move_event.y))) {
        result = on_mouse_move(event);

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->mouse_move(event);
        }
    }
    m_last_mouse_move_event = *event;

    return result;
}

bool
Module::mouse_button_down(ALLEGRO_MOUSE_EVENT *event) {
    bool result = true;
    ALLEGRO_MOUSE_EVENT last_event = get_last_button_event(event);

    if (m_active
        && (point_within_module(event->x, event->y)
            || point_within_module(last_event.x, last_event.y))) {
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

    if (m_active
        && (point_within_module(event->x, event->y)
            || point_within_module(last_event.x, last_event.y))) {
        result = on_mouse_button_up(event);

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->mouse_button_up(event);
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

        for (auto &i : m_modules) {
            if (!result) {
                break;
            }
            result = i->event(event);
        }
    }

    return result;
}
