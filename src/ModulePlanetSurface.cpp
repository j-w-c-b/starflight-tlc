/*
        STARFLIGHT - THE LOST COLONY
        ModulePlanetSurface.cpp - Handles planetary surface
        Author: Justin Sargent
        Date: January, 2007

        This module handles only planetary excursions, not planet landing in the
   ship's gui, which is found in the planet orbit module. This module handles
   only the terrain vehicle.

        Referenced Scripts:
                PlanetSurfacePlayerShip.lua
                PlanetSurfacePlayerTV.lua
                stunprojectile.lua
                Functions.lua

                These add lifeforms and minerals to planet:
                        PopAsteroid.lua
                        PopRockyPlanet.lua
                        PopOceanicPlanet.lua
                        PopMoltenPlanet.lua
                        PopFrozenPlanet.lua
            PopAcidicPlanet.lua

            mineral.lua
            artifact.lua
            ruin.lua
                        basicLifeform.lua

        All other scripts in data/planetsurface come from quests and can be
   deleted

        New objects added to planet surface via: L_CreateNewPSObyItemID

*/

#include <algorithm>
#include <cmath>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "AdvancedTileScroller.h"
#include "AudioSystem.h"
#include "Button.h"
#include "DataMgr.h"
#include "Game.h"
#include "GameState.h"
#include "Label.h"
#include "MessageBoxWindow.h"
#include "ModeMgr.h"
#include "ModuleCargoWindow.h"
#include "ModuleControlPanel.h"
#include "ModulePlanetSurface.h"
#include "ModuleTopGUI.h"
#include "PauseMenu.h"
#include "PlanetSurfaceObject.h"
#include "PlanetSurfacePlayerVessel.h"
#include "PlayerShipSprite.h"
#include "QuestMgr.h"
#include "Util.h"
#include "planetsurface_resources.h"

using namespace std;
using namespace planetsurface;

namespace planetsurface {
ALLEGRO_DEBUG_CHANNEL("ModulePlanetSurface")

#define SCROLLEROFFSETX (SCREEN_WIDTH / 2 - activeVessel->getFrameWidth() / 2)
#define SCROLLEROFFSETY                                                        \
    (SCREEN_HEIGHT / 2 - 128 - activeVessel->getFrameHeight() / 2)

static constexpr int TIMER_X = 408;
static constexpr int TIMER_Y = 460;

#define TV_NONESELECTED_TEXT "Click on objects to interact with them"
#define SHIP_TEXT                                                              \
    "Land your ship to explore the planet with your terrain vehicle"
#define TVOUTOFFUEL_TEXT                                                       \
    "You can pick up your Terrain Vehicle by flying over it and pressing the " \
    "Pick Up button"
#define OBJECT_NEEDS_SCANNED_TEXT                                              \
    "You can scan objects to learn how to interact with them"

TopGUI::TopGUI(const PlanetSurfacePlayerVessel *vessel)
    : Module(),
      m_background(make_shared<Bitmap>(
          images[I_GUI_GAUGES],
          static_cast<int>(g_game->getGlobalNumber("GUI_GAUGES_POS_X")),
          static_cast<int>(g_game->getGlobalNumber("GUI_GAUGES_POS_Y")))),
      m_position_label(make_shared<Label>(
          "",
          875,
          10,
          al_get_text_width(g_game->font24.get(), "XXXXXXXX"),
          al_get_font_line_height(g_game->font24.get()),
          false,
          ALLEGRO_ALIGN_LEFT,
          g_game->font24,
          LTGREEN)),
      m_armor_bar(make_shared<Bitmap>(images[I_ARMOR_BAR], 476, 10)),
      m_armor_gauge(make_shared<Bitmap>(images[I_ELEMENT_GAUGE_RED], 536, 12)),
      m_fuel_bar(make_shared<Bitmap>(images[I_FUEL_BAR], 666, 11)),
      m_fuel_gauge(
          make_shared<Bitmap>(images[I_ELEMENT_GAUGE_ORANGE], 709, 11)),
      m_hull_bar(make_shared<Bitmap>(images[I_HULL_BAR], 300, 11)),
      m_hull_gauge(make_shared<Bitmap>(images[I_ELEMENT_GAUGE_GREEN], 342, 13)),
      m_vessel(vessel), m_mode(VESSEL_MODE_SHIP_WITH_TV) {
    add_child_module(m_background);
    add_child_module(m_position_label);
    add_child_module(m_armor_bar);
    add_child_module(m_armor_gauge);
    add_child_module(m_fuel_bar);
    add_child_module(m_fuel_gauge);
    add_child_module(m_hull_bar);
    add_child_module(m_hull_gauge);
}

void
TopGUI::set_mode(VesselMode mode) {
    if (m_mode != mode) {
        m_mode = mode;
        if (mode == VESSEL_MODE_TV) {
            m_armor_bar->set_active(false);
            m_armor_gauge->set_active(false);
        } else {
            m_armor_bar->set_active(true);
            m_armor_gauge->set_active(true);
        }
    }
}

void
TopGUI::set_position(int x, int y) {
    string latitude;
    string longitude;

    if (x < 0) {
        longitude = to_string(-x) + "E";
    } else if (x == 0) {
        longitude = "0";
    } else {
        longitude = to_string(x) + "W";
    }

    if (y < 0) {
        latitude = to_string(-y) + "S";
    } else if (y == 0) {
        latitude = "0";
    } else {
        latitude = to_string(y) + "N";
    }

    // print position on top gui
    m_position_label->set_text(latitude + "," + longitude);
}

bool
TopGUI::on_draw(ALLEGRO_BITMAP * /*target */) {
    if (m_mode == VESSEL_MODE_TV) {
        float fuel_percent =
            static_cast<float>(m_vessel->getCounter3()) / 100.0;
        m_fuel_gauge->set_clip_width(
            static_cast<int>(fuel_percent * m_fuel_gauge->get_width()));
        float hull_percent = static_cast<float>(m_vessel->getHealth()) / 100.0;
        m_fuel_gauge->set_clip_width(
            static_cast<int>(hull_percent * m_hull_gauge->get_width()));
    } else {
        float armor_percent =
            g_game->gameState->getShip().getMaxArmorIntegrity() <= 0
                ? 0
                : g_game->gameState->getShip().getArmorIntegrity()
                      / g_game->gameState->getShip().getMaxArmorIntegrity();
        m_armor_gauge->set_clip_width(
            static_cast<int>(armor_percent * m_armor_gauge->get_width()));

        float fuel_percent = g_game->gameState->getShip().getFuel();
        m_fuel_gauge->set_clip_width(
            static_cast<int>(fuel_percent * m_fuel_gauge->get_width()));

        float hull_percent =
            g_game->gameState->getShip().getHullIntegrity() / 100.0;
        m_hull_gauge->set_clip_width(
            static_cast<int>(hull_percent * m_hull_gauge->get_width()));
    }
    return true;
}
}; // namespace planetsurface

PlanetSurfaceTimer::PlanetSurfaceTimer(
    int x,
    int y,
    int duration,
    const std::string &text)
    : Module(x, y), m_count(0), m_duration(duration), m_text(nullptr) {
    set_active(false);

    m_gauge_background =
        make_shared<Bitmap>(images[I_ELEMENT_BIGGAUGE_EMPTY], x, y);
    add_child_module(m_gauge_background);

    std::tie(m_gauge_width, m_gauge_height) = m_gauge_background->get_size();

    m_gauge_filled =
        make_shared<Bitmap>(images[I_ELEMENT_BIGGAUGE_YELLOW], x, y);
    m_gauge_filled->set_clip_width(0);
    add_child_module(m_gauge_filled);

    m_text = make_shared<Label>(
        text,
        x,
        y,
        m_gauge_width,
        m_gauge_height,
        false,
        0,
        g_game->font18,
        BLACK);
    add_child_module(m_text);
}

bool
PlanetSurfaceTimer::on_update() {
    if (m_duration != 0 && m_duration <= ++m_count) {
        stop();

        ALLEGRO_EVENT e = make_event(EVENT_PLANETSURFACE_TIMER_EXPIRED);
        g_game->broadcast_event(&e);
    }
    return true;
}

bool
PlanetSurfaceTimer::on_draw(ALLEGRO_BITMAP * /*target*/) {
    ALLEGRO_ASSERT(m_duration != 0);

    int filled = static_cast<int>(double(m_gauge_width) * m_count / m_duration);

    m_gauge_filled->set_clip_width(filled);
    return true;
}

void
PlanetSurfaceTimer::reset(int duration) {
    m_duration = duration;
    m_count = 0;
    set_active(true);
}

void
PlanetSurfaceTimer::stop() {
    m_duration = 0;
    m_count = 0;
    set_active(false);
}

ModulePlanetSurface::ModulePlanetSurface()
    : cinematicShip(nullptr), psObjectHolder(nullptr), playerShip(nullptr),
      playerTV(nullptr), activeVessel(nullptr), panFocus(nullptr),
      TVwasMoving(false), TVwasDamaged(false), selectedPSO(nullptr),
      runPlanetLoadScripts(true), runPlanetPopulate(true), vibration(0),
      panCamera(false), m_minimap(nullptr), activeButtons(0), scroller(nullptr),
      LuaVM(nullptr)

{
    g_game->PlanetSurfaceHolder = this;
}

bool
ModulePlanetSurface::on_key_down(ALLEGRO_KEYBOARD_EVENT *event) {
    switch (event->keycode) {
    case ALLEGRO_KEY_UP:
        activeVessel->ForwardThrust(true);
        break;
    case ALLEGRO_KEY_DOWN:
        activeVessel->ReverseThrust(true);
        break;
    case ALLEGRO_KEY_RIGHT:
        activeVessel->TurnRight(true);
        break;
    case ALLEGRO_KEY_LEFT:
        activeVessel->TurnLeft(true);
        break;
    }
    return true;
}

bool
ModulePlanetSurface::on_key_up(ALLEGRO_KEYBOARD_EVENT *event) {
    switch (event->keycode) {
    // reset ship anim frame when key released
    case ALLEGRO_KEY_LEFT:
        activeVessel->TurnLeft(false);
        break;
    case ALLEGRO_KEY_RIGHT:
        activeVessel->TurnRight(false);
        break;
    case ALLEGRO_KEY_UP:
        activeVessel->ForwardThrust(false);
        break;
    case ALLEGRO_KEY_DOWN:
        activeVessel->ReverseThrust(false);
        break;
    }
    return true;
}

bool
ModulePlanetSurface::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    int button = event->button - 1;
    int x = event->x;
    int y = event->y;

    if (y <= 475) // Make sure that the click was in the play area, otherwise
                  // ignore it
    {
        if (vessel_mode == VESSEL_MODE_TV) {
            x += (int)scroller->getScrollX();
            y += (int)scroller->getScrollY();
            for (auto &pso : surfaceObjects) {
                if (pso->OnMouseReleased(button, x, y)) {
                    g_game->PlanetSurfaceHolder->m_label->set_active(false);
                    return true;
                }
            }
        }
    }
    return true;
}

