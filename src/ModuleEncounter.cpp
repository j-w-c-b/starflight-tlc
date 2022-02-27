/*
        STARFLIGHT - THE LOST COLONY
        ModuleEncounter.cpp - Handles alien encounters
        Author: J.Harbour
        Date: December, 2007
*/

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <allegro5/allegro_primitives.h>

#include "AudioSystem.h"
#include "Button.h"
#include "CombatObject.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "MathTL.h"
#include "MessageBoxWindow.h"
#include "ModeMgr.h"
#include "ModuleEncounter.h"
#include "PauseMenu.h"
#include "PlayerShipSprite.h"
#include "ResourceManager.h"
#include "Script.h"
#include "ScrolledModule.h"
#include "Sprite.h"
#include "TileScroller.h"
#include "Timer.h"
#include "Util.h"

#include "encounter_resources.h"

using namespace std;
using namespace encounter;

#define CLR_MSG GRAY1        // color of info messages
#define CLR_TRANS DODGERBLUE // color of transmissions to/from aliens
#define CLR_LIST STEEL       // color of list items
#define CLR_CANCEL ORANGE    // color of cancel choice
#define CLR_ALERT DKORANGE   // color of alert messages

namespace encounter {
/* const int OBJ_PLAYERSHIP = 1; */
const int OBJ_EXPLOSION = 5;
const int OBJ_ALIENSHIP = 10;
const int OBJ_ASTEROID_BIG = 20;
const int OBJ_ASTEROID_MED = 21;
const int OBJ_PLAYERLASER = 100;
const int OBJ_PLAYERMISSILE = 101;
const int OBJ_ENEMYFIRE = 102;
const int OBJ_POWERUP_HEALTH = 30;
const int OBJ_POWERUP_SHIELD = 31;
const int OBJ_POWERUP_ARMOR = 32;
const int OBJ_POWERUP_MINERAL_FROM_SHIP = 33;
const int OBJ_POWERUP_MINERAL_FROM_ASTEROID = 34;

const int TILESIZE = 256;
const int TILESACROSS = 64;
const int TILESDOWN = 64;

ALLEGRO_DEBUG_CHANNEL("ModuleEncounter")
}; // namespace encounter

/*
 * This function tracks all printed messages, each containing a timestamp to
 * prevent messages from printing out repeatedly. delay = 0 means always print.
 * delay =-1 means print only once.
 * otherwise delay is number of millisecond to wait between same message.
 */
void
ModuleEncounter::Print(const string &str, ALLEGRO_COLOR color, long delay) {
    g_game->printout(str, color, delay);
}

void
ModuleEncounter::Print(
    OfficerType type,
    const string &str,
    ALLEGRO_COLOR color,
    long delay) {
    auto officer = g_game->gameState->get_effective_officer(type);
    Print(officer->get_abbreviated_title() + " " + str, color, delay);
}

ModuleEncounter::ModuleEncounter()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), dropitems({}), shield(NULL),
      spr_statusbar_shield(NULL), script(NULL), flag_thrusting(false),
      flag_nav(false), flag_rotation(0) {}

bool
ModuleEncounter::on_key_down(ALLEGRO_KEYBOARD_EVENT *event) {
    int keyCode = event->keycode;

    if (module_mode == 1) // if module_mode == combat
    {
        if (g_game->gameState->m_ship.getFuel() == 0) {
            playerShip->cruise();
        } else {
            switch (keyCode) {
            case ALLEGRO_KEY_D:
            case ALLEGRO_KEY_RIGHT:
                flag_rotation = 1;
                playerShip->turnright();
                break;

            case ALLEGRO_KEY_A:
            case ALLEGRO_KEY_LEFT:
                flag_rotation = -1;
                playerShip->turnleft();
                break;

            case ALLEGRO_KEY_S:
            case ALLEGRO_KEY_DOWN:
                flag_nav = false;
                playerShip->applybraking();
                break;

            case ALLEGRO_KEY_W:
            case ALLEGRO_KEY_UP:
                flag_nav = flag_thrusting = true;
                playerShip->applythrust();
                flag_thrusting = true;
                break;

            case ALLEGRO_KEY_Q:
                flag_nav = true;
                if (!flag_thrusting)
                    playerShip->applybraking();
                playerShip->starboard();
                break;

            case ALLEGRO_KEY_E:
                flag_nav = true;
                if (!flag_thrusting)
                    playerShip->applybraking();
                playerShip->port();
                break;

            default:
                break;
            }
        }
        switch (keyCode) {
        case ALLEGRO_KEY_ALT:
        case ALLEGRO_KEY_X:
            if (!firingMissile) {
                firingLaser = true;
            }
            break;

        case ALLEGRO_KEY_LCTRL:
        case ALLEGRO_KEY_RCTRL:
        case ALLEGRO_KEY_Z:
            if (!firingLaser) {
                firingMissile = true;
            }
            break;

        default:
            break;
        }
    }
    g_game->CrossModuleAngle = playerShip->getRotationAngle(); // JJH
    return true;
}

bool
ModuleEncounter::on_key_up(ALLEGRO_KEYBOARD_EVENT *event) {
    int keyCode = event->keycode;

    // AlienRaces alien;
    bool shieldStatus, weaponStatus;
    switch (keyCode) {
        // reset ship anim frame when key released

    case ALLEGRO_KEY_D:
    case ALLEGRO_KEY_A:
    case ALLEGRO_KEY_LEFT:
    case ALLEGRO_KEY_RIGHT:
        flag_rotation = 0;
        break;
    case ALLEGRO_KEY_DOWN:
        playerShip->cruise();
        break;

    case ALLEGRO_KEY_UP:
        flag_nav = flag_thrusting = false;
        playerShip->applybraking();
        playerShip->cruise();
        break;

    case ALLEGRO_KEY_Q:
    case ALLEGRO_KEY_E:
        flag_nav = false;
        playerShip->applybraking();
        playerShip->cruise();
        break;

    case ALLEGRO_KEY_PGUP:
        shieldStatus =
            !g_game->gameState->getShieldStatus(); // jjh - added back
                                                   // shields/weapons toggles
        g_game->gameState->setShieldStatus(shieldStatus);
        break;
    case ALLEGRO_KEY_PGDN:
        weaponStatus = !g_game->gameState->getWeaponStatus();
        g_game->gameState->setWeaponStatus(weaponStatus);
        break;
    case ALLEGRO_KEY_ESCAPE: // escape key opens pause menu
        break;

    case ALLEGRO_KEY_ALT:
    case ALLEGRO_KEY_X:
        firingLaser = false;
        break;
    case ALLEGRO_KEY_LCTRL:
    case ALLEGRO_KEY_RCTRL:
    case ALLEGRO_KEY_Z:
        firingMissile = false;
        break;

#ifdef DEBUGMODE
    case ALIEN_ATTITUDE_PLUS:
        {
            int attitude = g_game->gameState->getAlienAttitude();
            g_game->gameState->setAlienAttitude(++attitude);
        }
        break;

    case ALIEN_ATTITUDE_MINUS:
        {
            int attitude = g_game->gameState->getAlienAttitude();
            g_game->gameState->setAlienAttitude(--attitude);
        }
        break;

    case IST_QUEST_PLUS:
        {
            int questnum = g_game->gameState->getActiveQuest();
            g_game->gameState->setActiveQuest(questnum + 1);
        }
        break;

    case IST_QUEST_MINUS:
        {
            int questnum = g_game->gameState->getActiveQuest();
            g_game->gameState->setActiveQuest(questnum - 1);
        }
    case ALLEGRO_KEY_F:
        g_game->toggleShowControls();
        break;
#endif
    }
    return true;
}

bool
ModuleEncounter::on_init() {
    ALLEGRO_DEBUG("  Encounter Initialize\n");

    // 0=encounter; 1=combat
    module_mode = 1;
    alienHailingUs = false;
    flag_DoHyperspace = false;
    hyperspaceCountdown = 0;
    flag_thrusting = false;

    deathState = 0;
    shipcount = 0;
    playerAttacked = false;
    flag_greeting = false;
    scanStatus = 0;

    goto_question = 0;
    number_of_actions = 0;

    firingLaser = firingMissile = false;

    // enable the Pause Menu
    g_game->enable_pause_menu(true);

    // create the player ship
    playerShip = new PlayerShipSprite();

    m_encounter_display = make_shared<Bitmap>(shared_ptr<ALLEGRO_BITMAP>(
        al_create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT), al_destroy_bitmap));
    add_child_module(m_encounter_display, true);

    TileSet ts(images[I_IP_TILES], TILESIZE, TILESIZE, 5, 1);
    m_scroller = make_shared<TileScroller>(
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        ts,
        TILESACROSS,
        TILESDOWN,
        playerShip->get_screen_position() - Point2D(96, 96));
    add_child_module(m_scroller, true);

    g_game->gameState->player.posCombat.SetPosition(
        (TILESACROSS / 2) * TILESIZE, (TILESDOWN / 2) * TILESIZE);

    // create the RichTextLabel for messages
    static int gmx = (int)g_game->getGlobalNumber("GUI_MESSAGE_POS_X");
    static int gmy = (int)g_game->getGlobalNumber("GUI_MESSAGE_POS_Y");
    static int gmw = (int)g_game->getGlobalNumber("GUI_MESSAGE_WIDTH");
    static int gmh = (int)g_game->getGlobalNumber("GUI_MESSAGE_HEIGHT");

    // create the panel of buttons for dialog
    m_dialog = make_shared<DialogButtonPanel>(
        gmx + 41, gmy + 18, gmw - 56, gmh - 32, g_game->font20);
    add_child_module(m_dialog);

    // initialize the encounter sub-system
    if (!Encounter_Init()) {
        return false;
    }

    // initialize the combat sub-system
    if (!Combat_Init()) {
        return false;
    }

    // clear screen
    al_clear_to_color(BLACK);

    // if we start in "fullscreen" mode, all objects vertical coords must be
    // patched
    if (!g_game->doShowControls()) {
        adjustVerticalCoords(
            (SCREEN_HEIGHT - NormalScreenHeight) / 2); //  (768-512)/2 == 128
    }

    // force start in "show control" mode for the time being
    if (!g_game->doShowControls()) {
        g_game->toggleShowControls();
    }

    m_fire1 = samples[S_FIRE1];
    m_fire2 = samples[S_FIRE2];
    m_hit1 = samples[S_HIT1];
    m_hit2 = samples[S_HIT2];

    return true;
}

bool
ModuleEncounter::on_close() {
    ALLEGRO_DEBUG("*** Encounter Close\n\n");

    // force weapons and shield off
    g_game->gameState->setWeaponStatus(false);
    g_game->gameState->setShieldStatus(false);

    delete playerShip;

    Encounter_Close();
    Combat_Close();

    remove_child_module(m_encounter_display);
    m_encounter_display = nullptr;

    remove_child_module(m_scroller);
    m_scroller = nullptr;

    return true;
}

bool
ModuleEncounter::Encounter_Init() {
    ALLEGRO_DEBUG("  Encounter_Init\n");
    string scriptFile = "";
    bFlagDialogue = false;
    commMode = COMM_NONE;
    bFlagChatting = false;

    alienName = g_game->gameState->getCurrentAlienName();

    // fill the dialog "censor"
    dialogCensor.clear();
    auto captain = g_game->gameState->get_officer(OFFICER_CAPTAIN);
    string firstname = captain->get_first_name();
    string lastname = captain->get_last_name();
    dialogCensor.insert(make_pair("[CAPTAIN_FIRST]", firstname));
    dialogCensor.insert(make_pair("[CAPTAIN_LAST]", lastname));
    dialogCensor.insert(make_pair("[CAPTAIN]", firstname + " " + lastname));
    dialogCensor.insert(
        make_pair("[CAPTAIN_FULLNAME]", firstname + " " + lastname));
    dialogCensor.insert(
        make_pair("[SHIPNAME]", g_game->gameState->getShip().getName()));
    dialogCensor.insert(make_pair("[ALIEN]", alienName));

    AlienRaces region = g_game->gameState->getCurrentAlien();
    switch (region) {
    case ALIEN_ELOWAN:
        scriptFile = "encounter_elowan";
        portraitName = I_PORTRAIT_ELOWAN;
        schematicName = I_SCHEMATIC_ELOWAN;
        shipName = I_SHIP_ELOWAN;
        break;
    case ALIEN_SPEMIN:
        scriptFile = "encounter_spemin";
        portraitName = I_PORTRAIT_SPEMIN;
        schematicName = I_SCHEMATIC_SPEMIN;
        shipName = I_SHIP_SPEMIN;
        break;
    case ALIEN_THRYNN:
        scriptFile = "encounter_thrynn";
        portraitName = I_PORTRAIT_THRYNN;
        schematicName = I_SCHEMATIC_THRYNN;
        shipName = I_SHIP_THRYNN;
        break;
    case ALIEN_BARZHON:
        scriptFile = "encounter_barzhon";
        portraitName = I_PORTRAIT_BARZHON;
        schematicName = I_SCHEMATIC_BARZHON;
        shipName = I_SHIP_BARZHON;
        break;
    case ALIEN_NYSSIAN:
        scriptFile = "encounter_nyssian";
        portraitName = I_PORTRAIT_NYSSIAN;
        schematicName = I_SCHEMATIC_NYSSIAN;
        shipName = I_SHIP_NYSSIAN;
        break;
    case ALIEN_TAFEL:
        scriptFile = "encounter_tafel";
        portraitName = I_PORTRAIT_TAFEL;
        schematicName = I_SCHEMATIC_TAFEL;
        shipName = I_SHIP_TAFEL;
        break;
    case ALIEN_MINEX:
        scriptFile = "encounter_minex";
        portraitName = I_PORTRAIT_MINEX;
        schematicName = I_SCHEMATIC_MINEX;
        shipName = I_SHIP_MINEX;
        break;
    case ALIEN_COALITION:
        scriptFile = "encounter_coalition";
        portraitName = I_PORTRAIT_COALITION;
        schematicName = I_SCHEMATIC_COALITION;
        shipName = I_SHIP_COALITION;
        break;
    case ALIEN_PIRATE:
        scriptFile = "encounter_pirate";
        portraitName = I_PORTRAIT_PIRATE;
        schematicName = I_SCHEMATIC_PIRATE;
        shipName = I_SHIP_PIRATE;
        break;
    default:
        break;
    }

    scriptFile = "data/encounter/" + scriptFile + ".lua";

    ALLEGRO_DEBUG("  Loading encounter script: %s\n", scriptFile.c_str());

    // load the script for this encounter
    script = new Script();
    lua_register(script->getState(), "L_Debug", L_Debug);
    lua_register(script->getState(), "L_Terminate", L_Terminate);
    lua_register(script->getState(), "L_Attack", L_Attack);
    script->load(scriptFile);

    // set globals prior to initializing dialogue
    sendGlobalsToScript();

    // run dialogue build function in script
    if (!script->runFunction("Initialize"))
        return false;
    readGlobalsFromScript(); // read globals too, in case Initialize changed
                             // anything.
                             //
    int gvx = (int)g_game->getGlobalNumber("GUI_VIEWER_POS_X");
    int gvy = (int)g_game->getGlobalNumber("GUI_VIEWER_POS_Y");
    m_portrait_view = make_shared<Bitmap>(images[I_GUI_VIEWER], gvx, gvy);
    add_child_module(m_portrait_view);
    m_portrait_view->add_child_module(
        make_shared<Bitmap>(images[portraitName], gvx + 108, gvy + 34));
    m_portrait_view->set_active(false);

    // draw gui schematic window with ship schematic
    int gvrx = (int)g_game->getGlobalNumber("GUI_RIGHT_VIEWER_POS_X");
    int gvry = (int)g_game->getGlobalNumber("GUI_RIGHT_VIEWER_POS_Y");
    m_ship_view = make_shared<Bitmap>(images[I_GUI_VIEWER_RIGHT], gvrx, gvry);
    add_child_module(m_ship_view);
    m_ship_view->add_child_module(
        make_shared<Bitmap>(images[schematicName], gvrx + 34, gvry + 34));
    m_ship_view->set_active(false);

#ifdef DEBUGMODE
    Print("Posture: " + g_game->gameState->playerPosture, WHITE, 5000);
#endif

    // statements and questions are reused until player chooses them
    // to prevent cycling to the next item in the script when CANCEL is pressed
    bFlagLastStatementSuccess = true;
    bFlagLastQuestionSuccess = true;

    bFlagDoResponse = false;
    bFlagDoAttack = false;

    return true;
}

