/*
        STARFLIGHT - THE LOST COLONY
        ModuleEngineer.cpp - The Engineering module.
        Author: Keith "Daikaze" Patch
        Date: 5-27-2008
*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "AudioSystem.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "ModuleEngineer.h"
#include "Script.h"
#include "Util.h"
#include "engineer_resources.h"

using namespace engineer;
using namespace std;

static const int VIEWER_X = 120;
static const int VIEWER_Y = 0;
static const int VIEWER_W = 799;
static const int VIEWER_H = 500;
static const int VIEWER_CONTENT_X = VIEWER_X + 54;
static const int VIEWER_CONTENT_Y = VIEWER_Y + 78;
static const int SHIP_W = 132;
static const int SHIP_H = 342;
static const int TITLE_W = 114;
static const int TITLE_H = 12;
static const int GAUGE_W = 120;
static const int GAUGE_H = 20;

static const int SHIP_X = VIEWER_CONTENT_X + 345 - SHIP_W / 2;
static const int SHIP_Y = VIEWER_CONTENT_Y + 192 - SHIP_H / 2;
static const int REPAIR_BUTTON_W = 20;
static const int REPAIR_BUTTON_H = 20;
static const int LEFT_TITLE_OFFSET_X = 0;
static const int RIGHT_TITLE_OFFSET_X = REPAIR_BUTTON_W + 5;
static const int TITLE_OFFSET_Y = 0;

static const int LEFT_REPAIR_BUTTON_OFFSET_X = GAUGE_W + 5;
static const int RIGHT_REPAIR_BUTTON_OFFSET_X = 0;
static const int REPAIR_BUTTON_OFFSET_Y = 20;
static const int GAUGE_OFFSET_Y = REPAIR_BUTTON_OFFSET_Y;
static const int REPAIR_GAUGE_OFFSET_Y = VIEWER_CONTENT_Y + 80;
static const int LEFT_GAUGE_X_OFFSET = 150;
static const int RIGHT_GAUGE_X_OFFSET = 500;
static const int GAUGE_VERTICAL_SPACING = 80;
static const int LINE_W = GAUGE_W + REPAIR_BUTTON_W + 5;
static const int LINE_Y_OFFSET =
    TITLE_OFFSET_Y + REPAIR_BUTTON_OFFSET_Y / 2 + 5;

static const int MISSILE_Y = 74;
static const int ARMOR_X = 50;
static const int ARMOR_Y = 146;
static const int ENGINE_X = 64;
static const int ENGINE_Y = 260;
static const int LASER_X = 67;
static const int LASER_Y = 5;
static const int HULL_X = 67;
static const int HULL_Y = 155;
static const int SHIELDS_X = 67;
static const int SHIELDS_Y = 222;
static const int REPAIR_GAUGE_W = LINE_W;
static const int REPAIR_GAUGE_H = 2 * GAUGE_H;

ALLEGRO_DEBUG_CHANNEL("ModuleEngineer")

RepairGauge::RepairGauge(
    int x,
    int y,
    const std::string &fill_image_name,
    EventType repair_event,
    const std::string &title,
    GaugeSide gauge_side)
    : Module(
        x
            + ((gauge_side == RepairGauge::GAUGE_LEFT) ? LEFT_GAUGE_X_OFFSET
                                                       : RIGHT_GAUGE_X_OFFSET),
        y + TITLE_OFFSET_Y,
        REPAIR_GAUGE_W,
        REPAIR_GAUGE_H),
      m_title(make_shared<Label>(
          title,
          x
              + ((gauge_side == RepairGauge::GAUGE_LEFT)
                     ? (LEFT_GAUGE_X_OFFSET + LEFT_TITLE_OFFSET_X)
                     : (RIGHT_GAUGE_X_OFFSET + RIGHT_TITLE_OFFSET_X)),
          y + TITLE_OFFSET_Y,
          TITLE_W,
          TITLE_H,
          false,
          gauge_side == RepairGauge::GAUGE_LEFT ? 0 : ALLEGRO_ALIGN_RIGHT,
          g_game->font10,
          WHITE)),
      m_repair_button(make_shared<Button>(
          x
              + ((gauge_side == RepairGauge::GAUGE_LEFT)
                     ? (LEFT_GAUGE_X_OFFSET + LEFT_REPAIR_BUTTON_OFFSET_X)
                     : (RIGHT_GAUGE_X_OFFSET + RIGHT_REPAIR_BUTTON_OFFSET_X)),
          y + REPAIR_BUTTON_OFFSET_Y,
          REPAIR_BUTTON_W,
          REPAIR_BUTTON_H,
          EVENT_NONE,
          repair_event,
          images[I_AUX_REPAIR],
          images[I_AUX_REPAIR_HOVER],
          nullptr)),

      m_background(make_shared<Bitmap>(
          images[I_ELEMENT_GAUGE_GRAY],
          x
              + ((gauge_side == RepairGauge::GAUGE_LEFT)
                     ? LEFT_GAUGE_X_OFFSET
                     : (RIGHT_GAUGE_X_OFFSET + RIGHT_REPAIR_BUTTON_OFFSET_X
                        + REPAIR_BUTTON_W + 5)),
          y + GAUGE_OFFSET_Y,
          GAUGE_W,
          GAUGE_H)),
      m_fill(make_shared<Bitmap>(
          images[fill_image_name],
          x
              + ((gauge_side == RepairGauge::GAUGE_LEFT)
                     ? LEFT_GAUGE_X_OFFSET
                     : (RIGHT_GAUGE_X_OFFSET + RIGHT_REPAIR_BUTTON_OFFSET_X
                        + REPAIR_BUTTON_W + 5)),
          y + GAUGE_OFFSET_Y,
          GAUGE_W,
          GAUGE_H)),
      m_line(shared_ptr<ALLEGRO_BITMAP>(
          al_create_bitmap(LINE_W, 1),
          al_destroy_bitmap)),
      m_line_bitmap(make_shared<Bitmap>(
          m_line,
          x
              + ((gauge_side == RepairGauge::GAUGE_LEFT)
                     ? (LEFT_GAUGE_X_OFFSET)
                     : (RIGHT_GAUGE_X_OFFSET)),
          y + LINE_Y_OFFSET,
          LINE_W,
          1)),
      m_line_node_x(
          x
          + ((gauge_side == RepairGauge::GAUGE_LEFT)
                 ? (LEFT_GAUGE_X_OFFSET + LINE_W)
                 : (RIGHT_GAUGE_X_OFFSET))),
      m_line_node_y(y + LINE_Y_OFFSET) {
    al_set_target_bitmap(m_line.get());
    al_draw_line(0, 0, LINE_W, 0, GREEN, 1);
    add_child_module(m_title);
    add_child_module(m_repair_button);

    if (repair_event == EVENT_NONE) {
        m_repair_button->set_active(false);
    }
    add_child_module(m_background);
    add_child_module(m_fill);
    add_child_module(m_line_bitmap);
}

RepairGauge::~RepairGauge() {}

void
RepairGauge::set_value(int value) {
    int fill = value * GAUGE_W / 100;
    if (value == 100 || value == 0) {
        m_repair_button->set_enabled(false);
    }
    m_fill->set_clip_width(fill);
}

ModuleEngineer::ModuleEngineer()
    : Module(VIEWER_X, VIEWER_Y, VIEWER_W, VIEWER_H),
      m_viewer(make_shared<SlidingModule<Bitmap>>(
          SLIDE_FROM_TOP,
          EVENT_NONE,
          0.6,
          images[I_GUI],
          VIEWER_X,
          VIEWER_Y)),
      m_ship(make_shared<Bitmap>(nullptr, SHIP_X, SHIP_Y, SHIP_W, SHIP_H)),
      m_lines(
          make_shared<Bitmap>(nullptr, VIEWER_X, VIEWER_Y, VIEWER_W, VIEWER_H)),
      m_missile_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y,
          I_ELEMENT_GAUGE_PURPLE,
          EVENT_ENGINEER_REPAIR_MISSILES,
          "MISSILES",
          RepairGauge::GAUGE_LEFT)),
      m_armor_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y + GAUGE_VERTICAL_SPACING,
          I_ELEMENT_GAUGE_RED,
          EVENT_NONE,
          "ARMOR",
          RepairGauge::GAUGE_LEFT)),
      m_engine_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y + 2 * GAUGE_VERTICAL_SPACING,
          I_ELEMENT_GAUGE_ORANGE,
          EVENT_ENGINEER_REPAIR_ENGINES,
          "ENGINES",
          RepairGauge::GAUGE_LEFT)),
      m_laser_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y,
          I_ELEMENT_GAUGE_MAGENTA,
          EVENT_ENGINEER_REPAIR_LASERS,
          "LASERS",
          RepairGauge::GAUGE_RIGHT)),
      m_hull_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y + GAUGE_VERTICAL_SPACING,
          I_ELEMENT_GAUGE_GREEN,
          EVENT_ENGINEER_REPAIR_HULL,
          "HULL",
          RepairGauge::GAUGE_RIGHT)),
      m_shield_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y + 2 * GAUGE_VERTICAL_SPACING,
          I_ELEMENT_GAUGE_BLUE,
          EVENT_ENGINEER_REPAIR_SHIELDS,
          "SHIELDS",
          RepairGauge::GAUGE_RIGHT)) {
    add_child_module(m_viewer);
    m_viewer->add_child_module(m_ship);
    m_viewer->add_child_module(m_missile_gauge);
    m_viewer->add_child_module(m_armor_gauge);
    m_viewer->add_child_module(m_engine_gauge);
    m_viewer->add_child_module(m_laser_gauge);
    m_viewer->add_child_module(m_hull_gauge);
    m_viewer->add_child_module(m_shield_gauge);

    m_lines->set_bitmap(create_lines_bitmap());
    m_viewer->add_child_module(m_lines);
}

bool
ModuleEngineer::on_init() {
    ALLEGRO_DEBUG("  ModuleEngineer Initialize\n");

    m_viewer_is_active = false;
    if (m_viewer->is_open()) {
        m_viewer->toggle();
    }

    switch (g_game->gameState->getProfession()) {
    case PROFESSION_FREELANCE:
        m_ship->set_bitmap(images[I_HIGH_RES_SHIP_FREELANCE]);
        break;

    case PROFESSION_MILITARY:
        m_ship->set_bitmap(images[I_HIGH_RES_SHIP_MILITARY]);
        break;

    case PROFESSION_SCIENTIFIC:
    default:
        m_ship->set_bitmap(images[I_HIGH_RES_SHIP_SCIENCE]);
        break;
    }

    return true;
}

// return false if repair ceased due to lack of mineral, true otherwise
bool
ModuleEngineer::use_mineral(Ship &ship) {

    GameState *gs = g_game->gameState;
    ShipPart repairing = ship.partInRepair;

    if (repairing == PART_NONE) {
        ALLEGRO_DEBUG("engineer: [ERROR] use_mineral() was called while no "
                      "repair were in progress\n");
        return false;
    }

    // we do up to MAX_REPAIR_COUNT repair iterations before consuming one
    // mineral
    if (ship.repairCounters[repairing - 1] < MAX_REPAIR_COUNT) {
        ship.repairCounters[repairing - 1]++;
        return true;

    } else {
        int neededMineral = ship.repairMinerals[repairing - 1];
        Item mineral;
        int num_mineral;

        gs->m_items.Get_Item_By_ID(neededMineral, mineral, num_mineral);

        if (num_mineral == 0) {
            // mineral not in the cargo hold, stop repair
            std::string mineralName =
                g_game->dataMgr->GetItemByID(neededMineral)->name;
            g_game->printout(
                OFFICER_ENGINEER,
                "Repairing ceased due to lack of " + mineralName + ".",
                RED,
                5000);
            return false;

        } else {
            // consume the mineral
            std::string msg = "Using one cubic meter of " + mineral.name + ".";
            g_game->printout(OFFICER_ENGINEER, msg, GREEN, 1000);

            gs->m_items.RemoveItems(mineral.id, 1);
            ALLEGRO_EVENT e = {
                .type = static_cast<unsigned int>(EVENT_CARGO_UPDATE)};
            g_game->broadcast_event(&e);

            // roll a new one
            switch (sfrand() % 5) {
            case 0:
                ship.repairMinerals[repairing - 1] = ITEM_COBALT;
                break;
            case 1:
                ship.repairMinerals[repairing - 1] = ITEM_MOLYBDENUM;
                break;
            case 2:
                ship.repairMinerals[repairing - 1] = ITEM_ALUMINUM;
                break;
            case 3:
                ship.repairMinerals[repairing - 1] = ITEM_TITANIUM;
                break;
            case 4:
                ship.repairMinerals[repairing - 1] = ITEM_SILICA;
                break;
            default:
                ALLEGRO_ASSERT(0);
            }

            if (g_game->gameState->add_experience(SKILL_ENGINEERING, 1)) {
                g_game->printout(
                    OFFICER_ENGINEER,
                    "I think I'm getting better at this.",
                    PURPLE,
                    5000);
            }

            // reset the counter
            ship.repairCounters[repairing - 1] = 0;

            return true;
        }
    }

    // UNREACHABLE
    ALLEGRO_ASSERT(0);
}

shared_ptr<ALLEGRO_BITMAP>
ModuleEngineer::create_lines_bitmap() {
    ALLEGRO_BITMAP *target = al_create_bitmap(VIEWER_W, VIEWER_H);
    ALLEGRO_TRANSFORM translate;
    al_identity_transform(&translate);
    al_translate_transform(&translate, -VIEWER_X, -VIEWER_Y);
    al_set_target_bitmap(target);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_use_transform(&translate);

    pair<int, int> coords;
    auto &[x, y] = coords;

    coords = m_missile_gauge->get_line_node();
    al_draw_line(x, y, SHIP_X + SHIP_W / 2, SHIP_Y + MISSILE_Y, GREEN, 1);

    coords = m_armor_gauge->get_line_node();
    al_draw_line(
        coords.first,
        coords.second,
        SHIP_X + ARMOR_X,
        SHIP_Y + ARMOR_Y,
        GREEN,
        1);

    coords = m_engine_gauge->get_line_node();
    al_draw_line(
        coords.first,
        coords.second,
        SHIP_X + ENGINE_X,
        SHIP_Y + ENGINE_Y,
        GREEN,
        1);

    coords = m_laser_gauge->get_line_node();
    al_draw_line(
        coords.first,
        coords.second,
        SHIP_X + LASER_X,
        SHIP_Y + LASER_Y,
        GREEN,
        1);

    coords = m_hull_gauge->get_line_node();
    al_draw_line(
        coords.first,
        coords.second,
        SHIP_X + HULL_X,
        SHIP_Y + HULL_Y,
        GREEN,
        1);

    coords = m_shield_gauge->get_line_node();
    al_draw_line(
        coords.first,
        coords.second,
        SHIP_X + SHIELDS_X,
        SHIP_Y + SHIELDS_Y,
        GREEN,
        1);

    return shared_ptr<ALLEGRO_BITMAP>(target, al_destroy_bitmap);
}

bool
ModuleEngineer::on_update() {
    auto current_officer = g_game->gameState->getCurrentSelectedOfficer();

    if (current_officer != OFFICER_ENGINEER) {
        if (!m_viewer->is_closed()) {
            m_viewer->toggle();
        }
        m_viewer_is_active = false;
    } else if (m_viewer_is_active && !m_viewer->is_visible()) {
        m_viewer_is_active = false;
        return true;
    }

    Ship ship = g_game->gameState->getShip();
    float repair_time, repair_rate = 0;
    float repair_skill =
        g_game->gameState->CalcEffectiveSkill(SKILL_ENGINEERING);
    std::string s;

    s = "MISSILES: " + ship.getMissileLauncherClassString();
    m_missile_gauge->set_title(s);
    m_missile_gauge->set_value(ship.getMissileLauncherIntegrity());

    s = "ARMOR: " + ship.getArmorClassString();
    m_armor_gauge->set_title(s);
    m_armor_gauge->set_value(
        100 * ship.getArmorIntegrity() / ship.getMaxArmorIntegrity());

    s = "ENGINES: " + ship.getEngineClassString();
    m_engine_gauge->set_title(s);
    m_engine_gauge->set_value(ship.getEngineIntegrity());

    s = "LASERS: " + ship.getLaserClassString();
    m_laser_gauge->set_title(s);
    m_laser_gauge->set_value(ship.getLaserIntegrity());

    s = "HULL";
    m_shield_gauge->set_title(s);
    m_hull_gauge->set_value(ship.getHullIntegrity());

    s = "SHIELDS: " + ship.getShieldClassString();
    m_shield_gauge->set_title(s);
    m_shield_gauge->set_value(ship.getShieldIntegrity());

    if (ship.partInRepair == PART_LASERS) {
        if (ship.getLaserIntegrity() < 100 && ship.getLaserIntegrity() > 0) {
            if (g_game->gameState->CanSkillCheck(SKILL_ENGINEERING)) {
                g_game->gameState->add_experience(SKILL_ENGINEERING, 0);
                if (use_mineral(ship)) {
                    repair_time = 8 * (6 - repair_skill / 50);
                    repair_rate = 100 / repair_time;
                    ship.augLaserIntegrity(repair_rate);
                } else /* no more repair metal; stopping repair */
                    ship.partInRepair = PART_NONE;
            }
        } else {
            ship.partInRepair = PART_NONE;
            g_game->printout(
                OFFICER_ENGINEER,
                "The lasers are now fully functional!",
                BLUE,
                5000);
        }
    }

    s = "MISSILES: " + ship.getMissileLauncherClassString();
    if (ship.partInRepair == PART_MISSILES) {
        if (ship.getMissileLauncherIntegrity() < 100
            && ship.getMissileLauncherIntegrity() > 0) {
            if (g_game->gameState->CanSkillCheck(SKILL_ENGINEERING)) {
                g_game->gameState->add_experience(SKILL_ENGINEERING, 0);
                if (use_mineral(ship)) {
                    repair_time = 8 * (6 - repair_skill / 50);
                    repair_rate = 100 / repair_time;
                    ship.augMissileLauncherIntegrity(repair_rate);
                } else /* no more repair metal; stopping repair */
                    ship.partInRepair = PART_NONE;
            }
        } else {
            ship.partInRepair = PART_NONE;
            g_game->printout(
                OFFICER_ENGINEER,
                "The missile system is now fully functional!",
                BLUE,
                5000);
        }
    }

    s = "HULL";
    if (ship.partInRepair == PART_HULL) {
        if (ship.getHullIntegrity() < 100 && ship.getHullIntegrity() > 0) {
            if (g_game->gameState->CanSkillCheck(SKILL_ENGINEERING)) {
                g_game->gameState->add_experience(SKILL_ENGINEERING, 0);
                if (use_mineral(ship)) {
                    repair_time = 25 * (6 - repair_skill / 50);
                    repair_rate = 100 / repair_time;
                    ship.augHullIntegrity(repair_rate);
                } else /* no more repair metal; stopping repair */
                    ship.partInRepair = PART_NONE;
            }
        } else {
            ship.partInRepair = PART_NONE;
            g_game->printout(
                OFFICER_ENGINEER,
                "The hull is now fully repaired!",
                BLUE,
                5000);
        }
    }

    s = "SHIELDS: " + ship.getShieldClassString();
    if (ship.partInRepair == PART_SHIELDS) {
        if (ship.getShieldIntegrity() < 100 && ship.getShieldIntegrity() > 0) {
            if (g_game->gameState->CanSkillCheck(SKILL_ENGINEERING)) {
                g_game->gameState->add_experience(SKILL_ENGINEERING, 0);
                if (use_mineral(ship)) {
                    repair_time = 10 * (6 - repair_skill / 50);
                    repair_rate = 100 / repair_time;
                    ship.augShieldIntegrity(repair_rate);
                    ship.setShieldCapacity(ship.getMaxShieldCapacity());
                } else /* no more repair metal; stopping repair */
                    ship.partInRepair = PART_NONE;
            }
        } else {
            ship.partInRepair = PART_NONE;
            ship.setShieldCapacity(ship.getMaxShieldCapacity());
            g_game->printout(
                OFFICER_ENGINEER,
                "The shields are now fully functional!",
                BLUE,
                5000);
        }
    }

    s = "ENGINES: " + ship.getEngineClassString();
    if (ship.partInRepair == PART_ENGINES) {
        if (ship.getEngineIntegrity() < 100 && ship.getEngineIntegrity() > 0) {
            if (g_game->gameState->CanSkillCheck(SKILL_ENGINEERING)) {
                g_game->gameState->add_experience(SKILL_ENGINEERING, 0);
                if (use_mineral(ship)) {
                    repair_time = 10 * (6 - repair_skill / 50);
                    repair_rate = 100 / repair_time;
                    ship.augEngineIntegrity(repair_rate);
                } else /* no more repair metal; stopping repair */
                    ship.partInRepair = PART_NONE;
            }
        } else {
            ship.partInRepair = PART_NONE;
            g_game->printout(
                OFFICER_ENGINEER,
                "The engines are now fully repaired!",
                BLUE,
                5000);
        }
    }
    g_game->gameState->setShip(ship);
    return true;
}

