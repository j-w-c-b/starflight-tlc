/*
        STARFLIGHT - THE LOST COLONY
        ModulePlanetOrbit.cpp - Handles planet orbit
        Author: J.Harbour
        Date: January, 2007

        This module needs to allow the player to scan the planet (via Science
   Officer), and choose to land or break orbit. The planet surface is displayed
   in the Aux Window and the player chooses a location for the landing, then the
   landing sequence is displayed. When landing is complete, transfer controls to
   the planet surface module.
*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder-ctor"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "PerlinTL.h"
#pragma GCC diagnostic pop

#include "AudioSystem.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "ModuleControlPanel.h"
#include "ModulePlanetOrbit.h"
#include "PauseMenu.h"
#include "QuestMgr.h"
#include "Util.h"
#include "planetorbit_resources.h"

using namespace std;
using namespace planetorbit;

ALLEGRO_DEBUG_CHANNEL("ModulePlanetOrbit")

ModulePlanetOrbit::ModulePlanetOrbit() : Module(), flag_DoDock(false) {}

bool
ModulePlanetOrbit::on_event(ALLEGRO_EVENT *event) {
    string escape;
    switch (event->type) {
        // pause menu events
    case EVENT_SAVE_GAME: // save game
        g_game->gameState->AutoSave();
        g_game->printout("<Game Saved>", WHITE, 5000);
        set_modal_child(nullptr);
        break;

    case EVENT_LOAD_GAME: // load game
        g_game->gameState->AutoLoad();
        set_modal_child(nullptr);
        break;

    case EVENT_QUIT_GAME: // quit game
        g_game->set_vibration(0);
        escape = g_game->getGlobalString("ESCAPEMODULE");
        set_modal_child(nullptr);
        g_game->LoadModule(escape);
        break;

    case EVENT_CAPTAIN_LAUNCH:
        g_game->printout(
            OFFICER_NAVIGATION,

            "Yes, sir, leaving planet orbit...",
            ORANGE);
        g_game->LoadModule(MODULE_INTERPLANETARY);
        break;

    case EVENT_CAPTAIN_DESCEND:
        // if this is a homeworld or restricted world, prevent landing
        if (planet->landable == false) {
            (g_game->gameState->player.currentPlanet == 429)
                ? // planet #429 is Cermait VI, in Cermait system (247,218)
                g_game->printout(
                    OFFICER_NAVIGATION,

                    "Sir, a force field is repelling the ship and "
                    "preventing us from entering the gravity well!",
                    RED,
                    5000)
                : g_game->printout(
                    OFFICER_NAVIGATION,

                    "Sir, we are prohibited from landing "
                    "on this protected world!",
                    RED,
                    5000);
            break;
        }

        if (planet->type != PT_GASGIANT) {
            // fuel must be > 10% to land on a planet
            if (g_game->gameState->m_ship.getFuel() >= 0.1f) {
                g_game->gameState->m_ship.ConsumeFuel(100);
                g_game->LoadModule(MODULE_SURFACE);
                break;
            } else {
                g_game->printout(
                    OFFICER_NAVIGATION,

                    "We do not have enough fuel to land and "
                    "return to orbit.",
                    RED,
                    5000);
            }
        } else {
            g_game->printout(
                OFFICER_NAVIGATION,

                "Sir, we cannot survive the atmospheric "
                "entry of this planet!",
                RED,
                5000);
        }
        break;

    case EVENT_SCIENCE_SCAN:
        if ((planetScan == 0 || planetScan == 3) && planetAnalysis == 0) {
            // clear the message window only when user clicks the scan
            // button
            g_game->clear_printout();
            g_game->printout(
                OFFICER_SCIENCE,

                "Affirmative, Captain. Scanning planet...",
                LTGREEN,
                2000);
            scanplanet();
        }
        break;

    case EVENT_SCIENCE_ANALYSIS:
        if (planetScan == 3 && planetAnalysis == 0) {
            g_game->printout(
                OFFICER_SCIENCE,

                "Affirmative, Captain. Analyzing sensor data...",
                LTGREEN,
                2000);
            analyzeplanet();
        }
        break;

    case EVENT_NAVIGATOR_STARMAP:
        g_game->printout(
            OFFICER_NAVIGATION,

            "Starmap not available while orbiting a planet.",
            YELLOW,
            -1);
        break;

    case EVENT_NAVIGATOR_DOCK:
        // planet id #8 = Myrrdan in the database
        if (g_game->gameState->player.currentPlanet == 8) {
            g_game->printout(
                OFFICER_NAVIGATION,

                "Yes, sir! Docking with Starport.",
                ORANGE,
                5000);
            flag_DoDock = true;
        } else {
            g_game->printout(
                OFFICER_NAVIGATION,

                "Sorry sir! There are no Starports in the vicinity.",
                YELLOW,
                -1);
            flag_DoDock = false;
        }
        break;

    case EVENT_NAVIGATOR_ORBIT:
        g_game->printout(
            OFFICER_NAVIGATION,

            "We are already orbiting a planet!",
            YELLOW,
            -1);
        break;

    case EVENT_NAVIGATOR_HYPERSPACE:
        g_game->printout(
            OFFICER_NAVIGATION,

            "We can't enter hyperspace while in orbit!",
            YELLOW,
            -1);
        break;

    case EVENT_TACTICAL_SHIELDS:
        g_game->printout(OFFICER_TACTICAL, "We are not in combat", YELLOW, -1);
        break;

    case EVENT_TACTICAL_WEAPONS:
        g_game->printout(OFFICER_TACTICAL, "We are not in combat", YELLOW, -1);
        break;

    case EVENT_TACTICAL_COMBAT:
        g_game->printout(
            OFFICER_TACTICAL,

            "With WHOM shall we engage in combat, sir?",
            YELLOW,
            -1);
        break;

    case EVENT_ENGINEER_INJECT:
        g_game->gameState->getShip().injectEndurium();
        break;

    case EVENT_COMM_HAIL:
        g_game->printout(
            OFFICER_COMMUNICATION,

            "There are no other ships in the vicinity",
            YELLOW,
            -1);
        break;

    case EVENT_COMM_DISTRESS:
        g_game->printout(

            "<This feature won't be installed until Tuesday>", YELLOW, -1);
        break;

    case EVENT_COMM_STATEMENT:
        g_game->printout(
            OFFICER_COMMUNICATION,

            "We are not in contact with any other ship",
            YELLOW,
            -1);
        break;

    case EVENT_COMM_QUESTION:
        g_game->printout(
            OFFICER_COMMUNICATION,

            "We are not in contact with anyone",
            YELLOW,
            -1);
        break;

    case EVENT_COMM_POSTURE:
        g_game->printout(

            "<This feature won't be installed until Tuesday>", YELLOW, -1);
        break;

    case EVENT_COMM_TERMINATE:
        g_game->printout(
            OFFICER_COMMUNICATION,

            "We are not engaged in communication",
            YELLOW,
            -1);
        break;
    }
    return true;
}

bool
ModulePlanetOrbit::CreatePlanetTexture() {
    // these cannot change: just noted for reference
    static const int TEX_SIZE_ORBIT = 256;
    static const int TEX_SIZE_SURFACE = 500;
    // use starid and planetid for sfrandom seed
    int sfrandomness = starid * 1000 + planetid;
    string orbit_filename =
        "cache/planet_" + to_string(sfrandomness) + "_256.bmp";
    string surface_filename =
        "cache/planet_" + to_string(sfrandomness) + "_500.bmp";
    ALLEGRO_PATH *user_data_path = al_get_standard_path(ALLEGRO_USER_DATA_PATH);
    // Create directory using relative path w/physfs
    ALLEGRO_PATH *planetorbit_path = al_create_path("cache");
    al_make_directory(al_path_cstr(planetorbit_path, ALLEGRO_NATIVE_PATH_SEP));

    // planet textures are stored in user data's planetorbit using the starid
    // and planetid as a sfrandom seed once created they are re-used two
    // textures are required for each planet as a result of replacing the OpenGL
    // with a software renderer

    string orbit_pathname =
        string(al_path_cstr(user_data_path, '/')) + orbit_filename;
    string surface_pathname =
        string(al_path_cstr(user_data_path, '/')) + surface_filename;

    al_destroy_path(planetorbit_path);
    al_destroy_path(user_data_path);

    // try to find planet texture previously generated
    planet_texture = al_load_bitmap(orbit_filename.c_str());
    if (!planet_texture) {
        // generate planet texture for ORBIT render 256x256
        createPlanetSurface(
            TEX_SIZE_ORBIT,
            TEX_SIZE_ORBIT,
            sfrandomness,
            planetType,
            orbit_pathname);

        // generate planet texture for SURFACE tilemap 500x500 (used in
        // ModulePlanetSurface)
        createPlanetSurface(
            TEX_SIZE_SURFACE,
            TEX_SIZE_SURFACE,
            sfrandomness,
            planetType,
            surface_pathname);

        // load newly generated planet texture
        planet_texture = al_load_bitmap(orbit_filename.c_str());

        if (!planet_texture) {
            g_game->message("PlanetOrbit: Error loading planet texture");
            return false;
        }
    }

    // get AUX_SCREEN gui values from globals
    static int asw = (int)g_game->getGlobalNumber("AUX_SCREEN_WIDTH");
    static int ash = (int)g_game->getGlobalNumber("AUX_SCREEN_HEIGHT");

    // create planet topography ALLEGRO_BITMAP for minimap
    planet_topography = al_create_bitmap(asw, ash);
    al_set_target_bitmap(planet_topography);
    al_clear_to_color(BLACK);

    // scale planet texture onto topography, cutting skewed N/S poles (drop 10
    // pixels from top/bottom)

    al_set_target_bitmap(planet_topography);
    al_draw_scaled_bitmap(
        planet_texture,
        0,
        10,
        al_get_bitmap_width(planet_texture),
        al_get_bitmap_height(planet_texture) - 20,
        1,
        1,
        al_get_bitmap_width(planet_topography) - 2,
        al_get_bitmap_height(planet_topography) - 2,
        0);

    // now create a scratch image as a duplicate of topography used for sensor
    // scans
    planet_scanner_map = al_create_bitmap(asw, ash);
    al_set_target_bitmap(planet_scanner_map);
    al_draw_bitmap(planet_topography, 0, 0, 0);

    // create texture-mapped sphere of the planet
    texsphere = new TexturedSphere(TEX_SIZE_ORBIT);
    if (!texsphere->SetTexture(planet_texture)) {
        g_game->fatalerror("PlanetOrbit: error loading planet_texture");
        return false;
    }

    return true;
}

bool
ModulePlanetOrbit::on_close() {
    ALLEGRO_DEBUG("PlanetOrbit Destroy\n");

    if (planet_topography) {
        al_destroy_bitmap(planet_topography);
        planet_topography = NULL;
    }
    if (planet_scanner_map) {
        al_destroy_bitmap(planet_scanner_map);
        planet_scanner_map = NULL;
    }
    if (planet_texture) {
        al_destroy_bitmap(planet_texture);
        planet_texture = NULL;
    }
    m_starfield = nullptr;

    return true;
}

bool
ModulePlanetOrbit::on_init() {
    g_game->SetTimePaused(false); // game-time normal in this module.
    ALLEGRO_DEBUG("  PlanetOrbit Initialize\n");

#ifdef DEBUGMODE
    if (g_game->getGlobalString("STARTUPMODULE") == "PLANETORBIT") {
        starid = g_game->gameState->player.currentStar = 2;
        planetid = g_game->gameState->player.currentPlanet = 8;
    }
#endif

    // enable the Pause Menu
    g_game->enable_pause_menu(true);

    planetScan = 0;
    planetAnalysis = 0;
    planetRotation = 0;
    flag_DoDock = false;

    // get current star data
    auto star =
        g_game->dataMgr->GetStarByID(g_game->gameState->player.currentStar);
    if (star)
        starid = star->id;
    else
        starid = -1;

    string lightmapFilename;
    // read planet data
    if (star != nullptr && g_game->gameState->player.currentPlanet > -1) {
        planet = star->GetPlanetByID(g_game->gameState->player.currentPlanet);

        if (planet) {
            planetid = planet->id;
            planetType = planet->type;

            switch (planet->size) {
            case PS_HUGE:
                planetRadius = 240;
                planetRotationSpeed = 0.14;
                lightmapOffsetX = -250;
                lightmapOffsetY = -250;
                lightmapFilename = I_LIGHTMAP_PLANET_500;
                break;
            case PS_LARGE:
                planetRadius = 192;
                planetRotationSpeed = 0.17;
                lightmapOffsetX = -200;
                lightmapOffsetY = -200;
                lightmapFilename = I_LIGHTMAP_PLANET_400;
                break;
            case PS_MEDIUM:
                planetRadius = 144;
                planetRotationSpeed = 0.20;
                lightmapOffsetX = -150;
                lightmapOffsetY = -150;
                lightmapFilename = I_LIGHTMAP_PLANET_300;
                break;
            case PS_SMALL:
                planetRadius = 96;
                planetRotationSpeed = 0.23;
                lightmapOffsetX = -100;
                lightmapOffsetY = -100;
                lightmapFilename = I_LIGHTMAP_PLANET_200;
                break;
            default: // asteroid
                planetRadius = 48;
                planetRotationSpeed = 0.26;
                lightmapOffsetX = -50;
                lightmapOffsetY = -50;
                lightmapFilename = I_LIGHTMAP_PLANET_100;
            }
        } else
            planetid = -1;
    }

    if (starid != -1 && planetid != -1) {
        if (!CreatePlanetTexture())
            return false;
    }

    // load planet lightmap overlay
    lightmap_overlay = images[lightmapFilename];

    if (!lightmap_overlay) {
        g_game->fatalerror("PlanetOrbit: error loading lightmap_overlay");
        return false;
    }

    // tell questmgr that orbit event has occurred
    g_game->questMgr->raiseEvent(12, g_game->gameState->player.currentPlanet);

    m_starfield = make_shared<Bitmap>(images[I_STARFIELD]);

    return true;
}

bool
ModulePlanetOrbit::on_update() {
    std::string temp;
    if (planetScan == 1) {
        // get size of planet to determine scan time
        int scantime = 0;
        switch (planet->size) {
        case PS_SMALL:
            scantime = 2000;
            break;
        case PS_MEDIUM:
            scantime = 4000;
            break;
        case PS_LARGE:
            scantime = 6000;
            break;
        case PS_HUGE:
            scantime = 8000;
            break;
        default:
            scantime = 4000;
        }

        // show scanning process for short duration
        if (Util::ReentrantDelay(scantime)) {
            // done scanning
            planetScan++;
        }
    } else if (planetScan == 2) {
        // roll on crewman's skill with this operation
        if (g_game->gameState->SkillCheck(SKILL_SCIENCE)) {
            g_game->printout(
                OFFICER_SCIENCE,

                "Planetary scan complete. Ready for analysis.",
                LTGREEN,
                1000);
            // after success, increase skill
            if (Util::Random(1, 100) > 50) // 50% of the time
            {
                if (g_game->gameState->SkillUp(SKILL_SCIENCE)) {
                    g_game->printout(
                        OFFICER_SCIENCE,

                        "I think I'm getting the hang of this (SKILL "
                        "UP).",
                        PURPLE,
                        1000);
                }
            }
            planetScan++;
        }
    } else if (planetAnalysis == 1) {
        if (Util::ReentrantDelay(2000)) {
            // done analyzing
            planetAnalysis++;

            if (g_game->gameState->SkillCheck(SKILL_SCIENCE)) {
                if (Util::Random(1, 100) > 50
                    && g_game->gameState->SkillUp(SKILL_SCIENCE))
                    g_game->printout(
                        OFFICER_SCIENCE,

                        "I think I'm getting better at this (SKILL UP).",
                        PURPLE,
                        1000);
            }
        }
    } else if (planetAnalysis > 1) {
        if (planet->id == HOMEWORLD_ID) {
            temp = "This is our beautiful home world Myrrdan. ";
        } else {
            // start building the description based on planet type
            temp = "Astronomy records indicate this ";
            if (planet->type == PT_ASTEROID)
                temp += "asteroid ";
            else
                temp += Planet::PlanetTypeToString(planet->type) + " planet ";
            temp += "is called " + Util::ToUpper(planet->name) + ". ";
        }

        // build size and gravity description
        temp += "It is " + Planet::PlanetSizeToString(planet->size)
                + " in size, and gravity is "
                + Planet::PlanetGravityToString(planet->gravity) + ". ";

        // build the atmosphere description
        switch (planet->atmosphere) {
        case PA_ACIDIC:
            temp += "The atmosphere is ACIDIC";
            break;
        case PA_BREATHABLE:
            temp += "The atmosphere is BREATHABLE";
            break;
        case PA_FIRESTORM:
            temp += "The atmosphere is a FIRESTORM";
            break;
        case PA_TOXIC:
            temp += "The atmosphere is TOXIC";
            break;
        case PA_TRACEGASES:
            temp += "The atmosphere has only TRACE GASES";
            break;
        default:
            temp += "There is NO ATMOSPHERE";
            break;
        }

        // build temperature description
        temp += ", and the temperature is ";
        switch (planet->temperature) {
        case PTMP_SUBARCTIC:
            temp += "SUB-ARCTIC";
            break;
        case PTMP_ARCTIC:
            temp += "ARCTIC";
            break;
        case PTMP_TROPICAL:
            temp += "TROPICAL";
            break;
        case PTMP_TEMPERATE:
            temp += "TEMPERATE";
            break;
        case PTMP_SEARING:
            temp += "SEARING";
            break;
        case PTMP_INFERNO:
            temp += "INFERNO";
            break;
        default:
            temp += "UNKNOWN";
            break;
        }
        temp += ".";

        // build weather description
        temp += "The weather is ";
        switch (planet->weather) {
        case PW_NONE:
            temp += "NONEXISTENT";
            break;
        case PW_CALM:
            temp += "CALM";
            break;
        case PW_MODERATE:
            temp += "MODERATE";
            break;
        case PW_VIOLENT:
            temp += "VIOLENT";
            break;
        case PW_VERYVIOLENT:
            temp += "VERY VIOLENT";
            break;
        case PW_INVALID:
        default:
            temp += "UNKNOWN";
            break;
        }
        temp += ".";

        g_game->printout(
            OFFICER_SCIENCE,

            "Sensor analysis is complete.",
            WHITE,
            1000);
        g_game->printout(temp, STEEL, 1000);

        // detect artifacts and ruins on surface
        for (auto n = g_game->dataMgr->items_begin(),
                  e = g_game->dataMgr->items_end();
             n != e;
             ++n) {
            Item *item = *n;

            // is this item an artifact?
            if (item->itemType == IT_ARTIFACT
                || item->itemType == IT_RUIN) // jjh
            {
                // artifact located on this planet?
                if (item->planetid == planetid) {
                    // due to the -1 repeat code, this will only print once
                    // followed by one or more objects in a list
                    ostringstream os;
                    os << "I'm detecting an odd energy signature on the "
                          "surface. ";

                    if (item->itemType == IT_ARTIFACT)
                        os << "It appears to be an Artifact.";
                    else
                        os << "It appears to be an ancient Ruin.";

                    if (g_game->getGlobalBoolean("DEBUG_OUTPUT") == true) {
                        // Reporting position of artifacts/ruins disabled
                        // because we don't want to give them away too easily!
                        // But this could be used in a future upgrade by giving
                        // the player a planet scanner device.  REACTIVATED
                        // UNDER DEBUG BY JJH
                        int lat = item->x;
                        os << "  " << abs(lat);
                        if (lat < 0)
                            os << "N";
                        else if (lat > 0)
                            os << "S";
                        int lng = item->y;
                        os << " X " << abs(lng);
                        if (lng < 0)
                            os << "W";
                        else if (lng > 0)
                            os << "E";
                    }
                    g_game->printout(OFFICER_SCIENCE, os.str(), YELLOW, 1000);

                    // stop searching items
                    break;
                }
            }
        }

        // reset analysis flag to break out of this block of code
        planetAnalysis = 0;
    }

    else {
        // normal orbit message
        g_game->printout(OFFICER_NAVIGATION, "Orbit established.", LTGREEN, -1);
        switch (planet->size) {
        case PS_SMALL:
            temp = "Captain, this planetoid is tiny. Shall I perform a "
                   "full sensor scan?";
            break;
        case PS_MEDIUM:
            temp = "Captain, this looks like an average planet. Want a "
                   "full sensor scan?";
            break;
        case PS_LARGE:
            temp = "Captain, we've got a large planet here. Sensors?";
            break;
        case PS_HUGE:
            temp = "It's huge! We have to scan it! Er, on your command, "
                   "of course.";
            break;
        default:
            temp = "Our sensors can't measure the planet's size for "
                   "some reason!";
            break;
        }
        g_game->printout(OFFICER_SCIENCE, temp, ORANGE, -1);
    }

    // planet scan?
    if (planetScan == 1) {
        int pw = al_get_bitmap_width(planet_topography) - 7;
        int ph = al_get_bitmap_height(planet_topography) - 7;

        // draw a bunch of sfrandom sensor blips
        for (int n = 0; n < 8; n++) {
            Rect r;
            r.left = sfrand() % pw;
            ;
            r.top = sfrand() % ph;
            r.right = r.left + 4;
            r.bottom = r.top + 4;
            ALLEGRO_COLOR color =
                al_map_rgb(100 + sfrand() % 155, 0, 100 + sfrand() % 155);
            al_draw_filled_rectangle(r.left, r.top, r.right, r.bottom, color);
        }

        g_game->audioSystem->Play(samples[S_AUDIO_SCAN]);

    } else {
        // restore original topography
        al_set_target_bitmap(planet_topography);
        al_draw_bitmap(planet_scanner_map, 0, 0, 0);
    }

    // trying to dock with starport
    if (flag_DoDock) {
        if (Util::ReentrantDelay(2000)) {
            g_game->LoadModule(MODULE_STARPORT);
            return false;
        }
    }

    return true;
}

bool
ModulePlanetOrbit::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);
    m_starfield->draw(target);

    // clear aux window
    static int asx = (int)g_game->getGlobalNumber("AUX_SCREEN_X");
    static int asy = (int)g_game->getGlobalNumber("AUX_SCREEN_Y");
    static int asw = (int)g_game->getGlobalNumber("AUX_SCREEN_WIDTH");
    static int ash = (int)g_game->getGlobalNumber("AUX_SCREEN_HEIGHT");
    al_draw_filled_rectangle(
        asx, asy, asx + asw, asy + ash, al_map_rgb(0, 0, 0));

    // draw topography map of planet in the aux window
    al_draw_bitmap(planet_topography, asx, asy, 0);

    // draw rotating planet as textured sphere
    int cx = SCREEN_WIDTH / 2;
    int cy = 250;
    planetRotation += planetRotationSpeed;
    planetRotation = fmod(planetRotation, 256);

    texsphere->Draw(target, 0, 0, (int)planetRotation, planetRadius, cx, cy);

    // draw planet lightmap overlay
    al_draw_bitmap(
        lightmap_overlay.get(), cx + lightmapOffsetX, cy + lightmapOffsetY, 0);

#ifdef DEBUGMODE
    int y = 0;
    g_game->PrintDefault(
        target, 850, y, "planetScan: " + Util::ToString(planetScan));
    y += 10;
    g_game->PrintDefault(
        target, 850, y, "planetAnalysis: " + Util::ToString(planetAnalysis));
#endif
    return true;
}

void
ModulePlanetOrbit::scanplanet() {
    if (planetScan != 1) {
        // begin scanning
        planetScan = 1;
        planetAnalysis = 0;

        // notify quest manager of planet scan event
        g_game->questMgr->raiseEvent(14, planet->id);
    }
}

void
ModulePlanetOrbit::analyzeplanet() {
    if (planetScan == 3) {
        planetScan = 0;
        planetAnalysis = 1;
    }
}
// vi: ft=cpp
