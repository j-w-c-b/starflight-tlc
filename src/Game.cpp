/*
        STARFLIGHT - THE LOST COLONY
        Game.cpp
*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_physfs.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <physfs.h>

#include <algorithm>
#include <cstdio>
#include <memory.h>
#include <sstream>

#include "AudioSystem.h"
#include "DataMgr.h"
#include "Game.h"
#include "GameState.h"
#include "MessageBoxWindow.h"
#include "ModeMgr.h"
#include "Module.h"
#include "PauseMenu.h"
#include "QuestMgr.h"
#include "Script.h"
#include "Util.h"

#include "ModuleAuxiliaryDisplay.h"
#include "ModuleBank.h"
#include "ModuleCantina.h"
#include "ModuleCaptainCreation.h"
#include "ModuleCaptainsLounge.h"
#include "ModuleCargoWindow.h"
#include "ModuleControlPanel.h"
#include "ModuleCredits.h"
#include "ModuleCrewHire.h"
#include "ModuleEncounter.h"
#include "ModuleEngineer.h"
#include "ModuleGameOver.h"
#include "ModuleInterplanetaryTravel.h"
#include "ModuleInterstellarTravel.h"
#include "ModuleMedical.h"
#include "ModuleMessageGUI.h"
#include "ModulePlanetOrbit.h"
#include "ModulePlanetSurface.h"
#include "ModuleQuestLog.h"
#include "ModuleSettings.h"
#include "ModuleShipConfig.h"
#include "ModuleStarmap.h"
#include "ModuleStarport.h"
#include "ModuleStartup.h"
#include "ModuleTitleScreen.h"
#include "ModuleTopGUI.h"
#include "ModuleTradeDepot.h"

#include "auxiliary_resources.h"
#include "bank_resources.h"
#include "cantina_resources.h"
#include "captaincreation_resources.h"
#include "captainslounge_resources.h"
#include "cargohold_resources.h"
#include "controlpanel_resources.h"
#include "credits_resources.h"
#include "crewhire_resources.h"
#include "encounter_resources.h"
#include "engineer_resources.h"
#include "gui_resources.h"
#include "medical_resources.h"
#include "messagegui_resources.h"
#include "pausemenu_resources.h"
#include "planetorbit_resources.h"
#include "planetsurface_resources.h"
#include "questviewer_resources.h"
#include "settings_resources.h"
#include "shipconfig_resources.h"
#include "spacetravel_resources.h"
#include "starmap_resources.h"
#include "starport_resources.h"
#include "startup_resources.h"
#include "titlescreen_resources.h"
#include "topgui_resources.h"
#include "tradedepot_resources.h"

using namespace std;

// define global objects for project-wide visibility, not dependent on Game
// class for access
GameState *Game::gameState = NULL;
ModeMgr *Game::modeMgr = NULL;
DataMgr *Game::dataMgr = NULL;
AudioSystem *Game::audioSystem = NULL;
QuestMgr *Game::questMgr = NULL;

ALLEGRO_DEBUG_CHANNEL("Game")

Game::Game() {
    timePause = true;    // start paused.
    timeRateDivisor = 2; //=> 2 seconds == 1 game hour.
                         //*************************************JJH
    CrossModuleAngle = 0;
    //*************************************JJH
    showControls = true;
    m_pause = false;
    m_keepRunning = true;
    m_backbuffer = NULL;
    m_display = NULL;
    desktop_width = 0;
    desktop_height = 0;
    frameCount = 0;
    startTime = 0;
    frameRate = 0;
    vibration = 0;
    globals = NULL;
    gameState = NULL;
    modeMgr = NULL;
    questMgr = NULL;
    audioSystem = NULL;
    dataMgr = NULL;
    cursor = NULL;

    font10 = nullptr;
    font12 = nullptr;
    font18 = nullptr;
    font20 = nullptr;
    font22 = nullptr;
    font24 = nullptr;
    font32 = nullptr;
    font48 = nullptr;
    font60 = nullptr;

    MsgColors[MSG_INFO] = GREEN;
    MsgColors[MSG_ALERT] = RED;
    MsgColors[MSG_ERROR] = YELLOW;
    MsgColors[MSG_ACK] = LTGREEN;
    MsgColors[MSG_FAILURE] = LTRED;
    MsgColors[MSG_SUCCESS] = LTGREEN;
    MsgColors[MSG_TASK_COMPLETED] = BLUE;
    MsgColors[MSG_SKILLUP] = PURPLE;

    m_fps_timer = NULL;
    m_event_queue = NULL;
}

void
Game::message(const std::string &msg) {
    cout << msg << endl;
}

bool
Game::add_resources() {
    int rc;
    rc = PHYSFS_mount(Util::resource_path(auxiliary::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(bank::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(cantina::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(
        Util::resource_path(captaincreation::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc =
        PHYSFS_mount(Util::resource_path(captainslounge::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(cargohold::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(controlpanel::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(credits::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(crewhire::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(encounter::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(engineer::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(gui::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(medical::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(messagegui::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(pausemenu::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(planetorbit::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(planetsurface::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(questviewer::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(settings::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(shipconfig::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(spacetravel::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(starmap::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(starport::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(startup::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(titlescreen::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(topgui::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }
    rc = PHYSFS_mount(Util::resource_path(tradedepot::DAT).c_str(), NULL, 1);
    if (rc == 0) {
        return false;
    }

    images.add(auxiliary::IMAGES);
    images.add(bank::IMAGES);
    images.add(cantina::IMAGES);
    images.add(captaincreation::IMAGES);
    images.add(captainslounge::IMAGES);
    images.add(cargohold::IMAGES);
    images.add(controlpanel::IMAGES);
    images.add(credits::IMAGES);
    images.add(crewhire::IMAGES);
    images.add(encounter::IMAGES);
    images.add(engineer::IMAGES);
    images.add(gui::IMAGES);
    images.add(medical::IMAGES);
    images.add(messagegui::IMAGES);
    images.add(pausemenu::IMAGES);
    images.add(planetorbit::IMAGES);
    images.add(planetsurface::IMAGES);
    images.add(questviewer::IMAGES);
    images.add(settings::IMAGES);
    images.add(shipconfig::IMAGES);
    images.add(spacetravel::IMAGES);
    images.add(starmap::IMAGES);
    images.add(starport::IMAGES);
    images.add(startup::IMAGES);
    images.add(titlescreen::IMAGES);
    images.add(topgui::IMAGES);
    images.add(tradedepot::IMAGES);

    samples.add(auxiliary::SAMPLES);
    samples.add(bank::SAMPLES);
    samples.add(cantina::SAMPLES);
    samples.add(captaincreation::SAMPLES);
    samples.add(captainslounge::SAMPLES);
    samples.add(cargohold::SAMPLES);
    samples.add(controlpanel::SAMPLES);
    samples.add(credits::SAMPLES);
    samples.add(crewhire::SAMPLES);
    samples.add(encounter::SAMPLES);
    samples.add(engineer::SAMPLES);
    samples.add(gui::SAMPLES);
    samples.add(medical::SAMPLES);
    samples.add(messagegui::SAMPLES);
    samples.add(pausemenu::SAMPLES);
    samples.add(planetorbit::SAMPLES);
    samples.add(planetsurface::SAMPLES);
    samples.add(questviewer::SAMPLES);
    samples.add(settings::SAMPLES);
    samples.add(shipconfig::SAMPLES);
    samples.add(spacetravel::SAMPLES);
    samples.add(starmap::SAMPLES);
    samples.add(starport::SAMPLES);
    samples.add(startup::SAMPLES);
    samples.add(titlescreen::SAMPLES);
    samples.add(topgui::SAMPLES);
    samples.add(tradedepot::SAMPLES);

    return true;
}

void
Game::fatalerror(const std::string &error) {
    ALLEGRO_DEBUG("%s\n", error.c_str());
    message(error);
    if (g_game->modeMgr) {
        g_game->modeMgr->CloseCurrentModule();
    }
    Stop();
}

void
Game::shutdown() {
    ALLEGRO_DEBUG("[shutting down]");
    g_game->modeMgr->CloseCurrentModule();
    Stop();
}

void
Game::SetTimePaused(bool val) {
    if (timePause == val)
        return; // do not do anything for redundant call.
    timePause = val;
    if (timePause) // update base with current data:
        gameState->setBaseGameTimeSecs(gameState->getGameTimeSecs());
    globalTimer.reset(); // reset timer.
}

void
Game::TogglePauseMenu() {
    modeMgr->toggle_pause_menu();
}

// these three are identical to Script class methods but are more convenient
// within g_game
std::string
Game::getGlobalString(const std::string &name) {
    return globals->getGlobalString(name);
}
void
Game::setGlobalString(const std::string &name, const std::string &value) {
    globals->setGlobalString(name, value);
}

double
Game::getGlobalNumber(const std::string &name) {
    return globals->getGlobalNumber(name);
}
void
Game::setGlobalNumber(const std::string &name, double value) {
    globals->setGlobalNumber(name, value);
}
bool
Game::getGlobalBoolean(const std::string &name) {
    return globals->getGlobalBoolean(name);
}
void
Game::setGlobalBoolean(const std::string &name, bool value) {
    globals->setGlobalBoolean(name, value);
}

// this bogus callback is used for script verification
int
voidfunc(lua_State * /*L*/) {
    return 0;
}