bool
ModuleEngineer::on_event(ALLEGRO_EVENT *event) {
    ShipPart repairing = g_game->gameState->m_ship.partInRepair;

    switch (event->type) {
    case EVENT_ENGINEER_REPAIR:
        m_viewer->toggle();
        if (!m_viewer_is_active) {
            m_viewer_is_active = true;
        }
        return true;
    case EVENT_ENGINEER_INJECT:
        if (!m_viewer->is_closed()) {
            m_viewer_is_active = false;
            m_viewer->toggle();
        }
        return true;

    case EVENT_ENGINEER_REPAIR_LASERS:
        if (g_game->gameState->getShip().getLaserIntegrity() > 0
            && g_game->gameState->getShip().getLaserIntegrity() < 100
            && g_game->gameState->CanSkillCheck(SKILL_ENGINEERING)
            && repairing != PART_LASERS) {
            repairing = PART_LASERS;
        } else {
            repairing = PART_NONE;
        }
        break;
    case EVENT_ENGINEER_REPAIR_MISSILES:
        if (g_game->gameState->getShip().getMissileLauncherIntegrity() > 0
            && g_game->gameState->getShip().getMissileLauncherIntegrity() < 100
            && g_game->gameState->CanSkillCheck(SKILL_ENGINEERING)
            && repairing != PART_MISSILES) {
            repairing = PART_MISSILES;
        } else {
            repairing = PART_NONE;
        }
        break;
    case EVENT_ENGINEER_REPAIR_HULL:
        if (g_game->gameState->getShip().getHullIntegrity() > 0
            && g_game->gameState->getShip().getHullIntegrity() < 100
            && g_game->gameState->CanSkillCheck(SKILL_ENGINEERING)
            && repairing != PART_HULL) {
            repairing = PART_HULL;
        } else {
            repairing = PART_NONE;
        }
        break;
    case EVENT_ENGINEER_REPAIR_SHIELDS:
        if (g_game->gameState->getShip().getShieldIntegrity() > 0
            && g_game->gameState->getShip().getShieldIntegrity() < 100
            && g_game->gameState->CanSkillCheck(SKILL_ENGINEERING)
            && repairing != PART_SHIELDS) {
            repairing = PART_SHIELDS;
        } else {
            repairing = PART_NONE;
        }
        break;
    case EVENT_ENGINEER_REPAIR_ENGINES:
        if (g_game->gameState->getShip().getEngineIntegrity() > 0
            && g_game->gameState->getShip().getEngineIntegrity() < 100
            && g_game->gameState->CanSkillCheck(SKILL_ENGINEERING)
            && repairing != PART_ENGINES) {
            repairing = PART_ENGINES;
        } else {
            repairing = PART_NONE;
        }
        break;
    case 0:
    default:
        break;
    }

    if (g_game->gameState->m_ship.partInRepair != repairing) {
        g_game->gameState->m_ship.partInRepair = repairing;
        g_game->audioSystem->Play(samples[S_BUTTONCLICK]);
    }

    return true;
}
// vi: ft=cpp