void
ModuleEncounter::Encounter_Close() {
    remove_child_module(m_dialog);
    m_dialog = nullptr;

    delete script;
    script = NULL;
}

bool
ModuleEncounter::Combat_Init() {
    combatObjects.clear();

    int gax = (int)g_game->getGlobalNumber("GUI_AUX_POS_X");
    int gay = (int)g_game->getGlobalNumber("GUI_AUX_POS_Y");
    m_aux_gui = make_shared<Bitmap>(images[I_GUI_AUX], gax, gay);
    add_child_module(m_aux_gui);

    // create the minimap
    asw = (int)g_game->getGlobalNumber("AUX_SCREEN_WIDTH");
    ash = (int)g_game->getGlobalNumber("AUX_SCREEN_HEIGHT");
    asx = (int)g_game->getGlobalNumber("AUX_SCREEN_X");
    asy = (int)g_game->getGlobalNumber("AUX_SCREEN_Y");
    m_minimap = make_shared<Bitmap>(
        shared_ptr<ALLEGRO_BITMAP>(
            al_create_bitmap(asw, ash), al_destroy_bitmap),
        asx,
        asy,
        asw,
        ash);
    add_child_module(m_minimap);

    // radius is center of battle arena
    int radius = TILESACROSS / 2 * TILESIZE;

    // create alien fleet
    int fleetSize = g_game->gameState->player.getAlienFleetSize();
    if (fleetSize <= 0)
        fleetSize = 1; // this should not happen
    for (int n = 0; n < fleetSize; n++) {
        CombatObject *temp = new CombatObject();
        temp->setImage(images[shipName]);
        temp->setObjectType(OBJ_ALIENSHIP);
        temp->setTotalFrames(1);
        temp->setDamage(0);

        // get health property from script
        int health = script->getGlobalNumber("health");
        if (health < 1 || health > 10000) {
            ALLEGRO_DEBUG(
                "***Error in Combat_Init: health property is invalid\n");
            health = 100;
        }
        ALLEGRO_DEBUG("Combat_Init: health=%d\n", health);
        temp->setHealth(health);

        // get mass property from script
        int mass = script->getGlobalNumber("mass");
        if (mass < 1 || mass > 100) {
            ALLEGRO_DEBUG(
                "***Error in Combat_Init: mass property is invalid\n");
            mass = 1;
        }
        ALLEGRO_DEBUG("Combat_Init: mass=%d\n", mass);
        temp->setMass(mass);

        // get engine class property
        int engine = script->getGlobalNumber("engineclass");
        if (engine < 1 || engine > 6) {
            ALLEGRO_DEBUG("***Error in Combat_Init: engineclass is invalid\n");
            engine = 1;
        }
        ALLEGRO_DEBUG("Combat_Init: engineclass=%d\n", engine);
        setEngineProperties(temp, engine);

        // get shield props from script
        int shield = script->getGlobalNumber("shieldclass");
        if (shield < 0 || shield > 8) {
            ALLEGRO_DEBUG("***Error in Combat_Init: shieldclass is invalid\n");
            shield = 0;
        }
        ALLEGRO_DEBUG("Combat_Init: shieldclass=%d\n", shield);
        setShieldProperties(temp, shield);

        // get armor props from script
        int armor = script->getGlobalNumber("armorclass");
        if (armor < 0 || armor > 6) {
            ALLEGRO_DEBUG("***Error in Combat_Init: armorclass is invalid\n");
            armor = 0;
        }
        ALLEGRO_DEBUG("Combat_Init: armorclass=%d\n", armor);
        setArmorProperties(temp, armor);

        // get laser props from script
        int laser = script->getGlobalNumber("laserclass");
        if (laser < 0 || laser > 9) {
            ALLEGRO_DEBUG("***Error in Combat_Init: laserclass is invalid\n");
            laser = 0;
        }
        ALLEGRO_DEBUG("Combat_Init: laserclass=%d\n", laser);
        setLaserProperties(temp, laser);

        // get laser modifier from script
        int laserModifier = script->getGlobalNumber("laser_modifier");
        if (laserModifier < 0 || laserModifier > 100) {
            ALLEGRO_DEBUG(
                "***Error in Combat_Init: laser_modifier is invalid\n");
            laserModifier = 100;
        }
        ALLEGRO_DEBUG("Combat_Init: laser modifier=%d\n", laserModifier);
        temp->setLaserModifier(laserModifier);

        // get missile props from script
        int missile = script->getGlobalNumber("missileclass");
        if (missile < 0 || missile > 9) {
            ALLEGRO_DEBUG("***Error in Combat_Init: laserclass is invalid\n");
            missile = 0;
        }
        ALLEGRO_DEBUG("Combat_Init: missileclass=%d\n", missile);
        setMissileProperties(temp, missile);

        // get missile modifier from script
        int missileModifier = script->getGlobalNumber("missile_modifier");
        if (missileModifier < 0 || missileModifier > 100) {
            ALLEGRO_DEBUG(
                "***Error in Combat_Init: missile_modifier is invalid\n");
            missileModifier = 100;
        }
        ALLEGRO_DEBUG("Combat_Init: missile modifier=%d\n", missileModifier);
        temp->setMissileModifier(missileModifier);

        // set object to random location in battlespace (somewhat close to
        // player)
        temp->setPos(
            radius / 2 + Util::Random(0, radius),
            radius / 2 + Util::Random(0, radius));
        temp->setFaceAngle((float)Util::Random(1, 359));
        temp->ApplyThrust();
        AddCombatObject(temp);
    }

    // create big asteroids
    int num = sfrand() % 10 + 5;
    for (int a = 0; a < num; ++a) {
        CombatObject *temp = new CombatObject();
        temp->setImage(images[I_BIGASTEROID]);
        temp->setObjectType(OBJ_ASTEROID_BIG);
        temp->setTotalFrames(1);
        temp->setDamage(0);
        temp->setHealth(100);
        temp->setMass(4);
        temp->setVelX((double)Util::Random(0, 3) - 2);
        temp->setVelY((double)Util::Random(0, 3) - 2);
        temp->setMaxVelocity(4.0);
        temp->setPos(Util::Random(0, radius * 2), Util::Random(0, radius * 2));
        temp->setRotation(Util::Random(0, 6) - 3);
        AddCombatObject(temp);
    }

    // load animated shield sprite
    shield = new Sprite();
    shield->setImage(images[I_SHIELD]);
    shield->setAnimColumns(7);
    shield->setTotalFrames(14);
    shield->setFrameWidth(96);
    shield->setFrameHeight(96);
    shield->setFrameDelay(2);
    shield->setPos(464, 208);

    // validate ship condition before entering encounter
    if (g_game->gameState->m_ship.getHullIntegrity() == 0)
        g_game->gameState->m_ship.setHullIntegrity(100);

    // shield start at maximum possible capacity
    g_game->gameState->m_ship.setShieldCapacity(
        g_game->gameState->m_ship.getMaxShieldCapacity());

    // get drop items from script (5 must be defined)
    dropitems[0].id = script->getGlobalNumber("DROPITEM1");
    dropitems[0].rate = script->getGlobalNumber("DROPRATE1");
    dropitems[0].quantity = script->getGlobalNumber("DROPQTY1");
    dropitems[1].id = script->getGlobalNumber("DROPITEM2");
    dropitems[1].rate = script->getGlobalNumber("DROPRATE2");
    dropitems[1].quantity = script->getGlobalNumber("DROPQTY2");
    dropitems[2].id = script->getGlobalNumber("DROPITEM3");
    dropitems[2].rate = script->getGlobalNumber("DROPRATE3");
    dropitems[2].quantity = script->getGlobalNumber("DROPQTY3");
    dropitems[3].id = script->getGlobalNumber("DROPITEM4");
    dropitems[3].rate = script->getGlobalNumber("DROPRATE4");
    dropitems[3].quantity = script->getGlobalNumber("DROPQTY4");
    dropitems[4].id = script->getGlobalNumber("DROPITEM5");
    dropitems[4].rate = script->getGlobalNumber("DROPRATE5");
    dropitems[4].quantity = script->getGlobalNumber("DROPQTY5");

    return true;
}

void
ModuleEncounter::Combat_Close() {
    // Delete all the combatObjects
    combatObjects.clear();

    if (shield)
        delete shield;

    m_fire1 = nullptr;
    m_fire2 = nullptr;
    m_hit1 = nullptr;
    m_hit2 = nullptr;

    remove_child_module(m_aux_gui);
    m_aux_gui = nullptr;

    remove_child_module(m_minimap);
    m_minimap = nullptr;

    // fully regenerate shield
    g_game->gameState->m_ship.setShieldCapacity(
        g_game->gameState->m_ship.getMaxShieldCapacity());
}

void
ModuleEncounter::setMissileProperties(CombatObject *ship, int missileclass) {
    switch (missileclass) {
    case 1:
        ship->setMissileFiringRate(
            g_game->getGlobalNumber("MISSILE1_FIRERATE"));
        ship->setMissileDamage(g_game->getGlobalNumber("MISSILE1_DAMAGE"));
        break;
    case 2:
        ship->setMissileFiringRate(
            g_game->getGlobalNumber("MISSILE2_FIRERATE"));
        ship->setMissileDamage(g_game->getGlobalNumber("MISSILE2_DAMAGE"));
        break;
    case 3:
        ship->setMissileFiringRate(
            g_game->getGlobalNumber("MISSILE3_FIRERATE"));
        ship->setMissileDamage(g_game->getGlobalNumber("MISSILE3_DAMAGE"));
        break;
    case 4:
        ship->setMissileFiringRate(
            g_game->getGlobalNumber("MISSILE4_FIRERATE"));
        ship->setMissileDamage(g_game->getGlobalNumber("MISSILE4_DAMAGE"));
        break;
    case 5:
        ship->setMissileFiringRate(
            g_game->getGlobalNumber("MISSILE5_FIRERATE"));
        ship->setMissileDamage(g_game->getGlobalNumber("MISSILE5_DAMAGE"));
        break;
    case 6:
        ship->setMissileFiringRate(g_game->getGlobalNumber(
            "MISSILE6_FIRERATE")); // jjh need to add Thyrnn stuff
        ship->setMissileDamage(g_game->getGlobalNumber("MISSILE6_DAMAGE"));
        break;
    default:
        ship->setMissileFiringRate(0);
        ship->setMissileDamage(0);
    }
}

void
ModuleEncounter::setLaserProperties(CombatObject *ship, int laserclass) {
    switch (laserclass) {
    case 1:
        ship->setLaserFiringRate(g_game->getGlobalNumber("LASER1_FIRERATE"));
        ship->setLaserDamage(g_game->getGlobalNumber("LASER1_DAMAGE"));
        break;
    case 2:
        ship->setLaserFiringRate(g_game->getGlobalNumber("LASER2_FIRERATE"));
        ship->setLaserDamage(g_game->getGlobalNumber("LASER2_DAMAGE"));
        break;
    case 3:
        ship->setLaserFiringRate(g_game->getGlobalNumber("LASER3_FIRERATE"));
        ship->setLaserDamage(g_game->getGlobalNumber("LASER3_DAMAGE"));
        break;
    case 4:
        ship->setLaserFiringRate(g_game->getGlobalNumber("LASER4_FIRERATE"));
        ship->setLaserDamage(g_game->getGlobalNumber("LASER4_DAMAGE"));
        break;
    case 5:
        ship->setLaserFiringRate(g_game->getGlobalNumber("LASER5_FIRERATE"));
        ship->setLaserDamage(g_game->getGlobalNumber("LASER5_DAMAGE"));
        break;
    case 6:
        ship->setLaserFiringRate(g_game->getGlobalNumber(
            "LASER6_FIRERATE")); // jjh need to add Thyrnn stuff
        ship->setLaserDamage(g_game->getGlobalNumber("LASER6_DAMAGE"));
        break;
    default:
        ship->setLaserFiringRate(0);
        ship->setLaserDamage(0);
    }
}

void
ModuleEncounter::setArmorProperties(CombatObject *ship, int armorclass) {
    switch (armorclass) {
    case 1:
        ship->setArmorStrength(g_game->getGlobalNumber("ARMOR1_STRENGTH"));
        break;
    case 2:
        ship->setArmorStrength(g_game->getGlobalNumber("ARMOR2_STRENGTH"));
        break;
    case 3:
        ship->setArmorStrength(g_game->getGlobalNumber("ARMOR3_STRENGTH"));
        break;
    case 4:
        ship->setArmorStrength(g_game->getGlobalNumber("ARMOR4_STRENGTH"));
        break;
    case 5:
        ship->setArmorStrength(g_game->getGlobalNumber("ARMOR5_STRENGTH"));
        break;
    case 6:
        ship->setArmorStrength(g_game->getGlobalNumber("ARMOR6_STRENGTH"));
        break;
    default:
        ship->setArmorStrength(0);
    }
}