bool
ModulePlanetSurface::on_event(ALLEGRO_EVENT *event) {
    if (selectedPSO != nullptr) {
        if (event->type == EVENT_PLANETSURFACE_TIMER_EXPIRED) {
            selectedPSO->OnEvent(70196);
        } else {
            selectedPSO->OnEvent(event->type - EVENT_PLANETSURFACE_COMMAND1);
        }
    }

    switch (event->type) {
    case EVENT_CLOSE:
        set_modal_child(nullptr);
        break;
    case EVENT_SAVE_GAME:
        g_game->gameState->AutoSave();
        break;

    case EVENT_LOAD_GAME:
        g_game->gameState->AutoLoad();
        return false;

    case EVENT_QUIT_GAME:
        {
            g_game->set_vibration(0);
            string escape = g_game->getGlobalString("ESCAPEMODULE");
            g_game->LoadModule(escape);
            return false;
        }
        break;

    case EVENT_CARGO_UPDATE:
        {
            updateCargoFillPercent();
            break;
        }

    case EVENT_PLANETSURFACE_ACTION1:
        {
            // Landing
            if (vessel_mode == VESSEL_MODE_SHIP_WITH_TV) {
                // in some rare circumstances panCamera is not properly
                // reset to false in Draw() we need to force it here
                // otherwise scrolling won't happen
                panCamera = false;

                if (!g_game->gameState->getShip().getHasTV()) {
                    set_modal_child(make_shared<MessageBoxWindow>(
                        "",
                        "You can't land--no Terrain Vehicle! Acquire one "
                        "at the Starport."));
                    return false;
                }

                // do a 4 point valid tile check
                if (!IsValidTile(
                        (int)playerShip->getX(), (int)playerShip->getY())
                    || !IsValidTile(
                        (int)(playerShip->getX() + playerShip->getFrameWidth()),
                        (int)playerShip->getY())
                    || !IsValidTile(
                        (int)(playerShip->getX() + playerShip->getFrameWidth()),
                        (int)(playerShip->getY() + playerShip->getFrameHeight()))
                    || !IsValidTile(
                        (int)playerShip->getX(),
                        (int)(playerShip->getY() + playerShip->getFrameHeight()))) {
                    set_modal_child(make_shared<MessageBoxWindow>(
                        "", "You can't land here!"));
                    return false;
                }

                // check that the player has enough fuel
                if (g_game->gameState->getShip().getFuel() <= 0.00f) {
                    int number_of_endurium =
                        g_game->gameState->m_ship.getEnduriumOnBoard();
                    if (number_of_endurium <= 0) {
                        set_modal_child(make_shared<MessageBoxWindow>(
                            "", "You can't land--no fuel!"));
                        return false;
                    } else
                        g_game->gameState->getShip().injectEndurium();
                }
                g_game->gameState->m_ship.ConsumeFuel(10);

                vessel_mode = VESSEL_MODE_TV;
                playerTV->setFaceAngle(playerShip->getFaceAngle());
                playerTV->setPosOffset(
                    playerShip->getXOffset(), playerShip->getYOffset());
                playerTV->setSpeed(8); // This gives the TV a little boost
                                       // when it comes out of the ship
                set_active_buttons(0);

                BigBtns[0]->set_text("Dock");
                BigBtns[1]->set_text("Scan");

                m_label->set_text(TV_NONESELECTED_TEXT);

                // Clear any leftover movement residue
                playerTV->ResetNav();
                playerTV->setCounter3(100);
                activeVessel = playerTV;

                g_game->gameState->m_ship.setHasTV(false);

                g_game->audioSystem->Play(samples[S_LAUNCHTV]);
            }

            // Docking
            else if (vessel_mode == VESSEL_MODE_TV) {
                if (CalcDistance(playerTV, playerShip) < 80) {
                    vessel_mode = VESSEL_MODE_SHIP_WITH_TV;
                    activeVessel = playerShip;
                    set_active_buttons(0);

                    // Clear the selected PSO when docking
                    if (selectedPSO != nullptr) {
                        selectedPSO->setSelected(false);
                        selectedPSO = nullptr;
                    }

                    playerShip->setSpeed(0); // This stops the ship from moving
                                             // right after docking the TV
                    playerShip->ResetNav();
                    playerTV->setCounter3(100); // refuel the TV

                    BigBtns[0]->set_text("Land");
                    BigBtns[1]->set_text("Launch");

                    m_label->set_text(SHIP_TEXT);

                    m_timer->stop();

                    g_game->gameState->m_ship.setHasTV(true);

                    // stop the TV moving sound effects
                    g_game->audioSystem->Stop(samples[S_TVMOVE]);
                    g_game->audioSystem->Stop(samples[S_DAMAGEDTV]);
                    TVwasMoving = false;

                    g_game->audioSystem->Play(samples[S_DOCKINGTV]);
                } else {
                    set_modal_child(make_shared<MessageBoxWindow>(
                        "",
                        "You are not close enough to the ship yet to dock"));
                }
            }

            // Picking Up TV
            else if (vessel_mode == VESSEL_MODE_SHIP_WITHOUT_TV) {
                if (CalcDistance(playerTV, playerShip) < 100) {
                    vessel_mode = VESSEL_MODE_SHIP_WITH_TV;
                    activeVessel = playerShip;
                    set_active_buttons(0);

                    // Clear the selected PSO when docking
                    if (selectedPSO != nullptr) {
                        selectedPSO->setSelected(false);
                        selectedPSO = nullptr;
                    }

                    playerShip->setSpeed(0); // This stops the ship from moving
                                             // right after docking the TV
                    playerShip->ResetNav();
                    playerTV->setCounter3(100); // Fill up the TV

                    BigBtns[0]->set_text("Land");
                    BigBtns[1]->set_text("Launch");

                    m_label->set_text(SHIP_TEXT);

                    g_game->gameState->m_ship.setHasTV(true);

                    g_game->audioSystem->Play(samples[S_DOCKINGTV]);

                } else {
                    set_modal_child(make_shared<MessageBoxWindow>(
                        "",
                        "You are not close enough to pick up the Terrain "
                        "Vehicle"));
                }
            }
            break;
        }

    case EVENT_PLANETSURFACE_ACTION2:
        if (vessel_mode == VESSEL_MODE_SHIP_WITH_TV) {
            if (player_stranded == false) {
                if (!exitCinematicRunning) {
                    // Leaving
                    exitCinematicRunning = true;

                    // correction for ship getting stuck in corner of
                    // map when lifting off--issue 172 see fix at
                    // exitCinematicRunning code
                    double x = playerShip->getX();
                    double y = playerShip->getY();
                    cinematicShip->setX(x);
                    cinematicShip->setY(y);
                    cinematicShip->setFaceAngle(playerShip->getFaceAngle());
                    cinematicShip->setScale(playerShip->getScale());
                    cinematicShip->setSpeed(4);

                    PostMessage("Returning to Orbit", GREEN, 2, 6);
                }
            }
        } else if (vessel_mode == VESSEL_MODE_TV) {
            // Scanning
            if (selectedPSO != nullptr) {
                selectedPSO->setScanned(true);
                selectedPSO->Scan();
            } else {
                set_modal_child(make_shared<MessageBoxWindow>(
                    "",
                    "You need to select something to scan first! You "
                    "can select objects on the planet surface by clicking "
                    "on them."));
            }
        } else if (vessel_mode == VESSEL_MODE_SHIP_WITHOUT_TV) {
            // Picking Up TV
            if (CalcDistance(playerTV, playerShip) < 100) {
                vessel_mode = VESSEL_MODE_SHIP_WITH_TV;
                activeVessel = playerShip;
                set_active_buttons(0);

                // Clear the selected PSO when docking
                if (selectedPSO != nullptr) {
                    selectedPSO->setSelected(false);
                    selectedPSO = nullptr;
                }

                playerShip->setSpeed(0); // This stops the ship from moving
                                         // right after docking the TV
                playerShip->ResetNav();
                playerTV->setCounter3(100); // Fill up the TV

                BigBtns[0]->set_text("Land");
                BigBtns[1]->set_text("Launch");

                m_label->set_text(SHIP_TEXT);

                g_game->gameState->m_ship.setHasTV(true);

                g_game->audioSystem->Play(samples[S_DOCKINGTV]);
            } else {
                set_modal_child(make_shared<MessageBoxWindow>(
                    "",
                    "You are not close enough to pick up the Terrain "
                    "Vehicle"));
            }
        }
        break;

    case EVENT_PLANETSURFACE_TIMER_EXPIRED:
        // stop playback of the sound effects
        g_game->audioSystem->Stop(samples[S_SCANNING]);
        g_game->audioSystem->Stop(samples[S_PICKUPLIFEFORM]);
        g_game->audioSystem->Stop(samples[S_MINING]);
        break;
    }
    return true;
}

bool
ModulePlanetSurface::on_close() {
    ALLEGRO_DEBUG("PlanetSurface Destroy\n");

    PlanetSurfaceObject::EmptyGraphics();

    for (auto &i : BigBtns) {
        remove_child_module(i);
        i = nullptr;
    }

    portraits.clear();

    if (LuaVM != nullptr) {
        lua_close(LuaVM);
    }

    if (playerTV != nullptr) {
        delete playerTV;
        playerTV = nullptr;
    }

    if (playerShip != nullptr) {
        delete playerShip;
        playerShip = nullptr;
    }

    if (cinematicShip != nullptr) {
        delete cinematicShip;
        cinematicShip = nullptr;
    }

    if (scroller != nullptr) {
        delete scroller;
        scroller = nullptr;
    }

    for (auto i = surfaceObjects.begin(); i != surfaceObjects.end(); ++i) {
        delete *i;
        *i = nullptr;
    }
    surfaceObjects.clear();

    for (auto &i : Btns) {
        remove_child_module(i);
        i = nullptr;
    }

    remove_child_module(m_top_gui);
    m_top_gui = nullptr;

    remove_child_module(m_label);
    m_label = nullptr;

    remove_child_module(m_cargo_button);
    m_cargo_button = nullptr;

    remove_child_module(m_cargo_fill);
    m_cargo_fill = nullptr;

    remove_child_module(m_timer);
    m_timer = nullptr;

    // force the looping sound effects to stop
    for (auto i : m_cached_samples) {
        g_game->audioSystem->Stop(i);
    }
    m_cached_samples.clear();

    return true;
}

void
ModulePlanetSurface::set_active_buttons(int num) {
    for (int i = 0; i < 9; i++) {
        Btns[i]->set_active(i < num);
    }
    m_label->set_active(num == 0);
    activeButtons = num;
}

void
ModulePlanetSurface::create_timer(int length, const string &name) {
    m_timer->set_label(name);
    m_timer->reset(length);
}

// Init is a good place to load resources
bool
ModulePlanetSurface::on_init() {
    g_game->SetTimePaused(false); // game-time normal in this module.

    ALLEGRO_DEBUG("  PlanetSurface Initialize\n");

    // enable the Pause Menu
    g_game->enable_pause_menu(true);

    // Set Misc Variables
    sfsrand(time(nullptr));
    vessel_mode = VESSEL_MODE_SHIP_WITH_TV;
    activeButtons = 0;
    selectedPSO = nullptr;
    panFocus = nullptr;
    PlanetSurfaceObject::minX = 0;
    PlanetSurfaceObject::minY = 0;
    PlanetSurfaceObject::maxX = 64 * (500 - 4);
    PlanetSurfaceObject::maxY = 64 * (500 - 5);
    runPlanetLoadScripts = true;
    runPlanetPopulate = true;
    introCinematicRunning = true;
    exitCinematicRunning = false;
    player_stranded = false;
    badGravity = 0;
    deathState = 0;

    // get planet info
    const Star *star =
        g_game->dataMgr->GetStarByID(g_game->gameState->player.currentStar);
    const Planet *planet =
        star->GetPlanetByID(g_game->gameState->player.currentPlanet);

    // severely damage ship if this planet has very heavy gravity
    if (planet->gravity == PG_VERYHEAVY || planet->gravity == PG_CRUSHING) {
        // after a short delay the "you were crushed" message is displayed
        badGravity = 90;
    }

    // player's starting position
    g_game->gameState->player.posPlanet.x = Util::Random(10 * 64, 490 * 64);
    g_game->gameState->player.posPlanet.y = Util::Random(10 * 64, 490 * 64);

    // clear screen
    al_draw_filled_rectangle(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, BLACK);

    static int bx =
        static_cast<int>(g_game->getGlobalNumber("GUI_CONTROLPANEL_POS_X"));
    static int by =
        static_cast<int>(g_game->getGlobalNumber("GUI_CONTROLPANEL_POS_Y"));
    CMDBUTTONS_UL_X = bx + 18;
    CMDBUTTONS_UL_Y = by + 242;
    OFFICERICON_UL_X = bx + 45;
    OFFICERICON_UL_Y = by + 157;

    // Initialize cargo button
    CARGOFILL_X = OFFICERICON_UL_X + 47;
    CARGOFILL_Y = OFFICERICON_UL_Y - 38;

    updateCargoFillPercent();

    m_cargo_button = make_shared<Button>(
        OFFICERICON_UL_X - 15,
        OFFICERICON_UL_Y - 43,
        EVENT_NONE,
        EVENT_CAPTAIN_CARGO,
        images[I_CARGO_BAR],
        images[I_CARGO_BAR_MO]);
    add_child_module(m_cargo_button);
    m_cargo_fill = make_shared<Bitmap>(
        images[I_ELEMENT_GAUGE_PURPLE], CARGOFILL_X, CARGOFILL_Y);
    add_child_module(m_cargo_fill);

    m_timer = make_shared<PlanetSurfaceTimer>(TIMER_X, TIMER_Y, 0, "");
    add_child_module(m_timer);

    // Create command btns
    int cbx = CMDBUTTONS_UL_X;
    int cby = CMDBUTTONS_UL_Y;

    for (int col = 0; col < 3; col++) {
        for (int row = 0; row < 3; row++) {
            int i = row + col * 3;
            EventType e =
                static_cast<EventType>(EVENT_PLANETSURFACE_COMMAND1 + i);
            Btns[i] = make_shared<TextButton>(
                "",
                g_game->font18,
                BLACK,
                ALLEGRO_ALIGN_CENTER,
                cbx,
                cby,
                EVENT_NONE,
                e,
                images[I_COMMAND_BUTTON_BG],
                images[I_COMMAND_BUTTON_BG_MO],
                images[I_COMMAND_BUTTON_BG_DISABLED]);
            add_child_module(Btns[i]);
            cbx += al_get_bitmap_width(images[I_COMMAND_BUTTON_BG].get());
        }
        cbx = CMDBUTTONS_UL_X;
        cby += al_get_bitmap_height(images[I_COMMAND_BUTTON_BG].get());
    }

    BigBtns[0] = make_shared<TextButton>(
        "Land",
        g_game->font18,
        BLACK,
        ALLEGRO_ALIGN_CENTER,
        OFFICERICON_UL_X,
        OFFICERICON_UL_Y,
        EVENT_NONE,
        EVENT_PLANETSURFACE_ACTION1,
        images[I_COMMAND_BIGBUTTON_BG],
        images[I_COMMAND_BIGBUTTON_BG_MO],
        images[I_COMMAND_BIGBUTTON_BG_DISABLED]);
    add_child_module(BigBtns[0]);

    BigBtns[1] = make_shared<TextButton>(
        "Launch",
        g_game->font18,
        BLACK,
        ALLEGRO_ALIGN_CENTER,
        OFFICERICON_UL_X
            + al_get_bitmap_width(images[I_COMMAND_BIGBUTTON_BG].get()),
        OFFICERICON_UL_Y,
        EVENT_NONE,
        EVENT_PLANETSURFACE_ACTION2,
        images[I_COMMAND_BIGBUTTON_BG],
        images[I_COMMAND_BIGBUTTON_BG_MO],
        images[I_COMMAND_BIGBUTTON_BG_DISABLED]);
    add_child_module(BigBtns[1]);

    // Initialize label
    m_label = make_shared<Label>(
        SHIP_TEXT,
        CMDBUTTONS_UL_X + 10,
        CMDBUTTONS_UL_Y + 10,
        175,
        200,
        true,
        0,
        g_game->font24,
        WHITE);
    add_child_module(m_label);

    // Load all the Lua Scripts and Register all the Lua->C++ functions
    SetupLua();

    // generate tile map
    if (!fabTilemap(star)) {
        return false;
    }

    // create the ship object
    playerShip = new PlanetSurfacePlayerVessel(LuaVM, "PlayerShip");
    playerShip->Initialize();
    activeVessel = playerShip;
    playerShip->setHealth(g_game->gameState->m_ship.getHullIntegrity());
    playerShip->setFaceAngle(Util::Random(0, 360));

    m_top_gui = make_shared<planetsurface::TopGUI>(playerShip);
    add_child_module(m_top_gui);

    // Setup cinematicShip
    cinematicShip = new PlanetSurfaceObject(LuaVM, "PlayerShip");
    cinematicShip->Initialize();
    cinematicShip->setX(playerShip->getX());
    cinematicShip->setY(playerShip->getY());
    cinematicShip->setFaceAngle(playerShip->getFaceAngle());
    // Adjust for fly in position
    cinematicShip->setX(
        cinematicShip->getX()
        - (cos(cinematicShip->getFaceAngle() * 0.0174532925) * 900));
    cinematicShip->setY(
        cinematicShip->getY()
        - (sin(cinematicShip->getFaceAngle() * 0.0174532925) * 900));
    cinematicShip->setScale(5);

    // create the TV object
    playerTV = new PlanetSurfacePlayerVessel(LuaVM, "PlayerTV");
    playerTV->Initialize();

    asw = (int)g_game->getGlobalNumber("AUX_SCREEN_WIDTH");
    ash = (int)g_game->getGlobalNumber("AUX_SCREEN_HEIGHT");
    asx = (int)g_game->getGlobalNumber("AUX_SCREEN_X");
    asy = (int)g_game->getGlobalNumber("AUX_SCREEN_Y");

    m_minimap = al_create_bitmap(asw, ash);

    // scan database for artifacts and ruins that belong on this planet
    for (auto n = g_game->dataMgr->items_begin(),
              e = g_game->dataMgr->items_end();
         n != e;
         ++n) {
        Item *item = *n;

        // is this item an artifact?
        if (item->itemType == IT_ARTIFACT) {
            // artifact located on this planet?
            if (item->planetid == g_game->gameState->player.currentPlanet) {
                // convert longitude +/- value to tilemap coords
                // Note: these calculations are based on the following
                // dimensions for the map; if these change, artifact positions
                // will be messed up
                // 154W,154E  156N,156S
                // Also, y = lat, x = long, so invert values
                // 154W 154E  156N 156S

                int itemy =
                    CENTERX
                    + (int)((float)(item->x + 3) * ((float)MAPW / (154.0 * 2.0)));
                int itemx =
                    CENTERY
                    + (int)((float)(item->y - 1) * ((float)MAPH / (156.0 * 2.0)));

                CreatePSObyItemID("artifact", item->id, itemx, itemy);
            }
        } else if (item->itemType == IT_RUIN) {
            // is ruin located on this planet?
            if (item->planetid == g_game->gameState->player.currentPlanet) {
                // convert longitude +/- value to tilemap coords

                int itemy =
                    CENTERX
                    + (int)((float)(item->x + 3) * ((float)MAPW / (154.0 * 2.0)));
                int itemx =
                    CENTERY
                    + (int)((float)(item->y - 1) * ((float)MAPH / (156.0 * 2.0)));

                CreatePSObyItemID("ruin", item->id, itemx, itemy);
            }
        }
    }
    set_active_buttons(0);

    m_cached_samples.push_back(samples[S_TVMOVE]);
    m_cached_samples.push_back(samples[S_DAMAGEDTV]);
    m_cached_samples.push_back(samples[S_SCANNING]);
    m_cached_samples.push_back(samples[S_PICKUPLIFEFORM]);
    m_cached_samples.push_back(samples[S_MINING]);

    // notify quest manager of planet landing event
    g_game->questMgr->raiseEvent(16, g_game->gameState->player.currentPlanet);

    return true;
}

