/*
        STARFLIGHT - THE LOST COLONY
        ModuleInterstellarTravel.cpp - Handles interstellar space travel in the
   main window Author: J.Harbour Date: January, 2007
*/

#include <cmath>
#include <sstream>
#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "Label.h"
#include "MessageBoxWindow.h"
#include "ModeMgr.h"
#include "ModuleControlPanel.h"
#include "ModuleInterplanetaryTravel.h"
#include "PauseMenu.h"
#include "QuestMgr.h"
#include "Util.h"
#include "spacetravel_resources.h"

using namespace std;
using namespace spacetravel;

namespace {
// scroller properties
constexpr int PLANET_SCROLL_X = 0;
constexpr int PLANET_SCROLL_Y = 0;
constexpr int PLANET_SCROLL_WIDTH = SCREEN_WIDTH;
constexpr int PLANET_SCROLL_HEIGHT = SCREEN_HEIGHT;
constexpr int PLANETTILESIZE = 256;
constexpr int PLANETTILESACROSS = 100;
constexpr int PLANETTILESDOWN = 100;

ALLEGRO_DEBUG_CHANNEL("ModuleInterPlanetaryTravel")
}; // namespace

ModuleInterPlanetaryTravel::ModuleInterPlanetaryTravel()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT) {

    m_planet_label =
        make_shared<Label>("", 0, 0, 100, 22, false, 0, g_game->font18, ORANGE);

    add_child_module(m_planet_label);
}

ModuleInterPlanetaryTravel::~ModuleInterPlanetaryTravel() {}

bool
ModuleInterPlanetaryTravel::on_key_down(ALLEGRO_KEYBOARD_EVENT *event) {
    int keyCode = event->keycode;

    // Note: fuel consumption in a star system should be negligible since its
    // the impulse engine whereas we're using the hyperspace engine outside the
    // system
    if (g_game->gameState->m_ship.getFuel() == 0) {
        ship->cruise();
        return true;
    }
    switch (keyCode) {
    case ALLEGRO_KEY_D:
    case ALLEGRO_KEY_RIGHT:
        flag_rotation = 1;
        ship->turnright();
        break;
    case ALLEGRO_KEY_A:
    case ALLEGRO_KEY_LEFT:
        flag_rotation = -1;
        ship->turnleft();
        break;
    case ALLEGRO_KEY_S:
    case ALLEGRO_KEY_DOWN:
        flag_nav = false;
        ship->applybraking();
        break;
    case ALLEGRO_KEY_W:
    case ALLEGRO_KEY_UP:
        flag_nav = flag_thrusting = true;
        break;
    case ALLEGRO_KEY_Q:
        flag_nav = true;
        if (!flag_thrusting)
            ship->applybraking();
        ship->starboard();
        g_game->gameState->m_ship.ConsumeFuel();
        break;
    case ALLEGRO_KEY_E:
        flag_nav = true;
        if (!flag_thrusting)
            ship->applybraking();
        ship->port();
        g_game->gameState->m_ship.ConsumeFuel();
        break;
    }
    return true;
}

bool
ModuleInterPlanetaryTravel::on_key_up(ALLEGRO_KEYBOARD_EVENT *event) {
    int keyCode = event->keycode;

    if (g_game->gameState->m_ship.getFuel() == 0) {
        ship->cruise();
        return true;
    }

    switch (keyCode) {
    case ALLEGRO_KEY_D:
    case ALLEGRO_KEY_A:
    case ALLEGRO_KEY_LEFT:
    case ALLEGRO_KEY_RIGHT:
        flag_rotation = 0;
        break;
    case ALLEGRO_KEY_DOWN:
        ship->cruise();
        break;

    case ALLEGRO_KEY_UP:
        flag_nav = flag_thrusting = false;
        ship->applybraking();
        ship->cruise();
        break;

    case ALLEGRO_KEY_Q:
    case ALLEGRO_KEY_E:
        flag_nav = false;
        ship->applybraking();
        ship->cruise();
        break;
    }
    return true;
}