bool
ValidateScripts(const string &p_title) {
    const int PLANETFUNCS = 103;
    string planet_funcnames[PLANETFUNCS] = {
        "L_Debug",
        "L_LoadImage",
        "L_Move",
        "L_Draw",
        "L_Animate",
        "L_LaunchTV",
        "L_SetActions",
        "L_LoadScript",
        "L_PostMessage",
        "L_LoadPortrait",
        "L_ShowPortrait",
        "L_AddItemtoCargo",
        "L_DeleteSelf",
        "L_LoadPSObyID",
        "L_CreateNewPSO",
        "L_CreateNewPSObyItemID",
        "L_LoadPlayerTVasPSO",
        "L_LoadPlayerShipasPSO",
        "L_Test",
        "L_SetRunPlanetLoadScripts",
        "L_GetRunPlanetLoadScripts",
        "L_SetRunPlanetPopulate",
        "L_GetRunPlanetPopulate",
        "L_CheckInventorySpace",
        "L_KilledAnimal",
        "L_AttackTV",
        "L_TVDestroyed",
        "L_TVOutofFuel",
        "L_PlayerTVisAlive",
        "L_CheckInventoryFor",
        "L_RemoveItemFromInventory",
        "L_GetPlanetID",
        "L_CreateTimer",
        "L_GetPlayerShipPosition",
        "L_GetPlayerTVPosition",
        "L_GetActiveVesselPosition",
        "L_GetScrollerPosition",
        "L_GetPlayerProfession",
        "L_GetPosition",
        "L_GetOffsetPosition",
        "L_GetScreenWidth",
        "L_GetScreenHeight",
        "L_GetScreenDim",
        "L_GetSpeed",
        "L_GetFaceAngle",
        "L_GetPlayerNavVars",
        "L_GetScale",
        "L_GetCounters",
        "L_GetThresholds",
        "L_IsPlayerMoving",
        "L_GetItemID",
        "L_GetState",
        "L_GetVesselMode",
        "L_IsScanned",
        "L_GetName",
        "L_GetValue",
        "L_GetDamage",
        "L_IsBlackMarketItem",
        "L_IsShipRepairMetal",
        "L_IsAlive",
        "L_GetColHalfWidth",
        "L_GetColHalfHeight",
        "L_GetID",
        "L_GetScriptName",
        "L_GetHealth",
        "L_GetMaxHealth",
        "L_GetStunCount",
        "L_GetItemSize",
        "L_GetSelectedPSOid",
        "L_GetObjectType",
        "L_GetDanger",
        "L_GetMinimapColor",
        "L_GetMinimapSize",
        "L_SetPosition",
        "L_SetVelocity",
        "L_SetSpeed",
        "L_SetFaceAngle",
        "L_SetAnimInfo",
        "L_SetAngleOffset",
        "L_SetScale",
        "L_SetCounters",
        "L_SetThresholds",
        "L_SetState",
        "L_SetVesselMode",
        "L_SetScanned",
        "L_SetDamage",
        "L_SetAlive",
        "L_SetColHalfWidth",
        "L_SetColHalfHeight",
        "L_SetScriptName",
        "L_SetAlpha",
        "L_SetHealth",
        "L_SetMaxHealth",
        "L_SetStunCount",
        "L_SetObjectType",
        "L_SetName",
        "L_SetMinimapColor",
        "L_SetMinimapSize",
        "L_SetNewAnimation",
        "L_SetActiveAnimation",
        "L_PlaySound",
        "L_PlayLoopingSound",
        "L_StopSound",
    };

    const int PLANETNUM = 12;
    string planetScripts[PLANETNUM] = {
        "data/planetsurface/Functions.lua",
        "data/planetsurface/stunprojectile.lua",
        "data/planetsurface/mineral.lua",
        "data/planetsurface/basicLifeForm.lua",
        "data/planetsurface/PlanetSurfacePlayerShip.lua",
        "data/planetsurface/PlanetSurfacePlayerTV.lua",
        "data/planetsurface/PopAsteriod.lua",
        "data/planetsurface/PopFrozenPlanet.lua",
        "data/planetsurface/PopMoltenPlanet.lua",
        "data/planetsurface/PopOceanicPlanet.lua",
        "data/planetsurface/PopRockyPlanet.lua",
        "data/planetsurface/artifact.lua",
    };

    const int ENCFUNCS = 3;
    string enc_funcnames[ENCFUNCS] = {
        "L_Debug",
        "L_Terminate",
        "L_Attack",
    };

    const int ENCNUM = 12;
    string encounterScripts[ENCNUM] = {
        "data/globals.lua",
        "data/quests.lua",
        "data/encounter/encounter_common.lua",
        "data/encounter/encounter_pirate.lua",
        "data/encounter/encounter_spemin.lua",
        "data/encounter/encounter_tafel.lua",
        "data/encounter/encounter_thrynn.lua",
        "data/encounter/encounter_nyssian.lua",
        "data/encounter/encounter_minex.lua",
        "data/encounter/encounter_elowan.lua",
        "data/encounter/encounter_coalition.lua",
        "data/encounter/encounter_barzhon.lua"};

    string error, filename, linenum, message;
    int pos, n;

    // validate global and encounter scripts
    for (n = 0; n < ENCNUM; n++) {
        Script scr;

        // register all required C++ functions needed by encounter scripts
        for (int f = 0; f < ENCFUNCS; f++)
            lua_register(scr.getState(), enc_funcnames[f].c_str(), voidfunc);

        if (!scr.load(encounterScripts[n])) {
            error = scr.errorMessage;
            pos = (int)error.find(":");
            filename = error.substr(0, pos);
            error = error.substr(pos + 1);
            pos = (int)error.find(":");
            linenum = error.substr(0, pos);
            message = error.substr(pos + 1);
            error = "Filename: " + Util::resource_path(encounterScripts[n])
                    + "\n\nLine #: " + linenum + "\n\nError: " + filename + "\n"
                    + message;
            ALLEGRO_DEBUG("%s\n", error.c_str());
            al_show_native_message_box(
                nullptr,
                p_title.c_str(),
                "Script Error",
                error.c_str(),
                nullptr,
                ALLEGRO_MESSAGEBOX_ERROR);
            return false;
        }
    }

    // validate planet surface scripts
    for (n = 0; n < PLANETNUM; n++) {
        Script planetScript;

        // register all required C++ functions needed by planet scripts
        for (int f = 0; f < PLANETFUNCS; f++)
            lua_register(
                planetScript.getState(), planet_funcnames[f].c_str(), voidfunc);

        // feed the scripts fake planet info
        planetScript.setGlobalString("PLANETSIZE", "SMALL");
        planetScript.setGlobalString("TEMPERATURE", "SUBARCTIC");
        planetScript.setGlobalString("GRAVITY", "NEGLIGIBLE");
        planetScript.setGlobalString("ATMOSPHERE", "NONE");

        // open the planet script
        if (!planetScript.load(planetScripts[n])) {
            error = planetScript.errorMessage;
            pos = (int)error.find(":");
            filename = error.substr(0, pos);
            error = error.substr(pos + 1);
            pos = (int)error.find(":");
            linenum = error.substr(0, pos);
            message = error.substr(pos + 1);
            error = "Filename: " + planetScripts[n] + "\n\nLine #: " + linenum
                    + "\n\nError: " + filename + "\n" + message;
            ALLEGRO_DEBUG("%s\n", error.c_str());
            al_show_native_message_box(
                nullptr,
                p_title.c_str(),
                "Script Error",
                error.c_str(),
                nullptr,
                ALLEGRO_MESSAGEBOX_ERROR);
            return false;
        }
    }
    return true;
}
/*
   verify that the portraits for all the items in the database do exist + log
   the missing ones in allegro.log return true if all files where found, false
   if there was at least one file missing.
*/

