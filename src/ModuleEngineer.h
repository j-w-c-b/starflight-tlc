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
#include "ResourceManager.h"
#include "ScrollBox.h"

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
        GaugeSide gauge_side,
        ResourceManager<ALLEGRO_BITMAP> &resources);
    virtual ~RepairGauge();

    void set_value(int value);
    void set_title(const std::string &title) { m_title->set_text(title); }
    void get_line_node(int &x, int &y) {
        x = m_line_node_x;
        y = m_line_node_y;
    }

  private:
    std::shared_ptr<Label> m_title;
    std::shared_ptr<NewButton> m_repair_button;
    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<Bitmap> m_fill;
    ALLEGRO_BITMAP *m_line;
    std::shared_ptr<Bitmap> m_line_bitmap;
    int m_line_node_x;
    int m_line_node_y;
};

class ModuleEngineer : public Module {
  public:
    ModuleEngineer();
    virtual ~ModuleEngineer();

    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_close() override;
    virtual bool on_draw(ALLEGRO_BITMAP *bitmap) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    bool use_mineral(Ship &ship);

    ResourceManager<ALLEGRO_BITMAP> m_resources;

    std::shared_ptr<Bitmap> m_viewer;
    std::shared_ptr<Bitmap> m_ship;
    std::shared_ptr<RepairGauge> m_missile_gauge;
    std::shared_ptr<RepairGauge> m_armor_gauge;
    std::shared_ptr<RepairGauge> m_engine_gauge;
    std::shared_ptr<RepairGauge> m_laser_gauge;
    std::shared_ptr<RepairGauge> m_hull_gauge;
    std::shared_ptr<RepairGauge> m_shield_gauge;
};

#endif