bool
ModuleInterPlanetaryTravel::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    for (int i = 0; i < star->GetNumPlanets(); i++) {
        int planet = planets[i].tilenum;
        if (planet > 0) {
            // the various negative offsets after the parenthesized expressions
            // are mere empirical tweaks, so don't expect too much of them
            if (x > (asx + planets[i].tilex * 2.3) - 4 - planets[i].radius
                && x < (asx + planets[i].tilex * 2.3) - 2 + planets[i].radius
                && y > (asy + planets[i].tiley * 2.3) - 2 - planets[i].radius
                && y < (asy + planets[i].tiley * 2.3) + planets[i].radius) {
                m_planet_label->move(x + 10, y);
                m_planet_label->set_text(
                    star->GetPlanetByID(planets[i].planetid)->name);

                m_bOver_Planet = true;
                break;
            } else {
                m_bOver_Planet = false;
            }
        }
    }
    // check if mouse is over a star
    if (m_bOver_Planet == false) {
        int systemCenterTileX = PLANETTILESACROSS / 2;
        int systemCenterTileY = PLANETTILESDOWN / 2;
        if (x > (asx + systemCenterTileX * 2.3) - 6
            && x < (asx + systemCenterTileX * 2.3) + 6
            && y > (asy + systemCenterTileY * 2.3) - 6
            && y < (asy + systemCenterTileY * 2.3) + 6) {
            m_planet_label->move(x + 10, y);
            m_planet_label->set_text(star->name);
            m_bOver_Planet = true;
        } else {
            m_bOver_Planet = false;
        }
    }
    return true;
}

bool
ModuleInterPlanetaryTravel::on_event(ALLEGRO_EVENT *event) {
    Ship ship;
    std::string escape = "";

    int evtype = event->type;

    switch (evtype) {
    case EVENT_CLOSE:
        set_modal_child(nullptr);
        return true;
    case EVENT_SAVE_GAME: // save game
        g_game->gameState->AutoSave();
        g_game->printout("<Game Saved>", WHITE, 5000);
        break;
    case EVENT_LOAD_GAME: // load game
        g_game->gameState->AutoLoad();
        break;
    case EVENT_QUIT_GAME: // quit game
        g_game->set_vibration(0);
        escape = g_game->getGlobalString("ESCAPEMODULE");
        g_game->LoadModule(escape);
        break;

    case EVENT_CAPTAIN_LAUNCH:
        g_game->printout(
            OFFICER_NAVIGATION, "Sir, we are not on a planet.", BLUE, 8000);
        break;
    case EVENT_CAPTAIN_DESCEND:
        g_game->printout(
            OFFICER_NAVIGATION,
            "Sir, we are not orbiting a planet.",
            BLUE,
            8000);
        break;
    case EVENT_CAPTAIN_QUESTLOG:
        break;

    case EVENT_SCIENCE_SCAN:
        g_game->printout(
            OFFICER_SCIENCE,
            "Sir, we are not near any planets or vessels.",
            BLUE,
            8000);
        break;
    case EVENT_SCIENCE_ANALYSIS:
        g_game->printout(
            OFFICER_SCIENCE, "Sir, I have not scanned anything.", BLUE, 8000);
        break;

    case EVENT_NAVIGATOR_ORBIT:
        flag_DoOrbit = true;
        break;
    case EVENT_NAVIGATOR_DOCK:
        flag_DoDock = true;
        break;
    case EVENT_NAVIGATOR_HYPERSPACE:
        flag_DoHyperspace = true;
        break;

    case EVENT_TACTICAL_COMBAT:
        g_game->printout(
            OFFICER_TACTICAL,
            "Sir, we are not in range of any other ships.",
            BLUE,
            8000);
        break;
    case EVENT_TACTICAL_SHIELDS:
        g_game->printout(
            OFFICER_TACTICAL, "Sir, we are not in combat.", BLUE, 8000);
        break;
    case EVENT_TACTICAL_WEAPONS:
        g_game->printout(
            OFFICER_TACTICAL, "Sir, we are not in combat.", BLUE, 8000);
        break;

    case EVENT_ENGINEER_REPAIR:
        break;
    case EVENT_ENGINEER_INJECT:
        g_game->gameState->getShip().injectEndurium();
        break;

    case EVENT_COMM_HAIL:
        g_game->printout(
            OFFICER_COMMUNICATION,
            "We are not in range of any other ships.",
            BLUE,
            8000);
        break;
    case EVENT_COMM_STATEMENT:
        g_game->printout(
            OFFICER_COMMUNICATION,
            "We are not communicating with anyone!",
            BLUE,
            8000);
        break;
    case EVENT_COMM_QUESTION:
        g_game->printout(
            OFFICER_COMMUNICATION,
            "We are not communicating with anyone!",
            BLUE,
            8000);
        break;
    case EVENT_COMM_POSTURE:
        g_game->printout(
            OFFICER_COMMUNICATION,
            "We are not communicating with anyone!",
            BLUE,
            8000);
        break;
    case EVENT_COMM_TERMINATE:
        g_game->printout(
            OFFICER_COMMUNICATION,
            "We are not communicating with anyone!",
            BLUE,
            8000);
        break;
    case EVENT_COMM_DISTRESS:
        g_game->gameState->m_ship.SendDistressSignal();
        break;

    case EVENT_DOCTOR_EXAMINE:
        break;
    case EVENT_DOCTOR_TREAT:
        break;

    default:
        break;
    }
    return true;
}

