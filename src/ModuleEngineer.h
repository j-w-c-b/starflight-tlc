/*
        STARFLIGHT - THE LOST COLONY
        ModuleEngineer.h - The Engineering module.
        Author: Keith "Daikaze" Patch
        Date: 5-27-2008
*/

#ifndef MODULEENGINEER_H
#define MODULEENGINEER_H

#include "Button.h"
#include "GameState.h"
#include "Module.h"
#include "SlidingModule.h"

class RepairGauge : public Module {
  public:
    enum GaugeSide
    {
        GAUGE_LEFT,
        GAUGE_RIGHT
    };

    RepairGauge(
        int x,
        int y,
        const std::string &fill_image_name,
        EventType repair_event,
        const std::string &title,
        GaugeSide gauge_side);
    virtual ~RepairGauge();

    void set_value(int value);
    void set_title(const std::string &title) { m_title->set_text(title); }
    std::pair<int, int> get_line_node() {
        return {m_line_node_x, m_line_node_y};
    }

  private:
    std::shared_ptr<Label> m_title;
    std::shared_ptr<Button> m_repair_button;
    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<Bitmap> m_fill;
    std::shared_ptr<ALLEGRO_BITMAP> m_line;
    std::shared_ptr<Bitmap> m_line_bitmap;
    int m_line_node_x;
    int m_line_node_y;
};

class ModuleEngineer : public Module {
  public:
    ModuleEngineer();

    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    bool use_mineral(Ship &ship);
    std::shared_ptr<ALLEGRO_BITMAP> create_lines_bitmap();

    std::shared_ptr<SlidingModule<Bitmap>> m_viewer;
    std::shared_ptr<Bitmap> m_ship;
    std::shared_ptr<Bitmap> m_lines;
    std::shared_ptr<RepairGauge> m_missile_gauge;
    std::shared_ptr<RepairGauge> m_armor_gauge;
    std::shared_ptr<RepairGauge> m_engine_gauge;
    std::shared_ptr<RepairGauge> m_laser_gauge;
    std::shared_ptr<RepairGauge> m_hull_gauge;
    std::shared_ptr<RepairGauge> m_shield_gauge;
    bool m_viewer_is_active;
};

#endif
