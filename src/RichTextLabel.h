#ifndef RICH_TEXT_LABEL_H
#define RICH_TEXT_LABEL_H

#include <algorithm>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "Bitmap.h"
#include "Module.h"

struct RichText {
    RichText(const std::string &text, ALLEGRO_COLOR color)
        : m_text(text), m_color(color) {}

    std::string m_text;
    ALLEGRO_COLOR m_color;
};

class RichTextLabel : public Module {
  public:
    RichTextLabel(
        int x,
        int y,
        int width,
        int height,
        bool multiline,
        int flags,
        std::shared_ptr<ALLEGRO_FONT> font,
        ALLEGRO_COLOR color,
        ALLEGRO_COLOR background_color,
        const std::vector<RichText> &text);
    template <typename... Args>
    RichTextLabel(
        int x,
        int y,
        int width,
        int height,
        bool multiline,
        int flags,
        std::shared_ptr<ALLEGRO_FONT> font,
        ALLEGRO_COLOR color,
        ALLEGRO_COLOR background_color,
        Args &&...args)
        : RichTextLabel(
            x,
            y,
            width,
            height,
            multiline,
            flags,
            font,
            color,
            background_color,
            {std::forward<Args>(args)...}) {}

    void set_text(const std::string &text, bool resize_to_fit = false) {
        set_text({{text, m_default_color}}, resize_to_fit);
    }

    void
    set_text(const std::vector<RichText> &text, bool resize_to_fit = false) {
        m_text.clear();
        add_text(text, resize_to_fit);
    }

    void add_text(const RichText &text, bool resize_to_fit = false) {
        add_text(std::vector<RichText>{text}, resize_to_fit);
    }

    void add_line(const RichText &text, bool resize_to_fit = true) {
        add_text(
            std::vector<RichText>{{text.m_text + "\n", text.m_color}},
            resize_to_fit);
    }
    void add_line(const std::string &text, bool resize_to_fit = true) {
        add_text(
            std::vector<RichText>{{text + "\n", m_default_color}},
            resize_to_fit);
    }

    void
    add_text(const std::vector<RichText> &text, bool resize_to_fit = false) {
        std::copy(text.begin(), text.end(), std::back_inserter(m_text));
        if (resize_to_fit && m_multiline) {
            std::string all_text = std::accumulate(
                m_text.cbegin(),
                m_text.cend(),
                std::string(""),
                [](const std::string &accum, const RichText &rt) {
                    return accum + rt.m_text;
                });
            RefreshState r = {this, 0, m_text.cbegin(), m_text.cend(), 0, 0, 0};
            al_do_multiline_text(
                m_font.get(), get_width(), all_text.c_str(), count_line, &r);
            resize(get_width(), r.current_y_offset);
        } else if (resize_to_fit) {
            resize(get_width(), get_height());
        }
        m_dirty = true;
    }

    virtual void resize(int width, int height) override;

  protected:
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_init() override;
    virtual bool on_close() override;

  private:
    void refresh();

    static bool
    draw_line(int line_num, const char *line, int size, void *extra);
    static bool
    count_line(int line_num, const char *line, int size, void *extra);

    struct RefreshState {
        RichTextLabel *t;
        int line_num;
        std::vector<RichText>::const_iterator it;
        std::vector<RichText>::const_iterator end;
        int current_string_offset;
        float current_x_offset;
        float current_y_offset;
    };

    bool m_multiline;
    std::shared_ptr<ALLEGRO_FONT> m_font;
    ALLEGRO_COLOR m_default_color;
    ALLEGRO_COLOR m_default_background_color;
    std::vector<RichText> m_text;
    std::shared_ptr<ALLEGRO_BITMAP> m_canvas;
    std::shared_ptr<Bitmap> m_canvas_bitmap;
    bool m_dirty;
    int m_flags;
};

#endif // RICH_TEXT_LABEL_H
       // vi: ft=cpp