void
ModuleEncounter::setShieldProperties(CombatObject *ship, int shieldclass) {
    switch (shieldclass) {
    case 1:
        ship->setShieldStrength(g_game->getGlobalNumber("SHIELD1_STRENGTH"));
        break;
    case 2:
        ship->setShieldStrength(g_game->getGlobalNumber("SHIELD2_STRENGTH"));
        break;
    case 3:
        ship->setShieldStrength(g_game->getGlobalNumber("SHIELD3_STRENGTH"));
        break;
    case 4:
        ship->setShieldStrength(g_game->getGlobalNumber("SHIELD4_STRENGTH"));
        break;
    case 5:
        ship->setShieldStrength(g_game->getGlobalNumber("SHIELD5_STRENGTH"));
        break;
    case 6:
        ship->setShieldStrength(g_game->getGlobalNumber("SHIELD6_STRENGTH"));
        break;
    default:
        ship->setShieldStrength(0);
    }
}

void
ModuleEncounter::setEngineProperties(CombatObject *ship, int engineclass) {
    // set top speed, acceleration, and turn rate based on engine class
    // these props are pulled from globals.lua, not from alien script
    switch (engineclass) {
    case 1:
        ship->setMaxVelocity(g_game->getGlobalNumber("ENGINE1_TOPSPEED"));
        ship->setAcceleration(g_game->getGlobalNumber("ENGINE1_ACCEL"));
        ship->setTurnRate(g_game->getGlobalNumber("ENGINE1_TURNRATE"));
        break;
    case 2:
        ship->setMaxVelocity(g_game->getGlobalNumber("ENGINE2_TOPSPEED"));
        ship->setAcceleration(g_game->getGlobalNumber("ENGINE2_ACCEL"));
        ship->setTurnRate(g_game->getGlobalNumber("ENGINE2_TURNRATE"));
        break;
    case 3:
        ship->setMaxVelocity(g_game->getGlobalNumber("ENGINE3_TOPSPEED"));
        ship->setAcceleration(g_game->getGlobalNumber("ENGINE3_ACCEL"));
        ship->setTurnRate(g_game->getGlobalNumber("ENGINE3_TURNRATE"));
        break;
    case 4:
        ship->setMaxVelocity(g_game->getGlobalNumber("ENGINE4_TOPSPEED"));
        ship->setAcceleration(g_game->getGlobalNumber("ENGINE4_ACCEL"));
        ship->setTurnRate(g_game->getGlobalNumber("ENGINE4_TURNRATE"));
        break;
    case 5:
        ship->setMaxVelocity(g_game->getGlobalNumber("ENGINE5_TOPSPEED"));
        ship->setAcceleration(g_game->getGlobalNumber("ENGINE5_ACCEL"));
        ship->setTurnRate(g_game->getGlobalNumber("ENGINE5_TURNRATE"));
        break;
    case 6:
        ship->setMaxVelocity(g_game->getGlobalNumber("ENGINE6_TOPSPEED"));
        ship->setAcceleration(g_game->getGlobalNumber("ENGINE6_ACCEL"));
        ship->setTurnRate(g_game->getGlobalNumber("ENGINE6_TURNRATE"));
        break;
    }
}

std::string
ModuleEncounter::commGetAction() {
    std::string action = script->getGlobalString("ACTION");
    return action;
}

void
ModuleEncounter::commCheckCurrentAction() {
    // update engine with globals from script
    readGlobalsFromScript();

    std::string action = commGetAction();

    if (action == "terminate") {
        Print(
            OFFICER_COMMUNICATION,
            "Comm channel has been terminated!",
            CLR_ALERT);
        module_mode = 1;
        bFlagDialogue = false;
        bFlagChatting = false;
    } else if (action == "attack") {
        Print(
            OFFICER_COMMUNICATION,
            "Comm channel has been terminated!",
            CLR_ALERT);
        module_mode = 1;
        bFlagDialogue = false;
        bFlagChatting = false;
        bFlagDoAttack = true;
    }
}

/*
 * Responses to greetings, statements, and questions all handled here
 * This is called by Update with a timer slowdown
 */
void
ModuleEncounter::commDoAlienResponse() // jjh
{
    std::string out;
    static int randomDelay = 0;

    // alien response is delayed 1-4 sec
    if (randomDelay == 0) {
        randomDelay = Util::Random(1000, 4000);
    }
    if (!Util::ReentrantDelay(randomDelay)) {
        // temporarily disable the control panel
        g_game->ControlPanelActivity = false;
    } else {
        // done waiting, now display alien response
        bFlagChatting = true;
        g_game->printout("");
        out = script->getGlobalString("RESPONSE");
        Print(OFFICER_COMMUNICATION, "Response received.", CLR_MSG);

        // replace keywords in dialog string with data values
        out = replaceKeyWords(out);
        Print(alienName + "->" + out, CLR_TRANS, 1000);
        commCheckCurrentAction();

        // reset flags
        bFlagLastStatementSuccess = true;
        bFlagLastQuestionSuccess = true;
        g_game->ControlPanelActivity = true;
        bFlagDoResponse = false; // done with response
        randomDelay = 0;
    }
}

void
ModuleEncounter::commDoAlienAttack() {
    static int mode = 0;
    static int randomDelay = 0;

    switch (mode) {
    case 0: // first delay
        if (randomDelay == 0)
            randomDelay = Util::Random(2000, 4000);
        if (!Util::ReentrantDelay(randomDelay)) {
            // temporarily disable the control panel
            g_game->ControlPanelActivity = false;
        } else {
            // done waiting, now display alien response
            Print("", WHITE, 0);
            Print(
                OFFICER_TACTICAL,
                "Captain, they're arming weapons!",
                CLR_ALERT);
            Print("", WHITE, 0);
            // enable the CP
            g_game->ControlPanelActivity = true;
            // reset delay
            randomDelay = 0;
            // go to next delay mode
            mode = 1;
        }
        break;

    case 1: // second delay
        if (randomDelay == 0)
            randomDelay = Util::Random(2000, 4000);
        if (!Util::ReentrantDelay(randomDelay)) {
            // temporarily disable the control panel
            g_game->ControlPanelActivity = false;
        } else {
            // reset flag
            bFlagDoAttack = false;
            // reset delay
            randomDelay = 0;
            // enable the CP
            g_game->ControlPanelActivity = true;
            // engage in combat!
            module_mode = 1;
        }
        break;
    }
}

void
ModuleEncounter::commDoPosture(int index) {
    switch (index) {
    case 1:
        bFlagDialogue = false;
        g_game->printout("");
        if (g_game->gameState->playerPosture == "obsequious") {
            Print(
                OFFICER_COMMUNICATION, "Posture is still Obsequious.", CLR_MSG);
        } else {
            Print(
                OFFICER_COMMUNICATION,
                "Posture set to Patheti... Er... Obsequious",
                CLR_MSG);
            g_game->gameState->playerPosture = "obsequious";
        }
        break;
    case 2:
        bFlagDialogue = false;
        g_game->printout("");
        if (g_game->gameState->playerPosture == "friendly") {
            Print(OFFICER_COMMUNICATION, "Posture is still Friendly.", CLR_MSG);
        } else {
            Print(OFFICER_COMMUNICATION, "Posture set to Friendly", CLR_MSG);
            g_game->gameState->playerPosture = "friendly";
        }
        break;
    case 3:
        bFlagDialogue = false;
        g_game->printout("");
        if (g_game->gameState->playerPosture == "hostile") {
            Print(OFFICER_COMMUNICATION, "Posture is still Hostile.", CLR_MSG);
        } else {
            Print(
                OFFICER_COMMUNICATION,
                "Posture set to Hostile. I hope you know what "
                "you're doing!",
                CLR_MSG);
            g_game->gameState->playerPosture = "hostile";
        }
        break;
    case 4:
        bFlagDialogue = false;
        g_game->printout("");
        Print(
            OFFICER_COMMUNICATION,
            "Posture is " + g_game->gameState->playerPosture + ".",
            CLR_MSG);
        break;
    }

    // set script global and run the update function:
    script->setGlobalString("POSTURE", g_game->gameState->playerPosture);
    if (!script->runFunction("UpdatePosture")) {
        ALLEGRO_DEBUG("ModuleEncounter::commDoPosture\tProblem updating script "
                      "globals- exiting!\n");
        return;
    }
}

void
ModuleEncounter::commDoGreeting() {
    if (playerAttacked || flag_greeting)
        return;

    sendGlobalsToScript();
    if (!script->runFunction("Greeting"))
        return;
    std::string greeting = replaceKeyWords(script->getGlobalString("GREETING"));
    g_game->clear_printout();
    if (g_game->gameState->playerPosture != "hostile")
        Print(
            OFFICER_COMMUNICATION,
            "Hailing frequencies open. Sending greeting...",
            CLR_MSG,
            5000);
    else
        Print(
            OFFICER_COMMUNICATION,
            "Hailing frequencies open. Sending our demands...",
            CLR_MSG,
            5000);
    Print(greeting, CLR_TRANS, 5000);

    bFlagDoResponse = true;
    alienHailingUs = false;
    flag_greeting = true;
}

void
ModuleEncounter::commInitStatement() {
    // ignore message buttons while in combat
    if (module_mode == 1)
        return;

    bFlagDialogue = true;
    m_dialog->set_active(true);
    commMode = COMM_STATEMENT;

    // will succeed when player uses this statement
    if (bFlagLastStatementSuccess) {
        // since previous statement succeeded, we need a new one
        sendGlobalsToScript();
        if (!script->runFunction("Statement"))
            return;
        bFlagLastStatementSuccess = false;
    }
    std::string statement =
        replaceKeyWords(script->getGlobalString("STATEMENT"));

    m_dialog->clear();
    m_dialog->add_option({statement, CLR_LIST});
    m_dialog->add_option({"CANCEL", CLR_MSG});
}

void
ModuleEncounter::commDoStatement(int index) {
    switch (index) {
    case 0:
        bFlagDialogue = false; // done showing statement choices
        g_game->clear_printout();
        Print(OFFICER_COMMUNICATION, "Sending statement...", CLR_MSG);
        Print(
            OFFICER_COMMUNICATION,
            replaceKeyWords(script->getGlobalString("STATEMENT")),
            CLR_TRANS);
        g_game->printout("Waiting for response", CLR_MSG);
        bFlagDoResponse = true; // ready to handle alien response
        break;
    default: // cancel
        bFlagDialogue = false;
        Print(OFFICER_COMMUNICATION, "Statement cancelled", CLR_MSG);
        bFlagDoResponse = false;
        break;
    }
}

void
ModuleEncounter::commInitQuestion() {
    // ignore message buttons while in combat
    if (module_mode == 1)
        return;

    std::ostringstream os;
    std::string question;
    bFlagDialogue = true;
    m_dialog->set_active(true);
    commMode = COMM_QUESTION;

    // will succeed when player uses this question
    if (bFlagLastQuestionSuccess) {
        // since previous statement succeeded, we need a new one
        sendGlobalsToScript();
        if (!script->runFunction("Question"))
            return;
        bFlagLastStatementSuccess = false;
    }

    // look for a branch action
    if (script->getGlobalString("ACTION") == "branch") {
        m_dialog->clear();
        int choices = static_cast<int>(script->getGlobalNumber("CHOICES"));
        for (int n = 0; n < choices; n++) {
            os.str("");
            os << "QUESTION" << n + 1 << "_TITLE";
            question = replaceKeyWords(script->getGlobalString(os.str()));
            m_dialog->add_option({question, CLR_LIST});
        }

        m_dialog->add_option({"CANCEL", CLR_CANCEL}, 6);

    } else { // normal non-branching question
        m_dialog->clear();

        string text = script->getGlobalString(
            "QUESTION_TITLE"); // exception occurs in this stmt
        // use selected question text from branch if repeating text:
        if ((text == "[REPEAT]") || (text == "[AUTO_REPEAT]")) {
            int choice = (int)script->getGlobalNumber("CHOICE");
            os.str("");
            os << "QUESTION" << choice << "_TITLE";
            // perform censor string swapping:
            question = replaceKeyWords(script->getGlobalString(os.str()));
            if (text == "[REPEAT]")
                m_dialog->add_option({question, CLR_TRANS});
            if (text == "[AUTO_REPEAT]") {
                commDoQuestion(0);
                return;
            }    // proceed w/o waiting
        } else { // print out the question title:
            m_dialog->add_option({text, CLR_TRANS});
        }

        m_dialog->add_option({"CANCEL", CLR_CANCEL});
    }
}

void
ModuleEncounter::commDoQuestion(int index) {
    std::string out;

    if (script->getGlobalString("ACTION") == "branch") {
        if (index < 5) // any but cancel
        {
            // send chosen branch item number back to script
            script->setGlobalNumber("CHOICE", index + 1);
            if (!script->runFunction("Branch"))
                return;

            // get next question--should now be from new branch location
            bFlagLastQuestionSuccess = true;
            commInitQuestion();
        } else {
            bFlagDialogue = false;
            m_dialog->set_active(false);
        }
    } else {
        if (module_mode == 1)
            return;

        // normal non-branching question
        switch (index) {
        case 0:
            out = script->getGlobalString("QUESTION");
            // use selected question text from branch if repeating text:
            if ((out == "[REPEAT]") || (out == "[AUTO_REPEAT]")) {
                std::ostringstream os;
                int choice = (int)script->getGlobalNumber("CHOICE");
                os.str("");
                os << "QUESTION" << choice;
                // perform censor string swapping:
                out = replaceKeyWords(script->getGlobalString(os.str()));
            } else {
                out = replaceKeyWords(script->getGlobalString("QUESTION"));
            }

            Print(OFFICER_COMMUNICATION, out, CLR_MSG);
            bFlagDoResponse = true;
            break;

        default: // cancel
            break;
        }

        bFlagDialogue = false;
    }
}

void
ModuleEncounter::commInitPosture() {

    bFlagDialogue = true;
    m_dialog->set_active(true);
    commMode = COMM_POSTURE;
    m_dialog->clear();
    m_dialog->add_option({"Choose new posture:", CLR_MSG});
    m_dialog->add_option({"  OBSEQUIOUS", CLR_LIST});
    m_dialog->add_option({"  FRIENDLY", CLR_LIST});
    m_dialog->add_option({"  HOSTILE", CLR_LIST});
    m_dialog->add_option({"  CANCEL", CLR_CANCEL});
}

