#pragma once

#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "Module.h"

class Label : public Module {
  public:
    Label(
        const std::string &text,
        int x,
        int y,
        int width,
        int height,
        bool multiline,
        int flags,
        ALLEGRO_FONT *font,
        ALLEGRO_COLOR color,
        ALLEGRO_BITMAP *background = nullptr,
        ALLEGRO_COLOR background_color = al_map_rgba(0, 0, 0, 0));

    virtual ~Label();

    virtual bool on_draw(ALLEGRO_BITMAP *target) override;

    int get_text_width() const {
        return al_get_text_width(m_font, m_text.c_str());
    }

    void set_text(const std::string &text) {
        if (text != m_text) {
            m_text = text;
            m_dirty = true;
        }
    }

    void set_font(ALLEGRO_FONT *font) {
        if (font != m_font) {
            m_font = font;
            m_dirty = true;
        }
    }

    void set_flags(int flags) {
        if (flags != m_flags) {
            m_flags = flags;
            m_dirty = true;
        }
    }

    void set_color(ALLEGRO_COLOR color) {
        unsigned char r1, r2, g1, g2, b1, b2, a1, a2;

        al_unmap_rgba(color, &r1, &g1, &b1, &a1);
        al_unmap_rgba(m_color, &r2, &g2, &b2, &a2);

        if (r1 != r2 || g1 != g2 || b1 != b2 || a1 != a2) {
            m_color = color;
            m_dirty = true;
        }
    }

  private:
    void refresh();

    std::string m_text;
    bool m_multiline;
    int m_flags;
    ALLEGRO_FONT *m_font;
    ALLEGRO_COLOR m_color;
    ALLEGRO_BITMAP *m_background;
    ALLEGRO_COLOR m_background_color;
    ALLEGRO_BITMAP *m_bitmap;
    bool m_dirty;
};

// vi: ft=cpp
