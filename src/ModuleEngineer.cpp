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

using namespace engineer_resources;
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
    GaugeSide gauge_side,
    ResourceManager<ALLEGRO_BITMAP> &resources)
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
      m_repair_button(make_shared<NewButton>(
          x
              + ((gauge_side == RepairGauge::GAUGE_LEFT)
                     ? (LEFT_GAUGE_X_OFFSET + LEFT_REPAIR_BUTTON_OFFSET_X)
                     : (RIGHT_GAUGE_X_OFFSET + RIGHT_REPAIR_BUTTON_OFFSET_X)),
          y + REPAIR_BUTTON_OFFSET_Y,
          REPAIR_BUTTON_W,
          REPAIR_BUTTON_H,
          EVENT_NONE,
          repair_event,
          resources[I_AUX_REPAIR],
          resources[I_AUX_REPAIR_HOVER],
          nullptr)),

      m_background(make_shared<Bitmap>(
          resources[I_ELEMENT_GAUGE_GRAY],
          x
              + ((gauge_side == RepairGauge::GAUGE_LEFT)
                     ? LEFT_GAUGE_X_OFFSET
                     : (RIGHT_GAUGE_X_OFFSET + RIGHT_REPAIR_BUTTON_OFFSET_X
                        + REPAIR_BUTTON_W + 5)),
          y + GAUGE_OFFSET_Y,
          GAUGE_W,
          GAUGE_H)),
      m_fill(make_shared<Bitmap>(
          resources[fill_image_name],
          x
              + ((gauge_side == RepairGauge::GAUGE_LEFT)
                     ? LEFT_GAUGE_X_OFFSET
                     : (RIGHT_GAUGE_X_OFFSET + RIGHT_REPAIR_BUTTON_OFFSET_X
                        + REPAIR_BUTTON_W + 5)),
          y + GAUGE_OFFSET_Y,
          GAUGE_W,
          GAUGE_H)),
      m_line(al_create_bitmap(LINE_W, 1)),
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
    al_set_target_bitmap(m_line);
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

RepairGauge::~RepairGauge() { al_destroy_bitmap(m_line); }

void
RepairGauge::set_value(int value) {
    int fill = value * GAUGE_W / 100;
    if (value == 100 || value == 0) {
        m_repair_button->set_enabled(false);
    }
    m_fill->set_visible_region(0, 0, fill, GAUGE_H);
}

ModuleEngineer::ModuleEngineer()
    : Module(VIEWER_X, VIEWER_Y, VIEWER_W, VIEWER_H),
      m_resources(ENGINEER_IMAGES),
      m_viewer(make_shared<Bitmap>(m_resources[I_GUI], VIEWER_X, VIEWER_Y)),
      m_ship(make_shared<Bitmap>(nullptr, SHIP_X, SHIP_Y, SHIP_W, SHIP_H)),
      m_missile_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y,
          I_ELEMENT_GAUGE_PURPLE,
          EVENT_ENGINEER_REPAIR_MISSILES,
          "MISSILES",
          RepairGauge::GAUGE_LEFT,
          m_resources)),
      m_armor_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y + GAUGE_VERTICAL_SPACING,
          I_ELEMENT_GAUGE_RED,
          EVENT_NONE,
          "ARMOR",
          RepairGauge::GAUGE_LEFT,
          m_resources)),
      m_engine_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y + 2 * GAUGE_VERTICAL_SPACING,
          I_ELEMENT_GAUGE_ORANGE,
          EVENT_ENGINEER_REPAIR_ENGINES,
          "ENGINES",
          RepairGauge::GAUGE_LEFT,
          m_resources)),
      m_laser_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y,
          I_ELEMENT_GAUGE_MAGENTA,
          EVENT_ENGINEER_REPAIR_LASERS,
          "LASERS",
          RepairGauge::GAUGE_RIGHT,
          m_resources)),
      m_hull_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y + GAUGE_VERTICAL_SPACING,
          I_ELEMENT_GAUGE_GREEN,
          EVENT_ENGINEER_REPAIR_HULL,
          "HULL",
          RepairGauge::GAUGE_RIGHT,
          m_resources)),
      m_shield_gauge(make_shared<RepairGauge>(
          VIEWER_X,
          REPAIR_GAUGE_OFFSET_Y + 2 * GAUGE_VERTICAL_SPACING,
          I_ELEMENT_GAUGE_BLUE,
          EVENT_ENGINEER_REPAIR_SHIELDS,
          "SHIELDS",
          RepairGauge::GAUGE_RIGHT,
          m_resources))