bool
ModuleInterPlanetaryTravel::on_close() {
    ALLEGRO_DEBUG("*** Interplanetary Closing\n\n");

    m_scroller = nullptr;

    delete ship;
    ship = nullptr;

    return true;
}

bool
ModuleInterPlanetaryTravel::on_init() {
    g_game->SetTimePaused(false); // game-time normal in this module.

    ALLEGRO_DEBUG("  Interplanetary Initialize\n");

    // enable the Pause Menu
    g_game->enable_pause_menu(true);

    // reset flags
    flag_nav = flag_thrusting = false;
    flag_rotation = 0;

    planet = nullptr;
    flag_DoOrbit = false;
    flag_DoHyperspace = false;
    flag_DoDock = false;
    asx = 0;
    asy = 0;
    distressSignal = false;
    burning = 0;
    g_game->set_vibration(0);
    m_bOver_Planet = false;

    if (g_game->modeMgr->GetPrevModuleName() == MODULE_HYPERSPACE) {
        enter_system();
    }

    // create the ship sprite
    ship = new PlayerShipSprite();
    ship->allstop();

    // create tile scroller object
    TileSet ts(images[I_IP_TILES], PLANETTILESIZE, PLANETTILESIZE, 9, 1);

    m_scroller = make_shared<TileScroller>(
        PLANET_SCROLL_X,
        PLANET_SCROLL_Y,
        PLANET_SCROLL_WIDTH,
        PLANET_SCROLL_HEIGHT,
        ts,
        PLANETTILESACROSS,
        PLANETTILESDOWN,
        ship->get_screen_position() - Point2D(96, 96));

    // try to read star system data...
    star = g_game->dataMgr->GetStarByID(g_game->gameState->player.currentStar);
    if (!star) {
        g_game->message("Interplanetary: Error locating star system!");
        return false;
    }

    // create a tile map of this star system
    loadStarSystem(g_game->gameState->player.currentStar);

    // set player's location
    m_scroller->set_scroll_position(g_game->gameState->getSystemCoordinates());

    // notify questmgr that star system has been entered
    g_game->questMgr->raiseEvent(10, g_game->gameState->player.currentStar);

    return true;
}