void
ModulePlanetSurface::CreatePSObyItemID(
    const std::string &scriptName,
    int itemid,
    int itemx,
    int itemy) {
    int x, y;

    const Item *item = nullptr;
    if (itemid == 0)
        return;
    item = g_game->dataMgr->get_item(itemid);
    if (!item)
        return;

    PlanetSurfaceObject *pso = new PlanetSurfaceObject(LuaVM, scriptName, item);

    if (item->itemType == IT_ARTIFACT || item->itemType == IT_RUIN) {
        // graphics for artifact in data/tradedepot; for ruins in
        // data/planetsurface
        std::string filepath = "data/";
        filepath +=
            (item->itemType == IT_ARTIFACT) ? "tradedepot/" : "planetsurface/";
        filepath += item->portrait;

        int res = pso->load(filepath.c_str());
        ALLEGRO_ASSERT(res);
    }

    // default is random location
    if (itemx == -1 || itemy == -1) {
        x = Util::Random(0, MAPW);
        y = Util::Random(0, MAPH);

        // make sure tile is "ground" and can be landed on
        while (!IsValidTile(x, y)) {
            x = Util::Random(0, MAPW);
            y = Util::Random(0, MAPH);
        }
    } else {
        x = itemx;
        y = itemy;
    }

    pso->setPos(x, y);

    AddPlanetSurfaceObject(pso);

    return;
}

void
ModulePlanetSurface::AddPlanetSurfaceObject(PlanetSurfaceObject *PSO) {
    surfaceObjects.push_back(PSO);
    PSO->Initialize();
}

void
ModulePlanetSurface::RemovePlanetSurfaceObject(PlanetSurfaceObject *PSO) {
    if (PSO != nullptr) {
        auto i = find(surfaceObjects.begin(), surfaceObjects.end(), PSO);
        if (i != surfaceObjects.end()) {
            surfaceObjects.erase(i);
        }
    }
}

bool
ModulePlanetSurface::fabTilemap(const Star *star) {
    // get current star data
    int starid = -1;
    if (star) {
        starid = star->id;
    }
    if (starid == -1) {
        g_game->fatalerror(
            "ModulePlanetSurface::fabTilemap: starid is invalid");
        return false;
    }

    // get current planet data
    int planetid = -1;
    if (g_game->gameState->player.currentPlanet > -1) {
        auto planet =
            star->GetPlanetByID(g_game->gameState->player.currentPlanet);
        if (planet) {
            planetid = planet->id;
        }
    }
    if (planetid == -1) {
        g_game->fatalerror(
            "ModulePlanetSurface::fabTilemap: planetid is invalid");
    }

    // the planet surface is hard coded to 500x500 tiles
    // but the TexturedSphere software renderer works best at 256x256.
    // so, over in PlanetOrbit, both textures are generated

    // use starid and planetid for random seed
    int randomness = starid * 1000 + planetid;

    std::string planetTextureFilename =
        string("cache/planet_") + to_string(randomness) + "_500.bmp";

    // fill tilemap based on planet surface image
    m_surface = al_load_bitmap(planetTextureFilename.c_str());
    if (!m_surface) {
        g_game->message("PlanetSurface: Error loading planet texture");
        return false;
    }

    if (al_get_bitmap_width(m_surface) != 500
        && al_get_bitmap_height(m_surface) != 500) {
        g_game->message("PlanetSurface: Planet texture is invalid");
        return false;
    }

    // create tile scroller
    // these parameters CANNOT CHANGE despite being passed
    scroller = new AdvancedTileScroller(499, 499, 64, 64);

    const Planet *planet =
        star->GetPlanetByID(g_game->gameState->player.currentPlanet);

    // export planet info to lua scripts
    lua_pushstring(LuaVM, Planet::PlanetSizeToString(planet->size).c_str());
    lua_setglobal(LuaVM, "PLANETSIZE");
    lua_pushstring(
        LuaVM, Planet::PlanetTemperatureToString(planet->temperature).c_str());
    lua_setglobal(LuaVM, "TEMPERATURE");
    lua_pushstring(
        LuaVM, Planet::PlanetGravityToString(planet->gravity).c_str());
    lua_setglobal(LuaVM, "GRAVITY");
    lua_pushstring(
        LuaVM, Planet::PlanetAtmosphereToString(planet->atmosphere).c_str());
    lua_setglobal(LuaVM, "ATMOSPHERE");

    switch (planet->type) {
    case PT_OCEANIC:
        fabOceanic(planet);
        break;
    case PT_FROZEN:
        fabFrozen();
        break;
    case PT_MOLTEN:
        fabMolten();
        break;
    case PT_ASTEROID:
        fabAsteroid();
        break;
    case PT_ROCKY:
        fabRocky();
        break;
    case PT_ACIDIC:
        fabAcidic();
        break;
    case PT_GASGIANT:
        g_game->message(
            "PlanetSurface: Error, cannot land on a gas giant planet.");
        g_game->LoadModule(MODULE_ORBIT);
        break;
    default:
        g_game->message("PlanetSurface: Invalid planet type");
        g_game->LoadModule(MODULE_ORBIT);
        return false;
    }

    return true;
}

bool
ModulePlanetSurface::fabAsteroid() {
    int new_bitmap_flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *mem_bmp = al_clone_bitmap(m_surface);
    al_set_new_bitmap_flags(new_bitmap_flags);

    scroller->LoadTileSet(images[I_TILESET_ASH], 16);              // 0
    scroller->LoadTileSet(images[I_TILESET_ROCK_LIGHT], 16);       // 1
    scroller->LoadTileSet(images[I_TILESET_ICE], 16);              // 2
    scroller->LoadTileSet(images[I_TILESET_ROCK_DARK], 16, false); // 3
    scroller->LoadTileSet(images[I_TILESET_STARS], 16, false);     // 4

    if (!scroller->CreateScrollBuffer(SCREEN_WIDTH, 640)) {
        g_game->message("PlanetSurface: Error creating scroll buffer");
        return false;
    }

    ALLEGRO_COLOR color;
    unsigned char r, g, b;
    int tile;
    for (int y = 0; y < 500; y++) {
        for (int x = 0; x < 500; x++) {

            // test colors found on planet texture to determine which planet
            // tiles to draw

            color = al_get_pixel(mem_bmp, x, y);
            al_unmap_rgb(color, &r, &g, &b);

            // r = g = b, asteroid is dark gray shades
            if (r < 15) {
                tile = 4;
            } else if (r < 30) {
                tile = 3;
            } else if (r < 45) {
                tile = 1;
            } else if (r < 60) {
                tile = 2;
            } else if (r < 75) {
                tile = 0;
            } else if (r < 90) {
                tile = 0;
            } else {
                tile = 1;
            }

            scroller->setPointData(x, y, tile);
        }
    }
    scroller->GenerateTiles();

    lua_dofile("data/planetsurface/PopAsteriod.lua");

    al_destroy_bitmap(mem_bmp);
    return true;
}

bool
ModulePlanetSurface::fabRocky() {
    int new_bitmap_flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *mem_bmp = al_clone_bitmap(m_surface);
    al_set_new_bitmap_flags(new_bitmap_flags);
    scroller->LoadTileSet(images[I_TILESET_ROCK_DARK], 16, false); // 0
    scroller->LoadTileSet(images[I_TILESET_ROCK_LIGHT], 16);       // 1
    scroller->LoadTileSet(images[I_TILESET_DIRT], 16);             // 2
    scroller->LoadTileSet(images[I_TILESET_DESERT], 16);           // 3

    if (!scroller->CreateScrollBuffer(SCREEN_WIDTH, 640)) {
        g_game->message("PlanetSurface: Error creating scroll buffer");
        return false;
    }

    ALLEGRO_COLOR color;
    unsigned char r, g, b;
    int tile;
    for (int y = 0; y < 500; y++) {
        for (int x = 0; x < 500; x++) {

            // test colors found on planet texture to determine which planet
            // tiles to draw

            color = al_get_pixel(mem_bmp, x, y);
            al_unmap_rgb(color, &r, &g, &b);

            tile = 0;

            // whitish colors
            if (b > r && b > g && g > r) {
                if (b > 190) {
                    tile = 1;
                } else if (b > 170) {
                    tile = 2;
                } else if (b > 150) {
                    tile = 2;
                } else if (b > 130) {
                    tile = 3;
                } else if (b > 120) {
                    tile = 3;
                } else {
                    tile = 3;
                }
            }
            // reddish colors
            else if (r > g) {
                if (r > 160) {
                    tile = 0;
                } else if (r > 140) {
                    tile = 0;
                } else if (r > 120) {
                    tile = 0;
                } else if (r > 110) {
                    tile = 0;
                } else if (r > 100) {
                    tile = 0;
                } else {
                    tile = 0;
                }
            }

            scroller->setPointData(x, y, tile);
        }
    }
    scroller->GenerateTiles();

    lua_dofile("data/planetsurface/PopRockyPlanet.lua");

    al_destroy_bitmap(mem_bmp);
    return true;
}

bool
ModulePlanetSurface::fabFrozen() {
    int new_bitmap_flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *mem_bmp = al_clone_bitmap(m_surface);
    al_set_new_bitmap_flags(new_bitmap_flags);

    scroller->LoadTileSet(images[I_TILESET_WATER_DARK], 16, false); // 0
    scroller->LoadTileSet(images[I_TILESET_ICE], 16);               // 1
    scroller->LoadTileSet(images[I_TILESET_ROCK_LIGHT], 16);        // 2
    scroller->LoadTileSet(images[I_TILESET_SNOW], 16);              // 3

    if (!scroller->CreateScrollBuffer(SCREEN_WIDTH, 640)) {
        g_game->message("PlanetSurface: Error creating scroll buffer");
        return false;
    }

    ALLEGRO_COLOR color;
    unsigned char r, g, b;
    int tile;
    for (int y = 0; y < 500; y++) {
        for (int x = 0; x < 500; x++) {

            // test colors found on planet texture to determine which planet
            // tiles to draw

            color = al_get_pixel(mem_bmp, x, y);
            al_unmap_rgb(color, &r, &g, &b);

            // frozen ocean slightly bluish
            if (b > r && b > g) {
                tile = 0;
            }
            // normal land tiles, r = g = b (shades of gray)
            else {
                if (r > 249) {
                    tile = 2;
                } else if (r > 230) {
                    tile = 2;
                } else if (r > 200) {
                    tile = 2;
                } else if (r > 180) {
                    tile = 3;
                } else if (r > 160) {
                    tile = 1;
                } else if (r > 140) {
                    tile = 1;
                } else {
                    tile = 1;
                }
            }

            scroller->setPointData(x, y, tile);
        }
    }
    scroller->GenerateTiles();

    lua_dofile("data/planetsurface/PopFrozenPlanet.lua");

    al_destroy_bitmap(mem_bmp);
    return true;
}