bool
ValidatePortraits() {
    bool retval = true;
    bool doCheck = true; // to skip minerals -- we don't do portraits for
                         // minerals right now

    // TODO: right now ValidatePortraits() skip lifeforms portraits too, since
    // it
    //  proved itself to be much more work than expected. we need to handle the
    //  various data/planetsurface/Pop${planetType}Planet.lua for it to work.
    for (auto i = g_game->dataMgr->items_begin(),
              e = g_game->dataMgr->items_end();
         i != e;
         ++i) {
        auto item = *i;
        std::string filepath;

        switch (item->itemType) {
        case IT_INVALID:
            {
                // this one is not supposed to ever happen
                ALLEGRO_DEBUG(
                    "[ERROR]: item #%d is of invalid type\n", item->id);
                ALLEGRO_ASSERT(0);
            }
        case IT_ARTIFACT:
            {
                doCheck = true;
                filepath = "data/tradedepot/" + item->portrait;
                break;
            }
        case IT_RUIN:
            {
                doCheck = true;
                filepath = "data/planetsurface/" + item->portrait;
                break;
            }
        case IT_MINERAL:
            // minerals do not have portraits
            doCheck = false;
            break;
        case IT_LIFEFORM:
            {
                doCheck = false;
                break;
            }
        case IT_TRADEITEM:
            {
                doCheck = true;
                filepath = "data/tradedepot/" + item->portrait;
                break;
            }
        default:
            {
                // not supposed to happen either
                ALLEGRO_DEBUG(
                    "[ERROR]: item #%d is of unknown type\n", item->id);
                ALLEGRO_ASSERT(0);
                break;
            }
        }

        if (doCheck) {
            ALLEGRO_FS_ENTRY *fs_entry = al_create_fs_entry(filepath.c_str());
            if (!al_fs_entry_exists(fs_entry)) {
                ALLEGRO_DEBUG(
                    "[WARNING]: portrait %s for item #%d does not exist\n",
                    filepath.c_str(),
                    item->id);
                retval = false;
            }
            al_destroy_fs_entry(fs_entry);
        }
    }

    return retval;
}

