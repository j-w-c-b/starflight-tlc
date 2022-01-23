/*
        STARFLIGHT - THE LOST COLONY
        Game.cpp
*/

#pragma region HEADER

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

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

using namespace std;

// define global objects for project-wide visibility, not dependent on Game
// class for access
GameState *Game::gameState = NULL;
ModeMgr *Game::modeMgr = NULL;
DataMgr *Game::dataMgr = NULL;
AudioSystem *Game::audioSystem = NULL;
QuestMgr *Game::questMgr = NULL;

#pragma endregion
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
    messageBox = NULL;
    gameState = NULL;
    modeMgr = NULL;
    questMgr = NULL;
    audioSystem = NULL;
    dataMgr = NULL;
    pauseMenu = NULL;
    cursor = NULL;

    font10 = NULL;
    font12 = NULL;
    font18 = NULL;
    font20 = NULL;
    font22 = NULL;
    font24 = NULL;
    font32 = NULL;
    font48 = NULL;
    font60 = NULL;

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

Game::~Game() {
    // destroy the ScrollBox queue
    messages.clear();

    // destroy the video modes list
    videomodes.clear();

    // kill the pause menu
    if (pauseMenu != NULL)
        delete pauseMenu;

    // destroy fonts
    al_destroy_font(font10);
    al_destroy_font(font12);
    al_destroy_font(font18);
    al_destroy_font(font20);
    al_destroy_font(font22);
    al_destroy_font(font24);
    al_destroy_font(font32);
    al_destroy_font(font48);
    al_destroy_font(font60);
}

#pragma region UTILITY_FUNCS

void
Game::message(const std::string &msg) {
    cout << msg << endl;
}

void
Game::fatalerror(const std::string &error) {
    ALLEGRO_DEBUG("%s\n", error.c_str());
    message(error);
    g_game->modeMgr->CloseCurrentModule();
    this->Stop();
}

void
Game::shutdown() {
    ALLEGRO_DEBUG("[shutting down]");
    g_game->modeMgr->CloseCurrentModule();
    Stop();
}

void
Game::ShowMessageBoxWindow(const std::string &initHeading,
                           const std::string &initText,
                           int initWidth,
                           int initHeight,
                           ALLEGRO_COLOR initTextColor,
                           int initX,
                           int initY,
                           bool initCentered,
                           bool /*pauseGame*/) {
    m_pause = true;

    // if we have a msg box window, delete it
    KillMessageBoxWindow();

    messageBox = new MessageBoxWindow(initHeading,
                                      initText,
                                      initX,
                                      initY,
                                      initWidth,
                                      initHeight,
                                      initTextColor,
                                      initCentered);
}

void
Game::KillMessageBoxWindow() {
    if (messageBox != NULL) {
        m_pause = false;
        delete messageBox;
        messageBox = NULL;
    }
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
    static bool origTimePause;
    if (!pauseMenu->isEnabled())
        return;

    if (pauseMenu->isShowing()) {
        SetTimePaused(origTimePause);
        // hide pausemenu
        m_pause = false;
        pauseMenu->setShowing(false);
    } else {
        origTimePause = getTimePaused();
        SetTimePaused(true);
        // show pausemenu
        m_pause = true;
        pauseMenu->setShowing(true);
    }
}

#pragma endregion