bool
ModuleInterPlanetaryTravel::checkSystemBoundary(int x, int y) {
    /* FIXME: use system coordinates directly */
    return !(
        x <= 0 || x >= PLANETTILESACROSS * PLANETTILESIZE || y <= 0
        || y >= PLANETTILESDOWN * PLANETTILESIZE);
}

bool
ModuleInterPlanetaryTravel::on_update() {
    std::ostringstream s;

    // update the ship's position based on velocity
    Point2D f(
        g_game->gameState->player.posSystem.x + (ship->getVelocityX() * 6),
        g_game->gameState->player.posSystem.y + (ship->getVelocityY() * 6));

    // exit star system when edge is reached
    if (!checkSystemBoundary(f.x, f.y)) {
        ship->allstop();
        flag_DoHyperspace = true;
        return true;
    }

    // store ship position
    g_game->gameState->player.posSystem.x = f.x;
    g_game->gameState->player.posSystem.y = f.y;

    // update scrolling and draw tiles on the scroll buffer
    m_scroller->set_scroll_position(f / PLANETTILESIZE);

    // check if any planet is located near ship
    checkShipPosition();

    // reset vibration danger value
    g_game->set_vibration(0);

    // display any planet under ship
    if (planet) {
        if (planet->name.length() > 0) {
            s << "Captain, we are in orbital range of " << planet->name << ".";
        } else {
            s << "Captain, planet " << planet->name
              << " is not in our data banks.";
        }
        g_game->printout(
            OFFICER_NAVIGATION,
            s.str() + " Awaiting order to establish orbit.",
            ORANGE,
            15000);
    }
    // getting too close to the star?
    else if (burning) {
        if (burning > 240) {
            g_game->set_vibration(20);
            g_game->printout("AAARRRRRGGGGHHHHH!!!", YELLOW, 500);
            ship->allstop();
            if (Util::ReentrantDelay(2000)) {
                g_game->set_vibration(0);
                g_game->LoadModule(MODULE_GAMEOVER);
                return false;
            }
        } else if (burning > 160) {
            g_game->set_vibration(14);
            g_game->printout(
                OFFICER_SCIENCE, "Hull temperature is critical!", RED, 8000);
        } else if (burning > 80) {
            g_game->set_vibration(10);
            g_game->printout(
                OFFICER_SCIENCE, "Hull temperature is rising!", ORANGE, 8000);
        } else if (burning > 2) {
            g_game->set_vibration(6);
            g_game->printout(
                OFFICER_SCIENCE,
                "Captain! We're getting too close!",
                ORANGE,
                8000);
        } else if (burning == 2) {
            g_game->set_vibration(4);
            g_game->printout(
                OFFICER_NAVIGATION,
                "Sir, we must keep our distance from the sun.",
                LTGREEN,
                8000);
        } else {
            g_game->set_vibration(2);
            g_game->printout(
                OFFICER_NAVIGATION,
                "Captain, we should steer clear of the sun.",
                LTGREEN,
                8000);
        }
    }

    // when player tells navigator to orbit planet, we want to display a message
    // before immediately launching the planet orbit module. this displays the
    // message and pauses for 2 seconds.
    if (flag_DoOrbit) {
        checkShipPosition();
        if (!planet) {
            g_game->printout(
                OFFICER_NAVIGATION,
                "Unable to comply. Nothing to orbit here.",
                ORANGE,
                8000);
            if (Util::ReentrantDelay(2000)) {
                flag_DoOrbit = false;
            }
        } else {
            g_game->printout(OFFICER_NAVIGATION, "Aye, captain.", ORANGE, 8000);
            if (Util::ReentrantDelay(2000)) {
                g_game->printout(
                    OFFICER_NAVIGATION,
                    "Entering orbital trajectory.",
                    ORANGE,
                    2000);
                g_game->LoadModule(MODULE_ORBIT);
                return false;
            }
        }
    }

    // player orders navigator to exit system back into hyperspace
    // use delay mechanism to show message before lauching module
    // this also occurs if player reaches edge of star system
    if (flag_DoHyperspace) {
        Ship ship = g_game->gameState->getShip();

        if (g_game->gameState->player.hasHyperspacePermit() == false) {
            g_game->printout(
                OFFICER_NAVIGATION,
                "Captain, we can't enter hyperspace without a hyperspace "
                "permit.",
                ORANGE,
                30000);
            if (Util::ReentrantDelay(2000)) {
                flag_DoHyperspace = false;
            }
        } else if (planet) {
            g_game->printout(
                OFFICER_NAVIGATION,
                "Captain, we can't enter hyperspace due to "
                "the nearby planet's gravity well.",
                ORANGE,
                8000);
            if (Util::ReentrantDelay(2000)) {
                flag_DoHyperspace = false;
            }
        } else if (ship.getFuel() <= 0.0f) {
            g_game->printout(
                OFFICER_NAVIGATION,
                "Sir, we do not have enough fuel to enter hyperspace!",
                ORANGE,
                10000);
            if (Util::ReentrantDelay(2000)) {
                flag_DoHyperspace = false;
            }
        } else {
            // okay we're not near a planet
            g_game->printout(
                OFFICER_NAVIGATION,
                "Yes, sir! Engaging hyperspace engine.",
                ORANGE,
                8000);
            if (Util::ReentrantDelay(2000)) {
                if (g_game->gameState->getShip().getFuel()
                    >= 0.01f) // 1% of fuel required
                {
                    g_game->gameState->m_ship.ConsumeFuel(
                        20); // hyperspace engine consumes 20 units
                    g_game->LoadModule(MODULE_HYPERSPACE);
                    return false;
                } else {
                    g_game->printout(
                        OFFICER_NAVIGATION,
                        "We do not have enough fuel to enter hyperspace!",
                        RED,
                        8000);
                }
            }
        }
    }

    if (flag_DoDock) {
        // display any planet under ship
        if (planet) {
            // planet id #8 = Myrrdan in the database
            if (g_game->gameState->player.currentPlanet == 8) {
                // okay we're near myredan - however it's spelled
                g_game->printout(
                    OFFICER_NAVIGATION,
                    "Yes, sir! Docking with Starport.",
                    ORANGE,
                    8000);
                if (Util::ReentrantDelay(1000)) {
                    g_game->LoadModule(MODULE_STARPORT);
                    return false;
                }
            } else {
                // okay we're near a planet without a starport
                g_game->printout(
                    OFFICER_NAVIGATION,
                    "Sorry sir! There are no Starports in the vicinity.",
                    ORANGE,
                    8000);
                flag_DoDock = false;
            }
        } else {
            // okay we're not near a planet
            g_game->printout(
                OFFICER_NAVIGATION,
                "Sorry sir! We are not close enough to a "
                "planet to scan for Starports.",
                ORANGE,
                8000);
            flag_DoDock = false;
        }
    }

    // check fuel level
    if (g_game->gameState->getShip().getFuel() <= 0.00f) {
        g_game->gameState->m_ship.injectEndurium();
    }

    // increase navigation skill every FlyingHoursBeforeSkillUp hours spent in
    // space (the speed check is there to prevent the obvious abuse)
    if (g_game->gameState->CanSkillCheck(SKILL_NAVIGATION)
        && ship->getCurrentSpeed() > 0.0) {
        if (g_game->gameState->add_experience(SKILL_NAVIGATION, 1)) {
            g_game->printout(
                OFFICER_NAVIGATION,
                "I think I'm getting better at this.",
                PURPLE,
                5000);
        }
    }

    if (g_game->gameState->m_ship.getFuel() == 0) {
        flag_thrusting = flag_nav = false;
        flag_rotation = 0;
    }
    // slow down the ship automatically
    if (!flag_nav)
        ship->applybraking();

    if (flag_thrusting) {
        ship->applythrust();
        g_game->gameState->m_ship.ConsumeFuel();
    }
    if (flag_rotation == 1) {
        ship->turnright();
    } else if (flag_rotation == -1) {
        ship->turnleft();
    }

    return true;
}