void
Game::Initialize() {
    ALLEGRO_DEBUG("Firing up Allegro...\n");
    if (!al_init()) {
        fatalerror("Error during game initialization\n");
        return;
    }
    ALLEGRO_DEBUG("Enabling physfs");
    if (!PHYSFS_init("StarflightTLC")) {
        g_game->fatalerror("Error initializing physfs\n");
        return;
    }
    ALLEGRO_DEBUG("Mounting data");
    {
        auto data_dir = Util::resource_path("data");
        if (!PHYSFS_mount(data_dir.c_str(), "/data", 1)) {
            g_game->fatalerror(string("Error mounting ") + data_dir + "\n");
            return;
        }

        ALLEGRO_PATH *user_data_path =
            al_get_standard_path(ALLEGRO_USER_DATA_PATH);
        const char *user_data_name =
            al_path_cstr(user_data_path, ALLEGRO_NATIVE_PATH_SEP);
        if (!al_filename_exists(user_data_name)) {
            if (!al_make_directory(user_data_name)) {
                g_game->fatalerror(
                    string("Error creating ") + user_data_name + "\n");
                return;
            }
        }
        if (!PHYSFS_mount(user_data_name, NULL, 1)) {
            g_game->fatalerror(
                string("Error mounting user path ") + user_data_name + "\n");
            return;
        }
        if (!PHYSFS_setWriteDir(
                al_path_cstr(user_data_path, ALLEGRO_NATIVE_PATH_SEP))) {
            g_game->fatalerror(
                string("Error allowing writes to user data path ")
                + al_path_cstr(user_data_path, ALLEGRO_NATIVE_PATH_SEP) + "\n");
            return;
        }
        al_destroy_path(user_data_path);
        al_set_physfs_file_interface();
    }

    // initialize scripting and load globals.lua
    ALLEGRO_DEBUG("Loading startup script...\n");
    globals = new Script();
    if (!globals->load("data/globals.lua")) {
        fatalerror("Error finding data/globals.lua");
        return;
    }

    ALLEGRO_DEBUG("Initializing game...\n");
    if (!InitGame()) {
        fatalerror("Error during game initialization\n");
        return;
    }

    // validate scripts
    ALLEGRO_DEBUG("Validating Lua scripts...\n");
    if (!ValidateScripts(p_title)) {
        return;
    }

    ALLEGRO_DEBUG("Firing up game state...\n");
    gameState = new GameState();

    ALLEGRO_DEBUG("Firing up mode manager...\n");
    modeMgr = new ModeMgr(this);

    ALLEGRO_DEBUG("Firing up data manager...\n");
    dataMgr = new DataMgr();
    if (!dataMgr->Initialize()) {
        fatalerror("Error initializing data manager");
    }
    // check that all the graphics for items are present
    if (!ValidatePortraits())
        fatalerror("Some graphics where missing, see allegro.log for details");

    // initialize gamestate
    gameState->Reset();

    ALLEGRO_DEBUG("Firing up quest manager...\n");
    questMgr = new QuestMgr();
    if (!questMgr->Initialize()) {
        fatalerror("Error initializing quest manager");
    }

    ALLEGRO_DEBUG("Initializing modules...\n");
    InitializeModules();
    gameState->m_captainSelected = false;

    // editing this out since starting up in different modules causes problems
    // instead a module jump will be added as a debug feature in the starport

    std::string startupmodule;
    startupmodule = globals->getGlobalString("STARTUPMODULE");
    if (startupmodule.length() == 0)
        startupmodule = MODULE_STARTUP;
    ALLEGRO_DEBUG("\nLaunching Startup Module: %s\n", startupmodule.c_str());
    modeMgr->LoadModule(startupmodule);

    // set window caption with title, version
    std::ostringstream s;
    s << p_title << " (V" << p_version << ")";
    al_set_window_title(m_display, s.str().c_str());
}

void
Game::Run() {
    Initialize();

    ALLEGRO_DEBUG("\nLaunching game loop...\n");
    while (m_keepRunning) {
        RunGame();
    }

    ALLEGRO_DEBUG("\nBailing...\n");
    DestroyGame();
}

void
Game::Stop() {
    m_keepRunning = false;
}

/*
 * Initialize Allegro graphics callable from settings screen to reset mode as
 * needed.
 */