bool
ModuleEncounter::on_event(ALLEGRO_EVENT *event) {
    std::string escape;
    bool shieldStatus, weaponStatus;
    Ship ship;
    int laser, missile;

    switch (event->type) {
    case EVENT_SCIENCE_SCAN:
        scanStatus = 1;
        scanTimer.reset();
        break;

    case EVENT_SCIENCE_ANALYSIS:
        // when scan is complete, scanStatus is set to 2
        if (scanStatus == 2)
            scanStatus = 3;
        break;

    case EVENT_TACTICAL_WEAPONS:
        // arm/disarm weapons
        ship = g_game->gameState->getShip();
        laser = ship.getLaserClass();
        missile = ship.getMissileLauncherClass();
        if (laser == 0 && missile == 0) {
            if (!g_game->gameState->getWeaponStatus()) {
                // trying to arm nonexistent weapons
                Print(
                    OFFICER_TACTICAL, "Sir, we have no weapons.", ORANGE, 5000);
                if (g_game->gameState->has_officer(OFFICER_NAVIGATION)) {
                    // do random response
                    if (Util::Random(1, 5) == 1) {
                        Print(
                            OFFICER_NAVIGATION,
                            "Remember, you spent those credits at the "
                            "Cantina instead?",
                            GREEN,
                            5000);
                        // do random reaction
                        if (Util::Random(1, 5) == 1) {
                            Print(
                                OFFICER_SCIENCE,
                                "Watch the attitude, "
                                    + g_game->gameState
                                          ->get_effective_officer(
                                              OFFICER_NAVIGATION)
                                          ->get_last_name()
                                    + "!",
                                YELLOW,
                                5000);
                        }
                    }
                }
            } else {
                // weapons were destroyed during combat; force them down
                g_game->gameState->setWeaponStatus(false);
            }
        } else {
            // toggle weapon status
            weaponStatus = !g_game->gameState->getWeaponStatus();
            g_game->gameState->setWeaponStatus(weaponStatus);
            if (weaponStatus) {
                if (laser > 0)
                    Print(
                        OFFICER_TACTICAL,
                        "Laser capacitors charging",
                        ORANGE,
                        2000);
                if (missile > 0)
                    Print(
                        OFFICER_TACTICAL,
                        "Missile launcher primed and ready",
                        ORANGE,
                        2000);
            } else {
                if (laser > 0)
                    Print(OFFICER_TACTICAL, "Lasers disarmed", ORANGE, 2000);
                if (missile > 0)
                    Print(
                        OFFICER_TACTICAL,
                        "Missile launcher disarmed",
                        ORANGE,
                        2000);
            }
        }
        break;
    case EVENT_TACTICAL_SHIELDS:
        ship = g_game->gameState->getShip();
        if (ship.getShieldClass() == 0) {
            if (g_game->gameState->getShieldStatus() == false)
                Print(
                    OFFICER_TACTICAL, "Sir, we have no shields.", ORANGE, 2000);

            else // shield destroyed during combat; force them down
                g_game->gameState->setShieldStatus(false);
        } else {
            // toggle shield status
            shieldStatus = g_game->gameState->getShieldStatus();
            g_game->gameState->setShieldStatus(!shieldStatus);
            if (shieldStatus)
                Print(OFFICER_TACTICAL, "Dropping shields.", ORANGE, 2000);
            else
                Print(OFFICER_TACTICAL, "Aye, sir; shields up.", ORANGE, 2000);
        }
        break;
    case EVENT_SHOW_CONTROLS:
        adjustVerticalCoords(-(SCREEN_HEIGHT - NormalScreenHeight) / 2);
        break; // -128
    case EVENT_HIDE_CONTROLS:
        adjustVerticalCoords((SCREEN_HEIGHT - NormalScreenHeight) / 2);
        break; // +128
    // Pause Screen events
    case EVENT_SAVE_GAME: // save game
        Print("<Game Save is not available during encounters>", WHITE, -1);
        break;
    case EVENT_LOAD_GAME: // load game
        g_game->gameState->AutoLoad();
        break;
    case EVENT_QUIT_GAME: // quit game
        g_game->set_vibration(0);
        escape = g_game->getGlobalString("ESCAPEMODULE");
        g_game->LoadModule(escape);
        break;

    case EVENT_NAVIGATOR_HYPERSPACE:
        if (!flag_DoHyperspace) {
            flag_DoHyperspace = !flag_DoHyperspace;
            hyperspaceCountdown = 5;
        }
        break;

    case EVENT_COMM_HAIL:
        if (getShipCount() == 0) {
            Print(
                OFFICER_COMMUNICATION,
                "There are no alien ships to hail",
                YELLOW,
                2000);
            module_mode = 1;
        } else {
            if (alienHailingUs && !flag_greeting) {
                Print(OFFICER_COMMUNICATION, "Responding...", CLR_MSG, 5000);
                module_mode = 0;
                commDoGreeting();
            } else if (!flag_greeting) {
                Print(OFFICER_COMMUNICATION, "Hailing...", CLR_MSG, 2000);
                module_mode = 0;
                commDoGreeting();
            } else
                (module_mode == 0) ? Print(
                    OFFICER_COMMUNICATION,
                    "Sir, communication channel is open already.",
                    CLR_MSG,
                    5000)
                                   : Print(
                                       OFFICER_COMMUNICATION,
                                       "Sir, they are ignoring our hail.",
                                       CLR_MSG,
                                       5000);
        }
        break;

    case EVENT_COMM_DISTRESS:
        Print(
            OFFICER_COMMUNICATION,
            "Interstellar communications are currently being jammed by "
            "nearby alien hyperspace sources.",
            YELLOW,
            5000);
        break;

    case EVENT_COMM_STATEMENT:
        commInitStatement();
        break;
    case EVENT_COMM_QUESTION:
        commInitQuestion();
        break;
    case EVENT_COMM_POSTURE:
        commInitPosture();
        break;
    case EVENT_COMM_TERMINATE:
        if (module_mode == 0) {
            Print(OFFICER_COMMUNICATION, "Terminating...", CLR_MSG);
            module_mode = 1;
            bFlagDialogue = false;
            bFlagChatting = false;
        }
        break;

    case EVENT_ENCOUNTER_DIALOGUE:
        {
            int index = m_dialog->get_selection();
            switch (commMode) {
            case COMM_STATEMENT:
                commDoStatement(index);
                break;
            case COMM_QUESTION:
                commDoQuestion(index);
                break;
            case COMM_POSTURE:
                commDoPosture(index);
                break;

            default:
                ALLEGRO_ASSERT(0);
            }
            break;
        }

    case EVENT_ENCOUNTER_CLOSECOMM:
        {
            if (module_mode == 0) {
                Print(
                    OFFICER_COMMUNICATION,
                    "Comm channel has been terminated!",
                    CLR_ALERT);
                module_mode = 1;
                bFlagDialogue = false;
                bFlagChatting = false;
            }
            break;
        }

    case EVENT_ENCOUNTER_ALIENATTACK:
        {
            if (module_mode == 0) {
                Print(
                    OFFICER_COMMUNICATION,
                    "Comm channel has been terminated!",
                    CLR_ALERT);
                module_mode = 1;
                bFlagDialogue = false;
                bFlagChatting = false;
            }

            bFlagDoAttack = true;

            break;
        }
    }
    return true;
}

bool
ModuleEncounter::on_update() {
    static Timer countdown;
    ostringstream os;

    // update scrolling and draw tiles on the scroll buffer
    playerGlobal.x =
        g_game->gameState->player.posCombat.x + playerShip->getVelocityX();
    playerGlobal.y =
        g_game->gameState->player.posCombat.y + playerShip->getVelocityY();
    g_game->gameState->player.posCombat.x = playerGlobal.x;
    g_game->gameState->player.posCombat.y = playerGlobal.y;

    // does player want to bug out?
    if (flag_DoHyperspace) {
        if (g_game->gameState->getShieldStatus()) {
            Print(
                OFFICER_NAVIGATION,
                "We can't enter hyperspace with our shields activated.",
                ORANGE,
                5000);
            flag_DoHyperspace = false;
        } else if (g_game->gameState->getWeaponStatus()) {
            Print(
                OFFICER_NAVIGATION,
                "We can't enter hyperspace with our weapons armed.",
                ORANGE,
                5000);
            flag_DoHyperspace = false;
        }

        if (playerAttacked) {
            Print(
                OFFICER_NAVIGATION,
                "Sir, the hyperspace field has failed!",
                RED,
                5000);
            flag_DoHyperspace = false;
        }
    }
    // CAN the player bug out?
    if (flag_DoHyperspace) {
        Print(OFFICER_NAVIGATION, "Engaging hyperspace engine...", ORANGE, -1);
        // SW force player to stop
        playerShip->applybraking();
        // wait for countdown
        if (countdown.stopwatch(750)) {
            hyperspaceCountdown--;
            os << hyperspaceCountdown << "...";
            Print(os.str(), ORANGE, -1);
            if (hyperspaceCountdown == 0) {
                g_game->gameState->m_ship.ConsumeFuel();
                // g_game->LoadModule(MODULE_HYPERSPACE);
                string prev = g_game->modeMgr->GetPrevModuleName();
                g_game->LoadModule(prev);
                return false;
            }
        }
    }

    // keep player on the combat area
    Point2D s(
        (playerGlobal.x - SCREEN_WIDTH / 2) / TILESIZE,
        (playerGlobal.y - effectiveScreenHeight() / 2) / TILESIZE);
    if (s.x <= 0) {
        s.x = 0;
        playerShip->applybraking();
    } else if (s.x >= TILESACROSS) {
        s.x = TILESACROSS;
        playerShip->applybraking();
    }
    if (s.y <= 0) {
        s.y = 0;
        playerShip->applybraking();
    } else if (s.y >= TILESDOWN) {
        s.y = TILESDOWN;
        playerShip->applybraking();
    }

    m_scroller->set_scroll_position(s);

    // calculate player's screen position
    playerScreen.x =
        playerGlobal.x / (TILESIZE * TILESACROSS) + SCREEN_WIDTH / 2 - 32;
    playerScreen.y = playerGlobal.y / (TILESIZE * TILESDOWN)
                     + effectiveScreenHeight() / 2 - 32;

    // if this is a friendly alien, they will initiate conversation
    if (g_game->gameState->getAlienAttitude() > 60 && !bFlagChatting
        && !alienHailingUs && !flag_greeting && !playerAttacked) {
        alienHailingUs = true;
        Print(OFFICER_NAVIGATION, "Sir, we're being hailed", STEEL, 8000);
    }

    // update dialog and combat sections
    if (module_mode == 0)
        Encounter_Update();
    else
        Combat_Update();

    return true;
}

bool
ModuleEncounter::on_draw(ALLEGRO_BITMAP *target) {
    // ignore the normal target bitmap and draw to the m_encounter_display
    // so we can get the bitmaps stacked correctly.
    target = m_encounter_display->get_bitmap().get();
    al_set_target_bitmap(target);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    // draw space background

    // draw player ship
    playerShip->draw(target);

    // let encounter/combat draw their stuff
    if (module_mode == 1) {
        Combat_Draw(target);
    }

    // draw minimap
    DrawMinimap(target);

    al_set_target_bitmap(target);
    if (g_game->doShowControls()) {
        m_dialog->set_active(bFlagDialogue);
    }
    m_portrait_view->set_active(bFlagChatting);
    m_ship_view->set_active(bFlagChatting);

    return true;
}

void
ModuleEncounter::Encounter_Update() {
    // stop the ship
    playerShip->applybraking();

    // see if alien response is ready
    if (bFlagDoResponse) {
        commDoAlienResponse();
    }

    // see if alien is attacking
    if (bFlagDoAttack) {
        commDoAlienAttack();
    }
}

string
ModuleEncounter::replaceKeyWords(string input) {
    string key, value;
    string::size_type pos;

    map<string, string>::iterator iter;
    iter = dialogCensor.begin();
    while (iter != dialogCensor.end()) {
        key = iter->first;
        value = iter->second;

        pos = input.find(key, 0);
        if (pos != string::npos) {
            input.replace(pos, key.length(), value);
        }
        ++iter;
    }

    return input;
}

void
ModuleEncounter::ImpactPlayer(CombatObject *player, CombatObject *other) {
    static double bump = 1.2;
    double vx, vy, x1, y1, x2, y2;

    // first, move the objects off each other
    while (player->CheckCollision(other)) {
        x1 = g_game->gameState->player.posCombat.x + 32;
        y1 = g_game->gameState->player.posCombat.y + 32;
        x2 = other->getX() + other->getFrameWidth() / 2;
        y2 = other->getY() + other->getFrameHeight() / 2;

        if (x1 < x2) {
            g_game->gameState->player.posCombat.x -= bump;
            other->setX(other->getX() + bump);
        } else {
            g_game->gameState->player.posCombat.x += bump;
            other->setX(other->getX() - bump);
        }

        if (y1 < y2) {
            g_game->gameState->player.posCombat.y -= bump;
            other->setY(other->getY() + bump);
        } else {
            g_game->gameState->player.posCombat.y += bump;
            other->setY(other->getY() - bump);
        }
    }

    // second, velocity affected by mass
    double mass_factor = 1.0;
    double modifier = 0.005;

    // calculate mass ratio
    if (player->getMass() > 0.0)
        mass_factor = other->getMass() / (player->getMass());

    double angle = other->getFaceAngle() - 90.0;
    vx = Sprite::calcAngleMoveX((int)angle) * mass_factor * modifier;
    vy = Sprite::calcAngleMoveY((int)angle) * mass_factor * modifier;

    playerShip->setVelocityX(playerShip->getVelocityX() + vx);
    playerShip->setVelocityY(playerShip->getVelocityY() + vy);
}