bool
ModulePlanetSurface::fabOceanic(const Planet *planet) {
    int new_bitmap_flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *mem_bmp = al_clone_bitmap(m_surface);
    al_set_new_bitmap_flags(new_bitmap_flags);

    scroller->LoadTileSet(images[I_TILESET_WATER_DARK], 16, false); // 0
    if (planet->temperature == PTMP_TEMPERATE) {
        scroller->LoadTileSet(images[I_TILESET_MUD], 16);               // 1
        scroller->LoadTileSet(images[I_TILESET_GRASS_LIGHT], 16);       // 2
        scroller->LoadTileSet(images[I_TILESET_GRASS_DARK], 16);        // 3
        scroller->LoadTileSet(images[I_TILESET_WATER_MID], 16, false);  // 4
        scroller->LoadTileSet(images[I_TILESET_ROCK_DARK], 16, false);  // 5
        scroller->LoadTileSet(images[I_TILESET_SNOW], 16, false);       // 6}
        scroller->LoadTileSet(images[I_TILESET_ROCK_LIGHT], 16, false); // 7
    } else if (planet->temperature == PTMP_SEARING) {
        scroller->LoadTileSet(images[I_TILESET_DESERT], 16);            // 1
        scroller->LoadTileSet(images[I_TILESET_GRASS_DEAD], 16);        // 2
        scroller->LoadTileSet(images[I_TILESET_GRASS_LIGHT], 16);       // 3
        scroller->LoadTileSet(images[I_TILESET_WATER_MID], 16, false);  // 4
        scroller->LoadTileSet(images[I_TILESET_DIRT], 16);              // 5
        scroller->LoadTileSet(images[I_TILESET_ROCK_LIGHT], 16, false); // 6}
        scroller->LoadTileSet(images[I_TILESET_ROCK_DARK], 16, false);  // 7
    } else { // if(planet->temperature == PTMP_TROPICAL){
        scroller->LoadTileSet(images[I_TILESET_DESERT], 16);            // 1
        scroller->LoadTileSet(images[I_TILESET_GRASS_LIGHT], 16);       // 2
        scroller->LoadTileSet(images[I_TILESET_GRASS_DARK], 16);        // 3
        scroller->LoadTileSet(images[I_TILESET_WATER_MID], 16, false);  // 4
        scroller->LoadTileSet(images[I_TILESET_DIRT], 16);              // 5
        scroller->LoadTileSet(images[I_TILESET_ROCK_LIGHT], 16, false); // 6
        scroller->LoadTileSet(images[I_TILESET_ROCK_DARK], 16, false);  // 7
    }

    if (!scroller->CreateScrollBuffer(SCREEN_WIDTH, 640)) {
        g_game->message("PlanetSurface: Error creating scroll buffer");
        return false;
    }

    ALLEGRO_COLOR color;
    unsigned char r, g, b;
    int tile;
    for (int y = 0; y < 500; y++) {
        for (int x = 0; x < 500; x++) {

            color = al_get_pixel(mem_bmp, x, y);
            al_unmap_rgb(color, &r, &g, &b);

            if (r > 150 && g > 150 && b > 150) {
                if (r > 250 && g > 250 && b > 250) {
                    tile = 6;
                } else if (r > 200 && g > 200 && b > 200) {
                    tile = 7;
                } else if (r > 150 && g > 150 && b > 150) {
                    tile = 5;
                } else {
                    tile = 3;
                }
            }
            // WATER TILES
            // red should be 0 in water pixels
            else if (r == 0) {
                if (b < 110) {
                    tile = 0;
                } else if (b < 140) {
                    tile = 0;
                } else if (b < 160) {
                    tile = 0;
                } else if (b < 200) {
                    tile = 0;
                } else if (b < 230) {
                    tile = 4;
                } else {
                    tile = 4;
                }
            } else {
                // LAND TILES
                if (g < 140) {
                    tile = 1;
                } else if (g < 150) {
                    tile = 2;
                } else if (g < 160) {
                    tile = 2;
                } else if (g < 170) {
                    tile = 3;
                } else if (g < 180) {
                    tile = 3;
                } else {
                    tile = 3;
                }
            }

            scroller->setPointData(x, y, tile);
        }
    }
    scroller->GenerateTiles();

    lua_dofile("data/planetsurface/PopOceanicPlanet.lua");

    al_destroy_bitmap(mem_bmp);
    return true;
}

bool
ModulePlanetSurface::fabMolten() {
    int new_bitmap_flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *mem_bmp = al_clone_bitmap(m_surface);
    al_set_new_bitmap_flags(new_bitmap_flags);

    scroller->LoadTileSet(images[I_TILESET_LAVA], 16, false);  // 0
    scroller->LoadTileSet(images[I_TILESET_MAGMA], 16, false); // 1
    scroller->LoadTileSet(images[I_TILESET_ASH], 16);          // 2
    scroller->LoadTileSet(images[I_TILESET_ROCK_LIGHT], 16);   // 3

    if (!scroller->CreateScrollBuffer(SCREEN_WIDTH, 640)) {
        g_game->message("PlanetSurface: Error creating scroll buffer");
        return false;
    }

    ALLEGRO_COLOR color;
    unsigned char r, g, b;
    int tile;
    for (int y = 0; y < 500; y++) {
        for (int x = 0; x < 500; x++) {

            // test colors found on planet texture to determine which planet
            // tiles to draw

            color = al_get_pixel(mem_bmp, x, y);
            al_unmap_rgb(color, &r, &g, &b);

            // lava ocean
            if (r > g && r > b && g == b) {
                if (r > 249) {
                    tile = 0;
                } else if (r > 210) {
                    tile = 0;
                } else if (r > 180) {
                    tile = 0;
                } else if (r > 160) {
                    tile = 0;
                } else if (r > 140) {
                    tile = 1;
                } else {
                    tile = 1;
                }
            }
            // burnt ground
            else {
                if (g > 190) {
                    tile = 3;
                } else if (g > 150) {
                    tile = 3;
                } else if (g > 120) {
                    tile = 3;
                } else if (g > 80) {
                    tile = 2;
                } else if (g > 50) {
                    tile = 2;
                } else {
                    tile = 2;
                }
            }

            scroller->setPointData(x, y, tile);
        }
    }
    scroller->GenerateTiles();

    lua_dofile("data/planetsurface/PopMoltenPlanet.lua");

    al_destroy_bitmap(mem_bmp);
    return true;
}

bool
ModulePlanetSurface::fabAcidic() {
    int new_bitmap_flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *mem_bmp = al_clone_bitmap(m_surface);
    al_set_new_bitmap_flags(new_bitmap_flags);

    scroller->LoadTileSet(images[I_TILESET_GAS_GRASS], 16, true);   // 0
    scroller->LoadTileSet(images[I_TILESET_GAS_ACID_2], 16, false); // 1
    scroller->LoadTileSet(images[I_TILESET_GAS_ROCK_1], 16);        // 2
    scroller->LoadTileSet(images[I_TILESET_GAS_ROCK_2], 16, false); // 3

    if (!scroller->CreateScrollBuffer(SCREEN_WIDTH, 640)) {
        g_game->message("PlanetSurface: Error creating scroll buffer");
        return false;
    }

    ALLEGRO_COLOR color;
    unsigned char r, g, b;
    int tile;
    for (int y = 0; y < 500; y++) {
        for (int x = 0; x < 500; x++) {
            // test colors found on planet texture to determine which planet
            // tiles to draw
            color = al_get_pixel(mem_bmp, x, y);
            al_unmap_rgb(color, &r, &g, &b);

            // acid ocean
            if (g > 100) {
                tile = 1;
            }
            // toxic ground
            else {
                if (b > 120) {
                    tile = 3;
                } else if (b > 100) {
                    tile = 2;
                } else {
                    tile = 0;
                }
            }
            scroller->setPointData(x, y, tile);
        }
    }
    scroller->GenerateTiles();

    lua_dofile("data/planetsurface/PopAcidicPlanet.lua");

    al_destroy_bitmap(mem_bmp);
    return true;
}

bool
ModulePlanetSurface::on_update() {
    // check for crushing gravity with a time delay so ship appears to land
    // first
    if (badGravity > 0) {
        if (--badGravity <= 0) {
            badGravity = 0;

            // damage the ship's hull, random 5-25% dmg
            int damage = Util::Random(5, 25);
            g_game->gameState->m_ship.augHullIntegrity((float)-damage);

            if (g_game->gameState->m_ship.getHullIntegrity() <= 0.0f) {
                // ship destroyed, game over
                g_game->gameState->player.setAlive(false);
            } else {
                // ship damaged
                set_modal_child(make_shared<MessageBoxWindow>(
                    "",
                    "The heavy gravity of this planet has damaged your ship!",
                    SCREEN_WIDTH / 2,
                    SCREEN_HEIGHT / 2,
                    400,
                    200,
                    YELLOW));
                // key events won't be processed while the message window is
                // shown so, we force release of all keys here otherwise
                // confusing things will happen
                activeVessel->TurnLeft(false);
                activeVessel->TurnRight(false);
                activeVessel->ForwardThrust(false);
                activeVessel->ReverseThrust(false);

                // damage random ship system or crew (33%)
                g_game->gameState->m_ship.damageRandomSystemOrCrew();
            }
        }
    }

    // player ship destroyed?
    if (!g_game->gameState->player.getAlive()) {
        if (deathState == 0) {
            deathState++;
            g_game->printout(
                "The heavy gravity of this planet has CRUSHED your ship!", RED);
            set_modal_child(make_shared<MessageBoxWindow>(
                "",
                "The heavy gravity of this planet has CRUSHED your ship!",
                SCREEN_WIDTH / 2,
                SCREEN_HEIGHT / 2,
                400,
                200,
                RED));
        } else {
            // show pause menu since player has died
            g_game->TogglePauseMenu();
        }
        // break out of update function
        return false;
    }

    return true;
}