bool
ModuleInterPlanetaryTravel::on_draw(ALLEGRO_BITMAP *target) {
    static bool help1 = true;

    // draw the scrolling view
    m_scroller->draw(target);

    // draw the ship
    ship->draw(target);

    // redraw the mini map
    updateMiniMap(target);

    // display tutorial help messages for beginners
    if ((!g_game->gameState->firstTimeVisitor
         || g_game->gameState->getActiveQuest() > 10))
        help1 = false;
    if (help1) {
        help1 = false;
        string str = "You are now travelling in the star system. To move the "
                     "ship use the same keys you used in the starport.";
        set_modal_child(make_shared<MessageBoxWindow>(
            "", str, 10, 200, 400, 300, YELLOW, false));
    }
    // JJH - added CrossModuleAngle so that ship's heading stays consistent
    // between entering/leaving systems. same mod in ModuleInterstellarTravel
    // and some changes in PlayerShipSprite.
    g_game->CrossModuleAngle = ship->getRotationAngle();

    if (g_game->getGlobalBoolean("DEBUG_OUTPUT") == true) {
        int y = 90;
        g_game->PrintDefault(
            target, 850, y, "flag_nav: " + Util::ToString(flag_nav));
        y += 10;
        g_game->PrintDefault(
            target,
            850,
            y,
            "flag_thrusting: " + Util::ToString(flag_thrusting));
        y += 10;
        g_game->PrintDefault(
            target,
            850,
            y,
            "velocity: " + Util::ToString(ship->getVelocityX()) + ","
                + Util::ToString(ship->getVelocityY()));
        y += 10;
        g_game->PrintDefault(
            target,
            850,
            y,
            "speed: " + Util::ToString(ship->getCurrentSpeed()));
        y += 10;
        g_game->PrintDefault(
            target,
            850,
            y,
            "planetFound: " + Util::ToString(planet != nullptr));
        y += 10;
        g_game->PrintDefault(
            target,
            850,
            y,
            "navcounter: "
                + Util::ToString(
                    g_game->gameState->get_effective_officer(OFFICER_NAVIGATION)
                        ->get_experience(SKILL_NAVIGATION)));
        y += 10;
        g_game->PrintDefault(
            target,
            850,
            y,
            "angle: " + Util::ToString(ship->getRotationAngle()));
    }
    return true;
}