void
ModuleEncounter::pickupRandomDropItem() {
    // add random mineral to the ship's cargo
    int rate, itemid, numitems;
    ostringstream os;

    // mineral IDs range from 30 to 54 (in case no random item drop is chosen)
    // FIXME: should use itemType instead of hardcoded IDs
    itemid = Util::Random(30, 54);
    numitems = Util::Random(1, 4);
    Item *item = g_game->dataMgr->GetItemByID(itemid);
    if (item == NULL) {
        ALLEGRO_DEBUG(
            "*** Error: pickupRandomDropItem generated invalid item id\n");
        return;
    }

    // use script drop item data
    for (int n = 0; n < 5; n++) {
        // get a random #
        rate = Util::Random(1, 100);
        // is # within % drop rate?
        if (dropitems[n].rate <= rate) {
            // yes, set this itemid to scripted dropitem
            itemid = dropitems[n].id;

            // number of units to add
            numitems = Util::Random(1, dropitems[n].quantity);

            break;
        }
    }
    item = g_game->dataMgr->GetItemByID(itemid);
    if (item->IsArtifact())
        numitems = 1;

    // special-casing for artifacts.
    if (item->IsArtifact()) {
        Item itemInHold;
        int numInHold;
        g_game->gameState->m_items.Get_Item_By_ID(
            itemid, itemInHold, numInHold);

        // if the artifact is already in hold
        if (numInHold > 0) {
            Print(OFFICER_SCIENCE, "This stuff is useless!", RED, 1000);
            return;
        }

        // else we pick exactly one of that artifact
        g_game->gameState->m_items.AddItems(itemid, 1);
        Print(OFFICER_SCIENCE, "We found the " + item->name + "!", RED, 1000);

        // broadcast inventory change
        ALLEGRO_EVENT e = {
            .type = static_cast<unsigned int>(EVENT_CARGO_UPDATE)};
        g_game->broadcast_event(&e);

        return;
    }

    // Not an artifact
    int freeSpace = g_game->gameState->m_ship.getAvailableSpace();

    if (freeSpace <= 0) {
        Print(
            OFFICER_ENGINEER,
            "Sir, we don't have any space left in the cargo hold!",
            RED,
            1000);
        return;
    }

    // do not pick up more than available cargo space
    if (numitems > freeSpace)
        numitems = freeSpace;
    g_game->gameState->m_items.AddItems(itemid, numitems);

    // notify player
    os << "We picked up ";
    (numitems > 1) ? os << numitems << " cubic meters of " << item->name << "."
                   : os << "one cubic meter of " << item->name << ".";

    Print(OFFICER_SCIENCE, os.str(), YELLOW, 1000);

    // broadcast inventory change
    ALLEGRO_EVENT e = {.type = static_cast<unsigned int>(EVENT_CARGO_UPDATE)};
    g_game->broadcast_event(&e);
}

void
ModuleEncounter::pickupAsteroidMineral() {
    ostringstream os;
    int itemid, numitems;

    // mineral IDs range from 30 to 54.
    // FIXME: should use itemType instead of hardcoded IDs
    itemid = Util::Random(30, 54);
    numitems = Util::Random(1, 4);
    Item *item = g_game->dataMgr->GetItemByID(itemid);
    if (item == NULL) {
        ALLEGRO_DEBUG(
            "*** Error: pickupAsteroidMineral generated invalid item id\n");
        return;
    }

    int freeSpace = g_game->gameState->m_ship.getAvailableSpace();
    if (freeSpace <= 0) {
        Print(
            OFFICER_ENGINEER,
            "Sir, we don't have any space left in the cargo hold!",
            RED,
            1000);
        return;
    }

    // do not pick up more than available cargo space
    if (numitems > freeSpace)
        numitems = freeSpace;
    g_game->gameState->m_items.AddItems(itemid, numitems);

    // notify player
    os << "We picked up ";
    (numitems > 1) ? os << numitems << " cubic meters of " << item->name << "."
                   : os << "one cubic meter of " << item->name << ".";

    Print(OFFICER_SCIENCE, os.str(), YELLOW, 1000);

    // broadcast inventory change
    ALLEGRO_EVENT e = {.type = static_cast<unsigned int>(EVENT_CARGO_UPDATE)};
    g_game->broadcast_event(&e);
}

void
ModuleEncounter::applyDamageToShip(int damage, bool hullonly) {
    // reduce player's shield, armor, hull
    Ship ship = g_game->gameState->getShip();
    int shield = ship.getShieldCapacity();
    int armor = ship.getArmorIntegrity();
    int hull = ship.getHullIntegrity();

    // This is only used for a collision which immediately damages ship systems
    // regardless of a shield
    if (!hullonly) {
        // reduce shield level
        // is shield equipped and raised?
        if (ship.getShieldClass() > 0 && g_game->gameState->getShieldStatus()) {
            shield -= damage;
            ship.setShieldCapacity(shield);
            damage = 0 - shield;
            // there is still a small chance that ship systems get some damage
            // when shields are raised
            ship.damageRandomSystemOrCrew(10, 1, 5);
        }
    }

    // Armor is part of the hull

    // if shield gone, then hit the armor
    if (damage > 0) {
        if (ship.getArmorClass() > 0) {
            // get remaining damage from negative shield value
            armor -= damage;
            ship.setArmorIntegrity(armor);
            damage = 0 - armor;

            // damage random ship system or crew
            ship.damageRandomSystemOrCrew(10, 1, 5);
        }
    }

    // hit major systems
    if (damage > 0) {
        // get remaining damage from negative armor value
        ship.damageRandomSystemOrCrew(100, damage, damage);
        // ship destroyed?
        if (hull <= 0.0f)
            g_game->gameState->player.setAlive(false);
    }

    // if both weapon systems were destroyed, force them down
    if (ship.getLaserIntegrity() <= 1
        && ship.getMissileLauncherIntegrity() <= 1)
        g_game->gameState->setWeaponStatus(false);

    // if shields were damaged, force them down
    if (ship.getShieldIntegrity() <= 1)
        g_game->gameState->setShieldStatus(false);

    // save ship properties
    g_game->gameState->setShip(ship);
}

void
ModuleEncounter::combatTestPlayerCollision(CombatObject *other) {
    float shield, armor;
    Ship ship;

    // create scratch object for player ship
    CombatObject *player = new CombatObject();
    player->setX(playerScreen.x);
    player->setY(playerScreen.y);
    player->setFrameWidth(64);
    player->setFrameHeight(64);
    player->setMass(1.0);

    // reset player hit flag
    playerAttacked = false;

    if (player->CheckCollision(other)) {
        switch (other->getObjectType()) {
        case OBJ_ENEMYFIRE:
            Print(OFFICER_TACTICAL, "We're under attack!", RED, -1);
            player->ApplyImpact(other);
            ImpactPlayer(player, other);
            g_game->audioSystem->Play(m_hit1);
            other->setAlive(false);
            applyDamageToShip(other->getDamage());
            combatDoSmlExplosion(player, other);
            playerAttacked = true;
            break;

        case OBJ_ALIENSHIP:
            Print(OFFICER_NAVIGATION, "Collision alert!", YELLOW, 5000);
            applyDamageToShip(1, true);
            player->ApplyImpact(other);
            ImpactPlayer(player, other);
            playerAttacked = true;
            break;

        case OBJ_ASTEROID_BIG:
            Print(OFFICER_NAVIGATION, "Major collision alert!", YELLOW, 5000);
            applyDamageToShip(2, true);
            player->ApplyImpact(other);
            ImpactPlayer(player, other);
            playerAttacked = true;
            break;

        case OBJ_ASTEROID_MED:
            Print(OFFICER_NAVIGATION, "Collision alert!", YELLOW, 5000);
            applyDamageToShip(1, true);
            player->ApplyImpact(other);
            ImpactPlayer(player, other);
            playerAttacked = true;
            break;

        case OBJ_POWERUP_HEALTH: // fix hull
            Print(OFFICER_ENGINEER, "We got a Hull Powerup!", GREEN, 1000);
            other->setAlive(false);
            ship = g_game->gameState->getShip();
            ship.augHullIntegrity(20);
            g_game->gameState->setShip(ship);
            break;

        case OBJ_POWERUP_SHIELD: // fix shield
            Print(OFFICER_TACTICAL, "We got a Shield Powerup!", GREEN, 1000);
            other->setAlive(false);
            ship = g_game->gameState->getShip();
            shield = ship.getShieldCapacity()
                     + 20 * g_game->gameState->getShip().getShieldClass();
            if (shield > ship.getMaxShieldCapacity())
                shield = ship.getMaxShieldCapacity();
            ship.setShieldCapacity(shield);
            g_game->gameState->setShip(ship);
            break;

        case OBJ_POWERUP_ARMOR: // fix armor
            Print(OFFICER_ENGINEER, "We got an Armor Powerup!", GREEN, 1000);
            other->setAlive(false);
            ship = g_game->gameState->getShip();
            armor = ship.getArmorIntegrity()
                    + 20 * g_game->gameState->getShip().getArmorClass();
            if (armor > ship.getMaxArmorIntegrity())
                armor = ship.getMaxArmorIntegrity();
            ship.setArmorIntegrity(armor);
            g_game->gameState->setShip(ship);
            break;

        case OBJ_POWERUP_MINERAL_FROM_SHIP: // pickup mineral or artifact
                                            // from ship
            other->setAlive(false);
            pickupRandomDropItem();
            break;

        case OBJ_POWERUP_MINERAL_FROM_ASTEROID: // pickup mineral from
                                                // asteroid
            other->setAlive(false);
            pickupAsteroidMineral();
            break;
        }
    }

    delete player;
}

void
ModuleEncounter::damageAlienAttitude() {
    ostringstream os;

    // reduce attitude due to player's attack
    int attitude = g_game->gameState->getAlienAttitude() - 1;
    g_game->gameState->setAlienAttitude(attitude);

    // respond to the attack
    if (attitude < 30) {
        bFlagDoAttack = true;
        os.str("");
        os << "The " << alienName << " now despise us.";
        Print(OFFICER_COMMUNICATION, os.str(), STEEL, -1);
    } else if (attitude < 50) {
        bFlagDoAttack = true;
        os.str("");
        os << "The " << alienName << " now hate us.";
        Print(OFFICER_COMMUNICATION, os.str(), STEEL, -1);
    } else if (attitude < 60) {
        bFlagDoAttack = true;
        os.str("");
        os << "The " << alienName << " now distrust us.";
        Print(OFFICER_COMMUNICATION, os.str(), STEEL, -1);
    } else if (attitude < 80) {
        os.str("");
        os << "What are you doing!? The " << alienName << " trust us!";
        Print(OFFICER_COMMUNICATION, os.str(), STEEL, 5000);
    } else {
        os.str("");
        os << "Are you crazy!? The " << alienName << " are friendly!";
        Print(OFFICER_COMMUNICATION, os.str(), STEEL, 5000);
    }
}

void
ModuleEncounter::combatDoCollision(CombatObject *first, CombatObject *second) {
    int h, d, a, s;
    double damage_modifier;

    switch (first->getObjectType()) {
    case OBJ_ALIENSHIP:
    case OBJ_ASTEROID_BIG:
    case OBJ_ASTEROID_MED:
        switch (second->getObjectType()) {
        case OBJ_ALIENSHIP:
        case OBJ_ASTEROID_BIG:
        case OBJ_ASTEROID_MED:
            first->ApplyImpact(second);
            break;
        }
        break;

    case OBJ_PLAYERLASER:
    case OBJ_PLAYERMISSILE:
        switch (second->getObjectType()) {
        case OBJ_ALIENSHIP:
            {
                // player's projectile hits alien ship
                double d = Math::Distance(
                    playerShip->getX(),
                    playerShip->getY(),
                    second->getX(),
                    second->getY());
                if (d < 500)
                    g_game->audioSystem->Play(m_hit1);

                combatDoSmlExplosion(first, second);
                first->setAlive(false);

                s = second->getShieldStrength();
                a = second->getArmorStrength();
                h = second->getHealth();
                (first->getObjectType() == OBJ_PLAYERLASER)
                    ? damage_modifier = second->getLaserModifier() / 100.0
                    : damage_modifier = second->getMissileModifier() / 100.0;
                d = (int)(first->getDamage() * damage_modifier);

                // hit their shield
                if (d > 0 && s > 0) {
                    second->setShieldStrength(s - d);
                    d = 0 - (s - d);
                }
                // hit their armor
                if (d > 0 && a > 0) {
                    second->setArmorStrength(a - d);
                    d = 0 - (a - d);
                }
                // hit their hull
                if (d > 0)
                    second->setHealth(h - d);

                if (second->getHealth() < 0) {
                    //*** enemy ship destroyed--

                    second->setAlive(false);
                    combatDoBigExplosion(second);
                    combatDoPowerup(second);

                    // adjust attitude
                    damageAlienAttitude();

                    // award a skill point to the tactical officer
                    if (g_game->gameState->SkillUp(SKILL_TACTICAL)) {
                        auto tac = g_game->gameState->get_effective_officer(
                            OFFICER_TACTICAL);
                        Print(
                            tac->get_last_name()
                                + "-> I think I'm getting better at "
                                  "this.",
                            PURPLE,
                            5000);
                    }
                }

                // if alien doesn't realize it yet, tell them we're
                // hostile
                damageAlienAttitude();
            }

            break;

        case OBJ_ASTEROID_BIG:
            g_game->audioSystem->Play(m_hit1);
            combatDoSmlExplosion(first, second);
            first->setAlive(false);
            h = second->getHealth();
            d = (int)first->getDamage();
            second->setHealth(h - d);
            if (second->getHealth() < 0) {
                second->setAlive(false);
                combatDoMedExplosion(second);
                combatDoBreakAsteroid(second);
            }
            break;
        case OBJ_ASTEROID_MED:
            g_game->audioSystem->Play(m_hit1);
            combatDoSmlExplosion(first, second);
            first->setAlive(false);
            h = second->getHealth();
            d = (int)first->getDamage();
            second->setHealth(h - d);
            if (second->getHealth() < 0) {
                second->setAlive(false);
                combatDoMedExplosion(second);
                // launch random powerup/mineral
                combatDoPowerup(second);
            }
            break;
        }
        break;

    case OBJ_ENEMYFIRE:
        switch (second->getObjectType()) {
        case OBJ_ASTEROID_BIG:
            g_game->audioSystem->Play(m_hit1);
            combatDoSmlExplosion(first, second);
            first->setAlive(false);
            h = second->getHealth();
            d = (int)first->getDamage();
            second->setHealth(h - d);
            if (second->getHealth() < 0) {
                second->setAlive(false);
                combatDoMedExplosion(second);
                combatDoBreakAsteroid(second);
            }
            break;
        case OBJ_ASTEROID_MED:
            g_game->audioSystem->Play(m_hit1);
            combatDoSmlExplosion(first, second);
            first->setAlive(false);
            h = second->getHealth();
            d = (int)first->getDamage();
            second->setHealth(h - d);
            if (second->getHealth() < 0) {
                second->setAlive(false);
                combatDoMedExplosion(second);
            }
            break;
        }
        break;
    }
}

