/*
        STARFLIGHT - THE LOST COLONY
        ModuleStarmap.h - The Starmap module.
        Author: Keith "Daikaze" Patch
        Date: ??-??-2007
*/

#ifndef MODULESTARMAP_H
#define MODULESTARMAP_H

#include <array>

#include "DataMgr.h"
#include "Flux.h"
#include "Label.h"
#include "Module.h"
#include "ResourceManager.h"
#include "Sprite.h"

class ModuleStarmap : public Module {
  public:
    ModuleStarmap();
    virtual ~ModuleStarmap();
    virtual bool Init() override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void OnMouseMove(int x, int y) override;
    virtual void OnMouseClick(int button, int x, int y) override;
    virtual void OnEvent(Event *event) override;
    virtual void Close() override;

  private:
    bool map_active, dest_active, m_over_star;
    void draw_flux();
    void draw_stars();

    //! Ratio of the map width / width of the galaxy in tile coordinates
    float ratioX;
    //! Ratio of the map height / height of the galaxy in tile coordinates
    float ratioY;
    //! Width (pixels) of the starmap viewer including the frame.
    int VIEWER_WIDTH;
    //! Height (pixels) of the starmap viewer including the frame.
    int VIEWER_HEIGHT;
    //! X coordinate (pixels) of left side of the starmap viewer
    int VIEWER_X;
    //! Y coordinate (pixels) of top of the starmap viewer
    int VIEWER_Y;
    //! Width (pixels) of the map within the viewer
    int MAP_WIDTH;
    //! Height (pixels) of the map within the viewer
    int MAP_HEIGHT;
    //! X coordate (pixels) of the left side of the map relative to the viewer
    int MAP_X;
    //! Y coordate (pixels) of the top of the map relative to the viewer
    int MAP_Y;
    //! Width (pixels) of the area containing text
    int TEXT_WIDTH;
    //! Height (pixels) of the area containing text
    int TEXT_HEIGHT;
    //! Time in ticks for the viewer to fully show or be hidden
    int VIEWER_MOVE_TICKS;
    //! X coordinate (pixels) of the text fields (relative to the viewer)
    std::array<int, 6> TEXT_FIELD_X;
    //! Y coordate (pixels) of the top of the text relative to the viewer
    int TEXT_FIELD_Y;

    //! Current view y coordinate---will change as the viewer is opened or
    //  closed. This starts as -VIEWER_HEIGTH when the viewer is completely
    //  off screen, and rises to 0 when the viewer is completely on screen.
    int viewer_offset_y;

    //! Bitmap containing the set of stars and flux
    ALLEGRO_BITMAP *starview;

    //! Bitmap containing the text field values and location circles on the
    // map. This is the same size as the viewer.
    ALLEGRO_BITMAP *overlay;

    Point2D m_cursor_pos;
    Point2D m_dest_pos;

    Label *star_label;
    CoordValue star_x;
    CoordValue star_y;
    ResourceManager<ALLEGRO_BITMAP> resources;
};

#endif