#pragma region "Lua script validation and globals"

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
    string encounterScripts[ENCNUM] = {"data/globals.lua",
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
    Script *scr;
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
            error = "Filename: " + Util::resource_path(encounterScripts[n]) +
                    "\n\nLine #: " + linenum + "\n\nError: " + filename + "\n" +
                    message;
            ALLEGRO_DEBUG("%s\n", error.c_str());
            al_show_native_message_box(nullptr,
                                       p_title.c_str(),
                                       "Script Error",
                                       error.c_str(),
                                       nullptr,
                                       ALLEGRO_MESSAGEBOX_ERROR);
            return false;
        }
    }

    // validate planet surface scripts
    Script *planetScript;

    // validate all script files
    for (n = 0; n < PLANETNUM; n++) {
        planetScript = new Script();

        // register all required C++ functions needed by planet scripts
        for (int f = 0; f < PLANETFUNCS; f++)
            lua_register(planetScript->getState(),
                         planet_funcnames[f].c_str(),
                         voidfunc);

        // feed the scripts fake planet info
        planetScript->setGlobalString("PLANETSIZE", "SMALL");
        planetScript->setGlobalString("TEMPERATURE", "SUBARCTIC");
        planetScript->setGlobalString("GRAVITY", "NEGLIGIBLE");
        planetScript->setGlobalString("ATMOSPHERE", "NONE");

        // open the planet script
        if (!planetScript->load(planetScripts[n])) {
            error = planetScript->errorMessage;
            pos = (int)error.find(":");
            filename = error.substr(0, pos);
            error = error.substr(pos + 1);
            pos = (int)error.find(":");
            linenum = error.substr(0, pos);
            message = error.substr(pos + 1);
            error = "Filename: " + planetScripts[n] + "\n\nLine #: " + linenum +
                    "\n\nError: " + filename + "\n" + message;
            ALLEGRO_DEBUG("%s\n", error.c_str());
            al_show_native_message_box(nullptr,
                                       p_title.c_str(),
                                       "Script Error",
                                       error.c_str(),
                                       nullptr,
                                       ALLEGRO_MESSAGEBOX_ERROR);
            delete planetScript;
            return false;
        }
        delete planetScript;
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
    for (int n = 0; n < g_game->dataMgr->GetNumItems(); n++) {
        Item *item = g_game->dataMgr->GetItem(n);
        std::string filepath;

        switch (item->itemType) {
        case IT_INVALID: {
            // this one is not supposed to ever happen
            ALLEGRO_DEBUG("[ERROR]: item #%d is of invalid type\n", item->id);
            ALLEGRO_ASSERT(0);
        }
        case IT_ARTIFACT: {
            doCheck = true;
            filepath = Util::resource_path("data/tradedepot/" + item->portrait);
            break;
        }
        case IT_RUIN: {
            doCheck = true;
            filepath =
                Util::resource_path("data/planetsurface/" + item->portrait);
            break;
        }
        case IT_MINERAL:
            // minerals do not have portraits
            doCheck = false;
            break;
        case IT_LIFEFORM: {
            doCheck = false;
            break;
        }
        case IT_TRADEITEM: {
            doCheck = true;
            filepath = Util::resource_path("data/tradedepot/" + item->portrait);
            break;
        }
        default: {
            // not supposed to happen either
            ALLEGRO_DEBUG("[ERROR]: item #%d is of unknown type\n", item->id);
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

#pragma endregion

void
Game::Run() {
    ALLEGRO_DEBUG("Firing up Allegro...\n");
    if (!al_init()) {
        fatalerror("Error during game initialization\n");
        return;
    }

    // validate scripts
    ALLEGRO_DEBUG("Validating Lua scripts...\n");
    if (!ValidateScripts(p_title)) {
        return;
    }

    // initialize scripting and load globals.lua
    ALLEGRO_DEBUG("Loading startup script...\n");
    globals = new Script();
    globals->load("data/globals.lua");

    ALLEGRO_DEBUG("Initializing game...\n");
    if (!InitGame()) {
        fatalerror("Error during game initialization\n");
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
 * Initizlie Allegro graphics callable from settings screen to reset mode as
 * needed.
 */
bool
Game::Initialize_Graphics() {
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

    // try to get user-selected resolution chosen in the settings screen
    string resolution = g_game->getGlobalString("RESOLUTION");
    if (resolution == "") {
        actual_width = desktop_width;
        actual_height = desktop_height;
        g_game->setGlobalString("RESOLUTION",
                                Util::ToString(actual_width) + "x" +
                                    Util::ToString(actual_height));
    } else {
        std::size_t div = resolution.find_first_of("xX,");
        if (div != string::npos) {
            string ws = resolution.substr(0, div);
            string hs = resolution.substr(div + 1);
            actual_width = Util::StringToInt(ws);
            actual_height = Util::StringToInt(hs);
            if (actual_width < 1024)
                actual_width = 1024;
            if (actual_height < 768)
                actual_height = 768;
        } else {
            actual_width = desktop_width;
            actual_height = desktop_height;
        }
    }
    ALLEGRO_DEBUG("Settings resolution: %d,%d\n", actual_width, actual_height);

    // try to get user-selected fullscreen toggle from settings screen
    bool fullscreen = g_game->getGlobalBoolean("FULLSCREEN");
    int flags = fullscreen ? (ALLEGRO_FULLSCREEN)
                           : (ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    al_set_new_display_refresh_rate(60);

    if (!m_display) {
        al_set_new_display_flags(flags);
        m_display = al_create_display(actual_width, actual_height);
        if (!m_display) {
            ALLEGRO_DEBUG(
                "Video mode failed (%s), attempting default mode...\n",
                resolution.c_str());
            actual_width = SCREEN_WIDTH;
            actual_height = SCREEN_HEIGHT;

            if ((m_display = al_create_display(actual_width, actual_height)) ==
                nullptr) {
                ALLEGRO_DEBUG("Fatal Error: Unable to set graphics mode\n");
                return false;
            }
        }
    }
    ResizeDisplay(actual_width, actual_height);

    ALLEGRO_DEBUG("Refresh rate: %d\n", al_get_display_refresh_rate(m_display));

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
                if (vmode.width >= SCREEN_WIDTH &&
                    vmode.height >= SCREEN_HEIGHT) {
                    videomodes.push_back(vmode);
                }
            }
        }
        videomodes.sort([](const VideoMode &first, const VideoMode &second) {
            return (first.width < second.width && first.height < second.height);
        });

        ALLEGRO_DEBUG("Detected video modes:\n");
        for (auto mode = videomodes.begin(); mode != videomodes.end(); ++mode) {
            ALLEGRO_DEBUG("%d,%d\n", mode->width, mode->height);
        }
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

    if (x != actual_width || y != actual_height) {
        al_resize_display(m_display, x, y);
        actual_width = x;
        actual_height = y;
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
    event.user.data1 = reinterpret_cast<intptr_t>(new string(mod));
    al_emit_user_event(&m_user_event_source, &event, [](ALLEGRO_USER_EVENT *e) {
        string *s = reinterpret_cast<string *>(e->data1);
        delete s;
    });
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

    // load up default fonts
    string fontfile = Util::resource_path("data/gui/Xolonium-Regular.ttf");
    ALLEGRO_DEBUG("Creating default fonts...\n");
    font10 = al_load_font(fontfile.c_str(), 10, 0);
    if (font10 == NULL) {
        g_game->message("Error locating font file\n");
        return false;
    }
    font12 = al_load_font(fontfile.c_str(), 12, 0);
    font18 = al_load_font(fontfile.c_str(), 18, 0);
    font20 = al_load_font(fontfile.c_str(), 20, 0);
    font22 = al_load_font(fontfile.c_str(), 22, 0);
    font24 = al_load_font(fontfile.c_str(), 24, 0);
    font32 = al_load_font(fontfile.c_str(), 32, 0);
    font48 = al_load_font(fontfile.c_str(), 48, 0);
    font60 = al_load_font(fontfile.c_str(), 60, 0);

    // create the PauseMenu
    pauseMenu = new PauseMenu();

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
    int mins = 1;
    if (globalTimer.getTimer() >
        g_game->gameState->alienAttitudeUpdate + 200000 * mins) {
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

/*
WARNING!! The core loop used to have separate update/render timing but Allegro
was consuming 100% of a CPU core no matter what we did with the timing code so
the only option is to use a SINGLE update timed at 60 fps. Don't change it!

OpenGL has been REMOVED from the project.
*/
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
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            OnKeyPress(event.keyboard.keycode);
            break;
        case ALLEGRO_EVENT_KEY_CHAR:
            OnKeyPressed(event.keyboard.keycode);
            break;
        case ALLEGRO_EVENT_KEY_UP:
            OnKeyReleased(event.keyboard.keycode);
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            OnMouseMove(event.mouse.x, event.mouse.y);
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
            m_last_button_downs[event.mouse.button - 1] =
                make_pair(event.mouse.x, event.mouse.y);
            OnMousePressed(
                event.mouse.button - 1, event.mouse.x, event.mouse.y);
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            OnMouseReleased(
                event.mouse.button - 1, event.mouse.x, event.mouse.y);
            {
                pair<int, int> &last_down =
                    m_last_button_downs[event.mouse.button - 1];
                int last_x = last_down.first;
                int last_y = last_down.second;
                if (last_x == event.mouse.x && last_y == event.mouse.y) {
                    OnMouseClick(
                        event.mouse.button - 1, event.mouse.x, event.mouse.y);
                }
                last_down.first = -1;
                last_down.second = -1;
            }
            break;
        case EVENT_CHANGE_MODULE:
            need_redraw = true;
            change_module = true;
            new_module = *reinterpret_cast<string *>(event.user.data1);
            al_flush_event_queue(m_event_queue);
            break;
        }
    } while (!(need_redraw && al_is_event_queue_empty(m_event_queue)));

    if (change_module) {
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
        double newTime = gameState->getBaseGameTimeSecs() +
                         (double)globalTimer.getStartTimeMillis() / 1000.0;
        gameState->setGameTimeSecs(newTime);
        gameState->stardate.Update(newTime, timeRateDivisor);
    }

    if (!m_pause) {
        // call update on all modules
        modeMgr->Update();

        // global abort flag to end game
        if (!m_keepRunning)
            return;

        // tell active module to draw
        modeMgr->Draw();

        // perform generic updates to time-sensitive game data
        UpdateAlienRaceAttitudes();

    } // mpause

    // handle the pause menu
    if (pauseMenu->isShowing()) {
        // tell active module to draw
        modeMgr->Draw();

        // draw the pause popup
        pauseMenu->Draw();
    }

    // handle the messagebox
    if (messageBox != NULL) {
        if (messageBox->IsVisible()) {
            // tell active module to draw
            modeMgr->Draw();

            // draw messagebox
            messageBox->Draw();
        } else {
            KillMessageBoxWindow();
        }
    }

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
            if (!cursor->load("data/gui/cursor.tga")) {
                g_game->message("Error loading mouse cursor");
                g_game->shutdown();
                return;
            }
        }
    }

    // display debug info on the upper-left corner of screen
    // if (g_game->getGlobalBoolean("DEBUG_OUTPUT") == true)
    {
        ALLEGRO_COLOR GRAY = al_map_rgb(160, 160, 160);
        int y = 3;
        int x = 3;
        // x == 0 doesn't quite work on the Trade Depot Screen - made it a 3 -
        // jjh
        y += 10;
        g_game->PrintDefault(m_backbuffer,
                             x,
                             y,
                             "Screen: " + Util::ToString((int)scale_width) +
                                 "," + Util::ToString((int)scale_height) +
                                 " (" + Util::ToString(screen_scaling) + "x)",
                             GRAY);
        y += 10;
        g_game->PrintDefault(
            m_backbuffer,
            x,
            y,
            "Quest: " + Util::ToString(g_game->gameState->getActiveQuest()) +
                " (" + Util::ToString(g_game->gameState->getQuestCompleted()) +
                ")",
            GREEN);
        y += 10;
        g_game->PrintDefault(
            m_backbuffer,
            x,
            y,
            "Stage: " + Util::ToString(g_game->gameState->getPlotStage()),
            GREEN);
        y += 10;
        g_game->PrintDefault(
            m_backbuffer,
            x,
            y,
            "Date: " + Util::ToString(gameState->stardate.GetFullDateString()),
            GRAY);
        y += 10;
        g_game->PrintDefault(m_backbuffer,
                             x,
                             y,
                             "Prof: " +
                                 g_game->gameState->getProfessionString(),
                             GRAY);
        y += 10;
        g_game->PrintDefault(
            m_backbuffer,
            x,
            y,
            "Fuel: " + Util::ToString(g_game->gameState->getShip().getFuel()),
            GRAY);
        y += 10;
        g_game->PrintDefault(
            m_backbuffer,
            x,
            y,
            "Cred: " + Util::ToString(g_game->gameState->getCredits()),
            GRAY);
        y += 10;
        g_game->PrintDefault(
            m_backbuffer,
            x,
            y,
            "Cargo: " +
                Util::ToString(g_game->gameState->m_ship.getOccupiedSpace()) +
                "/" + Util::ToString(g_game->gameState->m_ship.getTotalSpace()),
            GRAY);
        y += 10;
        g_game->PrintDefault(
            m_backbuffer,
            x,
            y,
            "HyperPos: " +
                Util::ToString(
                    g_game->gameState->getHyperspaceCoordinates().x) +
                "," +
                Util::ToString(g_game->gameState->getHyperspaceCoordinates().y),
            GRAY);
        y += 10;
        g_game->PrintDefault(
            m_backbuffer,
            x,
            y,
            "SystemPos: " +
                Util::ToString(g_game->gameState->getSystemCoordinates().x) +
                "," +
                Util::ToString(g_game->gameState->getSystemCoordinates().y),
            GRAY);
        // Print out the aliens' attitude toward us:
        /*   PrintDefault(m_backbuffer,0,y,"Attitudes");
           for (int n=1; n<NUM_ALIEN_RACES; n++)
           {
                   y+=10;
                   PrintDefault(m_backbuffer,0,y,
           gameState->player->getAlienRaceName(n) + " : "
                           + Util::ToString( gameState->alienAttitudes[n] ));
           }*/
    }

    // vibrate the screen if needed (occurs near a star or flux)
    if (vibration)
        v = Util::Random(-vibration / 2, vibration / 2);
    else
        v = 0;

    ALLEGRO_BITMAP *display_buffer = al_get_backbuffer(m_display);

    al_set_target_bitmap(display_buffer);
    if (m_backbuffer_x_offset || m_backbuffer_y_offset) {
        /* Letterbox */
        al_clear_to_color(BLACK);
    }

    al_draw_scaled_bitmap(m_backbuffer,
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

#pragma region "UI events"

void
Game::OnKeyPress(int keyCode) {
    // send keypress event to messagebox
    if (messageBox != NULL) {
        // this stops buttons from under the messagebox from being clicked
        if (messageBox->OnKeyPress(keyCode))
            return;
    }

    if (!m_pause) {
        modeMgr->OnKeyPress(keyCode);
    }
}

void
Game::OnKeyPressed(int keyCode) {
    if (!m_pause) {
        modeMgr->OnKeyPressed(keyCode);
    }
}

void
Game::OnKeyReleased(int keyCode) {
    if (keyCode == ALLEGRO_KEY_ESCAPE)
        TogglePauseMenu();

    if (!m_pause) {
        modeMgr->OnKeyReleased(keyCode);
    }
}

void
Game::toggleShowControls() {
    showControls = !showControls;
    Event e(showControls ? EVENT_SHOW_CONTROLS : EVENT_HIDE_CONTROLS);
    modeMgr->BroadcastEvent(&e);
}

void
Game::OnMouseMove(int x, int y) {
    // send mouse event to pause menu
    if (pauseMenu->isShowing()) {
        if (pauseMenu->OnMouseMove(x, y))
            return;
    }

    // send mouse event to messagebox
    if (messageBox != NULL) {
        // this stops buttons from under the messagebox from being clicked
        if (messageBox->OnMouseMove(x, y))
            return;
    }

    if (!m_pause) {
        modeMgr->OnMouseMove(x, y);
    }
}

void
Game::OnMouseClick(int button, int x, int y) {
    // send mouse event to messagebox
    if (messageBox != NULL) {
        // this stops buttons from under the messagebox from being clicked
        if (messageBox->OnMouseClick(button, x, y))
            return;
    }
    if (!m_pause) {
        modeMgr->OnMouseClick(button, x, y);
    }
}

void
Game::OnMousePressed(int button, int x, int y) {
    // send mouse event to messagebox
    if (messageBox != NULL) {
        // this stops buttons from under the messagebox from being clicked
        if (messageBox->OnMousePressed(button, x, y))
            return;
    }
    if (!m_pause) {
        modeMgr->OnMousePressed(button, x, y);
    }
}

void
Game::OnMouseReleased(int button, int x, int y) {
    // send mouse event to pause menu
    if (pauseMenu->isShowing()) {
        if (pauseMenu->OnMouseReleased(button, x, y))
            return;
    }

    // send mouse event to messagebox
    if (messageBox != NULL) {
        // this stops buttons from under the messagebox from being clicked
        if (messageBox->OnMouseReleased(button, x, y))
            return;
    }
    if (!m_pause) {
        modeMgr->OnMouseReleased(button, x, y);
    }
}

void
Game::OnMouseWheelUp(int x, int y) {
    if (!m_pause) {
        modeMgr->OnMouseWheelUp(x, y);
    }
}

void
Game::OnMouseWheelDown(int x, int y) {
    if (!m_pause) {
        modeMgr->OnMouseWheelDown(x, y);
    }
}

#pragma endregion

bool
Game::InitializeModules() {
    bool result = true;

    // STARTUP MODE
    Module *mode_startup = new Module;
    Module *startup = new ModuleStartup;
    mode_startup->AddChildModule(startup);
    modeMgr->AddMode(
        MODULE_STARTUP, mode_startup, "data/startup/Starflight.ogg");

    // TITLE SCREEN GAME MODE
    Module *mode_titleScreen = new Module;
    Module *titleScreen = new ModuleTitleScreen;
    mode_titleScreen->AddChildModule(titleScreen);
    modeMgr->AddMode(
        MODULE_TITLESCREEN, mode_titleScreen, "data/startup/Starflight.ogg");

    // CREDITS GAME MODE
    Module *mode_Credits = new Module;
    Module *credits = new ModuleCredits;
    mode_Credits->AddChildModule(credits);
    modeMgr->AddMode(MODULE_CREDITS, mode_Credits, "data/credits/credits.ogg");

    // STARPORT MODE
    Module *mode_starport = new Module;
    Module *starport = new ModuleStarport;
    mode_starport->AddChildModule(starport);
    modeMgr->AddMode(
        MODULE_STARPORT, mode_starport, "data/starport/starport.ogg");

    // CAPTAIN CREATION MODE
    Module *mode_captainCreation = new Module;
    Module *captainCreation = new ModuleCaptainCreation;
    mode_captainCreation->AddChildModule(captainCreation);
    modeMgr->AddMode(MODULE_CAPTAINCREATION,
                     mode_captainCreation,
                     "data/startup/Starflight.ogg");

    // CAPTAIN'S LOUNGE MODE
    Module *mode_captainsLounge = new Module;
    Module *captainsLounge = new ModuleCaptainsLounge;
    mode_captainsLounge->AddChildModule(captainsLounge);
    modeMgr->AddMode(MODULE_CAPTAINSLOUNGE,
                     mode_captainsLounge,
                     "data/starport/starport.ogg");

    // SHIPCONFIG GAME MODE
    Module *mode_Shipconfig = new Module;
    Module *shipconfig = new ModuleShipConfig;
    mode_Shipconfig->AddChildModule(shipconfig);
    modeMgr->AddMode(
        MODULE_SHIPCONFIG, mode_Shipconfig, "data/starport/starport.ogg");

    // INTERSTELLAR (HYPERSPACE) TRAVEL GAME MODE
    Module *mode_hyperspace = new Module();
    mode_hyperspace->AddChildModule(new ModuleInterstellarTravel);
    mode_hyperspace->AddChildModule(new ModuleAuxiliaryDisplay);
    mode_hyperspace->AddChildModule(new ModuleControlPanel);
    mode_hyperspace->AddChildModule(new ModuleStarmap);
    mode_hyperspace->AddChildModule(new ModuleTopGUI);
    mode_hyperspace->AddChildModule(new ModuleQuestLog);
    mode_hyperspace->AddChildModule(new ModuleMedical);
    mode_hyperspace->AddChildModule(new ModuleEngineer);
    mode_hyperspace->AddChildModule(new ModuleCargoWindow);
    mode_hyperspace->AddChildModule(new ModuleMessageGUI);
    modeMgr->AddMode(
        MODULE_HYPERSPACE, mode_hyperspace, "data/spacetravel/spacetravel.ogg");

    // INTERPLANETARY TRAVEL GAME MODE
    Module *mode_interplanetaryTravel = new Module();
    mode_interplanetaryTravel->AddChildModule(new ModuleInterPlanetaryTravel);
    mode_interplanetaryTravel->AddChildModule(new ModuleAuxiliaryDisplay);
    mode_interplanetaryTravel->AddChildModule(new ModuleControlPanel);
    mode_interplanetaryTravel->AddChildModule(new ModuleStarmap);
    mode_interplanetaryTravel->AddChildModule(new ModuleTopGUI);
    mode_interplanetaryTravel->AddChildModule(new ModuleQuestLog);
    mode_interplanetaryTravel->AddChildModule(new ModuleMedical);
    mode_interplanetaryTravel->AddChildModule(new ModuleEngineer);
    mode_interplanetaryTravel->AddChildModule(new ModuleCargoWindow);
    mode_interplanetaryTravel->AddChildModule(new ModuleMessageGUI);
    modeMgr->AddMode(MODULE_INTERPLANETARY,
                     mode_interplanetaryTravel,
                     "data/spacetravel/spacetravel.ogg");

    // PLANET ORBIT GAME MODE
    // due to OpenGL rendering, 2D overlays will not work here
    Module *mode_orbit = new Module();
    mode_orbit->AddChildModule(new ModulePlanetOrbit);
    mode_orbit->AddChildModule(new ModuleAuxiliaryDisplay);
    mode_orbit->AddChildModule(new ModuleControlPanel);
    mode_orbit->AddChildModule(new ModuleMessageGUI);
    modeMgr->AddMode(
        MODULE_ORBIT, mode_orbit, "data/spacetravel/spacetravel.ogg");

    // PLANET SURFACE MODE
    Module *mode_planet = new Module();
    mode_planet->AddChildModule(new ModulePlanetSurface);
    mode_planet->AddChildModule(new ModuleCargoWindow);
    modeMgr->AddMode(
        MODULE_SURFACE, mode_planet, "data/planetsurface/planetsurface.ogg");

    // CREW ASSIGNMENT GAME MODE
    Module *mode_crew = new Module;
    Module *crewAssignment = new ModuleCrewHire();
    mode_crew->AddChildModule(crewAssignment);
    modeMgr->AddMode(MODULE_CREWBUY, mode_crew, "data/starport/starport.ogg");

    // BANK MODULE
    Module *mode_bank = new Module;
    Module *bank = new ModuleBank();
    mode_bank->AddChildModule(bank);
    modeMgr->AddMode(MODULE_BANK, mode_bank, "data/starport/starport.ogg");

    // TRADE DEPOT MODULE
    Module *mode_tradedepot = new Module;
    Module *tradedepot = new ModuleTradeDepot;
    mode_tradedepot->AddChildModule(tradedepot);
    g_game->modeMgr->AddMode(
        MODULE_TRADEDEPOT, mode_tradedepot, "data/starport/starport.ogg");

    // GAME OVER MODULE
    Module *mode_gameover = new Module;
    Module *gameOver = new ModuleGameOver;
    mode_gameover->AddChildModule(gameOver);
    g_game->modeMgr->AddMode(MODULE_GAMEOVER, mode_gameover, "");

    // CANTINA MODULE
    Module *mode_cantina = new Module;
    Module *cantina = new ModuleCantina;
    mode_cantina->AddChildModule(cantina);
    g_game->modeMgr->AddMode(
        MODULE_CANTINA, mode_cantina, "data/starport/starport.ogg");
    g_game->modeMgr->AddMode(
        MODULE_RESEARCHLAB, mode_cantina, "data/starport/starport.ogg");
    g_game->modeMgr->AddMode(
        MODULE_MILITARYOPS, mode_cantina, "data/starport/starport.ogg");

    // ALIEN ENCOUNTER MODULE
    Module *mode_encounter = new Module;
    mode_encounter->AddChildModule(new ModuleEncounter);
    mode_encounter->AddChildModule(new ModuleTopGUI);
    mode_encounter->AddChildModule(new ModuleControlPanel);
    mode_encounter->AddChildModule(new ModuleCargoWindow);
    g_game->modeMgr->AddMode(
        MODULE_ENCOUNTER, mode_encounter, "data/encounter/combat.ogg");

    // SETTINGS GAME MODE
    Module *mode_Settings = new Module;
    Module *settings = new ModuleSettings;
    mode_Settings->AddChildModule(settings);
    modeMgr->AddMode(
        MODULE_SETTINGS, mode_Settings, "data/startup/Starflight.ogg");

    return result;
}

#pragma region "Text output"

void
Game::PrintDefault(ALLEGRO_BITMAP *dest,
                   int x,
                   int y,
                   const std::string &text,
                   ALLEGRO_COLOR color) {
    al_set_target_bitmap(dest);
    al_draw_text(font12, color, x, y, 0, text.c_str());
}

void
Game::Print(ALLEGRO_BITMAP *dest,
            ALLEGRO_FONT *_font,
            int x,
            int y,
            const std::string &text,
            ALLEGRO_COLOR color,
            bool shadow) {
    al_set_target_bitmap(dest);
    if (shadow) {
        al_draw_text(_font, BLACK, x + 2, y + 2, 0, text.c_str());
    }
    al_draw_text(_font, color, x, y, 0, text.c_str());
}

void
Game::Print12(ALLEGRO_BITMAP *dest,
              int x,
              int y,
              const std::string &text,
              ALLEGRO_COLOR color,
              bool shadow) {
    Print(dest, font12, x, y, text, color, shadow);
}

void
Game::Print18(ALLEGRO_BITMAP *dest,
              int x,
              int y,
              const std::string &text,
              ALLEGRO_COLOR color,
              bool shadow) {
    Print(dest, font18, x, y, text, color, shadow);
}
void
Game::Print20(ALLEGRO_BITMAP *dest,
              int x,
              int y,
              const std::string &text,
              ALLEGRO_COLOR color,
              bool shadow) {
    Print(dest, font20, x, y, text, color, shadow);
}
void
Game::Print22(ALLEGRO_BITMAP *dest,
              int x,
              int y,
              const std::string &text,
              ALLEGRO_COLOR color,
              bool shadow) {
    Print(dest, font22, x, y, text, color, shadow);
}
void
Game::Print24(ALLEGRO_BITMAP *dest,
              int x,
              int y,
              const std::string &text,
              ALLEGRO_COLOR color,
              bool shadow) {
    Print(dest, font24, x, y, text, color, shadow);
}
void
Game::Print32(ALLEGRO_BITMAP *dest,
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
Game::printout(ScrollBox::ScrollBox *scroll,
               const string &str,
               ALLEGRO_COLOR color,
               long delay) {
    bool found = false;

    TimedText message = {str, color, globalTimer.getTimer() + delay};

    // do we care about repeating messages? -1 = one-time only, 0 = always, n =
    // ms delay
    if (delay == 0) {
        // just print it without remembering the message
        scroll->Write(message.text, message.color);
    } else {
        // scan timestamps of printed messages to see if ready to print again
        for (vector<TimedText>::iterator mess = messages.begin();
             mess != messages.end();
             ++mess) {
            // text found in vector?
            if (mess->text == message.text) {
                found = true;

                // print-once code
                if (delay == -1) {
                    mess->delay = -1;
                }

                // ready to print again?
                else if (globalTimer.getTimer() > mess->delay) {
                    // print text
                    scroll->Write(message.text, message.color);
                    // reset delay timer
                    mess->delay = globalTimer.getTimer() + delay;
                }
                break;
            }
        }
    }

    // text not found, add to vector and print
    if (!found) {
        if (delay == -1)
            message.delay = -1;
        messages.push_back(message);
        scroll->Write(message.text, message.color);
    }
}
#pragma endregion