void
ModuleEncounter::DoAlienShipCombat(CombatObject *ship) {
    double dist;
    double targetAngle;
    double x, y, pgx, pgy;

    // keep ship inside the arena
    Rect bounds = getBoundary();
    if (!bounds.contains(ship->getX(), ship->getY())) {
        ship->AllStop();
        // keep ship inside boundary--horizontal
        if (ship->getX() < bounds.left) {
            ship->setX(bounds.left);
            ship->setFaceAngle(Util::WrapValue(ship->getFaceAngle() + 180.0));
        } else if (ship->getX() + ship->getFrameWidth() > bounds.right) {
            ship->setX(bounds.right - ship->getFrameWidth());
            ship->setFaceAngle(Util::WrapValue(ship->getFaceAngle() + 180.0));
        }
        // keep ship inside boundary--vertical
        if (ship->getY() < bounds.top) {
            ship->setY(bounds.top);
            ship->setFaceAngle(Util::WrapValue(ship->getFaceAngle() + 180.0));
        } else if (ship->getY() + ship->getFrameHeight() > bounds.bottom) {
            ship->setY(bounds.bottom - ship->getFrameHeight());
            ship->setFaceAngle(Util::WrapValue(ship->getFaceAngle() + 180.0));
        }
    }

    // get current alien race attitude toward player
    AlienRaces region = g_game->gameState->player.getGalacticRegion();
    int attitude = g_game->gameState->alienAttitudes[region];

    // if alien is really angry or player starts attacking...
    if (attitude < 31 || bFlagDoAttack) {
        ship->setBehavior(BEHAVIOR_ATTACK);
    } else {
        // this alien is friendly
        ship->setBehavior(BEHAVIOR_WANDER);
    }

    // get current positions
    x = ship->getX() + 32;
    y = ship->getY() + 32;
    pgx = playerGlobal.x + SCREEN_WIDTH / 2 + 32;
    pgy = playerGlobal.y + effectiveScreenHeight() / 2 + 32;

    int missileRange = (int)g_game->getGlobalNumber("ALIEN_MISSILE_RANGE");
    int laserRange = (int)g_game->getGlobalNumber("ALIEN_LASER_RANGE");

    bool missileIsGreaterRange = missileRange > laserRange; // jjh
    int longRange = missileIsGreaterRange ? missileRange : laserRange;
    int shortRange = missileIsGreaterRange ? laserRange : missileRange;

    // move ship based on behavior
    switch (ship->getBehavior()) {
    case BEHAVIOR_WANDER:
        // normally just move at half speed
        if (ship->getRelativeSpeed() < ship->getMaxVelocity() - 1.0)
            ship->ApplyThrust();
        break;
        /*
                        case BEHAVIOR_FLEE:
                                //if in range, speed up and evade
                                dist = Math::Distance(x,y,pgx,pgy);
                                if (dist < safetyDistance) {
                                        //attempt to flee away from player
                                        targetAngle =
           Math::AngleToTarget(x,y,pgx,pgy); targetAngle =
           Math::wrapAngleDegs(90.0 + Math::toDegrees(targetAngle) );
                                        behindAngle = Math::wrapAngleDegs(
           ship->getFaceAngle() + 180.0 );
                                        //flee to the right
                                        if (targetAngle <= behindAngle) {
                                                ship->TurnRight();
                                                ship->ApplyThrust();
                                        }
                                        //flee to the left
                                        else if (targetAngle > behindAngle)
           { ship->TurnLeft(); ship->ApplyThrust();
                                        }
                                }
                                else {
                                        //out of danger, so start attacking
           again ship->setBehavior( BEHAVIOR_ATTACK );
                                }
                                break;
        */
    case BEHAVIOR_ATTACK: // jjh
        // attempt to turn toward player
        targetAngle = Math::AngleToTarget(x, y, pgx, pgy);
        targetAngle = Math::wrapAngleDegs(90.0 + Math::toDegrees(targetAngle));
        // need to turn left
        if (targetAngle < Math::wrapAngleDegs(ship->getFaceAngle() - 1.0)) {
            ship->TurnLeft();
        }
        // need to turn right
        else if (
            targetAngle > Math::wrapAngleDegs(ship->getFaceAngle() + 1.0)) {
            ship->TurnRight();
        }
        // we're pointed at player, fire!
        else {
            // get distance to player
            dist = Math::Distance(x, y, pgx, pgy);

            // is target out of range?
            if (dist > longRange) {
                // close in to firing range
                ship->ApplyThrust();
            }
            // is target long range? fire long range weapon only
            else if (dist > shortRange) {
                missileIsGreaterRange ? enemyFireMissile(ship)
                                      : enemyFireLaser(ship);
            }
            // is target close range? fire both
            else {
                ship->ApplyBraking();
                enemyFireLaser(ship);
                enemyFireMissile(ship);
            }
        }

        break;
    }
}

int
ModuleEncounter::getShipCount() {
    return count_if(combatObjects.begin(), combatObjects.end(), [](auto &i) {
        return i->isAlive() && i->getObjectType() == OBJ_ALIENSHIP;
    });
}

Rect
ModuleEncounter::getBoundary() {
    return Rect(0, 0, TILESACROSS * TILESIZE, TILESDOWN * TILESIZE);
}

// used by sensor scan to find an alien ship
CombatObject *
ModuleEncounter::GetFirstAlienShip() {
    auto alien =
        find_if(combatObjects.begin(), combatObjects.end(), [](auto &i) {
            return i->isAlive() && i->getObjectType() == OBJ_ALIENSHIP;
        });
    if (alien != combatObjects.end()) {
        return alien->get();
    }
    return nullptr;
}