bool
Game::Initialize_Graphics() {
    int requested_width, requested_height;

    // since this func can be called repeatedly, let's skip redundancies
    if (desktop_width == 0) {
        ALLEGRO_MONITOR_INFO info;
        int num_adapters = al_get_num_video_adapters();
        for (int i = 0; i < num_adapters; i++) {
            if (al_get_monitor_info(i, &info)) {
                if (info.x1 == 0 && info.y1 == 0) {
                    desktop_width = info.x2;
                    desktop_height = info.y2;
                }
            }
        }
    } else
        ALLEGRO_DEBUG("Attempting to reset graphics mode...\n");

    bool fullscreen = g_game->getGlobalBoolean("FULLSCREEN");

    // try to get user-selected resolution chosen in the settings screen
    string resolution = g_game->getGlobalString("RESOLUTION");
    if (resolution == "" || fullscreen) {
        requested_width = desktop_width;
        requested_height = desktop_height;
        g_game->setGlobalString(
            "RESOLUTION",
            Util::ToString(actual_width) + "x" + Util::ToString(actual_height));
    } else {
        std::size_t div = resolution.find_first_of("xX,");
        if (div != string::npos) {
            string ws = resolution.substr(0, div);
            string hs = resolution.substr(div + 1);
            requested_width = Util::StringToInt(ws);
            requested_height = Util::StringToInt(hs);
            if (requested_width < 1024)
                requested_width = 1024;
            if (requested_height < 768)
                requested_height = 768;
        } else {
            requested_width = desktop_width;
            requested_height = desktop_height;
        }
    }
    ALLEGRO_DEBUG(
        "Settings resolution: %d,%d\n", requested_width, requested_height);

    // try to get user-selected fullscreen toggle from settings screen
    int flags = fullscreen ? (ALLEGRO_FULLSCREEN_WINDOW)
                           : (ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);

    if (m_display) {
        al_destroy_display(m_display);
        m_display = nullptr;
    }
    al_set_new_display_flags(flags);
    m_display = al_create_display(requested_width, requested_height);
    if (!m_display) {
        ALLEGRO_DEBUG(
            "Video mode failed (%s), attempting default mode...\n",
            resolution.c_str());
        requested_width = SCREEN_WIDTH;
        requested_height = SCREEN_HEIGHT;

        if ((m_display = al_create_display(requested_width, requested_height))
            == nullptr) {
            ALLEGRO_DEBUG("Fatal Error: Unable to set graphics mode\n");
            return false;
        }
    }
    m_video_mode = {.width = requested_width, .height = requested_height};

    ResizeDisplay(requested_width, requested_height);

    /*
     * Retrieve complete list of resolutions supported by the display driver and
     * populate the global Game::VideoModes list for use in the Settings screen.
     * Minimum is 1024x768 since downscaling does not work properly--most UI
     * code is based on SCREEN_WIDTH/SCREEN_HEIGHT assumptions, such as the
     * mouse.
     */
    if (videomodes.empty()) {
        ALLEGRO_DISPLAY_MODE mode;
        int num_modes = al_get_num_display_modes();

        for (int i = 0; i < num_modes; i++) {
            if (al_get_display_mode(i, &mode)) {
                VideoMode vmode;
                vmode.width = mode.width;
                vmode.height = mode.height;
                if (vmode.width >= SCREEN_WIDTH
                    && vmode.height >= SCREEN_HEIGHT) {
                    videomodes.push_back(vmode);
                }
            }
        }
        videomodes.sort([](const VideoMode &first, const VideoMode &second) {
            return (first.width < second.width && first.height < second.height);
        });
    }

    return true;
}

