#include <iostream>

#include <allegro5/allegro.h>

#include "Bitmap.h"

using namespace std;

Bitmap::Bitmap(
    shared_ptr<ALLEGRO_BITMAP> bitmap,
    int x,
    int y,
    int width,
    int height,
    int flags)
    : Module(
        x,
        y,
        width == -1 && bitmap ? al_get_bitmap_width(bitmap.get()) : width,
        height == -1 && bitmap ? al_get_bitmap_height(bitmap.get()) : height),
      m_flags(flags), m_bitmap(bitmap) {}

bool
Bitmap::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);
    if (m_bitmap) {
        auto [x, y] = get_position();

        al_draw_bitmap_region(
            m_bitmap.get(),
            get_clip_x() - get_x(),
            get_clip_y() - get_y(),
            get_clip_width(),
            get_clip_height(),
            x,
            y,
            m_flags);
    }

    return true;
}

void
Bitmap::set_bitmap(shared_ptr<ALLEGRO_BITMAP> bitmap) {
    int w, h;

    if (bitmap) {
        w = al_get_bitmap_width(bitmap.get());
        h = al_get_bitmap_height(bitmap.get());
    } else {
        w = 0;
        h = 0;
    }
    m_bitmap = bitmap;
    resize(w, h);
    set_clip_width(w);
    set_clip_height(h);
}
// vi: ft=cpp
