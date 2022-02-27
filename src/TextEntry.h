#pragma once

#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "AudioSystem.h"
#include "Bitmap.h"
#include "Label.h"
#include "Module.h"

class TextEntry : public Module {
  public:
    TextEntry(
        std::shared_ptr<ALLEGRO_FONT> font,
        ALLEGRO_COLOR color,
        const std::string &prompt,
        int max_chars,
        std::shared_ptr<ALLEGRO_BITMAP> cursor,
        float cursor_display_time,
        int x,
        int y,
        int width,
        int height,
        std::shared_ptr<Sample> click_sound = nullptr,
        std::shared_ptr<Sample> error_sound = nullptr,
        const std::string &default_text = "");
    virtual ~TextEntry() {}

    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;

    void set_prompt(const std::string &prompt);
    virtual void reset();
    void set_text(const std::string &text);
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

    std::shared_ptr<Sample> m_click_sound;
    std::shared_ptr<Sample> m_error_sound;

    std::string m_prompt_text;
    std::string m_text;
    std::string m_default_text;
};

class NumericTextEntry : public TextEntry {
  public:
    NumericTextEntry(
        std::shared_ptr<ALLEGRO_FONT> font,
        ALLEGRO_COLOR color,
        const std::string &prompt,
        int max_chars,
        int max_value,
        int x,
        int y,
        int width,
        int height,
        std::shared_ptr<ALLEGRO_BITMAP> cursor,
        float cursor_display_time,
        std::shared_ptr<Sample> click_sound = nullptr,
        std::shared_ptr<Sample> error_sound = nullptr)
        : TextEntry(
            font,
            color,
            prompt,
            max_chars,
            cursor,
            cursor_display_time,
            x,
            y,
            width,
            height,
            click_sound,
            error_sound,
            ""),
          m_max_value(max_value) {}

    NumericTextEntry(
        std::shared_ptr<ALLEGRO_FONT> font,
        ALLEGRO_COLOR color,
        const std::string &prompt,
        int max_chars,
        int max_value,
        int x,
        int y,
        int width,
        int height,
        std::shared_ptr<Sample> click_sound = nullptr,
        std::shared_ptr<Sample> error_sound = nullptr)
        : NumericTextEntry(
            font,
            color,
            prompt,
            max_chars,
            max_value,
            x,
            y,
            width,
            height,
            nullptr,
            0.0,
            click_sound,
            error_sound) {}

    bool set_value(int value) {
        if (value >= 0 && value <= m_max_value) {
            set_text(std::to_string(value));
            return true;
        } else {
            return false;
        }
    }

    int get_value() const {
        const std::string text = get_text();
        if (text == "") {
            return 0;
        }
        return std::stoi(text);
    }

    void set_max_value(int max_value) {
        m_max_value = max_value;

        if (get_value() > m_max_value) {
            set_text(std::to_string(max_value));
        }
    }

  protected:
    virtual bool character_allowed(int unichar) override;

  private:
    int m_max_value;
};
// vi: ft=cpp
