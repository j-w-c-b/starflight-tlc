/*
        STARFLIGHT - THE LOST COLONY
        ModuleAuxiliaryWindow.cpp
        Author: J.Harbour
        Date: Jan 2008
*/

#ifndef _MODULEAUXILIARYDISPLAY_H
#define _MODULEAUXILIARYDISPLAY_H

#include <memory>

#include "Bitmap.h"
#include "GameState.h"
#include "Module.h"
#include "Sprite.h"
#include "TileScroller.h"

class ModuleAuxiliaryDisplay : public Module {
  public:
    ModuleAuxiliaryDisplay();

    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

    void init_nav();
    void updateCrew(ALLEGRO_BITMAP *target);
    void updateAll(ALLEGRO_BITMAP *target);
    void updateCap(ALLEGRO_BITMAP *target);
    void updateSci(ALLEGRO_BITMAP *target);
    void updateNav(ALLEGRO_BITMAP *target);
    void updateEng(ALLEGRO_BITMAP *target);
    void updateCom(ALLEGRO_BITMAP *target);
    void updateTac(ALLEGRO_BITMAP *target);
    void updateMed(ALLEGRO_BITMAP *target);
    void updateCargoFillPercent();
    void place_flux_tile(bool visible, int tile);
    void PrintSystemStatus(int x, int y, const std::string &title, int value);

    ALLEGRO_COLOR HEADING_COLOR;

  private:
    int gax, gay, asx, asy, asw, ash;
    int cargoFillPercent;

    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<ALLEGRO_BITMAP> ship_icon_image;
    Sprite *ship_icon_sprite;
    std::shared_ptr<TileScroller> m_scroller;

    void medical_display(
        ALLEGRO_BITMAP *target,
        const Officer *officer_data,
        int x,
        int y,
        const std::string &additional_data);
    void DrawBackground(ALLEGRO_BITMAP *target);
    void DrawContent(ALLEGRO_BITMAP *target);
};

#endif
