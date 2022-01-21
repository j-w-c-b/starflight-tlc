/*
        STARFLIGHT - THE LOST COLONY
        ModuleAuxiliaryWindow.cpp
        Author: J.Harbour
        Date: Jan 2008
*/

#ifndef _MODULEAUXILIARYDISPLAY_H
#define _MODULEAUXILIARYDISPLAY_H

#include "Module.h"
#include "ResourceManager.h"
#include "TileScroller.h"

class ModuleAuxiliaryDisplay : public Module {
  private:
    int gax, gay, asx, asy, asw, ash;

    int cargoFillPercent;

    ALLEGRO_BITMAP *ship_icon_image;
    Sprite *ship_icon_sprite;
    ALLEGRO_BITMAP *img_aux;
    ALLEGRO_BITMAP *canvas;
    TileScroller *scroller;
    ResourceManager<ALLEGRO_BITMAP> resources;

    void medical_display(Officer *officer_data,
                         int x,
                         int y,
                         const std::string &additional_data);
    void DrawBackground();
    void DrawContent();

  public:
    ModuleAuxiliaryDisplay();
    virtual ~ModuleAuxiliaryDisplay();
    virtual bool Init() override;
    virtual void Draw() override;
    virtual void OnEvent(Event *event) override;
    virtual void Close() override;

    void init_nav();
    void updateCrew();
    void updateAll();
    void updateCap();
    void updateSci();
    void updateNav();
    void updateEng();
    void updateCom();
    void updateTac();
    void updateMed();
    void updateCargoFillPercent();
    void place_flux_tile(bool visible, int tile);
    void PrintSystemStatus(int x, int y, const std::string &title, int value);

    ALLEGRO_COLOR HEADING_COLOR;
};

#endif
