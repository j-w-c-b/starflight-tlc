#ifndef STLC_BITMAP_H
#define STLC_BITMAP_H

#include <memory>

#include <allegro5/allegro.h>

#include "Module.h"

class Bitmap : public Module {
  public:
    explicit Bitmap(
        std::shared_ptr<ALLEGRO_BITMAP> bitmap,
        int x = 0,
        int y = 0,
        int width = -1,
        int height = -1,
        int flags = 0);

    void set_bitmap(std::shared_ptr<ALLEGRO_BITMAP> bitmap);
    std::shared_ptr<ALLEGRO_BITMAP> get_bitmap() const { return m_bitmap; }
    void set_flags(int flags);
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;

  private:
    int m_flags;
    std::shared_ptr<ALLEGRO_BITMAP> m_bitmap;
};
#endif // STLC_BITMAP_H
// vi: ft=cpp