void
ModuleInterPlanetaryTravel::checkShipPosition() {
    Point2D pos = g_game->gameState->getSystemCoordinates();
    Point2D sun_pos = Point2D(PLANETTILESACROSS / 2, PLANETTILESDOWN / 2);

    // is ship burning up in the star?
    // calculate distance from ship to star
    double dx = pos.x - sun_pos.x;
    double dy = pos.y - sun_pos.y;
    double distance = hypot(dx, dy);

    if (distance > 2.0f) {
        burning = 0;
    } else if (distance > 1.5f) {
        burning = 1;
    } else if (distance > 0.5f) {
        burning = 2;
    } else {
        burning++;
    }

    // see if ship is over planet tile
    planet = nullptr;
    for (int i = 0; i < 10 && !planet; i++) {
        // check tilex,tiley,and tilenum for a planet match
        if (static_cast<int>(round(pos.x)) == planets[i].tilex
            && static_cast<int>(round(pos.y)) == planets[i].tiley) {
            planet = star->GetPlanetByID(planets[i].planetid);
            if (planet) {
                // store current planet in global player object
                g_game->gameState->player.currentPlanet = planet->id;
                break;
            }
        }
    }
    if (!planet) {
        g_game->gameState->player.currentPlanet = -1;
    }
}

void
ModuleInterPlanetaryTravel::updateMiniMap(ALLEGRO_BITMAP *target) {
    // get AUX_SCREEN gui values from globals
    asx = (int)g_game->getGlobalNumber("AUX_SCREEN_X");
    asy = (int)g_game->getGlobalNumber("AUX_SCREEN_Y");
    static double asw = g_game->getGlobalNumber("AUX_SCREEN_WIDTH");
    static double ash = g_game->getGlobalNumber("AUX_SCREEN_HEIGHT");
    al_set_target_bitmap(target);

    // clear aux window
    al_draw_filled_rectangle(
        asx, asy, asx + asw, asy + ash, al_map_rgb(0, 0, 0));

    // draw ellipses representing planetary orbits
    int rx, ry, cx, cy;
    for (int i = 0; i < star->GetNumPlanets(); i++) {
        planet = star->GetPlanet(i);
        if (planet) {
            cx = asx + asw / 2;
            cy = asy + ash / 2;
            rx = (int)((2 + i) * 8.7);
            ry = (int)((2 + i) * 8.7);
            al_draw_ellipse(cx, cy, rx, ry, al_map_rgb(48, 48, 96), 2);
        }
    }

    // draw the star in aux window
    int systemCenterTileX = PLANETTILESACROSS / 2;
    int systemCenterTileY = PLANETTILESDOWN / 2;

    ALLEGRO_COLOR color;
    switch (star->spectralClass) {
    case SC_A:
        color = WHITE;
        break;
    case SC_B:
        color = LTBLUE;
        break;
    case SC_F:
        color = LTYELLOW;
        break;
    case SC_G:
        color = YELLOW;
        break;
    case SC_K:
        color = ORANGE;
        break;
    case SC_M:
        color = RED;
        break;
    case SC_O:
        color = BLUE;
        break;
    default:
        color = ORANGE;
        break;
    }

    float starx = (int)(asx + systemCenterTileX * 2.3);
    float stary = (int)(asy + systemCenterTileY * 2.3);
    al_draw_filled_circle(starx, stary, 6, color);

    // draw planets in aux window
    color = BLACK;
    int planet = -1, px = 0, py = 0;
    for (int i = 0; i < star->GetNumPlanets(); i++) {
        planet = planets[i].tilenum;
        if (planet > 0) {
            switch (planet) {
            case 1:
                color = al_map_rgb(255, 182, 0);
                planets[i].radius = 6;
                break; // sun
            case 2:
                color = al_map_rgb(100, 0, 100);
                planets[i].radius = 4;
                break; // gas giant
            case 3:
                color = al_map_rgb(160, 12, 8);
                planets[i].radius = 3;
                break; // molten
            case 4:
                color = al_map_rgb(200, 200, 255);
                planets[i].radius = 3;
                break; // ice
            case 5:
                color = al_map_rgb(30, 100, 240);
                planets[i].radius = 3;
                break; // oceanic
            case 6:
                color = al_map_rgb(134, 67, 30);
                planets[i].radius = 2;
                break; // rocky
            case 7:
                color = al_map_rgb(95, 93, 93);
                planets[i].radius = 1;
                break; // asteroid
            case 8:
                color = al_map_rgb(55, 147, 84);
                planets[i].radius = 3;
                break; // acidic
            default:
                color = al_map_rgb(90, 90, 90);
                planets[i].radius = 1;
                break; // none
            }
            px = (int)(asx + planets[i].tilex * 2.3);
            py = (int)(asy + planets[i].tiley * 2.3);
            al_draw_filled_circle(px, py, planets[i].radius, color);
        }
    }

    m_planet_label->set_active(m_bOver_Planet);

    // draw player's location on minimap
    float fx = asx + g_game->gameState->player.posSystem.x / 256 * 2.3;
    float fy = asy + g_game->gameState->player.posSystem.y / 256 * 2.3;
    al_draw_rectangle(
        (int)fx - 1, (int)fy - 1, (int)fx + 2, (int)fy + 2, LTBLUE, 1);
}

