#ifndef BUTTON_H
#define BUTTON_H
#pragma once

#include <memory>
#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "AudioSystem.h"
#include "Bitmap.h"
#include "Label.h"
#include "Module.h"

class Button : public Module {
  public:
    Button(
        int x,
        int y,
        int width,
        int height,
        EventType mouse_over_event,
        EventType click_event,
        std::shared_ptr<ALLEGRO_BITMAP> normal,
        std::shared_ptr<ALLEGRO_BITMAP> mouse_over = nullptr,
        std::shared_ptr<ALLEGRO_BITMAP> disabled = nullptr,
        std::shared_ptr<Sample> sound = nullptr);

    Button(
        int x,
        int y,
        EventType mouse_over_event,
        EventType click_event,
        std::shared_ptr<ALLEGRO_BITMAP> normal,
        std::shared_ptr<ALLEGRO_BITMAP> mouse_over = nullptr,
        std::shared_ptr<ALLEGRO_BITMAP> disabled = nullptr,
        std::shared_ptr<Sample> sound = nullptr)
        : Button(
            x,
            y,
            -1,
            -1,
            mouse_over_event,
            click_event,
            normal,
            mouse_over,
            disabled,
            sound) {}

    void set_enabled(bool enabled);
    void set_highlight(bool highlight);
    void set_click_event(EventType event) { m_click_event = event; }

  protected:
    virtual bool on_init() override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_click(ALLEGRO_MOUSE_EVENT *event) override;

  private:
    void update_active_bitmap(bool mouse_over = false);

    EventType m_mouse_over_event;
    EventType m_click_event;

    bool m_is_enabled;
    bool m_is_highlight;

    std::shared_ptr<Bitmap> m_normal;
    std::shared_ptr<Bitmap> m_mouse_over;
    std::shared_ptr<Bitmap> m_disabled;

    std::shared_ptr<Sample> m_sound;
};

class TextButton : public Button {
  public:
    TextButton(
        std::shared_ptr<Label> text,
        int x,
        int y,
        int width,
        int height,
        EventType mouse_over_event,
        EventType click_event,
        std::shared_ptr<ALLEGRO_BITMAP> normal,
        std::shared_ptr<ALLEGRO_BITMAP> mouse_over = nullptr,
        std::shared_ptr<ALLEGRO_BITMAP> disabled = nullptr,
        std::shared_ptr<Sample> sound = nullptr);
    TextButton(
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
        std::shared_ptr<ALLEGRO_BITMAP> mouse_over = nullptr,
        std::shared_ptr<ALLEGRO_BITMAP> disabled = nullptr,
        std::shared_ptr<Sample> sound = nullptr);
    TextButton(
        const std::string &text,
        std::shared_ptr<ALLEGRO_FONT> font,
        ALLEGRO_COLOR color,
        int flags,
        int x,
        int y,
        EventType mouse_over_event,
        EventType click_event,
        std::shared_ptr<ALLEGRO_BITMAP> normal,
        std::shared_ptr<ALLEGRO_BITMAP> mouse_over = nullptr,
        std::shared_ptr<ALLEGRO_BITMAP> disabled = nullptr,
        std::shared_ptr<Sample> sound = nullptr)
        : TextButton(
            text,
            font,
            color,
            flags,
            x,
            y,
            -1,
            -1,
            mouse_over_event,
            click_event,
            normal,
            mouse_over,
            disabled,
            sound) {}

    void set_text(const std::string &text) { m_text->set_text(text); }
    void set_color(ALLEGRO_COLOR color) { m_text->set_color(color); }

  protected:
    std::shared_ptr<Label> m_text;
};

#endif
// vi: ft=cpp
