#pragma once

#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "Bitmap.h"
#include "Label.h"
#include "Module.h"

class TextEntry : public Module {
  public:
    TextEntry(
        ALLEGRO_FONT *font,
        ALLEGRO_COLOR color,
        const std::string &prompt,
        int max_chars,
        ALLEGRO_BITMAP *cursor,
        float cursor_display_time,
        int x,
        int y,
        int width,
        int height,
        const std::string &click_sound = "",
        const std::string &error_sound = "",
        const std::string &default_text = "");
    virtual ~TextEntry() {}

    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;

    void set_prompt(const std::string &prompt);
    virtual void reset();
    std::string get_text() const;

  protected:
    virtual bool character_allowed(int unichar);

  private:
    std::shared_ptr<Label> m_text_display;
    std::shared_ptr<Bitmap> m_cursor;

    float m_cursor_display_time;
    float m_next_cursor_flip;
    bool m_show_cursor;

    int m_max_chars;

    std::string m_click_sound;
    std::string m_error_sound;

    std::string m_prompt_text;
    std::string m_text;
    std::string m_default_text;
};

class NumericTextEntry : public TextEntry {
  public:
    NumericTextEntry(
        ALLEGRO_FONT *font,
        ALLEGRO_COLOR color,
        const std::string &prompt,
        int max_chars,
        int max_value,
        int x,
        int y,
        int width,
        int height,
        const std::string &click_sound = "",
        const std::string &error_sound = "")
        : TextEntry(
            font,
            color,
            prompt,
            max_chars,
            nullptr,
            0.0,
            x,
            y,
            width,
            height,
            click_sound,
            error_sound,
            ""),
          m_max_value(max_value) {}

    int get_value() const {
        const std::string text = get_text();
        if (text == "") {
            return 0;
        }
        return std::stoi(text);
    }

  protected:
    virtual bool character_allowed(int unichar) override;

  private:
    int m_max_value;
};
// vi: ft=cpp
