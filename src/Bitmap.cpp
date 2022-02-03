#include <allegro5/allegro.h>

#include "Bitmap.h"

Bitmap::Bitmap(ALLEGRO_BITMAP *bitmap, int x, int y, int width, int height)
    : Module(
        x,
        y,
        width == -1 && bitmap ? al_get_bitmap_width(bitmap) : width,
        height == -1 && bitmap ? al_get_bitmap_height(bitmap) : height),
      m_visible_x(0), m_visible_y(0), m_bitmap(bitmap) {}

bool
Bitmap::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);
    if (m_bitmap) {
        al_draw_bitmap_region(
            m_bitmap, m_visible_x, m_visible_y, m_width, m_height, m_x, m_y, 0);
    }

    return true;
}

void
Bitmap::set_bitmap(ALLEGRO_BITMAP *bitmap) {
    int w, h;

    if (bitmap) {
        w = al_get_bitmap_width(bitmap);
    } else {
        w = -1;
    }
    if (bitmap) {
        h = al_get_bitmap_height(bitmap);
    } else {
        h = -1;
    }
    m_bitmap = bitmap;
    resize(w, h);
}

void
Bitmap::set_visible_region(int x, int y, int width, int height) {
    m_visible_x = x;
    m_visible_y = y;

    resize(width, height);
}
// vi: ft=cpp
