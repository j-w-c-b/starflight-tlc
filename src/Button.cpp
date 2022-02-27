#include "Button.h"
#include "AudioSystem.h"
#include "Events.h"
#include "Game.h"
#include "ModeMgr.h"

ALLEGRO_DEBUG_CHANNEL("Button")

using namespace std;

Button::Button(
    int x,
    int y,
    int width,
    int height,
    EventType mouse_over_event,
    EventType click_event,
    std::shared_ptr<ALLEGRO_BITMAP> normal,
    std::shared_ptr<ALLEGRO_BITMAP> mouse_over,
    std::shared_ptr<ALLEGRO_BITMAP> disabled,
    std::shared_ptr<Sample> sound)
    : Module(x, y, width, height), m_mouse_over_event(mouse_over_event),
      m_click_event(click_event), m_is_enabled(true), m_is_highlight(false),
      m_normal(nullptr), m_mouse_over(nullptr), m_disabled(nullptr),
      m_sound(sound) {
    ALLEGRO_ASSERT(normal != nullptr);
    if (width == -1 || height == 1) {
        if (width == -1) {
            width = al_get_bitmap_width(normal.get());
        }
        if (height == -1) {
            height = al_get_bitmap_height(normal.get());
        }

        resize(width, height);
        set_clip_width(width);
        set_clip_height(height);
    }

    m_normal = make_shared<Bitmap>(normal, x, y);
    add_child_module(m_normal);

    if (mouse_over != nullptr) {
        m_mouse_over = make_shared<Bitmap>(mouse_over, x, y);
        add_child_module(m_mouse_over);
    }
    if (disabled != nullptr) {
        m_disabled = make_shared<Bitmap>(disabled, x, y);
        add_child_module(m_disabled);
    }

    update_active_bitmap();
}

bool
Button::on_init() {
    update_active_bitmap();
    return true;
}

void
Button::set_enabled(bool enabled) {
    if (enabled != m_is_enabled) {
        m_is_enabled = enabled;
        update_active_bitmap();
    }
}

void
Button::set_highlight(bool highlight) {
    if (highlight != m_is_highlight) {
        m_is_highlight = highlight;
        update_active_bitmap();
    }
}

void
Button::update_active_bitmap(bool mouse_over) {
    m_normal->set_active(true);
    if (m_mouse_over != nullptr) {
        if (m_is_enabled && (m_is_highlight || mouse_over)) {
            m_normal->set_active(false);
            m_mouse_over->set_active(true);
        } else {
            m_mouse_over->set_active(false);
        }
    }
    if (m_disabled != nullptr) {
        if (!m_is_enabled) {
            m_normal->set_active(false);
            m_disabled->set_active(true);
        } else {
            m_disabled->set_active(false);
        }
    }
}

bool
Button::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    bool mouse_over = event_within_module(event);
    if (m_is_enabled && mouse_over) {
        ALLEGRO_EVENT e = make_event(m_mouse_over_event);
        g_game->broadcast_event(&e);
    }
    update_active_bitmap(mouse_over);
    return true;
}

bool
Button::on_mouse_button_click(ALLEGRO_MOUSE_EVENT *event) {
    if (!m_is_enabled || event->button != 1) {
        return true;
    }
    // make sure button sound isn't playing
    if (m_sound) {
        if (Game::audioSystem->IsPlaying(m_sound))
            Game::audioSystem->Stop(m_sound);

        // play button sound
        Game::audioSystem->Play(m_sound);
    }

    ALLEGRO_EVENT e = make_event(m_click_event);
    g_game->broadcast_event(&e);

    return false;
}

TextButton::TextButton(
    shared_ptr<Label> text,
    int x,
    int y,
    int width,
    int height,
    EventType mouse_over_event,
    EventType click_event,
    std::shared_ptr<ALLEGRO_BITMAP> normal,
    std::shared_ptr<ALLEGRO_BITMAP> mouse_over,
    std::shared_ptr<ALLEGRO_BITMAP> disabled,
    std::shared_ptr<Sample> sound)
    : Button(
        x,
        y,
        width,
        height,
        mouse_over_event,
        click_event,
        normal,
        mouse_over,
        disabled,
        sound),
      m_text(text) {
    add_child_module(m_text);
}

TextButton::TextButton(
    const std::string &text,
    std::shared_ptr<ALLEGRO_FONT> font,
    ALLEGRO_COLOR color,
    int flags,
    int x,
    int y,
    int width,
    int height,
    EventType mouse_over_event,
    EventType click_event,
    std::shared_ptr<ALLEGRO_BITMAP> normal,
    std::shared_ptr<ALLEGRO_BITMAP> mouse_over,
    std::shared_ptr<ALLEGRO_BITMAP> disabled,
    std::shared_ptr<Sample> sound)
    : Button(
        x,
        y,
        width,
        height,
        mouse_over_event,
        click_event,
        normal,
        mouse_over,
        disabled,
        sound) {
    auto [x_, y_] = get_position();
    int text_y = y_;
    int text_height = get_height();
    int line_height = al_get_font_line_height(font.get());

    if (flags & ALLEGRO_ALIGN_CENTER) {
        text_y += (get_height() - line_height) / 2;
        text_height = line_height;
    }
    m_text = make_shared<Label>(
        text, x_, text_y, get_width(), text_height, false, flags, font, color);
    add_child_module(m_text);
}

// vi: ft=cpp
