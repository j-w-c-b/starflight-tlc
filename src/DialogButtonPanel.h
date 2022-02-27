#ifndef DIALOG_BUTTON_PANEL_H
#define DIALOG_BUTTON_PANEL_H

#include <map>
#include <memory>
#include <optional>
#include <string>

#include <allegro5/allegro_font.h>

#include "Button.h"
#include "Module.h"
#include "RichTextLabel.h"

class DialogButton : public TextButton {
  public:
    DialogButton(
        int index,
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
            mouse_over_event,
            click_event,
            normal,
            mouse_over,
            disabled,
            sound),
          m_index(index) {}

    virtual ALLEGRO_EVENT make_event(EventType t) override {
        ALLEGRO_EVENT e = {
            .user = {.type = t, .data1 = m_id, .data2 = m_index}};
        return e;
    }

  private:
    int m_index;
};

class DialogButtonPanel : public Bitmap {
  public:
    DialogButtonPanel(
        int x,
        int y,
        int w,
        int h,
        std::shared_ptr<ALLEGRO_FONT> font);

    virtual bool on_event(ALLEGRO_EVENT *event) override;

    void clear();
    void
    add_option(const RichText &text, std::optional<int> index = std::nullopt);
    int get_selection() const;

  private:
    //! Map button index to button
    std::map<int, std::shared_ptr<DialogButton>> m_options;
    std::shared_ptr<ALLEGRO_FONT> m_font;
    int m_line_size;
    int m_next_index;
    int m_selection;
};
#endif // DIALOG_BUTTON_PANEL_H
       // vi: ft=cpp