bool
ModulePlanetSurface::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);
    al_clear_to_color(BLACK);
    int centerx = ((scroller->getTilesAcross() - 16) * scroller->getTileWidth())
                  / 2; //-16 accomodates right edge adj
    int centery = ((scroller->getTilesDown() - 10) * scroller->getTileHeight())
                  / 2; //-10 accomodates bottom edge adj
    int x = (centerx - (int)scroller->getScrollX()) / 100;
    int y = (centery - (int)scroller->getScrollY()) / 100
            + 4; // for some reason it's off by 4?

    // print position on top gui
    m_top_gui->set_position(x, y);

    if (vibration > 0)
        vibration -= 10;
    if (vibration < 0)
        vibration = 0;
    g_game->set_vibration(vibration);

    for (int i = 0; i < (int)surfaceObjects.size(); ++i) {
        surfaceObjects[i]->Update();
    }

    if (vessel_mode != VESSEL_MODE_SHIP_WITH_TV) {
        playerTV->Update();
    }

    playerShip->Update();

    // update scrolling and draw tiles on the scroll buffer
    if (panCamera && panFocus != nullptr) {
        int scrX = (int)scroller->getScrollX();
        int scrY = (int)scroller->getScrollY();

        int desiredX = (int)panFocus->getX()
                       - (SCREEN_WIDTH / 2 - panFocus->getFrameWidth() / 2);
        int desiredY =
            (int)panFocus->getY()
            - (SCREEN_HEIGHT / 2 - 128 - panFocus->getFrameHeight() / 2);

        double angle = atan2((double)desiredY - scrY, (double)desiredX - scrX)
                       * 180 / 3.1415926535;
        double distance = hypot(
            static_cast<double>(desiredX - scrX),
            static_cast<double>(desiredY - scrY));
        double Increment = distance / 10;
        if (Increment < 5)
            Increment = distance;

        scrX += (int)(cos(angle * 0.0174532925) * Increment);
        scrY += (int)(sin(angle * 0.0174532925) * Increment);

        // this misses some cases, so we have to force panCamera=false on
        // EVENT_PLANETSURFACE_ACTION1 event
        if (scrX == desiredX && scrY == desiredY) {
            panCamera = false;
            panFocus = nullptr;
        }

        scrX = std::clamp(
            scrX, 0, scroller->getTilesAcross() * scroller->getTileWidth());
        scrY = std::clamp(
            scrY, 0, scroller->getTilesDown() * scroller->getTileHeight());

        scroller->setScrollPosition(scrX, scrY);
        g_game->gameState->player.posPlanet.x = scrX;
        g_game->gameState->player.posPlanet.y = scrY;
    } else {
        int scrX = (int)activeVessel->getX() - SCROLLEROFFSETX;
        int scrY = (int)activeVessel->getY() - SCROLLEROFFSETY;

        // keep scroll view from going off the "edge of the world"
        int maxwidth =
            ((scroller->getTilesAcross() - 9) * scroller->getTileWidth())
            - SCROLLEROFFSETX;
        int maxheight =
            ((scroller->getTilesDown() - 6) * scroller->getTileHeight())
            - SCROLLEROFFSETY;
        scrX = std::clamp(scrX, 0, maxwidth);
        scrY = std::clamp(scrY, 0, maxheight);

        scroller->setScrollPosition(scrX, scrY);
        g_game->gameState->player.posPlanet.x = scrX;
        g_game->gameState->player.posPlanet.y = scrY;
    }

    scroller->UpdateScrollBuffer();

    // draw scroll buffer
    scroller->DrawScrollWindow(target, 0, 0, SCREEN_WIDTH, 540);

    // TV goes on bottom so projectiles and lifeforms appear to crawl on top of
    // it
    if (vessel_mode != VESSEL_MODE_SHIP_WITH_TV) {
        bool TVisMoving =
            (playerTV->getSpeed() != 0 || playerTV->TurnLeft()
             || playerTV->TurnRight());
        bool TVisDamaged = (playerTV->getHealth() < 75);

        // the TV just passed under 75% health, and is moving
        if (!TVwasDamaged && TVisDamaged && TVisMoving) {
            g_game->audioSystem->Stop(samples[S_TVMOVE]);
            g_game->audioSystem->Stop(samples[S_DAMAGEDTV]);
            g_game->audioSystem->Play(samples[S_DAMAGEDTV], true);
            TVwasDamaged = true;
        }

        // the TV just stopped moving
        if (TVwasMoving && !TVisMoving) {
            g_game->audioSystem->Stop(samples[S_TVMOVE]);
            g_game->audioSystem->Stop(samples[S_DAMAGEDTV]);
            TVwasMoving = false;
        }

        // the TV just started moving
        if (TVisMoving && !TVwasMoving) {
            // this seems to be needed to prevent a race condition
            g_game->audioSystem->Stop(samples[S_TVMOVE]);
            g_game->audioSystem->Stop(samples[S_DAMAGEDTV]);

            TVisDamaged ? g_game->audioSystem->Play(samples[S_DAMAGEDTV], true)
                        : g_game->audioSystem->Play(samples[S_TVMOVE], true);
            TVwasMoving = true;
        }
        playerTV->TimedUpdate();
    }

    for (int i = 0; i < (int)surfaceObjects.size(); ++i) {
        surfaceObjects[i]->TimedUpdate();
    }

    // cause cinematic ship to fly automatically
    if (introCinematicRunning) {
        double distance = hypot(
            static_cast<double>(playerShip->getX() - cinematicShip->getX()),
            static_cast<double>(playerShip->getY() - cinematicShip->getY()));
        double angle = atan2(
                           (double)playerShip->getY() - cinematicShip->getY(),
                           (double)playerShip->getX() - cinematicShip->getX())
                       * 180 / 3.1415926535;

        double Increment = distance / 10;
        if (Increment < 1)
            Increment = distance;
        cinematicShip->setX(
            cinematicShip->getX() + (cos(angle * 0.0174532925) * Increment));
        cinematicShip->setY(
            cinematicShip->getY() + (sin(angle * 0.0174532925) * Increment));

        if (cinematicShip->getScale() > playerShip->getScale())
            cinematicShip->setScale(cinematicShip->getScale() - .1);

        if (cinematicShip->getX() == playerShip->getX()
            && cinematicShip->getY() == playerShip->getY()) {
            introCinematicRunning = false;
        }
        cinematicShip->Draw(target);
    }

    // taking off from the surface into orbit
    else if (exitCinematicRunning) {
        double angle = cinematicShip->getFaceAngle();
        cinematicShip->setSpeed(cinematicShip->getSpeed() * 1.05);

        cinematicShip->setX(
            cinematicShip->getX()
            + (cos(angle * 0.0174532925) * cinematicShip->getSpeed()));
        cinematicShip->setY(
            cinematicShip->getY()
            + (sin(angle * 0.0174532925) * cinematicShip->getSpeed()));

        cinematicShip->setScale(cinematicShip->getScale() + .15);

        double distance = hypot(
            static_cast<double>(playerShip->getX() - cinematicShip->getX()),
            static_cast<double>(playerShip->getY() - cinematicShip->getY()));
        cinematicShip->Draw(target);

        // this is the bug fix for issue 172
        // when at the edge of the screen and oriented outward, we'll never move
        // 900 units if (distance > 900)
        if (distance > 900 || cinematicShip->getScale() > 6.0) {
            introCinematicRunning = false;
            g_game->gameState->m_ship.ConsumeFuel(100);
            g_game->LoadModule(MODULE_ORBIT);
            return true;
        }
    } else {
        playerShip->TimedUpdate();
    }

    if (selectedPSO != nullptr && selectedPSO->IsScanned())
        drawHPBar(selectedPSO);

    // draw correct gauges in top gui
    m_top_gui->set_mode(vessel_mode);

    // draw the aux gui
    static int gax = (int)g_game->getGlobalNumber("GUI_AUX_POS_X");
    static int gay = (int)g_game->getGlobalNumber("GUI_AUX_POS_Y");
    al_draw_bitmap(images[I_GUI_AUX].get(), gax, gay, 0);

    static int gcpx = (int)g_game->getGlobalNumber("GUI_CONTROLPANEL_POS_X");
    static int gcpy = (int)g_game->getGlobalNumber("GUI_CONTROLPANEL_POS_Y");
    al_draw_bitmap(images[I_GUI_CONTROLPANEL].get(), gcpx, gcpy, 0);

    // always draw help text unless it would interfere with buttons
    m_label->set_active(!activeButtons);

    m_cargo_fill->set_clip_width(
        static_cast<int>(cargoFillPercent * m_cargo_fill->get_width()));

    if (player_stranded == true && vessel_mode == VESSEL_MODE_SHIP_WITH_TV) {
        BigBtns[1]->set_text("S.O.S.");
    }

    drawMinimap(target);
    return true;
}

void
ModulePlanetSurface::drawMinimap(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(m_minimap);
    al_clear_to_color(BLACK);

    // draw the planet scanner image
    al_draw_scaled_bitmap(
        m_surface,
        0,
        0,
        al_get_bitmap_width(m_surface),
        al_get_bitmap_height(m_surface),
        0,
        0,
        al_get_bitmap_width(m_minimap),
        al_get_bitmap_height(m_minimap),
        0);

    // draw the player's position on the minimap
    float x = playerShip->getX()
              / (scroller->getTilesAcross() * scroller->getTileWidth())
              * al_get_bitmap_width(m_minimap);
    float y = playerShip->getY()
              / (scroller->getTilesDown() * scroller->getTileHeight())
              * al_get_bitmap_height(m_minimap);
    al_draw_filled_circle(x, y, 3, LTRED);
    al_draw_circle(x, y, 3, BLACK, 1);

    // draw terrain vehicle on minimap
    if (vessel_mode != VESSEL_MODE_SHIP_WITH_TV) {
        x = (playerTV->getX())
            / (scroller->getTilesAcross() * scroller->getTileWidth())
            * al_get_bitmap_width(m_minimap);
        y = (playerTV->getY())
            / (scroller->getTilesDown() * scroller->getTileHeight())
            * al_get_bitmap_height(m_minimap);
        al_draw_filled_circle(x, y, 3, YELLOW);
        al_draw_circle(x, y, 3, BLACK, 1);
    }

    // draw lifeforms and minerals on minimap
    ALLEGRO_COLOR color;
    for (int i = 0; i < (int)surfaceObjects.size(); ++i) {
        int objtype = surfaceObjects[i]->getObjectType();
        if (objtype == 0 || objtype == 1) // draw only minerals/lifeforms
        {
            switch (objtype) {
            case 0:
                color = al_map_rgb(30, 180, 30);
                break; // lifeform
            case 1:
                color = al_map_rgb(120, 100, 50);
                break; // mineral
            }

            x = surfaceObjects[i]->getX()
                / (scroller->getTilesAcross() * scroller->getTileWidth())
                * al_get_bitmap_width(m_minimap);
            y = surfaceObjects[i]->getY()
                / (scroller->getTilesDown() * scroller->getTileHeight())
                * al_get_bitmap_height(m_minimap);
            al_draw_filled_circle(x, y, 2, color);
            al_draw_circle(x, y, 2, BLACK, 1);
        }
    }

    al_set_target_bitmap(target);
    al_draw_bitmap(m_minimap, asx, asy, 0);
}

void
ModulePlanetSurface::updateCargoFillPercent() {
    double occupiedSpace = g_game->gameState->m_ship.getOccupiedSpace();
    double totalSpace = g_game->gameState->m_ship.getTotalSpace();
    cargoFillPercent = occupiedSpace / totalSpace;
    if (cargoFillPercent > 1)
        cargoFillPercent = 1;
}

void
ModulePlanetSurface::drawHPBar(PlanetSurfaceObject *PSO) {
    al_draw_bitmap(images[I_ELEMENT_SMALLGAUGE_GREEN].get(),
                   (int)(PSO->getX() + (PSO->getFrameWidth() / 2) -
                         (al_get_bitmap_width(images[I_ELEMENT_SMALLGAUGE_GREEN].get()) / 2) -
                         g_game->gameState->player.posPlanet.x),
                   (int)(PSO->getY() - g_game->gameState->player.posPlanet.y),
                   0);
}

double
ModulePlanetSurface::CalcDistance(
    PlanetSurfaceObject *PSO1,
    PlanetSurfaceObject *PSO2) {
    return hypot(PSO2->getX() - PSO1->getX(), PSO2->getY() - PSO1->getY());
}

void
ModulePlanetSurface::PostMessage(const std::string &text) {
    g_game->printout(text);
}

void
ModulePlanetSurface::PostMessage(const std::string &text, ALLEGRO_COLOR color) {
    g_game->printout(text, color);
}

void
ModulePlanetSurface::PostMessage(
    const std::string &text,
    ALLEGRO_COLOR color,
    int blanksBefore) {
    for (int i = 0; i < blanksBefore; ++i)
        g_game->printout("");
    g_game->printout(text, color);
}

void
ModulePlanetSurface::PostMessage(
    const std::string &text,
    ALLEGRO_COLOR color,
    int blanksBefore,
    int blanksAfter) {
    for (int i = 0; i < blanksBefore; ++i)
        g_game->printout("");
    g_game->printout(text, color);
    for (int i = 0; i < blanksAfter; ++i)
        g_game->printout("");
}

void
ModulePlanetSurface::LoadPortrait(
    const std::string &name,
    const std::string &filepath) {
    auto it = portraits.find(name);
    // Only load a portrait once
    if (it == portraits.end()) {
        ALLEGRO_BITMAP *portrait = al_load_bitmap(filepath.c_str());

        // Make sure the image load just fine
        if (portrait != nullptr && name != "")
            portraits[name] =
                shared_ptr<ALLEGRO_BITMAP>(portrait, al_destroy_bitmap);
    }
}

void
ModulePlanetSurface::ShowPortrait(const std::string &name) {
    this->showPortrait = name;
    this->showPortraitCounter = 0;
}

void
ModulePlanetSurface::CheckForCollisions(PlanetSurfaceObject *PSO) {
    for (int i = 0; i < (int)surfaceObjects.size(); ++i) {
        if (PSO != surfaceObjects[i])
            PSO->CheckCollision(surfaceObjects[i]);
    }
    if (vessel_mode != VESSEL_MODE_SHIP_WITH_TV) {
        if (PSO != playerTV && PSO != playerShip)
            PSO->CheckCollision(playerTV);
    }

    // Check Top left
    int x = (int)PSO->getXOffset() - PSO->getColHalfWidth();
    int y = (int)PSO->getYOffset() - PSO->getColHalfHeight();
    CheckTileCollision(PSO, x, y);

    // Check Top right
    x = (int)PSO->getXOffset() + PSO->getColHalfWidth();
    y = (int)PSO->getYOffset() - PSO->getColHalfHeight();
    CheckTileCollision(PSO, x, y);

    // Check Bottom right
    x = (int)PSO->getXOffset() + PSO->getColHalfWidth();
    y = (int)PSO->getYOffset() + PSO->getColHalfHeight();
    CheckTileCollision(PSO, x, y);

    // Check Bottom left
    x = (int)PSO->getXOffset() - PSO->getColHalfWidth();
    y = (int)PSO->getYOffset() + PSO->getColHalfHeight();
    CheckTileCollision(PSO, x, y);
}

void
ModulePlanetSurface::CheckTileCollision(
    PlanetSurfaceObject *PSO,
    int x,
    int y) {
    if (scroller->CheckCollisionbyCoords(x, y)) {
        scroller->ConvertCoordstoNearestPoint(x, y);
        PSO->CheckCollision(
            (x * scroller->getTileWidth()) - (scroller->getTileWidth() / 2),
            (y * scroller->getTileHeight()) - (scroller->getTileWidth() / 2),
            scroller->getTileWidth(),
            scroller->getTileHeight());
    }
}

bool
ModulePlanetSurface::IsValidTile(int x, int y) {
    return !(scroller->CheckCollisionbyCoords(x, y));
}