bool
Game::ResizeDisplay(int x, int y) {
    float scalex, scaley;

    if (x < 1024) {
        x = 1024;
    }
    if (y < 768) {
        y = 1024;
    }
    scalex = float(x) / SCREEN_WIDTH;
    scaley = float(y) / SCREEN_HEIGHT;

    if (scalex > scaley) {
        m_backbuffer_scale = scaley;
        m_backbuffer_x_offset = int((x - SCREEN_WIDTH * scaley) / 2.0);
        m_backbuffer_y_offset = 0;
    } else {
        m_backbuffer_scale = scalex;
        m_backbuffer_x_offset = 0;
        m_backbuffer_y_offset = int((y - SCREEN_HEIGHT * scalex) / 2.0);
    }

    ALLEGRO_DEBUG(
        "Creating back buffer (%d,%d)...\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!m_backbuffer) {
        m_backbuffer = al_create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    if (!m_backbuffer) {
        ALLEGRO_DEBUG("Error creating back buffer\n");
        return false;
    }
    return true;
}

void
Game::LoadModule(const string &mod) {
    ALLEGRO_EVENT event;
    event.user.type = EVENT_CHANGE_MODULE;
    event.user.data2 = reinterpret_cast<intptr_t>(new string(mod));
    al_emit_user_event(&m_user_event_source, &event, [](ALLEGRO_USER_EVENT *e) {
        string *s = reinterpret_cast<string *>(e->data2);
        delete s;
    });
}

void
Game::broadcast_event(ALLEGRO_EVENT *event) {
    if (event->type == EVENT_NONE) {
        return;
    }

    al_emit_user_event(&m_user_event_source, event, nullptr);
}

void
Game::enable_pause_menu(bool enable) {
    modeMgr->enable_pause_menu(enable);
}

/*
 * INITIALIZE LOW-LEVEL LIBRARY AND ENGINE RESOURCES
 */
bool
Game::InitGame() {
    Util::Init();

    // get title and version from script
    p_title = getGlobalString("GAME_TITLE");
    p_version = getGlobalString("GAME_VERSION");
    ALLEGRO_DEBUG("%s v%s\n", p_title.c_str(), p_version.c_str());

    ALLEGRO_DEBUG("Firing up Alfont...\n");
    if (!al_init_font_addon()) {
        g_game->message("Error initializing font system");
        return false;
    }
    ALLEGRO_DEBUG("Firing up Alttf...\n");
    if (!al_init_ttf_addon()) {
        g_game->message("Error initializing ttf loader");
        return false;
    }
    ALLEGRO_DEBUG("Firing up primitives...\n");
    if (!al_init_primitives_addon()) {
        g_game->message("Error initializing primitives");
        return false;
    }
    ALLEGRO_DEBUG("Firing up graphics system...\n");
    if (!Initialize_Graphics()) {
        g_game->fatalerror("Error initializing graphics\n");
        return false;
    }
    if (!al_init_image_addon()) {
        g_game->fatalerror("Error initializing image_addon\n");
        return false;
    }
    ALLEGRO_DEBUG("Firing up keyboard and mouse handlers...\n");
    if (!al_install_keyboard()) {
        g_game->message("Error initializing keyboard\n");
        return false;
    }
    if (!al_install_mouse()) {
        g_game->message("Error initializing mouse\n");
        return false;
    }
    int num_buttons = al_get_mouse_num_buttons();
    for (int i = 0; i < num_buttons; i++) {
        m_last_button_downs.push_back(make_pair(-1, -1));
    }

    ALLEGRO_DEBUG("Firing up sound system...\n");
    audioSystem = new AudioSystem();
    if (!audioSystem->Init()) {
        g_game->message("Error initializing the sound system\n");
        return false;
    }
    // Create event queue
    m_event_queue = al_create_event_queue();

    // Add event listeners
    m_fps_timer = al_create_timer(1.0 / 60.0);
    al_start_timer(m_fps_timer);
    ALLEGRO_EVENT_SOURCE *es = al_get_timer_event_source(m_fps_timer);
    al_register_event_source(m_event_queue, es);

    es = al_get_display_event_source(m_display);
    al_register_event_source(m_event_queue, es);

    es = al_get_keyboard_event_source();
    al_register_event_source(m_event_queue, es);

    es = al_get_mouse_event_source();
    al_register_event_source(m_event_queue, es);

    al_init_user_event_source(&m_user_event_source);
    al_register_event_source(m_event_queue, &m_user_event_source);

    // mount the resource dat files and add mappings to images and samples
    add_resources();

    // load up default fonts
    string fontfile = "data/gui/Xolonium-Regular.ttf";
    ALLEGRO_DEBUG("Creating default fonts...\n");
    font10 = shared_ptr<ALLEGRO_FONT>(
        al_load_font(fontfile.c_str(), 10, 0), al_destroy_font);
    if (!font10) {
        g_game->message("Error locating font file\n");
        return false;
    }
    font12 = shared_ptr<ALLEGRO_FONT>(
        al_load_font(fontfile.c_str(), 12, 0), al_destroy_font);
    font18 = shared_ptr<ALLEGRO_FONT>(
        al_load_font(fontfile.c_str(), 18, 0), al_destroy_font);
    font20 = shared_ptr<ALLEGRO_FONT>(
        al_load_font(fontfile.c_str(), 20, 0), al_destroy_font);
    font22 = shared_ptr<ALLEGRO_FONT>(
        al_load_font(fontfile.c_str(), 22, 0), al_destroy_font);
    font24 = shared_ptr<ALLEGRO_FONT>(
        al_load_font(fontfile.c_str(), 24, 0), al_destroy_font);
    font32 = shared_ptr<ALLEGRO_FONT>(
        al_load_font(fontfile.c_str(), 32, 0), al_destroy_font);
    font48 = shared_ptr<ALLEGRO_FONT>(
        al_load_font(fontfile.c_str(), 48, 0), al_destroy_font);
    font60 = shared_ptr<ALLEGRO_FONT>(
        al_load_font(fontfile.c_str(), 60, 0), al_destroy_font);

    // hide the default mouse cursor
    al_hide_mouse_cursor(m_display);

    // warp to center
    al_set_mouse_xy(m_display, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    ALLEGRO_DEBUG("Initialization succeeded\n");
    return true;
}

void
Game::DestroyGame() {
    ALLEGRO_DEBUG("*** DestroyGame\n");

    if (cursor != NULL) {
        delete cursor;
        cursor = NULL;
    }
    if (globals != NULL) {
        delete globals;
        globals = NULL;
    }
    if (modeMgr != NULL) {
        delete modeMgr;
        modeMgr = NULL;
    }

    if (gameState != NULL) {
        delete gameState;
        gameState = NULL;
    }

    if (dataMgr != NULL) {
        delete dataMgr;
        dataMgr = NULL;
    }

    if (questMgr != NULL) {
        delete questMgr;
        questMgr = NULL;
    }

    if (audioSystem != NULL) {
        delete audioSystem;
        audioSystem = NULL;
    }

    if (m_backbuffer != NULL) {
        al_show_mouse_cursor(m_display);
        al_destroy_bitmap(m_backbuffer);
        m_backbuffer = NULL;
    }

    ALLEGRO_DEBUG("\nShutdown completed.\n");

    /* Don't uninstall system until the modules using the ResourceManager
     * are destroyed
     */
    /*
    al_uninstall_system();
    */
}

void
Game::UpdateAlienRaceAttitudes() {
    // update alien attitudes (200,000 is 3 1/3 minute, changing from attitude 1
    // to 10 will require 30 minutes)
    if (m_pause) {
        return;
    }
    int mins = 1;
    if (globalTimer.getTimer()
        > g_game->gameState->alienAttitudeUpdate + 200000 * mins) {
        g_game->gameState->alienAttitudeUpdate = globalTimer.getTimer();

        // update alien attitudes, all but for NONE(0) and PIRATE(1)
        for (int n = 2; n < NUM_ALIEN_RACES; n++) {
            if (g_game->gameState->alienAttitudes[n] < 25)
                g_game->gameState->alienAttitudes[n]++;
            else if (g_game->gameState->alienAttitudes[n] > 65)
                g_game->gameState->alienAttitudes[n]--;
        }
    }
}

void
Game::RunGame() {
    static int v;
    bool need_redraw = false;
    bool need_resize = false;
    int resize_x = -1;
    int resize_y = -1;
    bool change_module = false;
    string new_module;
    ALLEGRO_EVENT event;
    bool hide_cursor = false;
    bool show_cursor = false;

    do {
        al_wait_for_event(m_event_queue, &event);

        /* handle input events until our timer clicks */
        switch (event.type) {
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            need_resize = true;
            resize_x = event.display.width;
            resize_y = event.display.height;
            break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            m_keepRunning = false;
            return;
        case ALLEGRO_EVENT_TIMER:
            need_redraw = true;
            if (!m_pause) {
                modeMgr->update();
                UpdateAlienRaceAttitudes();
            }
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            modeMgr->key_down(&event.keyboard);
            break;
        case ALLEGRO_EVENT_KEY_CHAR:
            modeMgr->key_pressed(&event.keyboard);
            break;
        case ALLEGRO_EVENT_KEY_UP:
            modeMgr->key_up(&event.keyboard);
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            modeMgr->mouse_move(&event.mouse);
            break;
        case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
        case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
            hide_cursor = true;
            show_cursor = false;
            break;
        case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
        case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
            hide_cursor = false;
            show_cursor = true;
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            modeMgr->mouse_button_down(&event.mouse);
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            modeMgr->mouse_button_up(&event.mouse);
            break;
        case EVENT_CHANGE_MODULE:
            need_redraw = true;
            change_module = true;
            new_module = *reinterpret_cast<string *>(event.user.data2);
            al_flush_event_queue(m_event_queue);
            break;
        default:
            if (ALLEGRO_EVENT_TYPE_IS_USER(event.type)) {
                modeMgr->event(&event);
                al_unref_user_event(&event.user);
            }
        }
    } while (!(need_redraw && al_is_event_queue_empty(m_event_queue)));

    if (change_module) {
        if (!showControls) {
            toggleShowControls();
        }
        modeMgr->LoadModule(new_module);
    }
    if (hide_cursor) {
        al_hide_mouse_cursor(m_display);
    } else if (show_cursor) {
        al_show_mouse_cursor(m_display);
    }

    if (need_resize) {
        if (ResizeDisplay(resize_x, resize_y)) {
            al_acknowledge_resize(m_display);
        }
    }

    al_set_target_bitmap(m_backbuffer);
    al_clear_to_color(BLACK);

    if (!timePause) { // Update the current stardate:
        // base game time is needed to properly restore the date from a savegame
        // file
        double newTime = gameState->getBaseGameTimeSecs()
                         + (double)globalTimer.getStartTimeMillis() / 1000.0;
        gameState->setGameTimeSecs(newTime);
        gameState->stardate.Update(newTime, timeRateDivisor);
    }

    if (!m_pause) {
        // global abort flag to end game
        if (!m_keepRunning)
            return;
    }

    // tell active module to draw
    modeMgr->draw(m_backbuffer);

    // draw mouse everywhere but during startup
    if (g_game->gameState->getCurrentModule() != "STARTUP") {
        if (cursor != NULL) {
            ALLEGRO_MOUSE_STATE state;
            al_get_mouse_state(&state);
            cursor->setX(state.x);
            cursor->setY(state.y);
            cursor->draw(m_backbuffer);
        } else {
            // load the custom mouse cursor
            cursor = new Sprite();
            if (!cursor->setImage(images[gui::I_CURSOR])) {
                g_game->message("Error loading mouse cursor");
                g_game->shutdown();
                return;
            }
        }
    }

    // vibrate the screen if needed (occurs near a star or flux)
    if (vibration) {
        v = Util::Random(-vibration / 2, vibration / 2);
    } else {
        v = 0;
    }

    ALLEGRO_BITMAP *display_buffer = al_get_backbuffer(m_display);

    al_set_target_bitmap(display_buffer);
    if (m_backbuffer_x_offset || m_backbuffer_y_offset) {
        /* Letterbox */
        al_clear_to_color(BLACK);
    }

    al_draw_scaled_bitmap(
        m_backbuffer,
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        m_backbuffer_x_offset + v,
        m_backbuffer_y_offset,
        SCREEN_WIDTH * m_backbuffer_scale - v,
        SCREEN_HEIGHT * m_backbuffer_scale - v,
        0);

    al_flip_display();
}

void
Game::toggleShowControls() {
    showControls = !showControls;
    m_message_gui->set_active(showControls);
    m_auxiliary_display->set_active(showControls);
    ALLEGRO_EVENT e = {
        .type = static_cast<unsigned int>(
            showControls ? EVENT_SHOW_CONTROLS : EVENT_HIDE_CONTROLS)};
    broadcast_event(&e);
}

bool
Game::InitializeModules() {
    bool result = true;

    // STARTUP MODE
    modeMgr->AddMode(
        MODULE_STARTUP,
        make_shared<ModuleStartup>(),
        "data/startup/Starflight.ogg");

    // TITLE SCREEN GAME MODE
    modeMgr->AddMode(
        MODULE_TITLESCREEN,
        make_shared<ModuleTitleScreen>(),
        "data/startup/Starflight.ogg");

    // CREDITS GAME MODE
    modeMgr->AddMode(
        MODULE_CREDITS,
        make_shared<ModuleCredits>(),
        "data/credits/credits.ogg");

    // STARPORT MODE
    modeMgr->AddMode(
        MODULE_STARPORT,
        make_shared<ModuleStarport>(),
        "data/starport/starport.ogg");

    // CAPTAIN CREATION MODE
    modeMgr->AddMode(
        MODULE_CAPTAINCREATION,
        make_shared<ModuleCaptainCreation>(),
        "data/startup/Starflight.ogg");

    // CAPTAIN'S LOUNGE MODE
    modeMgr->AddMode(
        MODULE_CAPTAINSLOUNGE,
        make_shared<ModuleCaptainsLounge>(),
        "data/starport/starport.ogg");

    // SHIPCONFIG GAME MODE
    modeMgr->AddMode(
        MODULE_SHIPCONFIG,
        make_shared<ModuleShipConfig>(),
        "data/starport/starport.ogg");

    // Cargo Hold (shared with all ship-based modes)
    auto cargo_hold = make_shared<ModuleCargoWindow>();
    auto auxiliary_display = make_shared<ModuleAuxiliaryDisplay>();
    auto control_panel = make_shared<ModuleControlPanel>();
    auto starmap = make_shared<ModuleStarmap>();
    auto top_gui = make_shared<ModuleTopGUI>();
    auto quest_log = make_shared<ModuleQuestLog>();
    auto medical = make_shared<ModuleMedical>();
    auto engineering = make_shared<ModuleEngineer>();
    auto message_gui = make_shared<ModuleMessageGUI>();
    m_message_gui = message_gui;
    m_auxiliary_display = auxiliary_display;

    // INTERSTELLAR (HYPERSPACE) TRAVEL GAME MODE
    auto mode_hyperspace = make_shared<ModuleInterstellarTravel>();
    mode_hyperspace->add_child_module(auxiliary_display);
    mode_hyperspace->add_child_module(control_panel);
    mode_hyperspace->add_child_module(starmap);
    mode_hyperspace->add_child_module(top_gui);
    mode_hyperspace->add_child_module(quest_log);
    mode_hyperspace->add_child_module(medical);
    mode_hyperspace->add_child_module(engineering);
    mode_hyperspace->add_child_module(cargo_hold);
    mode_hyperspace->add_child_module(message_gui);
    modeMgr->AddMode(
        MODULE_HYPERSPACE, mode_hyperspace, "data/spacetravel/spacetravel.ogg");

    // INTERPLANETARY TRAVEL GAME MODE
    auto mode_interplanetaryTravel = make_shared<ModuleInterPlanetaryTravel>();
    mode_interplanetaryTravel->add_child_module(auxiliary_display);
    mode_interplanetaryTravel->add_child_module(control_panel);
    mode_interplanetaryTravel->add_child_module(starmap);
    mode_interplanetaryTravel->add_child_module(top_gui);
    mode_interplanetaryTravel->add_child_module(quest_log);
    mode_interplanetaryTravel->add_child_module(medical);
    mode_interplanetaryTravel->add_child_module(engineering);
    mode_interplanetaryTravel->add_child_module(cargo_hold);
    mode_interplanetaryTravel->add_child_module(message_gui);
    modeMgr->AddMode(
        MODULE_INTERPLANETARY,
        mode_interplanetaryTravel,
        "data/spacetravel/spacetravel.ogg");

    // PLANET ORBIT GAME MODE
    auto mode_orbit = make_shared<ModulePlanetOrbit>();
    mode_orbit->add_child_module(auxiliary_display);
    mode_orbit->add_child_module(control_panel);
    mode_orbit->add_child_module(starmap);
    mode_orbit->add_child_module(top_gui);
    mode_orbit->add_child_module(quest_log);
    mode_orbit->add_child_module(medical);
    mode_orbit->add_child_module(engineering);
    mode_orbit->add_child_module(cargo_hold);
    mode_orbit->add_child_module(message_gui);
    modeMgr->AddMode(
        MODULE_ORBIT, mode_orbit, "data/spacetravel/spacetravel.ogg");

    // PLANET SURFACE MODE
    auto mode_planet = make_shared<ModulePlanetSurface>();
    mode_planet->add_child_module(cargo_hold);
    mode_planet->add_child_module(message_gui);
    modeMgr->AddMode(
        MODULE_SURFACE, mode_planet, "data/planetsurface/planetsurface.ogg");

    // CREW ASSIGNMENT GAME MODE
    modeMgr->AddMode(
        MODULE_CREWBUY,
        make_shared<ModuleCrewHire>(),
        "data/starport/starport.ogg");

    // BANK MODULE
    modeMgr->AddMode(
        MODULE_BANK, make_shared<ModuleBank>(), "data/starport/starport.ogg");

    // TRADE DEPOT MODULE
    g_game->modeMgr->AddMode(
        MODULE_TRADEDEPOT,
        make_shared<ModuleTradeDepot>(),
        "data/starport/starport.ogg");

    // GAME OVER MODULE
    g_game->modeMgr->AddMode(
        MODULE_GAMEOVER, make_shared<ModuleGameOver>(), "");

    // CANTINA MODULE
    auto cantina = make_shared<ModuleCantina>();
    g_game->modeMgr->AddMode(
        MODULE_CANTINA, cantina, "data/starport/starport.ogg");
    g_game->modeMgr->AddMode(
        MODULE_RESEARCHLAB, cantina, "data/starport/starport.ogg");
    g_game->modeMgr->AddMode(
        MODULE_MILITARYOPS, cantina, "data/starport/starport.ogg");

    // ALIEN ENCOUNTER MODULE
    auto mode_encounter = make_shared<ModuleEncounter>();
    mode_encounter->add_child_module(top_gui);
    mode_encounter->add_child_module(control_panel);
    mode_encounter->add_child_module(cargo_hold);
    mode_encounter->add_child_module(message_gui);
    g_game->modeMgr->AddMode(
        MODULE_ENCOUNTER, mode_encounter, "data/encounter/combat.ogg");

    // SETTINGS GAME MODE
    modeMgr->AddMode(
        MODULE_SETTINGS,
        make_shared<ModuleSettings>(),
        "data/startup/Starflight.ogg");

    return result;
}

void
Game::PrintDefault(
    ALLEGRO_BITMAP *dest,
    int x,
    int y,
    const std::string &text,
    ALLEGRO_COLOR color) {
    al_set_target_bitmap(dest);
    al_draw_text(font12.get(), color, x, y, 0, text.c_str());
}

void
Game::Print(
    ALLEGRO_BITMAP *dest,
    shared_ptr<ALLEGRO_FONT> _font,
    int x,
    int y,
    const std::string &text,
    ALLEGRO_COLOR color,
    bool shadow) {
    al_set_target_bitmap(dest);
    if (shadow) {
        al_draw_text(_font.get(), BLACK, x + 2, y + 2, 0, text.c_str());
    }
    al_draw_text(_font.get(), color, x, y, 0, text.c_str());
}

void
Game::Print12(
    ALLEGRO_BITMAP *dest,
    int x,
    int y,
    const std::string &text,
    ALLEGRO_COLOR color,
    bool shadow) {
    Print(dest, font12, x, y, text, color, shadow);
}

void
Game::Print18(
    ALLEGRO_BITMAP *dest,
    int x,
    int y,
    const std::string &text,
    ALLEGRO_COLOR color,
    bool shadow) {
    Print(dest, font18, x, y, text, color, shadow);
}
void
Game::Print20(
    ALLEGRO_BITMAP *dest,
    int x,
    int y,
    const std::string &text,
    ALLEGRO_COLOR color,
    bool shadow) {
    Print(dest, font20, x, y, text, color, shadow);
}
void
Game::Print22(
    ALLEGRO_BITMAP *dest,
    int x,
    int y,
    const std::string &text,
    ALLEGRO_COLOR color,
    bool shadow) {
    Print(dest, font22, x, y, text, color, shadow);
}
void
Game::Print24(
    ALLEGRO_BITMAP *dest,
    int x,
    int y,
    const std::string &text,
    ALLEGRO_COLOR color,
    bool shadow) {
    Print(dest, font24, x, y, text, color, shadow);
}
void
Game::Print32(
    ALLEGRO_BITMAP *dest,
    int x,
    int y,
    const std::string &text,
    ALLEGRO_COLOR color,
    bool shadow) {
    Print(dest, font32, x, y, text, color, shadow);
}

/**
This function tracks all printed messages, each containing a timestamp to
prevent messages from printing out repeatedly, which occurs frequently in
state-based timed sections of code that is called repeatedly, where we don't
want messages printing repeatedly. Delay of -1 causes message to print only once
(until ScrollBox is cleared). Default delay of 0 forces printout.
 **/
void
Game::clear_printout() {
    if (m_message_gui) {
        m_message_gui->clear_printout();
    }
}

void
Game::printout(const string &str, ALLEGRO_COLOR color, long delay) {
    if (m_message_gui) {
        m_message_gui->printout(str, color, delay);
    }
}

void
Game::printout(
    OfficerType type,
    const string &str,
    ALLEGRO_COLOR color,
    long delay) {
    if (m_message_gui) {
        auto officer = gameState->get_effective_officer(type);
        m_message_gui->printout(
            officer->get_last_name() + ": " + str, color, delay);
    }
}