int
ModuleInterPlanetaryTravel::loadStarSystem(int id) {
    int i;

    ALLEGRO_DEBUG("  Loading star system %i.\n", id);
    sfsrand(time(NULL));

    // save current star id in global player object
    g_game->gameState->player.currentStar = id;

    // clear the temp array of planets (used to simplify searches)
    for (i = 0; i < 10; i++) {
        planets[i].tilex = -1;
        planets[i].tiley = -1;
        planets[i].tilenum = -1;
        planets[i].planetid = -1;
    }

    // clear the tile map
    m_scroller->reset_tiles();

    // calculate center of tile map
    int systemCenterTileX = PLANETTILESACROSS / 2;
    int systemCenterTileY = PLANETTILESDOWN / 2;

    // position star tile image at center
    m_scroller->set_tile(systemCenterTileX, systemCenterTileY, 1);

    // read starid passed over by the interstellar module
    star = g_game->dataMgr->GetStarByID(id);
    if (!star) {
        g_game->message("Interplanetary: Error loading star info");
        return 0;
    }

    // add planets to the solar system from the planet database
    if (star->GetNumPlanets() == 0)
        return 0;

    // seed random number generator with star id #
    sfsrand(id);

    // calculate position of each planet in orbit around the star
    float radius, angle;
    int rx, ry;
    for (i = 0; i < star->GetNumPlanets(); i++) {

        planet = star->GetPlanet(i);
        if (planet) {

            planets[i].planetid = planet->id;

            radius = (2 + i) * 4;
            angle = sfrand() % 360;
            rx = (int)(cos(angle) * radius);
            ry = (int)(sin(angle) * radius);
            planets[i].tilex = systemCenterTileX + rx;
            planets[i].tiley = systemCenterTileY + ry;
            planets[i].tilenum = planet->type;

            ALLEGRO_ASSERT(planets[i].tilenum > 0);
            ALLEGRO_ASSERT(planets[i].tilenum < 9);

            m_scroller->set_tile(
                planets[i].tilex, planets[i].tiley, planets[i].tilenum);
        }
    }

    return 1;
}