void
ModulePlanetSurface::SetupLua() {
    /* initialize Lua */
    LuaVM = luaL_newstate();

    /* load Lua base libraries */
    luaL_openlibs(LuaVM);

    lua_register(LuaVM, "L_Debug", L_Debug);
    lua_register(LuaVM, "L_LoadImage", L_LoadImage);
    lua_register(LuaVM, "L_Move", L_Move);
    lua_register(LuaVM, "L_Draw", L_Draw);
    lua_register(LuaVM, "L_Animate", L_Animate);
    lua_register(LuaVM, "L_LaunchTV", L_LaunchTV);

    lua_pushlightuserdata(LuaVM, this);
    lua_pushcclosure(LuaVM, L_SetActions, 1);
    lua_setglobal(LuaVM, "L_SetActions");
    // lua_register(LuaVM, "L_SetActions", L_SetActions);

    lua_register(LuaVM, "L_LoadScript", L_LoadScript);
    lua_register(LuaVM, "L_PostMessage", L_PostMessage);
    lua_register(LuaVM, "L_LoadPortrait", L_LoadPortrait);
    lua_register(LuaVM, "L_ShowPortrait", L_ShowPortrait);
    lua_register(LuaVM, "L_AddItemtoCargo", L_AddItemtoCargo);
    lua_register(LuaVM, "L_AddArtifactToCargo", L_AddArtifactToCargo);
    lua_register(LuaVM, "L_DeleteSelf", L_DeleteSelf);
    lua_register(LuaVM, "L_LoadPSObyID", L_LoadPSObyID);
    lua_register(LuaVM, "L_CreateNewPSO", L_CreateNewPSO);
    lua_register(LuaVM, "L_CreateNewPSObyItemID", L_CreateNewPSObyItemID);
    lua_register(LuaVM, "L_LoadPlayerTVasPSO", L_LoadPlayerTVasPSO);
    lua_register(LuaVM, "L_LoadPlayerShipasPSO", L_LoadPlayerShipasPSO);
    lua_register(LuaVM, "L_Test", L_Test);
    lua_register(LuaVM, "L_SetRunPlanetLoadScripts", L_SetRunPlanetLoadScripts);
    lua_register(LuaVM, "L_GetRunPlanetLoadScripts", L_GetRunPlanetLoadScripts);
    lua_register(LuaVM, "L_SetRunPlanetPopulate", L_SetRunPlanetPopulate);
    lua_register(LuaVM, "L_GetRunPlanetPopulate", L_GetRunPlanetPopulate);
    lua_register(LuaVM, "L_CheckInventorySpace", L_CheckInventorySpace);
    lua_register(LuaVM, "L_KilledAnimal", L_KilledAnimal);
    lua_register(LuaVM, "L_AttackTV", L_AttackTV);
    lua_register(LuaVM, "L_TVDestroyed", L_TVDestroyed);
    lua_register(LuaVM, "L_TVOutofFuel", L_TVOutofFuel);
    lua_register(LuaVM, "L_PlayerTVisAlive", L_PlayerTVisAlive);
    lua_register(LuaVM, "L_CheckInventoryFor", L_CheckInventoryFor);
    lua_register(LuaVM, "L_RemoveItemFromInventory", L_RemoveItemFromInventory);
    lua_register(LuaVM, "L_GetPlanetID", L_GetPlanetID);
    lua_register(LuaVM, "L_CreateTimer", L_CreateTimer);

    lua_register(LuaVM, "L_GetPlayerShipPosition", L_GetPlayerShipPosition);
    lua_register(LuaVM, "L_GetPlayerTVPosition", L_GetPlayerTVPosition);
    lua_register(LuaVM, "L_GetActiveVesselPosition", L_GetActiveVesselPosition);
    lua_register(LuaVM, "L_GetScrollerPosition", L_GetScrollerPosition);
    lua_register(LuaVM, "L_GetPlayerProfession", L_GetPlayerProfession);
    lua_register(LuaVM, "L_GetPosition", L_GetPosition);
    lua_register(LuaVM, "L_GetOffsetPosition", L_GetOffsetPosition);
    lua_register(LuaVM, "L_GetScreenWidth", L_GetScreenWidth);
    lua_register(LuaVM, "L_GetScreenHeight", L_GetScreenHeight);
    lua_register(LuaVM, "L_GetScreenDim", L_GetScreenDim);
    lua_register(LuaVM, "L_GetSpeed", L_GetSpeed);
    lua_register(LuaVM, "L_GetFaceAngle", L_GetFaceAngle);
    lua_register(LuaVM, "L_GetPlayerNavVars", L_GetPlayerNavVars);
    lua_register(LuaVM, "L_GetScale", L_GetScale);
    lua_register(LuaVM, "L_GetCounters", L_GetCounters);
    lua_register(LuaVM, "L_GetThresholds", L_GetThresholds);
    lua_register(LuaVM, "L_IsPlayerMoving", L_IsPlayerMoving);
    lua_register(LuaVM, "L_GetItemID", L_GetItemID);
    lua_register(LuaVM, "L_GetState", L_GetState);
    lua_register(LuaVM, "L_GetVesselMode", L_GetVesselMode);
    lua_register(LuaVM, "L_IsScanned", L_IsScanned);
    lua_register(LuaVM, "L_GetName", L_GetName);
    lua_register(LuaVM, "L_GetValue", L_GetValue);
    lua_register(LuaVM, "L_GetDamage", L_GetDamage);
    lua_register(LuaVM, "L_IsBlackMarketItem", L_IsBlackMarketItem);
    lua_register(LuaVM, "L_IsShipRepairMetal", L_IsShipRepairMetal);
    lua_register(LuaVM, "L_IsAlive", L_IsAlive);
    lua_register(LuaVM, "L_GetColHalfWidth", L_GetColHalfWidth);
    lua_register(LuaVM, "L_GetColHalfHeight", L_GetColHalfHeight);
    lua_register(LuaVM, "L_GetID", L_GetID);
    lua_register(LuaVM, "L_GetScriptName", L_GetScriptName);
    lua_register(LuaVM, "L_GetHealth", L_GetHealth);
    lua_register(LuaVM, "L_GetMaxHealth", L_GetMaxHealth);
    lua_register(LuaVM, "L_GetStunCount", L_GetStunCount);
    lua_register(LuaVM, "L_GetItemSize", L_GetItemSize);
    lua_register(LuaVM, "L_GetSelectedPSOid", L_GetSelectedPSOid);
    lua_register(LuaVM, "L_GetObjectType", L_GetObjectType);
    lua_register(LuaVM, "L_GetDanger", L_GetDanger);
    lua_register(LuaVM, "L_GetMinimapColor", L_GetMinimapColor);
    lua_register(LuaVM, "L_GetMinimapSize", L_GetMinimapSize);

    lua_register(LuaVM, "L_SetPosition", L_SetPosition);
    lua_register(LuaVM, "L_SetVelocity", L_SetVelocity);
    lua_register(LuaVM, "L_SetSpeed", L_SetSpeed);
    lua_register(LuaVM, "L_SetFaceAngle", L_SetFaceAngle);
    lua_register(LuaVM, "L_SetAnimInfo", L_SetAnimInfo);
    lua_register(LuaVM, "L_SetAngleOffset", L_SetAngleOffset);
    lua_register(LuaVM, "L_SetScale", L_SetScale);
    lua_register(LuaVM, "L_SetCounters", L_SetCounters);
    lua_register(LuaVM, "L_SetThresholds", L_SetThresholds);
    lua_register(LuaVM, "L_SetState", L_SetState);
    lua_register(LuaVM, "L_SetVesselMode", L_SetVesselMode);
    lua_register(LuaVM, "L_SetScanned", L_SetScanned);
    lua_register(LuaVM, "L_SetDamage", L_SetDamage);
    lua_register(LuaVM, "L_SetAlive", L_SetAlive);
    lua_register(LuaVM, "L_SetColHalfWidth", L_SetColHalfWidth);
    lua_register(LuaVM, "L_SetColHalfHeight", L_SetColHalfHeight);
    lua_register(LuaVM, "L_SetScriptName", L_SetScriptName);
    lua_register(LuaVM, "L_SetAlpha", L_SetAlpha);
    lua_register(LuaVM, "L_SetHealth", L_SetHealth);
    lua_register(LuaVM, "L_SetMaxHealth", L_SetMaxHealth);
    lua_register(LuaVM, "L_SetStunCount", L_SetStunCount);
    lua_register(LuaVM, "L_SetObjectType", L_SetObjectType);
    lua_register(LuaVM, "L_SetName", L_SetName);
    lua_register(LuaVM, "L_SetMinimapColor", L_SetMinimapColor);
    lua_register(LuaVM, "L_SetMinimapSize", L_SetMinimapSize);
    lua_register(LuaVM, "L_SetNewAnimation", L_SetNewAnimation);
    lua_register(LuaVM, "L_SetActiveAnimation", L_SetActiveAnimation);

    lua_register(LuaVM, "L_PlaySound", L_PlaySound);
    lua_register(LuaVM, "L_PlayLoopingSound", L_PlayLoopingSound);
    lua_register(LuaVM, "L_StopSound", L_StopSound);

    // added for ruins
    lua_register(LuaVM, "L_GetDescription", L_GetDescription);

    /* load the scripts */
    lua_dofile("data/planetsurface/Functions.lua");
    lua_dofile("data/planetsurface/PlanetSurfacePlayerShip.lua");
    lua_dofile("data/planetsurface/PlanetSurfacePlayerTV.lua");
    lua_dofile("data/planetsurface/stunprojectile.lua");
}

int
ModulePlanetSurface::lua_dofile(const string &script_filename) {
    ALLEGRO_FILE *f = al_fopen(script_filename.c_str(), "rb");
    int ret;
    int64_t size = al_fsize(f);

    char *data = new char[size + 1];
    al_fread(f, static_cast<void *>(data), size);
    data[size] = 0;
    al_fclose(f);
    ret = luaL_dostring(LuaVM, data);
    delete[] data;
    return ret;
}

//********************************************************
//*					Global Lua Functions                 *
//********************************************************

int
L_Debug(lua_State *luaVM) {
    std::string text = lua_tostring(luaVM, -1);
    lua_pop(luaVM, 1);

    ALLEGRO_DEBUG("%s\n", text.append("\n").c_str());

    return 0;
}

int
L_LoadImage(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->load(
            lua_tostring(luaVM, -1));
    }
    lua_pop(luaVM, 1);

    return 0;
}

// Lua Example: L_Move()
int
L_Move(lua_State * /*luaVM*/) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->Move();
    }

    return 0;
}

// Lua Example: L_Draw()
int
L_Draw(lua_State * /*luaVM*/) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->Draw();
    }

    return 0;
}

// Lua Example: L_Animate()
int
L_Animate(lua_State * /*luaVM*/) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->Animate();
    }

    return 0;
}

// Lua Example: L_LaunchTV()
int
L_LaunchTV(lua_State * /*luaVM*/) {
    if (g_game->gameState->getShip().getFuel() <= 0.00f)
        g_game->gameState->getShip().injectEndurium();
    g_game->gameState->m_ship.ConsumeFuel(10);
    g_game->PlanetSurfaceHolder->playerTV->setCounter3(100);
    g_game->PlanetSurfaceHolder->activeVessel =
        g_game->PlanetSurfaceHolder->playerTV;
    return 0;
}

// Lua Example: L_SetActions("Stun","PickUp","Mine") --You can go up to 9, one
// for each button
int
L_SetActions(lua_State *luaVM) {
    ModulePlanetSurface *ps = static_cast<ModulePlanetSurface *>(
        lua_touserdata(luaVM, lua_upvalueindex(1)));
    int i = 0;
    for (i = 0; lua_gettop(luaVM); ++i) {
        if (i < 9) {
            ps->Btns[i]->set_text(lua_tostring(luaVM, -1));
            lua_pop(luaVM, 1);
        } else {
            break;
        }
    }
    if (i == 0) {
        ps->m_label->set_text(OBJECT_NEEDS_SCANNED_TEXT);
    }
    ps->set_active_buttons(i);
    return 0;
}

int
L_LoadScript(lua_State *luaVM) {
    string lua_script = lua_tostring(luaVM, -1);

    g_game->PlanetSurfaceHolder->lua_dofile(lua_script);
    lua_pop(luaVM, 1);

    return 0;
}

// L_PostMessage(b, g, r, "message")
// L_PostMessage("message")
// Lua Example: L_PostMessage(0, 0, 255, "You lick it...it doesn't taste good")
int
L_PostMessage(lua_State *luaVM) {
    int rgb[3];
    std::string text = lua_tostring(luaVM, -1);
    lua_pop(luaVM, 1);

    int i = 0;
    for (i = 0; lua_gettop(luaVM); ++i) {
        if (i < 3) {
            rgb[i] = (int)lua_tonumber(luaVM, -1);
            lua_pop(luaVM, 1);
        } else {
            break;
        }
    }

    if (i >= 3)
        g_game->printout(text, al_map_rgb(rgb[0], rgb[1], rgb[2]));
    else
        g_game->printout(text);

    return 0;
}

int
L_LoadPortrait(lua_State *luaVM) {
    std::string filepath = lua_tostring(luaVM, -1);
    lua_pop(luaVM, 1);

    std::string name = lua_tostring(luaVM, -1);
    lua_pop(luaVM, 1);

    g_game->PlanetSurfaceHolder->LoadPortrait(name, filepath);

    return 0;
}

// Lua Example: L_ShowPortrait("mineral")
int
L_ShowPortrait(lua_State *luaVM) {
    std::string name = lua_tostring(luaVM, -1);
    lua_pop(luaVM, 1);

    g_game->PlanetSurfaceHolder->ShowPortrait(name);

    return 0;
}

// Lua Example: L_AddItemtoCargo(amount, itemID)  ||  L_AddItemtoCargo(itemID)
// defaults to 1 amount
int
L_AddItemtoCargo(lua_State *luaVM) {
    int amount = 1;
    int id = (int)lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    if (lua_gettop(luaVM)) {
        amount = (int)lua_tonumber(luaVM, -1);
        lua_pop(luaVM, 1);
    }

    g_game->gameState->m_items.AddItems(id, amount); // jjh

    ALLEGRO_EVENT e =
        g_game->PlanetSurfaceHolder->make_event(EVENT_CARGO_UPDATE);
    g_game->broadcast_event(&e);

    return 0;
}

/**
    Lua Example: L_AddArtifactToCargo(artifactID)
**/
int
L_AddArtifactToCargo(lua_State *luaVM) {
    // get artifact ID
    int id = (int)lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    // allow only one artifact by itemID to exist in cargo
    g_game->gameState->m_items.SetItemCount(id, 1);

    // notify cargo window to update list
    ALLEGRO_EVENT e =
        g_game->PlanetSurfaceHolder->make_event(EVENT_CARGO_UPDATE);
    g_game->broadcast_event(&e);

    return 0;
}