{
    add_child_module(m_viewer);
    add_child_module(m_ship);
    add_child_module(m_missile_gauge);
    add_child_module(m_armor_gauge);
    add_child_module(m_engine_gauge);
    add_child_module(m_laser_gauge);
    add_child_module(m_hull_gauge);
    add_child_module(m_shield_gauge);

    set_draw_after_children(true);
}

ModuleEngineer::~ModuleEngineer() {}

bool
ModuleEngineer::on_init() {
    ALLEGRO_DEBUG("  ModuleEngineer Initialize\n");

    g_game->audioSystem->Load("data/engineer/buttonclick.ogg", "click");

    switch (g_game->gameState->getProfession()) {
    case PROFESSION_FREELANCE:
        m_ship->set_bitmap(m_resources[I_HIGH_RES_SHIP_FREELANCE]);
        break;

    case PROFESSION_MILITARY:
        m_ship->set_bitmap(m_resources[I_HIGH_RES_SHIP_MILITARY]);
        break;

    case PROFESSION_SCIENTIFIC:
    default:
        m_ship->set_bitmap(m_resources[I_HIGH_RES_SHIP_SCIENCE]);
        break;
    }

    return true;
}

bool
ModuleEngineer::on_close() {
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

        Officer *currentEngineer = g_game->gameState->getCurrentEng();
        std::string eng = currentEngineer->getLastName() + "-> ";
        int neededMineral = ship.repairMinerals[repairing - 1];
        Item mineral;
        int num_mineral;

        gs->m_items.Get_Item_By_ID(neededMineral, mineral, num_mineral);

        if (num_mineral == 0) {
            // mineral not in the cargo hold, stop repair
            std::string mineralName =
                g_game->dataMgr->GetItemByID(neededMineral)->name;
            g_game->printout(
                g_game->g_scrollbox,
                eng + "Repairing ceased due to lack of " + mineralName + ".",
                RED,
                5000);
            return false;

        } else {
            // consume the mineral
            std::string msg =
                eng + "Using one cubic meter of " + mineral.name + ".";
            g_game->printout(g_game->g_scrollbox, msg, GREEN, 1000);

            gs->m_items.RemoveItems(mineral.id, 1);
            ALLEGRO_EVENT e = {
                .type = static_cast<unsigned int>(EVENT_CARGO_UPDATE)};
            g_game->broadcast_event(&e);

            // roll a new one
            switch (rand() % 5) {
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

            // increase engineering skill every 4 minerals consumed
            // doing it here ensures the player will never be able to get free
            // skill increase
            currentEngineer->attributes.extra_variable++;
            if (currentEngineer->attributes.extra_variable >= 4) {
                currentEngineer->attributes.extra_variable = 0;

                if (currentEngineer->SkillUp(SKILL_ENGINEERING))
                    g_game->printout(
                        g_game->g_scrollbox,
                        eng + "I think I'm getting better at this.",
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

bool
ModuleEngineer::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);
    int x, y;

    m_missile_gauge->get_line_node(x, y);
    al_draw_line(x, y, SHIP_X + SHIP_W / 2, SHIP_Y + MISSILE_Y, GREEN, 1);

    m_armor_gauge->get_line_node(x, y);
    al_draw_line(x, y, SHIP_X + ARMOR_X, SHIP_Y + ARMOR_Y, GREEN, 1);

    m_engine_gauge->get_line_node(x, y);
    al_draw_line(x, y, SHIP_X + ENGINE_X, SHIP_Y + ENGINE_Y, GREEN, 1);

    m_laser_gauge->get_line_node(x, y);
    al_draw_line(x, y, SHIP_X + LASER_X, SHIP_Y + LASER_Y, GREEN, 1);

    m_hull_gauge->get_line_node(x, y);
    al_draw_line(x, y, SHIP_X + HULL_X, SHIP_Y + HULL_Y, GREEN, 1);

    m_shield_gauge->get_line_node(x, y);
    al_draw_line(x, y, SHIP_X + SHIELDS_X, SHIP_Y + SHIELDS_Y, GREEN, 1);

    return true;
}

bool
ModuleEngineer::on_update() {
    Ship ship = g_game->gameState->getShip();
    Officer *currentEngineer = g_game->gameState->getCurrentEng();
    float repair_time, repair_rate = 0;
    float repair_skill =
        g_game->gameState->CalcEffectiveSkill(SKILL_ENGINEERING);
    std::string eng = currentEngineer->getLastName() + "-> ";
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
            if (currentEngineer->CanSkillCheck() == true) {
                currentEngineer->FakeSkillCheck();
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
                g_game->g_scrollbox,
                eng + "The lasers are now fully functional!",
                BLUE,
                5000);
        }
    }

    s = "MISSILES: " + ship.getMissileLauncherClassString();
    if (ship.partInRepair == PART_MISSILES) {
        if (ship.getMissileLauncherIntegrity() < 100
            && ship.getMissileLauncherIntegrity() > 0) {
            if (currentEngineer->CanSkillCheck() == true) {
                currentEngineer->FakeSkillCheck();
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
                g_game->g_scrollbox,
                eng + "The missile system is now fully functional!",
                BLUE,
                5000);
        }
    }

    s = "HULL";
    if (ship.partInRepair == PART_HULL) {
        if (ship.getHullIntegrity() < 100 && ship.getHullIntegrity() > 0) {
            if (currentEngineer->CanSkillCheck() == true) {
                currentEngineer->FakeSkillCheck();
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
                g_game->g_scrollbox,
                eng + "The hull is now fully repaired!",
                BLUE,
                5000);
        }
    }

    s = "SHIELDS: " + ship.getShieldClassString();
    if (ship.partInRepair == PART_SHIELDS) {
        if (ship.getShieldIntegrity() < 100 && ship.getShieldIntegrity() > 0) {
            if (currentEngineer->CanSkillCheck() == true) {
                currentEngineer->FakeSkillCheck();
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
                g_game->g_scrollbox,
                eng + "The shields are now fully functional!",
                BLUE,
                5000);
        }
    }

    s = "ENGINES: " + ship.getEngineClassString();
    if (ship.partInRepair == PART_ENGINES) {
        if (ship.getEngineIntegrity() < 100 && ship.getEngineIntegrity() > 0) {
            if (currentEngineer->CanSkillCheck() == true) {
                currentEngineer->FakeSkillCheck();
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
                g_game->g_scrollbox,
                eng + "The engines are now fully repaired!",
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
    case EVENT_ENGINEER_REPAIR_LASERS:
        if (g_game->gameState->getShip().getLaserIntegrity() > 0
            && g_game->gameState->getShip().getLaserIntegrity() < 100
            && g_game->gameState->officerEng->CanSkillCheck() == true
            && repairing != PART_LASERS) {
            repairing = PART_LASERS;
        } else {
            repairing = PART_NONE;
        }
        break;
    case EVENT_ENGINEER_REPAIR_MISSILES:
        if (g_game->gameState->getShip().getMissileLauncherIntegrity() > 0
            && g_game->gameState->getShip().getMissileLauncherIntegrity() < 100
            && g_game->gameState->officerEng->CanSkillCheck() == true
            && repairing != PART_MISSILES) {
            repairing = PART_MISSILES;
        } else {
            repairing = PART_NONE;
        }
        break;
    case EVENT_ENGINEER_REPAIR_HULL:
        if (g_game->gameState->getShip().getHullIntegrity() > 0
            && g_game->gameState->getShip().getHullIntegrity() < 100
            && g_game->gameState->officerEng->CanSkillCheck() == true
            && repairing != PART_HULL) {
            repairing = PART_HULL;
        } else {
            repairing = PART_NONE;
        }
        break;
    case EVENT_ENGINEER_REPAIR_SHIELDS:
        if (g_game->gameState->getShip().getShieldIntegrity() > 0
            && g_game->gameState->getShip().getShieldIntegrity() < 100
            && g_game->gameState->officerEng->CanSkillCheck() == true
            && repairing != PART_SHIELDS) {
            repairing = PART_SHIELDS;
        } else {
            repairing = PART_NONE;
        }
        break;
    case EVENT_ENGINEER_REPAIR_ENGINES:
        if (g_game->gameState->getShip().getEngineIntegrity() > 0
            && g_game->gameState->getShip().getEngineIntegrity() < 100
            && g_game->gameState->officerEng->CanSkillCheck() == true
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
        g_game->audioSystem->Play("click");
    }

    return true;
}
