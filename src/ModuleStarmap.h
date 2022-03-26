/*
        STARFLIGHT - THE LOST COLONY
        ModuleStarmap.h - The Starmap module.
        Author: Keith "Daikaze" Patch
        Date: ??-??-2007
*/

#ifndef MODULESTARMAP_H
#define MODULESTARMAP_H

#include <memory>

#include "Bitmap.h"
#include "DataMgr.h"
#include "Label.h"
#include "Module.h"
#include "Point2D.h"
#include "SlidingModule.h"

struct StarmapConfig {
    StarmapConfig();

    //! Width (pixels) of the starmap viewer including the frame.
    int viewer_width;
    //! Height (pixels) of the starmap viewer including the frame.
    int viewer_height;
    //! X coordinate (pixels) of left side of the starmap viewer
    int viewer_x;
    //! Y coordinate (pixels) of top of the starmap viewer
    int viewer_y;
    //! Width (pixels) of the map within the viewer
    int map_width;
    //! Height (pixels) of the map within the viewer
    int map_height;
    //! X coordinate (pixels) of the left side of the map relative to the viewer
    int map_x;
    //! Y coordinate (pixels) of the top of the map relative to the viewer
    int map_y;

    //! Width (pixels) of a text label
    int text_width;

    //! Height (pixels) of a text label
    int text_height;

    //! X coordinate (pixels) of the position x text label relative to the
    //! viewer
    int position_x;
    //! X coordinate (pixels) of the position y text label relative to the
    //! viewer
    int position_y;
    //! X coordinate (pixels) of the destination x text label relative to the
    //! viewer
    int destination_x;
    //! X coordinate (pixels) of the destination y text label relative to the
    //! viewer
    int destination_y;
    //! X coordinate (pixels) of the distance label relative to the viewer
    int distance_x;
    //! X coordinate (pixels) of the fuel label relative to the viewer
    int fuel_x;
    //! Y coordinate (pixels) of the fuel label relative to the viewer
    int text_field_y;

    //! X Scaling factor due to different ratio of galaxy w/h to viewer w/h
    double scale_x;
    //! Y Scaling factor due to different ratio of galaxy w/h to viewer w/h
    double scale_y;
};

class ModuleStarmap : public Module {
  public:
    ModuleStarmap();
    virtual bool on_init() override;
    virtual bool on_close() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_click(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_update() override;

    void save_map(const std::string &filename);

  private:
    void draw_flux();
    void draw_stars();

    StarmapConfig m_config;

    bool m_dest_active;
    bool m_over_star;

    std::shared_ptr<SlidingModule<Bitmap>> m_viewer;

    //! Bitmap containing the set of stars and flux
    std::shared_ptr<ALLEGRO_BITMAP> m_starview;
    std::shared_ptr<Bitmap> m_starview_bitmap;

    Point2D m_cursor_pos;
    Point2D m_dest_pos;

    std::shared_ptr<Label> m_star_label;
    std::shared_ptr<Label> m_position_x_label;
    std::shared_ptr<Label> m_position_y_label;
    std::shared_ptr<Label> m_destination_x_label;
    std::shared_ptr<Label> m_destination_y_label;
    std::shared_ptr<Label> m_distance_label;
    std::shared_ptr<Label> m_fuel_label;
    std::shared_ptr<Bitmap> m_circles_bitmap;
    std::shared_ptr<ALLEGRO_BITMAP> m_flux_image;

    CoordValue m_star_x;
    CoordValue m_star_y;
    bool m_viewer_active;
};

#endif
