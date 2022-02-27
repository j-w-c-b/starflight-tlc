/*
        STARFLIGHT - THE LOST COLONY
        ModuleTopGUI.cpp
        Author: Keith Patch
        Date: April 2008
*/

#ifndef MODULETOPGUI_H
#define MODULETOPGUI_H

#include <memory>

#include <allegro5/allegro.h>

#include "Bitmap.h"
#include "Module.h"

class ModuleTopGUI : public Module {
  public:
    virtual bool on_init() override;
    virtual bool on_close() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;

  private:
    std::shared_ptr<Bitmap> m_top_gauge;
    std::shared_ptr<Bitmap> m_hull_gauge;
    std::shared_ptr<Bitmap> m_armor_gauge;
    std::shared_ptr<Bitmap> m_shields_gauge;
    std::shared_ptr<Bitmap> m_fuel_gauge;
};

#endif
