#include <allegro5/allegro.h>

#include "Bitmap.h"

Bitmap::Bitmap(ALLEGRO_BITMAP *bitmap, int x, int y, int width, int height)
    : Module(
        x,
        y,
        width == -1 ? al_get_bitmap_width(bitmap) : width,
        height == -1 ? al_get_bitmap_height(bitmap) : height),
      m_visible_x(0), m_visible_y(0), m_visible_width(m_width),
      m_visible_height(m_height), m_bitmap(bitmap) {}

bool
Bitmap::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);
    al_draw_bitmap_region(
        m_bitmap,
        m_visible_x,
        m_visible_y,
        m_visible_width,
        m_visible_height,
        m_x,
        m_y,
        0);

    return true;
}

void
Bitmap::set_visible_region(int x, int y, int width, int height) {
    m_visible_x = x;
    m_visible_y = y;

    m_visible_width = width;
    m_visible_height = height;
}