void
ModuleEncounter::Combat_Update() {
    // player ship destroyed?
    if (!g_game->gameState->player.getAlive()) {
        if (deathState == 0) {
            deathState++;
            g_game->printout("YOUR SHIP HAS BEEN DESTROYED!!", RED, 0);
            set_modal_child(make_shared<MessageBoxWindow>(
                "",
                "YOUR SHIP HAS BEEN DESTROYED!!",
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
        return;
    }

    double x, y;

    // keep player inside boundary
    Rect bounds = getBoundary();
    if (playerGlobal.x < bounds.left) {
        playerGlobal.x = bounds.left;
        playerShip->allstop();
    } else if (playerGlobal.x > bounds.right) {
        playerGlobal.x = bounds.right;
        playerShip->allstop();
    }

    if (playerGlobal.y < bounds.top) {
        playerGlobal.y = bounds.top;
        playerShip->allstop();
    } else if (playerGlobal.y > bounds.bottom) {
        playerGlobal.y = bounds.bottom;
        playerShip->allstop();
    }

    if (g_game->gameState->m_ship.getFuel() == 0) {
        flag_thrusting = flag_nav = false;
        flag_rotation = 0;
    }
    // slow down the ship automatically
    if (!flag_nav)
        playerShip->applybraking();

    if (flag_thrusting) {
        playerShip->applythrust();
    }
    if (flag_rotation == 1) {
        playerShip->turnright();
    } else if (flag_rotation == -1) {
        playerShip->turnleft();
    }
    if (firingLaser) {
        fireLaser();
    }
    if (firingMissile) {
        fireMissile();
    }
    // update all combat objects
    for (int i = 0; i < (int)combatObjects.size(); i++) {
        // fast/untimed update
        combatObjects[i]->Update();
        x = combatObjects[i]->getX();
        y = combatObjects[i]->getY();

        // perform fast updates as needed
        switch (combatObjects[i]->getObjectType()) {
        case OBJ_ALIENSHIP:
            shipcount++;
            DoAlienShipCombat(combatObjects[i].get());
            break;

        case OBJ_ASTEROID_BIG:
        case OBJ_ASTEROID_MED:
            // warp asteroid around edges of combat arena
            if (x < bounds.left)
                combatObjects[i]->setX(bounds.right - 10);
            if (x > bounds.right)
                combatObjects[i]->setX(bounds.left + 10);
            if (y < bounds.top)
                combatObjects[i]->setY(bounds.bottom - 10);
            if (y > bounds.bottom)
                combatObjects[i]->setY(bounds.top + 10);
            break;

        case OBJ_PLAYERLASER:
        case OBJ_PLAYERMISSILE:
        case OBJ_ENEMYFIRE:
            // remove bullet if it reaches boundary
            if (!bounds.contains(x, y))
                combatObjects[i]->setAlive(false);
            break;
        }

        // perform collision testing
        for (int other = 0; other < (int)combatObjects.size(); other++) {
            if (other != i && combatObjects[other]->isAlive()) {
                if (combatObjects[i]->CheckCollision(
                        combatObjects[other].get())) {
                    combatDoCollision(
                        combatObjects[i].get(), combatObjects[other].get());
                }
            }
        }
    }

    // enemy ships destroyed?
    if (shipcount == 0) {
        Print(
            OFFICER_TACTICAL,
            "All enemy ships have been destroyed!",
            STEEL,
            10000);
        Print(
            OFFICER_NAVIGATION,
            "Captain, we can return to hyperspace when you're ready.",
            GREEN,
            10000);
    } else {
        // check for sensor scan/analysis
        if (scanStatus == 1) // scan
        {
            // find first alien ship in use
            CombatObject *alien = GetFirstAlienShip();
            if (alien != NULL) {
                Print(OFFICER_SCIENCE, "Scanning alien ship...", STEEL, 10000);
                if (scanTimer.stopwatch(4000)) {
                    Print(
                        OFFICER_SCIENCE,
                        "Scan complete, ready for analysis.",
                        STEEL,
                        10000);
                    scanStatus = 2;
                }
            } else {
                Print(
                    OFFICER_SCIENCE,
                    "There are no alien ships to scan.",
                    YELLOW,
                    10000);
                scanStatus = 0;
            }
        } else if (scanStatus == 3) // analysis
        {
            scanStatus = 0;

            // find first alien ship in use
            CombatObject *alien = GetFirstAlienShip();
            if (alien != NULL) {
                string analysis = "The ship is ";
                int mass = script->getGlobalNumber("mass");
                if (mass <= 2)
                    analysis += "tiny";
                else if (mass <= 4)
                    analysis += "small";
                else if (mass <= 6)
                    analysis += "medium";
                else if (mass <= 8)
                    analysis += "large";
                else
                    analysis += "huge";
                analysis += " in size, with a ";

                int engine = script->getGlobalNumber("engineclass");
                if (engine <= 2)
                    analysis += "slow engine";
                else if (engine <= 4)
                    analysis += "medium engine";
                else
                    analysis += "fast engine";
                analysis += ". The hull is ";

                int armor = script->getGlobalNumber("armorclass");
                if (armor == 0)
                    analysis += "not armored";
                else if (armor <= 2)
                    analysis += "lightly armored";
                else if (armor <= 4)
                    analysis += "moderately armored";
                else
                    analysis += "heavily armored";

                int shield = script->getGlobalNumber("shieldclass");
                if (shield == 0)
                    analysis += ", but this ship is not equipped with ";
                else {
                    analysis += " and it is equipped with ";
                    if (shield <= 2)
                        analysis += "light";
                    else if (shield <= 4)
                        analysis += "medium";
                    else
                        analysis += "heavy";
                }
                analysis += " shields. ";

                int laserModifier = script->getGlobalNumber("laser_modifier");
                int missileModifier =
                    script->getGlobalNumber("missile_modifier");
                if (laserModifier < missileModifier)
                    analysis += "Defensive capabilities are especially "
                                "effective against laser beams.";
                else if (missileModifier < laserModifier)
                    analysis += "Defensive capabalities are especially "
                                "effective against missiles.";

                analysis += " Offensive capabilities include ";

                int laser = script->getGlobalNumber("laserclass");
                if (laser == 0)
                    analysis += "no lasers";
                else if (laser <= 2)
                    analysis += "light lasers";
                else if (laser <= 4)
                    analysis += "medium lasers";
                else
                    analysis += "heavy lasers";
                analysis += ", ";

                int missile = script->getGlobalNumber("missileclass");
                if (missile > 0)
                    analysis += " and ";
                if (missile == 0)
                    analysis += "but no missiles";
                else if (missile <= 2)
                    analysis += "low-yield missiles";
                else if (missile <= 4)
                    analysis += "medium-yield missiles";
                else
                    analysis += "high-yield missiles";
                analysis += ". ";

                int alienPow = engine + armor + shield + laser + missile;
                int playerPow =
                    g_game->gameState->m_ship.getEngineClass()
                    + g_game->gameState->m_ship.getArmorClass()
                    + g_game->gameState->m_ship.getShieldClass()
                    + g_game->gameState->m_ship.getLaserClass()
                    + g_game->gameState->m_ship.getMissileLauncherClass();
                if (alienPow > playerPow * 2)
                    analysis += "They greatly outclass us.";
                else if (alienPow > playerPow)
                    analysis += "They moderately outclass us.";
                else if (alienPow == playerPow)
                    analysis += "We're comparably equipped.";
                else if (alienPow * 2 < playerPow)
                    analysis += "We greatly outclass them.";
                else if (alienPow < playerPow)
                    analysis += "We moderately outclass them.";

                Print(
                    OFFICER_SCIENCE,
                    "Here is my analysis of the alien ship:",
                    STEEL,
                    10000);
                g_game->printout(analysis, WHITE, 10000);

            } else {
                Print(
                    OFFICER_SCIENCE,
                    "Sorry, Captain, I don't understand the "
                    "sensor results.",
                    STEEL,
                    10000);
            }
        }
    }

    // remove dead combat objects
    combatObjects.erase(
        remove_if(
            combatObjects.begin(),
            combatObjects.end(),
            [](auto &i) { return !i->isAlive(); }),
        combatObjects.end());

    // slowly recharge ship's shield
    Ship ship = g_game->gameState->getShip();
    int shield = ship.getShieldClass();
    if (shield > 0) {
        // increase shield by 0.001 per frame
        float strength = ship.getShieldCapacity() + (float)shield / 100.0f;
        if (strength > ship.getMaxShieldCapacity())
            strength = ship.getMaxShieldCapacity();
        ship.setShieldCapacity(strength);
        g_game->gameState->setShip(ship);
    }
}

// Combat_Draw functions as a timed update and draw routine
void
ModuleEncounter::Combat_Draw(ALLEGRO_BITMAP *target) {
    double tx, ty;
    double rx, ry;
    float angle;

    // Loop through all combat objects
    for (int i = 0; i < (int)combatObjects.size(); ++i) {
        if (combatObjects[i]->isAlive()) {
            // update sprite movement, animation
            combatObjects[i]->TimedUpdate();

            // save absolute position
            tx = combatObjects[i]->getX();
            ty = combatObjects[i]->getY();

            // calculate position relative to player
            rx = tx - g_game->gameState->player.posCombat.x;
            ry = ty - g_game->gameState->player.posCombat.y;

            // is this sprite in range of the screen?
            if (rx > 0 - combatObjects[i]->getFrameWidth() && rx < SCREEN_WIDTH
                && ry > 0 - combatObjects[i]->getFrameHeight()
                && ry < effectiveScreenHeight()) {
                // set relative location to simplify collision code
                combatObjects[i]->setPos(rx, ry);

                // perform special collision test with player ship
                combatTestPlayerCollision(combatObjects[i].get());

                // special handling for each object based on type
                switch (combatObjects[i]->getObjectType()) {
                // draw objects requiring rotation
                case OBJ_ALIENSHIP:
                case OBJ_ASTEROID_BIG:
                case OBJ_ASTEROID_MED:
                case OBJ_PLAYERLASER:
                case OBJ_PLAYERMISSILE:
                case OBJ_ENEMYFIRE:
                    angle = combatObjects[i]->getFaceAngle();
                    combatObjects[i]->drawframe_rotate(target, (int)angle);
                    break;

                // draw objects that do not require rotation
                case OBJ_POWERUP_HEALTH:
                case OBJ_POWERUP_SHIELD:
                case OBJ_POWERUP_ARMOR:
                case OBJ_POWERUP_MINERAL_FROM_SHIP:
                case OBJ_POWERUP_MINERAL_FROM_ASTEROID:
                    combatObjects[i]->drawframe(target);
                    break;

                // special case explosion only animates once
                case OBJ_EXPLOSION:
                    combatObjects[i]->drawframe(target);

                    // delete explosion when it reaches last frame
                    if (combatObjects[i]->getCurrFrame()
                        == combatObjects[i]->getTotalFrames() - 1)
                        combatObjects[i]->setAlive(false);
                    break;
                }

                // restore absolute position
                combatObjects[i]->setX(tx);
                combatObjects[i]->setY(ty);
            }
        }
    }

    // draw the player's shield
    if (g_game->gameState->getShieldStatus()) {
        if (g_game->gameState->getShip().getShieldCapacity() > 0.0) {
            shield->animate();
            shield->drawframe(target);
        } else {
            g_game->gameState->setShieldStatus(false);
            Print(OFFICER_TACTICAL, "Sir! Shields are depleted!", RED, 6000);
        }
    }
}

// in "show control" mode the player ship is centered on the upper 1024x512 part
// of the screen in "hide control" mode the player ship is centered on the whole
// 1024x768 screen we need to adjust the y coordinate of all the objects
// relatively to that of the player ship
void
ModuleEncounter::adjustVerticalCoords(int delta) {
    // adjust the player ship & shield sprites
    playerShip->setY(playerShip->getY() + delta);
    shield->setY(shield->getY() + delta);

    // adjust everything else
    for (int i = 0; i < (int)combatObjects.size(); ++i) {
        if (combatObjects[i]->isAlive())
            combatObjects[i]->setY(combatObjects[i]->getY() + delta);
    }
}

void
ModuleEncounter::DrawMinimap(ALLEGRO_BITMAP * /*target*/) {
    al_set_target_bitmap(m_minimap->get_bitmap().get());
    al_clear_to_color(BLACK);

    for (int i = 0; i < (int)combatObjects.size(); i++) {
        int x = (int)(combatObjects[i]->getX()) / 78;
        int y = (int)(combatObjects[i]->getY()) / 76;

        switch (combatObjects[i]->getObjectType()) {
        case OBJ_ALIENSHIP:
            al_draw_rectangle(x - 1, y - 1, x + 1, y + 1, STEEL, 1);
            break;
        case OBJ_ASTEROID_BIG:
            al_draw_circle(x, y, 3, KHAKI, 1);
            break;
        case OBJ_ASTEROID_MED:
            al_draw_circle(x, y, 2, DKKHAKI, 1);
            break;
        case OBJ_PLAYERMISSILE:
        case OBJ_ENEMYFIRE:
            al_put_pixel(x, y, RED);
            break;
        case OBJ_POWERUP_HEALTH:
        case OBJ_POWERUP_SHIELD:
        case OBJ_POWERUP_ARMOR:
            al_draw_triangle(x, y - 2, x - 2, y + 2, x + 2, y + 2, GREEN, 1);
            break;
        case OBJ_POWERUP_MINERAL_FROM_SHIP:
        case OBJ_POWERUP_MINERAL_FROM_ASTEROID:
            al_draw_triangle(x, y - 2, x - 2, y + 2, x + 2, y + 2, YELLOW, 1);
            break;
        }
    }

    // show player on minimap
    int px =
        (int)((g_game->gameState->player.posCombat.x + SCREEN_WIDTH / 2) / 78);
    int py =
        (int)((g_game->gameState->player.posCombat.y + effectiveScreenHeight() / 2) / 76);
    al_draw_circle(px, py, 2, GREEN, 1);
}

void
ModuleEncounter::AddCombatObject(CombatObject *CObject) {
    combatObjects.push_back(unique_ptr<CombatObject>(CObject));
}

void
ModuleEncounter::enemyFireLaser(CombatObject *ship) {
    static Timer timer;
    int laserDamage = ship->getLaserDamage();

    // make sure this ship has a primary weapon
    if (laserDamage == 0)
        return;

    // reloaded yet?
    int time = timer.getTimer();
    int rate = ship->getLaserFiringRate();
    int counter = ship->getLaserFiringTimer();
    if (counter == 0 || time > counter + rate) {
        // reset reload timer
        ship->setLaserFiringTimer(time);
    } else
        return;

    // create the laser sprite
    CombatObject *laser = new CombatObject();
    laser->setObjectType(OBJ_ENEMYFIRE);

    double x = ship->getX() + ship->getFrameWidth() / 2 - 8;
    double y = ship->getY() + ship->getFrameHeight() / 2 - 8;
    float velx = playerShip->getVelocityX();
    float vely = playerShip->getVelocityY();
    int angle = (int)ship->getFaceAngle();
    createLaser(laser, x, y, velx, vely, angle, laserDamage);
}

void
ModuleEncounter::enemyFireMissile(CombatObject *ship) {
    static Timer timer;
    int missileDamage = ship->getMissileDamage();

    // bail if ship has no missiles
    if (missileDamage == 0)
        return;

    // reloaded yet?
    int time = timer.getTimer();
    int rate = ship->getMissileFiringRate();
    int counter = ship->getMissileFiringTimer();
    if (counter == 0 || time > counter + rate) {
        // reset reload timer
        ship->setMissileFiringTimer(time);
    } else
        return;

    // create the missile sprite
    CombatObject *missile = new CombatObject();
    missile->setObjectType(OBJ_ENEMYFIRE);

    double x = ship->getX() + ship->getFrameWidth() / 2 - 8;
    double y = ship->getY() + ship->getFrameHeight() / 2 - 8;
    float velx = ship->getVelX();
    float vely = ship->getVelY();
    int angle = (int)ship->getFaceAngle();
    createMissile(missile, x, y, velx, vely, angle, missileDamage);
}

// fire primary weapon
void
ModuleEncounter::fireLaser() {
    static int fireLast = 0;

    // get player's laser props
    int laserClass = g_game->gameState->getShip().getLaserClass();
    int laserDamage = g_game->gameState->getShip().getLaserDamage();
    int fireRate = g_game->gameState->getShip().getLaserFiringRate();

    if (laserClass == 0 || laserDamage == 0) {
        Print(OFFICER_TACTICAL, "We do not have a laser", YELLOW, -1);
        return;
    }

    // are weapons armed?
    if (!g_game->gameState->getWeaponStatus()) {
        Print(OFFICER_TACTICAL, "The weapons are not armed yet!", YELLOW, 5000);
        return;
    }

    // slow down firing rate
    if (g_game->globalTimer.getTimer() > fireLast + fireRate) {
        fireLast = g_game->globalTimer.getTimer();
    } else
        return;

    // create the laser sprite
    CombatObject *laser = new CombatObject();
    laser->setObjectType(OBJ_PLAYERLASER);

    double x = g_game->gameState->player.posCombat.x + SCREEN_WIDTH / 2 - 8;
    double y =
        g_game->gameState->player.posCombat.y + effectiveScreenHeight() / 2 - 8;
    float velx = playerShip->getVelocityX();
    float vely = playerShip->getVelocityY();
    int angle = (int)playerShip->getRotationAngle();
    createLaser(laser, x, y, velx, vely, angle, laserDamage);
}

// fire secondary weapon
void
ModuleEncounter::fireMissile() {
    static int fireLast = 0;

    // get player's missile props
    int missileClass = g_game->gameState->getShip().getMissileLauncherClass();
    int missileDamage = g_game->gameState->getShip().getMissileLauncherDamage();
    int fireRate = g_game->gameState->getShip()
                       .getMissileLauncherFiringRate(); // used to be 1000

    if (missileClass == 0 || missileDamage == 0) {
        Print(OFFICER_TACTICAL, "We have no missile launcher", YELLOW, -1);
        return;
    }

    // are weapons armed?
    if (!g_game->gameState->getWeaponStatus()) {
        Print(OFFICER_TACTICAL, "The weapons are not armed!", YELLOW, 5000);
        return;
    }

    // slow down firing rate
    if (g_game->globalTimer.getTimer() > fireLast + fireRate) {
        fireLast = g_game->globalTimer.getTimer();
    } else
        return;

    // create the missile sprite
    CombatObject *missile = new CombatObject();
    missile->setObjectType(OBJ_PLAYERMISSILE);

    double x =
        (int)g_game->gameState->player.posCombat.x + SCREEN_WIDTH / 2 - 8;
    double y = (int)g_game->gameState->player.posCombat.y
               + effectiveScreenHeight() / 2 - 8;
    float velx = playerShip->getVelocityX();
    float vely = playerShip->getVelocityY();
    int angle = (int)playerShip->getRotationAngle();
    createMissile(missile, x, y, velx, vely, angle, missileDamage);
}

void
ModuleEncounter::createLaser(
    CombatObject *laser,
    double x,
    double y,
    float velx,
    float vely,
    int angle,
    int laserDamage) {
    double duration = g_game->getGlobalNumber("LASER_DURATION");
    double speed = g_game->getGlobalNumber("LASER_SPEED");

    laser->setImage(images[I_LASER_BEAM]);
    laser->setTotalFrames(1); // was 4--too fast for animation
    laser->setAnimColumns(1); // was 4
    laser->setFrameWidth(16);
    laser->setFrameHeight(16);
    laser->setFrameDelay(0); // no animation
    laser->setMass(0.1);
    laser->setAlpha(true);
    laser->setExpireDuration(duration); // was 800 for alien, 1000 for player
    laser->setDamage(laserDamage);

    // set projectile's direction and velocity
    laser->setFaceAngle(angle);
    laser->setVelX(
        velx
        + Sprite::calcAngleMoveX(Math::wrapAngleDegs(angle - 90))
              * speed); // alien laser used to have * 20.0, player laser
                        // used to not have +velx
    laser->setVelY(
        vely
        + Sprite::calcAngleMoveY(Math::wrapAngleDegs(angle - 90))
              * speed); // alien laser used to have * 20.0, player laser
                        // used to not have +vely

    // set projectile's starting position
    laser->setPos(x, y);

    AddCombatObject(laser);

    // missile sound effect
    g_game->audioSystem->Play(m_fire1);
}

void
ModuleEncounter::createMissile(
    CombatObject *missile,
    double x,
    double y,
    float velx,
    float vely,
    int angle,
    int missileDamage) {
    double duration = g_game->getGlobalNumber("MISSILE_DURATION");
    double speed = g_game->getGlobalNumber("MISSILE_SPEED");

    missile->setImage(images[I_RED_BOLT]);
    missile->setTotalFrames(30);
    missile->setAnimColumns(10);
    missile->setFrameWidth(16);
    missile->setFrameHeight(16);
    missile->setFrameDelay(2);
    missile->setMass(0.2);
    missile->setAlpha(true);
    missile->setExpireDuration(duration);
    missile->setDamage(missileDamage);

    // set projectile's direction and velocity
    missile->setFaceAngle(angle);
    missile->setVelX(
        velx
        + Sprite::calcAngleMoveX(Math::wrapAngleDegs(angle - 90))
              * speed); // was *20.0 for alien missiles
    missile->setVelY(
        vely
        + Sprite::calcAngleMoveY(Math::wrapAngleDegs(angle - 90))
              * speed); // was *20.0 for alien missiles

    // set projectile's starting position
    missile->setPos(x, y);

    AddCombatObject(missile);

    // missile sound effect
    g_game->audioSystem->Play(m_fire2);
}

void
ModuleEncounter::combatDoBigExplosion(CombatObject *victim) {
    // play explosion sound
    g_game->audioSystem->Play(m_hit2);

    // create explosion sprite
    CombatObject *exp = new CombatObject();
    exp->setImage(images[I_EXPLOSION_30_128]);
    exp->setAlpha(true);
    exp->setObjectType(OBJ_EXPLOSION);
    exp->setTotalFrames(30);
    exp->setAnimColumns(6);
    exp->setFrameWidth(128);
    exp->setFrameHeight(128);
    exp->setFrameDelay(1);
    exp->setExpireDuration(2000);

    int x, y, x1, y1, w1, h1, w2, h2, Xoff, Yoff;
    x1 = (int)victim->getX();
    w1 = (int)victim->getFrameWidth();
    w2 = exp->getFrameWidth();
    Xoff = (w1 - w2) / 2;

    y1 = (int)victim->getY();
    h1 = victim->getFrameHeight();
    h2 = exp->getFrameHeight();
    Yoff = (h1 - h2) / 2;

    x = x1 + Xoff;
    y = y1 + Yoff;
    exp->setPos(x, y);

    AddCombatObject(exp);
}

void
ModuleEncounter::combatDoMedExplosion(CombatObject *victim) {
    // play explosion sound
    g_game->audioSystem->Play(m_hit2);

    // create explosion sprite
    CombatObject *exp = new CombatObject();
    exp->setImage(images[I_EXPLOSION_30_64]);
    exp->setAlpha(true);
    exp->setObjectType(OBJ_EXPLOSION);
    exp->setTotalFrames(30);
    exp->setAnimColumns(6);
    exp->setFrameWidth(64);
    exp->setFrameHeight(64);
    exp->setFrameDelay(1);
    exp->setExpireDuration(2000);

    int x, y, x1, y1, w1, h1, w2, h2, Xoff, Yoff;
    x1 = (int)victim->getX();
    w1 = (int)victim->getFrameWidth();
    w2 = exp->getFrameWidth();
    Xoff = (w1 - w2) / 2;

    y1 = (int)victim->getY();
    h1 = victim->getFrameHeight();
    h2 = exp->getFrameHeight();
    Yoff = (h1 - h2) / 2;

    x = x1 + Xoff;
    y = y1 + Yoff;
    exp->setPos(x, y);

    AddCombatObject(exp);
}

void
ModuleEncounter::combatDoSmlExplosion(
    CombatObject *victim,
    CombatObject *source) {
    // play explosion sound
    g_game->audioSystem->Play(m_hit1);

    // create explosion sprite
    CombatObject *exp = new CombatObject();
    exp->setImage(images[I_EXPLOSION_30_48]);
    exp->setAlpha(true);
    exp->setObjectType(OBJ_EXPLOSION);
    exp->setTotalFrames(30);
    exp->setAnimColumns(6);
    exp->setFrameWidth(48);
    exp->setFrameHeight(48);
    exp->setFrameDelay(1);
    exp->setExpireDuration(3000);

    // unlike med/big, small exp is for projectile impacts, so it's a bit random
    int vcx = (int)(victim->getX() + victim->getFrameWidth() / 2);
    int vcy = (int)(victim->getY() + victim->getFrameHeight() / 2);
    int ecx = exp->getFrameWidth() / 2;
    int ecy = exp->getFrameHeight() / 2;
    int rx = Util::Random(8, 16) - 8;
    int ry = Util::Random(8, 16) - 8;
    exp->setPos(vcx - ecx + rx, vcy - ecy + ry);

    // set explosion's velocity
    exp->setVelX(source->getVelX());
    exp->setVelY(source->getVelY());

    AddCombatObject(exp);
}

// when large asteroid is destroyed, it breaks up into smaller ones
// the small ones release powerups!
void
ModuleEncounter::combatDoBreakAsteroid(CombatObject *victim) {
    int r = Util::Random(4, 9);

    for (int n = 0; n < r; n++) {
        // play explosion sound
        g_game->audioSystem->Play(m_hit1);

        // create explosion sprite
        CombatObject *exp = new CombatObject();
        exp->setImage(images[I_SMLASTEROID]);
        exp->setObjectType(21); // sub-asteroids
        exp->setAnimColumns(8);
        exp->setTotalFrames(64);
        exp->setFrameWidth(60);
        exp->setFrameHeight(60);
        exp->setFrameDelay(Util::Random(1, 4));
        exp->setCurrFrame(Util::Random(1, 60));
        exp->setHealth(10);
        exp->setMass(3.0);

        double x = victim->getX();
        double y = victim->getY();
        exp->setPos(x, y);

        exp->setVelX(Util::Random(0, 4) - 2);
        exp->setVelY(Util::Random(0, 4) - 2);

        exp->setRotation(Util::Random(0, 8) - 4);

        AddCombatObject(exp);
    }
}

void
ModuleEncounter::combatDoPowerup(CombatObject *victim) {
    double vcx, vcy, velx, vely;

    // create new powerup sprite
    CombatObject *pow = new CombatObject();

    // set type of powerup
    int r = Util::Random(1, 7);
    switch (r) {
    case 1: // health
        pow->setImage(images[I_POWERUP_HEALTH]);
        pow->setObjectType(OBJ_POWERUP_HEALTH);
        pow->setAlpha(true);
        pow->setTotalFrames(9);
        pow->setAnimColumns(9);
        pow->setFrameWidth(32);
        pow->setFrameHeight(32);
        break;
    case 2: // shield
        pow->setImage(images[I_POWERUP_SHIELD]);
        pow->setObjectType(OBJ_POWERUP_SHIELD);
        pow->setAlpha(true);
        pow->setTotalFrames(9);
        pow->setAnimColumns(9);
        pow->setFrameWidth(32);
        pow->setFrameHeight(32);
        break;
    case 3: // armor
        pow->setImage(images[I_POWERUP_ARMOR]);
        pow->setObjectType(OBJ_POWERUP_ARMOR);
        pow->setAlpha(true);
        pow->setTotalFrames(9);
        pow->setAnimColumns(9);
        pow->setFrameWidth(32);
        pow->setFrameHeight(32);
        break;
    case 4: // mineral
    case 5:
    case 6:
    case 7:
        pow->setImage(images[I_POWERUP_MINERAL]);
        (victim->getObjectType() == OBJ_ALIENSHIP)
            ? pow->setObjectType(OBJ_POWERUP_MINERAL_FROM_SHIP)
            : pow->setObjectType(OBJ_POWERUP_MINERAL_FROM_ASTEROID);
        pow->setAlpha(false);
        pow->setTotalFrames(7);
        pow->setAnimColumns(7);
        pow->setFrameWidth(30);
        pow->setFrameHeight(24);
        break;
    }

    // set shared properties
    pow->setExpireDuration(45000);
    pow->setFrameDelay(3);

    // set position
    vcx = victim->getX() + victim->getFrameWidth() / 2;
    vcy = victim->getY() + victim->getFrameHeight() / 2;
    pow->setPos(vcx - 32, vcy - 32);

    // set velocity
    velx = (double)(Util::Random(1, 3) - 2);
    vely = (double)(Util::Random(1, 3) - 2);
    pow->setVelX(velx);
    pow->setVelY(vely);

    // add the powerup
    AddCombatObject(pow);
}

void
ModuleEncounter::sendGlobalsToScript() {
    script->setGlobalNumber("number_of_actions", number_of_actions);
    script->setGlobalNumber("goto_question", goto_question);
    script->setGlobalNumber("ATTITUDE", g_game->gameState->getAlienAttitude());
    script->setGlobalString("POSTURE", g_game->gameState->playerPosture);
    script->setGlobalNumber("player_money", g_game->gameState->getCredits());
    script->setGlobalNumber("plot_stage", g_game->gameState->getPlotStage());
    script->setGlobalNumber(
        "active_quest", g_game->gameState->getActiveQuest());
    script->setGlobalString(
        "player_profession", g_game->gameState->getProfessionString());
    script->setGlobalNumber(
        "ship_engine_class", g_game->gameState->getShip().getEngineClass());
    script->setGlobalNumber(
        "ship_shield_class", g_game->gameState->getShip().getShieldClass());
    script->setGlobalNumber(
        "ship_armor_class", g_game->gameState->getShip().getArmorClass());
    script->setGlobalNumber(
        "ship_laser_class", g_game->gameState->getShip().getLaserClass());
    script->setGlobalNumber(
        "ship_missile_class",
        g_game->gameState->getShip().getMissileLauncherClass());
    script->setGlobalNumber(
        "max_engine_class", g_game->gameState->getShip().getMaxEngineClass());
    script->setGlobalNumber(
        "max_shield_class", g_game->gameState->getShip().getMaxShieldClass());
    script->setGlobalNumber(
        "max_armor_class", g_game->gameState->getShip().getMaxArmorClass());
    script->setGlobalNumber(
        "max_laser_class", g_game->gameState->getShip().getMaxLaserClass());
    script->setGlobalNumber(
        "max_missile_class",
        g_game->gameState->getShip().getMaxMissileLauncherClass());
    script->setGlobalNumber(
        "comm_skill",
        g_game->gameState->get_effective_officer(OFFICER_COMMUNICATION)
            ->get_skill(SKILL_COMMUNICATION));

    // set artifact numbers, other ship items (endurium, etc.)
    for (int n = 0; n < g_game->dataMgr->GetNumItems(); n++) {

        Item *pItem = g_game->dataMgr->GetItem(n);
        if (!pItem->IsArtifact() && !pItem->IsMineral())
            continue;

        std::string luaName;
        Item itemInHold;
        int numInHold;

        // artifact are known as "artifactN" in the scripts; minerals are known
        // as "player_mineralName"
        pItem->IsArtifact() ? luaName = "artifact" + Util::ToString(pItem->id)
                            : luaName = "player_" + pItem->name;

        // get number of that item currently in hold
        g_game->gameState->m_items.Get_Item_By_ID(
            pItem->id, itemInHold, numInHold);

        script->setGlobalNumber(luaName, numInHold);
    }
}

void
ModuleEncounter::readGlobalsFromScript() {
    goto_question = script->getGlobalNumber("goto_question");
    number_of_actions = script->getGlobalNumber("number_of_actions");

    g_game->gameState->setAlienAttitude(script->getGlobalNumber("ATTITUDE"));
    g_game->gameState->playerPosture =
        script->getGlobalString("POSTURE"); // added.
    g_game->gameState->setCredits(script->getGlobalNumber("player_money"));
    // this is a controversial setting...
    g_game->gameState->setPlotStage(script->getGlobalNumber("plot_stage"));
    g_game->gameState->setActiveQuest(script->getGlobalNumber("active_quest"));

    // see if script has increased communication skill
    auto *currentCom =
        g_game->gameState->get_effective_officer(OFFICER_COMMUNICATION);
    int current_skill = currentCom->get_skill(SKILL_COMMUNICATION);
    int new_skill = script->getGlobalNumber("comm_skill");

    if (new_skill > current_skill)
        if (g_game->gameState->SkillUp(
                SKILL_COMMUNICATION, new_skill - current_skill))
            Print(
                OFFICER_COMMUNICATION,
                "I think i am getting better at this",
                PURPLE,
                5000);

    // see if script has upgraded any ship systems
    Ship ship = g_game->gameState->getShip();

    int engine = script->getGlobalNumber("ship_engine_class");
    if (engine > ship.getEngineClass()) {
        if (engine <= ship.getMaxEngineClass()) {
            Print(
                "Engines upgraded to class " + Util::ToString(engine) + "!",
                YELLOW,
                1000);
            ship.setEngineClass(engine);
        } else
            Print("Engines already at maximum level!", RED, 1000);
    }

    int shield = script->getGlobalNumber("ship_shield_class");
    if (shield > ship.getShieldClass()) {
        if (shield <= ship.getMaxShieldClass()) {
            Print(
                "Shields upgraded to class " + Util::ToString(shield) + "!",
                YELLOW,
                1000);
            ship.setShieldClass(shield);
        } else
            Print("Shields already at maximum level!", RED, 1000);
    }

    int armor = script->getGlobalNumber("ship_armor_class");
    if (armor > ship.getArmorClass()) {
        if (armor <= ship.getMaxArmorClass()) {
            Print(
                "Armor upgraded to class " + Util::ToString(armor) + "!",
                YELLOW,
                1000);
            ship.setArmorClass(armor);
        } else
            Print("Armor already at maximum level!", RED, 1000);
    }

    int laser = script->getGlobalNumber("ship_laser_class");
    if (laser > ship.getLaserClass()) {
        Print(
            "Lasers upgraded to class " + Util::ToString(laser) + "!",
            YELLOW,
            1000);
        ship.setLaserClass(laser);
    }
    if (laser < ship.getLaserClass()) {
        Print(
            "Lasers downgraded to class " + Util::ToString(laser) + "!",
            YELLOW,
            1000);
        ship.setLaserClass(laser);
    }
    int missile = script->getGlobalNumber("ship_missile_class");
    if (missile > ship.getMissileLauncherClass()) {
        Print(
            "Missile launcher upgraded to class " + Util::ToString(missile)
                + "!",
            YELLOW,
            1000);
        ship.setMissileLauncherClass(missile);
    }
    if (missile < ship.getMissileLauncherClass()) {
        Print(
            "Missile launcher downgraded to class " + Util::ToString(missile)
                + "!",
            YELLOW,
            1000);
        ship.setMissileLauncherClass(missile);
    }
    g_game->gameState->setShip(ship);

    // get artifact cargo updates, other ship items (endurium, etc.)
    for (int n = 0; n < g_game->dataMgr->GetNumItems(); n++) {
        std::string luaName;
        Item itemInHold;
        int numInHold, newcount;

        Item *pItem = g_game->dataMgr->GetItem(n);
        // item is neither an artifact nor a mineral; next!
        if (!pItem->IsArtifact() && !pItem->IsMineral())
            continue;

        // get number of that item currently in hold
        g_game->gameState->m_items.Get_Item_By_ID(
            pItem->id, itemInHold, numInHold);

        // artifact are known as "artifactN" in the scripts;  minerals are known
        // as "player_mineralName"
        pItem->IsArtifact() ? luaName = "artifact" + Util::ToString(pItem->id)
                            : luaName = "player_" + pItem->name;

        newcount = script->getGlobalNumber(luaName);

        // nothing changed for this item; next!
        if (newcount == numInHold)
            continue;

        // artifact
        if (pItem->IsArtifact()) {
            if (newcount > numInHold) {
                Print(
                    "We received the " + pItem->name + " from the " + alienName
                        + ".",
                    PURPLE,
                    1000); // artifacts to/from aliens jjh
                g_game->gameState->m_items.SetItemCount(
                    pItem->id,
                    1); // get exactly one
            } else {
                Print(
                    "We gave the " + pItem->name + " to the " + alienName + ".",
                    PURPLE,
                    1000);
                g_game->gameState->m_items.RemoveItems(
                    pItem->id,
                    numInHold); // give all
            }

            // broadcast inventory change
            ALLEGRO_EVENT e = {
                .type = static_cast<unsigned int>(EVENT_CARGO_UPDATE)};
            g_game->broadcast_event(&e);

            // nothing more to do for this artifact; next!
            continue;
        }

        // not an artifact
        ostringstream msg;
        bool received = (newcount > numInHold);
        int delta = abs(newcount - numInHold);
        int freeSpace = g_game->gameState->m_ship.getAvailableSpace();

        if (received) {

            if (freeSpace <= 0) {
                Print(
                    "We don't have any space left in the cargo hold!",
                    RED,
                    1000);
                return;
            }

            // do not pick up more than available cargo space
            if (delta > freeSpace)
                delta = freeSpace;

        } else {

            if (numInHold <= 0) {
                msg << "We did not give anything to the " + alienName + ".";
                msg << " We had no " + pItem->name + " in the hold.";
                Print(msg.str(), RED, 1000);
                return;
            }

            // do not give more than available in the hold
            if (delta > numInHold)
                delta = numInHold;
        }

        msg << (received ? "We received " : "We gave ");

        (delta >= 2) ? msg << delta << " cubic meters of "
                     : msg << "one cubic meter of ";

        msg << pItem->name << (received ? " from the " : " to the ")
            << alienName << ".";
        Print(msg.str(), PURPLE, 1000);

        // update the inventory
        received ? g_game->gameState->m_items.AddItems(pItem->id, delta)
                 : g_game->gameState->m_items.RemoveItems(pItem->id, delta);

        // broadcast inventory change
        ALLEGRO_EVENT e = {
            .type = static_cast<unsigned int>(EVENT_CARGO_UPDATE)};
        g_game->broadcast_event(&e);
    }
}

/*******************************************************
 *
 *    LUA functions
 *
 *******************************************************/

// Alien will close the communication channel.
// usage: L_Terminate()
int
L_Terminate(lua_State * /*luaVM*/) {
    ALLEGRO_EVENT e = {
        .type = static_cast<unsigned int>(EVENT_ENCOUNTER_CLOSECOMM)};
    g_game->broadcast_event(&e);

    return 0;
}

// Alien will close the communication channel and attack.
// usage: L_Attack()
int
L_Attack(lua_State * /*luaVM*/) {
    ALLEGRO_EVENT e = {
        .type = static_cast<unsigned int>(EVENT_ENCOUNTER_ALIENATTACK)};
    g_game->broadcast_event(&e);

    return 0;
}
