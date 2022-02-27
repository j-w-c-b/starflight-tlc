#pragma once

#include <memory>
#include <string>

#include <allegro5/allegro.h>

#include "Button.h"
#include "Label.h"
#include "Module.h"
#include "RichTextLabel.h"

class MessageBoxWindow : public Module {
  public:
    MessageBoxWindow(
        const std::string &heading,
        const std::string &text,
        int x = SCREEN_WIDTH / 2,
        int y = SCREEN_HEIGHT / 2,
        int width = 400,
        int height = 300,
        ALLEGRO_COLOR text_color = WHITE,
        bool centered = true,
        bool scrollbar = false);

    virtual ~MessageBoxWindow();

    void set_text(const std::string &text) { m_label_text->set_text(text); }

    template <typename... Args> void set_text(Args &&...args) {
        m_label_text->set_text(std::forward<Args>(args)...);
    }

    bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;

  private:
    void create_background();

    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<Label> m_label_heading;
    std::shared_ptr<RichTextLabel> m_label_text;
    std::shared_ptr<TextButton> m_ok_button;
    bool m_pause_state;
};
// vi: ft=cpp