// Lua Example: L_DeletePSO()
int
L_DeleteSelf(lua_State * /*luaVM*/) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        if (g_game->PlanetSurfaceHolder->selectedPSO
            == g_game->PlanetSurfaceHolder->psObjectHolder) {
            g_game->PlanetSurfaceHolder->selectedPSO = nullptr;
            g_game->PlanetSurfaceHolder->set_active_buttons(0);
        }

        g_game->PlanetSurfaceHolder->RemovePlanetSurfaceObject(
            g_game->PlanetSurfaceHolder->psObjectHolder);

        g_game->PlanetSurfaceHolder->psObjectHolder = nullptr;
    }

    return 0;
}

// Lua Example: L_LoadPSObyID(id)
int
L_LoadPSObyID(lua_State *luaVM) {
    int id = (int)lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    if (id >= 0 && id < (int)g_game->PlanetSurfaceHolder->surfaceObjects.size()
        && g_game->PlanetSurfaceHolder->surfaceObjects[id] != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder =
            g_game->PlanetSurfaceHolder->surfaceObjects[id];
    }
    return 0;
}

// Lua Example: id = L_CreateNewPSO("scriptname")
int
L_CreateNewPSO(lua_State *luaVM) {
    g_game->PlanetSurfaceHolder->AddPlanetSurfaceObject(
        new PlanetSurfaceObject(luaVM, lua_tostring(luaVM, -1)));
    lua_pushnumber(
        luaVM, g_game->PlanetSurfaceHolder->surfaceObjects.size() - 1);

    return 1;
}

// Lua Example: id = L_CreateNewPSObyItemID("scriptname", itemid)
int
L_CreateNewPSObyItemID(lua_State *luaVM) {
    int itemid = (int)lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    std::string scriptName = lua_tostring(luaVM, -1);
    lua_pop(luaVM, 1);

    g_game->PlanetSurfaceHolder->CreatePSObyItemID(scriptName, itemid);
    lua_pushnumber(
        luaVM, g_game->PlanetSurfaceHolder->surfaceObjects.size() - 1);

    return 1;
}

// Lua Example: L_LoadPlayerTVasPSO()
int
L_LoadPlayerTVasPSO(lua_State * /*luaVM*/) {
    g_game->PlanetSurfaceHolder->psObjectHolder =
        g_game->PlanetSurfaceHolder->playerTV;

    return 0;
}

// Lua Example: L_LoadPlayerShipasPSO()
int
L_LoadPlayerShipasPSO(lua_State * /*luaVM*/) {
    g_game->PlanetSurfaceHolder->psObjectHolder =
        g_game->PlanetSurfaceHolder->playerShip;

    return 0;
}

// Lua Example: L_Test()  --This function is used to test lua scripts
int
L_Test(lua_State * /*luaVM*/) {
    // I don't do anything I'm just used as a break point!
    return 0;
}

// Lua Example: L_SetRunPlanetLoadScripts(false)
int
L_SetRunPlanetLoadScripts(lua_State *luaVM) {
    g_game->PlanetSurfaceHolder->runPlanetLoadScripts =
        lua_toboolean(luaVM, -1) != 0;
    lua_pop(luaVM, 1);

    return 0;
}

// Lua Example: L_GetRunPlanetLoadScripts()
int
L_GetRunPlanetLoadScripts(lua_State *luaVM) {
    lua_pushnumber(luaVM, g_game->PlanetSurfaceHolder->runPlanetLoadScripts);
    return 1;
}

// Lua Example: L_SetRunPlanetPopulate(false)
int
L_SetRunPlanetPopulate(lua_State *luaVM) {
    g_game->PlanetSurfaceHolder->runPlanetPopulate =
        lua_toboolean(luaVM, -1) != 0;
    lua_pop(luaVM, 1);

    return 0;
}

// Lua Example: L_GetRunPlanetPopulate()
int
L_GetRunPlanetPopulate(lua_State *luaVM) {
    lua_pushnumber(luaVM, g_game->PlanetSurfaceHolder->runPlanetPopulate);
    return 1;
}

// Lua Example: L_CheckInventorySpace(quantity)
int
L_CheckInventorySpace(lua_State *luaVM) {
    int quantity = (int)lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    lua_pushboolean(
        luaVM, g_game->gameState->m_ship.getAvailableSpace() >= quantity);

    return 1;
}

// Lua Example: L_KilledAnimal(itemid)
int
L_KilledAnimal(lua_State *luaVM) {
    lua_pop(luaVM, 1);

    return 0;
}

// Lua Example: L_AttackTV(damage)
int
L_AttackTV(lua_State *luaVM) {
    int damage = (int)lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    g_game->audioSystem->Play(samples[S_LIFEFORMATTACK]);

    if (g_game->PlanetSurfaceHolder->playerTV != nullptr) {
        int realdamage = damage / 10;

        g_game->PlanetSurfaceHolder->playerTV->setHealth((
            int)(g_game->PlanetSurfaceHolder->playerTV->getHealth() - realdamage));
        g_game->PlanetSurfaceHolder->vibration = 20;

        int health = g_game->PlanetSurfaceHolder->playerTV->getHealth();

        if (health < 25)
            g_game->PlanetSurfaceHolder->PostMessage(
                "CAPTAIN! THE TERRAIN VEHICLE IS UNDER ATTACK AND IN CRITICAL "
                "CONDITION! HURRY AND GET IT OUT OF THERE!",
                RED,
                0,
                5);
        else if (health < 50)
            g_game->PlanetSurfaceHolder->PostMessage(
                "CAPTAIN! A LIFEFORM IS ATTACKING THE TERRAIN VEHICLE! DO "
                "SOMETHING QUICK!",
                RED,
                0,
                5);
        else if (health < 75)
            g_game->PlanetSurfaceHolder->PostMessage(
                "CAPTAIN! A LIFEFORM IS ATTACKING THE TERRAIN VEHICLE!",
                RED,
                0,
                6);
        else
            g_game->PlanetSurfaceHolder->PostMessage(
                "Captain, the Terrain Vehicle is under attack.", RED, 0, 6);
    }

    return 0;
}

// Lua Example: L_TVDestroyed()
int
L_TVDestroyed(lua_State * /*luaVM*/) {
    g_game->PlanetSurfaceHolder->panCamera = true;
    g_game->PlanetSurfaceHolder->panFocus =
        g_game->PlanetSurfaceHolder->playerShip;
    g_game->PlanetSurfaceHolder->activeVessel =
        g_game->PlanetSurfaceHolder->playerShip;
    g_game->PlanetSurfaceHolder->vessel_mode = VESSEL_MODE_SHIP_WITH_TV;
    if (g_game->PlanetSurfaceHolder->selectedPSO != nullptr)
        g_game->PlanetSurfaceHolder->selectedPSO->setSelected(false);
    g_game->PlanetSurfaceHolder->selectedPSO = nullptr;
    g_game->PlanetSurfaceHolder->set_active_buttons(0);

    g_game->PlanetSurfaceHolder->playerShip->setSpeed(
        0); // This stops the ship from moving right after docking the TV
    g_game->PlanetSurfaceHolder->playerShip->ResetNav();
    g_game->PlanetSurfaceHolder->playerTV->setCounter3(
        0); // This empties the fuel bar

    g_game->PlanetSurfaceHolder->BigBtns[0]->set_text("Land");
    g_game->PlanetSurfaceHolder->BigBtns[1]->set_text("Launch");

    g_game->PlanetSurfaceHolder->m_label->set_text(SHIP_TEXT);

    return 0;
}

// Lua Example: L_TVOutofFuel()
int
L_TVOutofFuel(lua_State * /*luaVM*/) {
    g_game->PlanetSurfaceHolder->panCamera = true;
    g_game->PlanetSurfaceHolder->panFocus =
        g_game->PlanetSurfaceHolder->playerShip;
    g_game->PlanetSurfaceHolder->activeVessel =
        g_game->PlanetSurfaceHolder->playerShip;
    g_game->PlanetSurfaceHolder->vessel_mode = VESSEL_MODE_SHIP_WITHOUT_TV;
    if (g_game->PlanetSurfaceHolder->selectedPSO != nullptr)
        g_game->PlanetSurfaceHolder->selectedPSO->setSelected(false);

    if (g_game->PlanetSurfaceHolder->m_timer) {
        g_game->PlanetSurfaceHolder->m_timer->stop();
    }

    g_game->PlanetSurfaceHolder->selectedPSO = nullptr;
    g_game->PlanetSurfaceHolder->set_active_buttons(0);

    g_game->PlanetSurfaceHolder->BigBtns[0]->set_text("Pick Up");
    g_game->PlanetSurfaceHolder->BigBtns[1]->set_text("Pick Up");

    g_game->PlanetSurfaceHolder->m_label->set_text(TVOUTOFFUEL_TEXT);

    g_game->PlanetSurfaceHolder->playerShip->setSpeed(
        0); // This stops the ship from moving right after docking the TV
    g_game->PlanetSurfaceHolder->playerShip->ResetNav();

    g_game->PlanetSurfaceHolder->playerTV->setCounter3(
        0); // This empties the fuel bar
    g_game->PlanetSurfaceHolder->playerTV->ResetNav();
    g_game->PlanetSurfaceHolder->playerTV->setSpeed(
        0); // This stops the tv from moving

    return 0;
}

// Lua Example: alive = L_PlayerTVisAlive()
int
L_PlayerTVisAlive(lua_State *luaVM) {
    lua_pushboolean(luaVM, g_game->PlanetSurfaceHolder->playerTV->isAlive());

    return 1;
}

// Lua Example: haveCargo = L_CheckInventoryFor( itemID, amount)
int
L_CheckInventoryFor(lua_State *luaVM) {
    int amount = (int)lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    int itemID = (int)lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    bool result = false;
    for (auto &[id, numItems] : g_game->gameState->m_items) {
        if (id == itemID && numItems >= amount) {
            result = true;
            break;
        }
    }

    lua_pushboolean(luaVM, result);

    return 1;
}

// Lua Example: L_RemoveItemFromInventory( itemID, amount)
int
L_RemoveItemFromInventory(lua_State *luaVM) {
    int amount = (int)lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    int itemID = (int)lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    g_game->gameState->m_items.RemoveItems(itemID, amount);

    return 0;
}

// Lua Example: planetid = L_GetPlanetID()
int
L_GetPlanetID(lua_State *luaVM) {
    lua_pushnumber(luaVM, g_game->gameState->player.currentPlanet);

    return 1;
}

// Lua Example: L_CreateTimer("Extract",100)
int
L_CreateTimer(lua_State *luaVM) {
    g_game->PlanetSurfaceHolder->stop_timer();

    int timerLength = (int)lua_tonumber(luaVM, -1);
    lua_pop(luaVM, 1);

    std::string timerName = lua_tostring(luaVM, -1);
    lua_pop(luaVM, 1);

    g_game->PlanetSurfaceHolder->create_timer(timerLength, timerName);

    return 0;
}

// Lua Example: x,y = L_GetPlayerShipPosition()
int
L_GetPlayerShipPosition(lua_State *luaVM) {
    lua_pushnumber(luaVM, g_game->PlanetSurfaceHolder->playerShip->getX());
    lua_pushnumber(luaVM, g_game->PlanetSurfaceHolder->playerShip->getY());

    return 2;
}

// Lua Example: x,y = L_GetPlayerTVPosition()
int
L_GetPlayerTVPosition(lua_State *luaVM) {
    lua_pushnumber(luaVM, g_game->PlanetSurfaceHolder->playerTV->getX());
    lua_pushnumber(luaVM, g_game->PlanetSurfaceHolder->playerTV->getY());

    return 2;
}

// Lua Example: x,y = L_GetActiveVesselPosition()
int
L_GetActiveVesselPosition(lua_State *luaVM) {
    lua_pushnumber(luaVM, g_game->PlanetSurfaceHolder->activeVessel->getX());
    lua_pushnumber(luaVM, g_game->PlanetSurfaceHolder->activeVessel->getY());

    return 2;
}

// Lua Example: x,y = L_GetScrollerPosition()
int
L_GetScrollerPosition(lua_State *luaVM) {
    lua_pushnumber(luaVM, g_game->gameState->player.posPlanet.x);
    lua_pushnumber(luaVM, g_game->gameState->player.posPlanet.y);

    return 2;
}

// Lua Example: prof = L_GetPlayerProfession()
int
L_GetPlayerProfession(lua_State *luaVM) {
    lua_pushnumber(luaVM, (int)g_game->gameState->getProfession());

    return 1;
}

// Lua Example: x,y = L_GetPosition()
int
L_GetPosition(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getX());
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getY());
    } else {
        lua_pushnumber(luaVM, 0);
        lua_pushnumber(luaVM, 0);
    }

    return 2;
}

// Lua Example: x,y = L_GetOffsetPosition()
int
L_GetOffsetPosition(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getXOffset());
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getYOffset());
    } else {
        lua_pushnumber(luaVM, 0);
        lua_pushnumber(luaVM, 0);
    }

    return 2;
}

// Lua Example: width = L_GetScreenWidth()
int
L_GetScreenWidth(lua_State *luaVM) {
    lua_pushnumber(luaVM, SCREEN_WIDTH);

    return 1;
}

// Lua Example: height = L_GetScreenHeight()
int
L_GetScreenHeight(lua_State *luaVM) {
    lua_pushnumber(luaVM, SCREEN_HEIGHT);

    return 1;
}

// Lua Example: width, height = L_GetScreenDim()
int
L_GetScreenDim(lua_State *luaVM) {
    lua_pushnumber(luaVM, SCREEN_WIDTH);
    lua_pushnumber(luaVM, SCREEN_HEIGHT);

    return 2;
}