// Place the ship in the system based on its current orientation.
void
ModuleInterPlanetaryTravel::enter_system() {
    // normalize ship angle in radians with 0 pointing to the right
    float a = fmod(90 - g_game->CrossModuleAngle, 360.0);
    if (a < 0) {
        a += 360;
    }
    float arad = a * M_PI / 180.0;
    // angle from the center to the top right corner, used to determine
    // which side the ship will enter from
    float wall_angle = atan2(
        static_cast<float>(PLANETTILESDOWN) / 2.0,
        static_cast<float>(PLANETTILESACROSS) / 2.0);

    // unit circle bearing
    float x = cos(arad);
    float y = sin(arad);

    float pos_x;
    float pos_y;

    // determine which side we enter from, and then scale the other coordinate
    // by the system dimensions.
    if ((arad >= (M_PI * 2 - wall_angle)) || (arad < wall_angle)) {
        // left side
        pos_x = 1;
        pos_y = (1.0 + y) * PLANETTILESDOWN / 2.0;
    } else if ((arad >= wall_angle) && (arad < (M_PI - wall_angle))) {
        // bottom side
        pos_x = (1.0 - x) * PLANETTILESACROSS / 2.0;
        pos_y = PLANETTILESDOWN - 2;
    } else if ((arad >= (M_PI - wall_angle)) && (arad < (M_PI + wall_angle))) {
        // right side
        pos_x = PLANETTILESACROSS - 2;
        pos_y = (1.0 + y) * PLANETTILESDOWN / 2.0;
    } else {
        // top side
        pos_x = (1.0 - x) * PLANETTILESACROSS / 2.0;
        pos_y = 1;
    }

    g_game->gameState->player.posSystem.x = pos_x * PLANETTILESIZE;
    g_game->gameState->player.posSystem.y = pos_y * PLANETTILESIZE;
}
