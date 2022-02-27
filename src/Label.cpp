#include "Label.h"

using namespace std;

Label::Label(
    const string &text,
    int x,
    int y,
    int width,
    int height,
    bool multiline,
    int flags,
    std::shared_ptr<ALLEGRO_FONT> font,
    ALLEGRO_COLOR color,
    std::shared_ptr<ALLEGRO_BITMAP> background,
    ALLEGRO_COLOR background_color,
    bool shadow)
    : Module(x, y, width, height), m_text(text), m_multiline(multiline),
      m_flags(flags), m_font(font), m_color(color), m_background(background),
      m_background_color(background_color),
      m_bitmap(al_create_bitmap(width, height)), m_shadow(shadow),
      m_dirty(true) {
    refresh();
}

Label::~Label() {
    if (m_bitmap) {
        al_destroy_bitmap(m_bitmap);
    }
}

bool
Label::on_draw(ALLEGRO_BITMAP *target) {
    refresh();

    auto [x, y] = get_position();

    al_set_target_bitmap(target);
    al_draw_bitmap(m_bitmap, x, y, 0);

    return true;
}

void
Label::refresh() {
    if (m_dirty) {
        auto [x, y] = get_position();

        al_set_target_bitmap(m_bitmap);
        al_clear_to_color(m_background_color);

        if (m_background) {
            al_draw_bitmap(m_background.get(), x, y, 0);
        }

        float text_x = 0;

        if (m_flags & ALLEGRO_ALIGN_RIGHT) {
            text_x = al_get_bitmap_width(m_bitmap);
        } else if (m_flags & ALLEGRO_ALIGN_CENTER) {
            text_x = al_get_bitmap_width(m_bitmap) / 2.0;
        }

        if (m_multiline) {
            if (m_shadow) {
                al_draw_multiline_text(
                    m_font.get(),
                    BLACK,
                    text_x + 1,
                    1,
                    al_get_bitmap_width(m_bitmap),
                    0,
                    m_flags,
                    m_text.c_str());
            }
            al_draw_multiline_text(
                m_font.get(),
                m_color,
                text_x,
                0,
                al_get_bitmap_width(m_bitmap),
                0,
                m_flags,
                m_text.c_str());
        } else {
            if (m_shadow) {
                al_draw_text(
                    m_font.get(),
                    BLACK,
                    text_x + 1,
                    1,
                    m_flags,
                    m_text.c_str());
            }
            al_draw_text(
                m_font.get(), m_color, text_x, 0, m_flags, m_text.c_str());
        }
        m_dirty = false;
    }
}
// vi: ft=cpp