// Lua Example: speed = L_GetSpeed()
int
L_GetSpeed(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getSpeed());
    }

    return 1;
}

// Lua Example: angle = L_GetFaceAngle()
int
L_GetFaceAngle(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getFaceAngle());
    }

    return 1;
}

// Lua Example: forwardThrust, reverseThrust, turnLeft, turnRight =
// L_GetPlayerNavVars()
int
L_GetPlayerNavVars(lua_State *luaVM) {
    lua_pushboolean(
        luaVM, g_game->PlanetSurfaceHolder->activeVessel->ForwardThrust());
    lua_pushboolean(
        luaVM, g_game->PlanetSurfaceHolder->activeVessel->ReverseThrust());
    lua_pushboolean(
        luaVM, g_game->PlanetSurfaceHolder->activeVessel->TurnLeft());
    lua_pushboolean(
        luaVM, g_game->PlanetSurfaceHolder->activeVessel->TurnRight());

    return 4;
}

// Lua Example: scale = L_GetScale()
int
L_GetScale(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getScale());
    }

    return 1;
}

// Lua Example: counter1, counter2, counter3 = L_SetCounters()
int
L_GetCounters(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getCounter1());
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getCounter2());
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getCounter3());
    }

    return 3;
}

// Lua Example: threshold1, threshold2, threshold3 = L_SetThresholds()
int
L_GetThresholds(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM,
            g_game->PlanetSurfaceHolder->psObjectHolder->getThreshold1());
        lua_pushnumber(
            luaVM,
            g_game->PlanetSurfaceHolder->psObjectHolder->getThreshold2());
        lua_pushnumber(
            luaVM,
            g_game->PlanetSurfaceHolder->psObjectHolder->getThreshold3());
    }

    return 3;
}

// Lua Example: if (L_IsPlayerMoving() == true)
int
L_IsPlayerMoving(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->activeVessel->getSpeed() != 0) {
        lua_pushboolean(luaVM, true);
    } else {
        lua_pushboolean(luaVM, false);
    }

    return 1;
}

// Lua Example: id = L_GetItemID()
int
L_GetItemID(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getID());
    } else
        g_game->fatalerror("GetItemID: psObjectHolder is null!");

    return 1;
}

// Lua Example: state = L_GetState()
int
L_GetState(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getState());
    }

    return 1;
}

// Lua Example: mode = L_GetVesselMode()
int
L_GetVesselMode(lua_State *luaVM) {
    lua_pushnumber(
        luaVM, static_cast<int>(g_game->PlanetSurfaceHolder->vessel_mode));

    return 1;
}

// Lua Example: scanned = L_IsScanned()
int
L_IsScanned(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushboolean(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->IsScanned());
    }

    return 1;
}

// Lua Example: name = L_GetName()
int
L_GetName(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushstring(
            luaVM,
            g_game->PlanetSurfaceHolder->psObjectHolder->getName().c_str());
    }

    return 1;
}

// Lua Example: worth = L_GetValue()
int
L_GetValue(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getValue());
    }

    return 1;
}

// Lua Example: damage = L_GetDamage()
int
L_GetDamage(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getDamage());
    }

    return 1;
}

// Lua Example: isBlackMarket = L_IsBlackMarketItem()
int
L_IsBlackMarketItem(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushboolean(
            luaVM,
            g_game->PlanetSurfaceHolder->psObjectHolder->IsBlackMarketItem());
    }

    return 1;
}

// Lua Example: shipRepairMetal = L_IsShipRepairMetal()
int
L_IsShipRepairMetal(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushboolean(
            luaVM,
            g_game->PlanetSurfaceHolder->psObjectHolder->IsShipRepairMetal());
    }

    return 1;
}

// Lua Example: isAlive = L_IsAlive()
int
L_IsAlive(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushboolean(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->isAlive());
    }

    return 1;
}

// Lua Example: halfwidth = L_GetColHalfWidth()
int
L_GetColHalfWidth(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM,
            g_game->PlanetSurfaceHolder->psObjectHolder->getColHalfWidth());
    }

    return 1;
}

// Lua Example: halfheight = L_GetColHalfHeight()
int
L_GetColHalfHeight(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM,
            g_game->PlanetSurfaceHolder->psObjectHolder->getColHalfHeight());
    }

    return 1;
}

// Lua Example: id = L_GetID()
int
L_GetID(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        for (int i = 0;
             i < (int)g_game->PlanetSurfaceHolder->surfaceObjects.size();
             ++i) {
            if (g_game->PlanetSurfaceHolder->psObjectHolder
                == g_game->PlanetSurfaceHolder->surfaceObjects[i]) {
                lua_pushnumber(luaVM, i);
                break;
            }
        }
    }

    return 1;
}

// Lua Example: scriptName = L_GetScriptName()
int
L_GetScriptName(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushstring(
            luaVM,
            g_game->PlanetSurfaceHolder->psObjectHolder->GetScriptName()
                .c_str());
    }

    return 1;
}

// Lua Example: health = L_GetHealth()
int
L_GetHealth(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getHealth());
    }

    return 1;
}

// Lua Example: health = L_GetMaxHealth()
int
L_GetMaxHealth(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getMaxHealth());
    }

    return 1;
}

// Lua Example: stuncount = L_GetStunCount()
int
L_GetStunCount(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getStunCount());
    }

    return 1;
}

// Lua Example: size = L_GetItemSize()
int
L_GetItemSize(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getSize());
    }

    return 1;
}

// Lua Example: id = L_GetSelectedPSOid()
int
L_GetSelectedPSOid(lua_State *luaVM) {
    for (int i = 0; i < (int)g_game->PlanetSurfaceHolder->surfaceObjects.size();
         ++i) {
        if (g_game->PlanetSurfaceHolder->selectedPSO
            == g_game->PlanetSurfaceHolder->surfaceObjects[i]) {
            lua_pushnumber(luaVM, i);
            break;
        }
    }

    return 1;
}

// Lua Example: objectType = L_GetObjectType()
int
L_GetObjectType(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM,
            g_game->PlanetSurfaceHolder->psObjectHolder->getObjectType());
    }

    return 1;
}

// Lua Example: danger = L_GetDanger()
int
L_GetDanger(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM, g_game->PlanetSurfaceHolder->psObjectHolder->getDangerLvl());
    }

    return 1;
}

// Lua Example: color = L_GetMinimapColor()
int
L_GetMinimapColor(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        unsigned char r, g, b;
        uint32_t color;
        al_unmap_rgb(
            g_game->PlanetSurfaceHolder->psObjectHolder->getMinimapColor(),
            &r,
            &g,
            &b);
        color = (r << 16) | (g << 8) | b;
        lua_pushnumber(luaVM, color);
    }

    return 1;
}

// Lua Example: size = L_GetMinimapSize()
int
L_GetMinimapSize(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pushnumber(
            luaVM,
            g_game->PlanetSurfaceHolder->psObjectHolder->getMinimapSize());
    }

    return 1;
}

// Lua Example: name = L_GetDescription()
int
L_GetDescription(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        string desc =
            g_game->PlanetSurfaceHolder->psObjectHolder->get_description();
        lua_pushstring(luaVM, desc.c_str());
    }

    return 1;
}

// Lua Example: L_SetPosition(x,y)
int
L_SetPosition(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setY(
            lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);

        g_game->PlanetSurfaceHolder->psObjectHolder->setX(
            lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetVelocity(velX,velY)
int
L_SetVelocity(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setVelY(
            lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);

        g_game->PlanetSurfaceHolder->psObjectHolder->setVelX(
            lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetSpeed(speed)
int
L_SetSpeed(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setSpeed(
            lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }
    return 0;
}

// Lua Example: L_SetFaceAngle(angle)
int
L_SetFaceAngle(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setFaceAngle(
            (float)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }
    return 0;
}

// Lua Example: L_SetAnimInfo(FrameWidth, FrameHeight, AnimColumns, TotalFrames,
// CurFrame)
int
L_SetAnimInfo(lua_State *luaVM) {
    g_game->PlanetSurfaceHolder->psObjectHolder->setCurrFrame(
        (int)lua_tonumber(luaVM, -1));
    lua_pop(luaVM, 1);

    g_game->PlanetSurfaceHolder->psObjectHolder->setTotalFrames(
        (int)lua_tonumber(luaVM, -1));
    lua_pop(luaVM, 1);

    g_game->PlanetSurfaceHolder->psObjectHolder->setAnimColumns(
        (int)lua_tonumber(luaVM, -1));
    lua_pop(luaVM, 1);

    g_game->PlanetSurfaceHolder->psObjectHolder->setFrameHeight(
        (int)lua_tonumber(luaVM, -1));
    lua_pop(luaVM, 1);

    g_game->PlanetSurfaceHolder->psObjectHolder->setFrameWidth(
        (int)lua_tonumber(luaVM, -1));
    lua_pop(luaVM, 1);

    return 0;
}

// Lua Example: L_SetAngleOffset(angle)
int
L_SetAngleOffset(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setAngleOffset(
            (float)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetScale(angle)
int
L_SetScale(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setScale(
            lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetCounters(counter1, counter2, counter3)
int
L_SetCounters(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setCounter3(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);

        g_game->PlanetSurfaceHolder->psObjectHolder->setCounter2(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);

        g_game->PlanetSurfaceHolder->psObjectHolder->setCounter1(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetThresholds(threshold1, threshold2, threshold3)
int
L_SetThresholds(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setThreshold3(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);

        g_game->PlanetSurfaceHolder->psObjectHolder->setThreshold2(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);

        g_game->PlanetSurfaceHolder->psObjectHolder->setThreshold1(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetState(state)
int
L_SetState(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setState(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_GetVesselMode(mode)
int
L_SetVesselMode(lua_State *luaVM) {
    g_game->PlanetSurfaceHolder->vessel_mode =
        static_cast<VesselMode>(lua_tonumber(luaVM, -1));
    lua_pop(luaVM, 1);

    return 0;
}

// Lua Example: L_SetScanned(true)
int
L_SetScanned(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setScanned(
            lua_toboolean(luaVM, -1) != 0);
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetDamage(damage)
int
L_SetDamage(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setDamage(
            lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetAlive(isAlive)
int
L_SetAlive(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setAlive(
            lua_toboolean(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetColHalfWidth(halfwidth)
int
L_SetColHalfWidth(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setColHalfWidth(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetColHalfHeight(halfheight)
int
L_SetColHalfHeight(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setColHalfHeight(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetScriptName(scriptName)
int
L_SetScriptName(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setScriptName(
            lua_tostring(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetAlpha(true)
int
L_SetAlpha(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetHealth(health)
int
L_SetHealth(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setHealth(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetMaxHealth(health)
int
L_SetMaxHealth(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setMaxHealth(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetStunCount(stuncount)
int
L_SetStunCount(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setStunCount(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetObjectType(objectType)
int
L_SetObjectType(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setObjectType(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetName("Justin")
int
L_SetName(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setName(
            lua_tostring(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetMinimapColor(color)
int
L_SetMinimapColor(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        unsigned char r, g, b;
        uint32_t color;
        color = (uint32_t)lua_tonumber(luaVM, -1);
        r = (color & 0xff0000) >> 16;
        g = (color & 0x00ff00) >> 8;
        b = (color & 0x0000ff);
        g_game->PlanetSurfaceHolder->psObjectHolder->setMinimapColor(
            al_map_rgb(r, g, b));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetMinimapSize(size)
int
L_SetMinimapSize(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->setMinimapSize(
            (int)lua_tonumber(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_SetNewAnimation("walk", 0, 4, 2)
int
L_SetNewAnimation(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        int delay = (int)lua_tonumber(luaVM, -1);
        lua_pop(luaVM, 1);

        int endFrame = (int)lua_tonumber(luaVM, -1);
        lua_pop(luaVM, 1);

        int startFrame = (int)lua_tonumber(luaVM, -1);
        lua_pop(luaVM, 1);

        std::string name = lua_tostring(luaVM, -1);
        lua_pop(luaVM, 1);

        g_game->PlanetSurfaceHolder->psObjectHolder->AddAnimation(
            name, startFrame, endFrame, delay);
    }

    return 0;
}

// Lua Example: L_SetActiveAnimation("walk")
int
L_SetActiveAnimation(lua_State *luaVM) {
    if (g_game->PlanetSurfaceHolder->psObjectHolder != nullptr) {
        g_game->PlanetSurfaceHolder->psObjectHolder->SetActiveAnimation(
            lua_tostring(luaVM, -1));
        lua_pop(luaVM, 1);
    }

    return 0;
}

// Lua Example: L_PlaySound("stunner")
int
L_PlaySound(lua_State *luaVM) {
    std::string sample = lua_tostring(luaVM, -1);
    transform(sample.begin(), sample.end(), sample.begin(), ::toupper);
    g_game->audioSystem->Play(samples["PLANETSURFACE::" + sample]);
    lua_pop(luaVM, 1);

    return 0;
}

// Lua Example: L_PlayLoopingSound("TVmove")
int
L_PlayLoopingSound(lua_State *luaVM) {
    std::string sample = lua_tostring(luaVM, -1);
    transform(sample.begin(), sample.end(), sample.begin(), ::toupper);
    g_game->audioSystem->Play(samples["PLANETSURFACE::" + sample], true);
    lua_pop(luaVM, 1);

    return 0;
}

// Lua Example: L_StopSound("TVmove")
int
L_StopSound(lua_State *luaVM) {
    std::string sample = lua_tostring(luaVM, -1);
    transform(sample.begin(), sample.end(), sample.begin(), ::toupper);
    g_game->audioSystem->Stop(samples["PLANETSURFACE::" + sample]);
    lua_pop(luaVM, 1);

    return 0;
}
// vi: ft=cpp
