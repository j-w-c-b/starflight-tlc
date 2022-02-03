#include "TextEntry.h"
#include "AudioSystem.h"

using namespace std;

TextEntry::TextEntry(
    ALLEGRO_FONT *font,
    ALLEGRO_COLOR color,
    const string &prompt,
    int max_chars,
    ALLEGRO_BITMAP *cursor,
    float cursor_display_time,
    int x,
    int y,
    int width,
    int height,
    const string &click_sound,
    const string &error_sound,
    const string &default_text)
    : Module(x, y, width, height),
      m_text_display(
          make_shared<
              Label>(prompt, x, y, width, height, false, 0, font, color)),
      m_cursor(
          cursor ? make_shared<Bitmap>(
              cursor,
              x + m_text_display->get_text_width(),
              y)
                 : nullptr),
      m_cursor_display_time(cursor_display_time),
      m_next_cursor_flip(al_get_time() + cursor_display_time),
      m_show_cursor(true), m_max_chars(max_chars), m_click_sound(click_sound),
      m_error_sound(error_sound), m_prompt_text(prompt), m_text(""),
      m_default_text(default_text) {
    add_child_module(m_text_display);
    if (m_cursor) {
        add_child_module(m_cursor);
    }
}

bool
TextEntry::on_init() {
    reset();
    if (m_cursor) {
        m_cursor->set_active(true);
        m_next_cursor_flip = al_get_time() + m_cursor_display_time;
    }
    return true;
}

bool
TextEntry::on_update() {
    if (m_cursor) {
        float now = al_get_time();

        if (now >= m_next_cursor_flip) {
            m_show_cursor = !m_show_cursor;
            m_next_cursor_flip = now + m_cursor_display_time;
            if (m_cursor) {
                m_cursor->set_active(m_show_cursor);
            }
        }
    }
    return true;
}

bool
TextEntry::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    bool play_click = false;
    bool play_err = false;

    if (event->keycode == ALLEGRO_KEY_BACKSPACE) {
        if (m_text.size() > 0) {
            m_text.pop_back();

            play_click = true;
            if (m_text != "") {
                m_text_display->set_text(m_prompt_text + m_text);
            } else {
                m_text_display->set_text(m_prompt_text + m_default_text);
            }
            if (m_cursor) {
                m_cursor->move(m_x + m_text_display->get_text_width(), m_y);
            }
        } else {
            play_err = true;
        }
    } else if (event->unichar && !character_allowed(event->unichar)) {
        play_err = true;
    } else if (event->unichar) {
        if (m_text.size() < static_cast<string::size_type>(m_max_chars)) {
            m_text.push_back(static_cast<char>(event->unichar));
            m_text_display->set_text(m_prompt_text + m_text);
            if (m_cursor) {
                m_cursor->move(m_x + m_text_display->get_text_width(), m_y);
            }
            play_click = true;
        } else {
            play_err = true;
        }
    } else {
        // some control character not handled by this widget.
        return true;
    }

    if (play_click && m_click_sound != "") {
        g_game->audioSystem->Play(m_click_sound);
    }

    if (play_err && m_error_sound != "") {
        g_game->audioSystem->Play(m_error_sound);
    }
    return false;
}

void
TextEntry::set_prompt(const std::string &prompt) {
    m_prompt_text = prompt;
    if (m_cursor) {
        m_cursor->move(m_x + m_text_display->get_text_width(), m_y);
    }
}

void
TextEntry::reset() {
    m_text = "";
    m_text_display->set_text(m_prompt_text + m_default_text);
    if (m_cursor) {
        m_cursor->move(m_x + m_text_display->get_text_width(), m_y);
    }
}

string
TextEntry::get_text() const {
    return m_text;
}

bool
TextEntry::character_allowed(int unichar) {
    return isalnum(unichar) || unichar == ' ';
}

bool
NumericTextEntry::character_allowed(int unichar) {
    int value = get_value() * 10;
    string unistr = "";
    unistr.push_back(static_cast<int>(unichar));

    if (!isdigit(unichar)) {
        return false;
    } else if ((value + stoi(unistr)) > m_max_value) {
        return false;
    }
    return true;
}
// vi: ft=cpp
