#pragma once

#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "Module.h"

class Bitmap : public Module {
  public:
    Bitmap(
        ALLEGRO_BITMAP *bitmap,
        int x,
        int y,
        int width = -1,
        int height = -1);
    virtual ~Bitmap() {}

    virtual bool on_draw(ALLEGRO_BITMAP *target) override;

    virtual void set_visible_region(int x, int y, int width, int height);

  private:
    int m_visible_x;
    int m_visible_y;
    int m_visible_width;
    int m_visible_height;
    ALLEGRO_BITMAP *m_bitmap;
};

// vi: ft=cpp
