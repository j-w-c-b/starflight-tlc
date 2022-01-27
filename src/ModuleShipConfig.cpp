/*
        STARFLIGHT - THE LOST COLONY
        ModuleStarport.cpp - Handles Starport activity
        Author: Scott Idler
        Date: June 29, 2007

        This module is the common starport area, where the user gains access to
   the areas where he can create his character, customize his ship, hire a crew,
        go to the bank, trade goods and receive orders.
*/

#include "ModuleShipConfig.h"
#include "AudioSystem.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "ModeMgr.h"
#include "QuestMgr.h"
#include "Util.h"
#include "shipconfig_resources.h"

using namespace std;
using namespace shipconfig_resources;

ALLEGRO_DEBUG_CHANNEL("ModuleShipConfig")

#define SHIPNAME_MAXLEN 20

ModuleShipConfig::ModuleShipConfig(void) : m_resources(SHIPCONFIG_IMAGES) {}

// Init is a good place to load resources
bool
ModuleShipConfig::on_init() {
    ALLEGRO_DEBUG("  EVENT_SHIP_CONFIG_SYSTEMS Initialize\n");

    // load the datafile
    if (!m_resources.load()) {
        g_game->message("EVENT_SHIP_CONFIG_SYSTEMS: Error loading resources");
        return false;
    }

    inputName = false;

    // create button images
    ALLEGRO_BITMAP *btnNorm, *btnOver, *btnDeact;
    btnNorm = m_resources[I_SHIPCONFIG_BTN_NORM];
    btnOver = m_resources[I_SHIPCONFIG_BTN_OVER];
    btnDeact = m_resources[I_SHIPCONFIG_BTN_DEACTIVE];

    // initialize array of button ptrs
    for (int i = 0; i < NUMBER_OF_BUTTONS; ++i) {
        if (i < NUMBER_OF_BUTTONS) {
            buttons[i] = new Button(
                btnNorm,
                btnOver,
                btnDeact,
                BUTTON_X_START,
                BUTTON_Y_START + i * (BUTTON_HEIGHT + PIXEL_BUFFER),
                0,
                0,
                g_game->font22,
                "def",
                al_map_rgb(0, 255, 0));
        }
        if (buttons[i]) {
            if (!buttons[i]->IsInitialized())
                return false;
        } else
            return false;
    }

    buttonsActive = 0;

    // setup up the pathing
    menuPath.clear();
    ALLEGRO_EVENT e = {.type = EVENT_SHIP_CONFIG_MENU_ENTRY};
    on_event(&e);

    // load background image
    shipConfig = m_resources[I_SHIPCONFIG];

    // load ship image
    switch (g_game->gameState->getProfession()) {
    case PROFESSION_FREELANCE:
        shipImage = m_resources[I_FREELANCE];
    case PROFESSION_MILITARY:
        shipImage = m_resources[I_MILITARY];
    case PROFESSION_SCIENTIFIC:
        shipImage = m_resources[I_SCIENCE];
    default:
        ALLEGRO_ERROR("***ERROR: EVENT_SHIP_CONFIG_SYSTEMS: Player's "
                      "profession is invalid.");
    }

    // load audio files
    m_sndClick = g_game->audioSystem->Load("data/shipconfig/click.ogg");
    if (!m_sndClick) {
        g_game->message("EVENT_SHIP_CONFIG_SYSTEMS: Error loading click.ogg");
        return false;
    }
    m_sndErr = g_game->audioSystem->Load("data/shipconfig/error.ogg");
    if (!m_sndErr) {
        g_game->message("EVENT_SHIP_CONFIG_SYSTEMS: Error loading error.ogg");
        return false;
    }
    m_cursor = m_resources[I_SHIPCONFIG_CURSOR0];

    // tell questmgr that this module has been entered
    g_game->questMgr->raiseEvent(22);

    return true;
}

bool
ModuleShipConfig::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    if (inputName) {
        bool playKeySnd = false;
        bool playErrSnd = false;

        if (isalnum(event->unichar) || event->unichar == ' ') {
            if (shipName.size() < SHIPNAME_MAXLEN) {
                char c = event->unichar;
                shipName.push_back(c);
                playKeySnd = true;
            } else
                playErrSnd = true;
        } else if (event->keycode == ALLEGRO_KEY_BACKSPACE) {
            if (shipName.size() > 0) {
                shipName.erase(--(shipName.end()));

                playKeySnd = true;
            } else
                playErrSnd = true;
        } else {
            playErrSnd = true;
        }

        if (playKeySnd) {
            g_game->audioSystem->Play(m_sndClick);
        }

        if (playErrSnd) {
            g_game->audioSystem->Play(m_sndErr);
        }
        return false;
    }
    return true;
}

bool
ModuleShipConfig::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    for (int i = 0; i < buttonsActive; ++i)
        buttons[i]->OnMouseMove(x, y);

    return true;
}

bool
ModuleShipConfig::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    int button = event->button - 1;
    int x = event->x;
    int y = event->y;

    for (int i = 0; i < buttonsActive; ++i)
        if (buttons[i]->OnMouseReleased(button, x, y))
            return false;

    return true;
}

bool
ModuleShipConfig::on_event(ALLEGRO_EVENT *event) {
    int evnum, maxclass = -1;
    ALLEGRO_EVENT e = {.type = static_cast<unsigned int>(EVENT_NONE)};

    // check for general events
    switch (event->type) {
    case EVENT_SHIP_CONFIG_MENU_ENTRY:
        menuPath.push_back(EVENT_SHIP_CONFIG_MENU_ENTRY);
        buttonsActive = 4;
        configureButton(0, EVENT_SHIP_CONFIG_SYSTEMS);
        configureButton(1, EVENT_SHIP_CONFIG_TV_CONFIG);
        configureButton(2, EVENT_SHIP_CONFIG_LAUNCH);
        configureButton(3, EVENT_SHIP_CONFIG_EXIT);
        break;

    case EVENT_SHIP_CONFIG_SYSTEMS:
        menuPath.push_back(EVENT_SHIP_CONFIG_SYSTEMS);
        buttonsActive = 5;
        configureButton(0, EVENT_SHIP_CONFIG_BUY);
        configureButton(1, EVENT_SHIP_CONFIG_SELL);
        configureButton(2, EVENT_SHIP_CONFIG_REPAIR);
        configureButton(3, EVENT_SHIP_CONFIG_NAME);
        configureButton(4, EVENT_SHIP_CONFIG_BACK);
        break;

    case EVENT_SHIP_CONFIG_LAUNCH:
        if (g_game->gameState->PreparedToLaunch()) {
            ID starid = 2;
            g_game->gameState->player->currentStar = starid;
            int start_pos_x =
                g_game->getGlobalNumber("PLAYER_HYPERSPACE_START_X");
            int start_pos_y =
                g_game->getGlobalNumber("PLAYER_HYPERSPACE_START_Y");
            g_game->gameState->player->set_galactic_pos(
                start_pos_x, start_pos_y);

            // compute myrrdan position
            int orbitalpos = 3;
            int tilesacross = 100, tilesdown = 100, tileswidth = 256,
                tilesheight = 256;
            int starX = tilesacross / 2, starY = tilesdown / 2;

            srand(starid);
            for (int i = 0; i < orbitalpos; i++)
                rand();
            float radius = (2 + orbitalpos) * 4;
            float angle = rand() % 360;
            int rx = (int)(cos(angle) * radius);
            int ry = (int)(sin(angle) * radius);

            // set player position to start near myrrdan
            g_game->gameState->player->posSystem.SetPosition(
                (starX + rx) * tileswidth - 256,
                (starY + ry) * tilesheight - 135);

            g_game->LoadModule(MODULE_INTERPLANETARY);
        } else {
            g_game->ShowMessageBoxWindow("Not prepared to launch! Make sure "
                                         "you have an engine and a crew.");
        }
        return true;

    case EVENT_SHIP_CONFIG_BUY:
        menuPath.push_back(EVENT_SHIP_CONFIG_BUY);
        buttonsActive = 7;
        configureButton(0, EVENT_SHIP_CONFIG_CARGO_PODS);
        configureButton(1, EVENT_SHIP_CONFIG_ENGINES);
        configureButton(2, EVENT_SHIP_CONFIG_SHIELDS);
        configureButton(3, EVENT_SHIP_CONFIG_ARMOR);
        configureButton(4, EVENT_SHIP_CONFIG_MISSILES);
        configureButton(5, EVENT_SHIP_CONFIG_LASERS);
        configureButton(6, EVENT_SHIP_CONFIG_BACK);
        break;

    case EVENT_SHIP_CONFIG_SELL:
        menuPath.push_back(EVENT_SHIP_CONFIG_SELL);
        buttonsActive = 7;
        configureButton(0, EVENT_SHIP_CONFIG_CARGO_PODS);
        configureButton(1, EVENT_SHIP_CONFIG_ENGINES);
        configureButton(2, EVENT_SHIP_CONFIG_SHIELDS);
        configureButton(3, EVENT_SHIP_CONFIG_ARMOR);
        configureButton(4, EVENT_SHIP_CONFIG_MISSILES);
        configureButton(5, EVENT_SHIP_CONFIG_LASERS);
        configureButton(6, EVENT_SHIP_CONFIG_BACK);
        break;

    case EVENT_SHIP_CONFIG_REPAIR:
        {
            // calculate repair cost
            repairCost = 0;
            Ship ship = g_game->gameState->m_ship;
            if (ship.getHullIntegrity() < 100)
                repairCost += getHullRepair();
            if (ship.getArmorIntegrity() < 100)
                repairCost += getArmorRepair();
            if (ship.getEngineIntegrity() < 100)
                repairCost += getEngineRepair();
            if (ship.getShieldIntegrity() < 100)
                repairCost += getShieldRepair();
            if (ship.getLaserIntegrity() < 100)
                repairCost += getLaserRepair();
            if (ship.getMissileLauncherIntegrity() < 100)
                repairCost += getMissileRepair();

            menuPath.push_back(EVENT_SHIP_CONFIG_REPAIR);
            buttonsActive = 7;
            configureButton(0, EVENT_SHIP_CONFIG_ENGINES);
            configureButton(1, EVENT_SHIP_CONFIG_SHIELDS);
            configureButton(2, EVENT_SHIP_CONFIG_ARMOR);
            configureButton(3, EVENT_SHIP_CONFIG_MISSILES);
            configureButton(4, EVENT_SHIP_CONFIG_LASERS);
            configureButton(5, EVENT_SHIP_CONFIG_HULL);
            configureButton(6, EVENT_SHIP_CONFIG_BACK);
        }
        break;

    case EVENT_SHIP_CONFIG_NAME:
        menuPath.push_back(EVENT_SHIP_CONFIG_NAME);
        buttonsActive = 2;
        configureButton(0, EVENT_SHIP_CONFIG_SAVE_NAME);
        configureButton(1, EVENT_SHIP_CONFIG_CANCEL);
        inputName = true;
        shipName = g_game->gameState->m_ship.getName();
        break;

    case EVENT_SHIP_CONFIG_EXIT:
        g_game->LoadModule(MODULE_STARPORT);
        return false;
        break;

    case EVENT_SHIP_CONFIG_CARGO_PODS:
        if (menuPath[2] == EVENT_SHIP_CONFIG_BUY
            && g_game->gameState->m_ship.getCargoPodCount() < MAX_CARGOPODS
            && g_game->gameState->m_credits >= CARGOPODS) {
            g_game->gameState->m_ship.cargoPodPlusPlus();
            g_game->gameState->m_credits -= CARGOPODS;
        } else if (
            menuPath[2] == EVENT_SHIP_CONFIG_SELL
            && g_game->gameState->m_ship.getCargoPodCount() > 0) {
            if (g_game->gameState->m_ship.getAvailableSpace() < POD_CAPACITY)
                g_game->ShowMessageBoxWindow(
                    "",
                    "You can't sell any of your cargo pods--you have too "
                    "many "
                    "items in the hold.");
            else {
                g_game->gameState->m_ship.cargoPodMinusMinus();
                g_game->gameState->m_credits += CARGOPODS;
            }
        }
        break;

    // buy/sell ship components
    case EVENT_SHIP_CONFIG_ENGINES:
    case EVENT_SHIP_CONFIG_SHIELDS:
    case EVENT_SHIP_CONFIG_ARMOR:
    case EVENT_SHIP_CONFIG_MISSILES:
    case EVENT_SHIP_CONFIG_LASERS:
    case EVENT_SHIP_CONFIG_HULL:
        menuPath.push_back(static_cast<EventType>(event->type));
        if (menuPath[2] == EVENT_SHIP_CONFIG_BUY) {
            buttonsActive = 7;
            configureButton(0, EVENT_SHIP_CONFIG_CLASS1);
            configureButton(1, EVENT_SHIP_CONFIG_CLASS2);
            configureButton(2, EVENT_SHIP_CONFIG_CLASS3);
            configureButton(3, EVENT_SHIP_CONFIG_CLASS4);
            configureButton(4, EVENT_SHIP_CONFIG_CLASS5);
            configureButton(5, EVENT_SHIP_CONFIG_CLASS6);
            configureButton(6, EVENT_SHIP_CONFIG_BACK);

            // limit purchase to maximum class by profession
            evnum = event->type;
            switch (evnum) {
            case EVENT_SHIP_CONFIG_ENGINES:
                maxclass = g_game->gameState->m_ship.getMaxEngineClass();
                break;
            case EVENT_SHIP_CONFIG_SHIELDS:
                maxclass = g_game->gameState->m_ship.getMaxShieldClass();
                break;
            case EVENT_SHIP_CONFIG_ARMOR:
                maxclass = g_game->gameState->m_ship.getMaxArmorClass();
                break;
            case EVENT_SHIP_CONFIG_MISSILES:
                maxclass =
                    g_game->gameState->m_ship.getMaxMissileLauncherClass();
                break;
            case EVENT_SHIP_CONFIG_LASERS:
                maxclass = g_game->gameState->m_ship.getMaxLaserClass();
                break;
            default:
                ALLEGRO_ASSERT(0);
            }

            for (int n = 6; n > maxclass; n--) {
                configureButton(n - 1, EVENT_NONE);
            }

        } else if (menuPath[2] == EVENT_SHIP_CONFIG_SELL) {
            if (checkComponent()) {
                sellComponent();
                menuPath.pop_back();
            } else
                g_game->ShowMessageBoxWindow(
                    "", "You don't have one to sell!", 400, 200);

            menuPath.pop_back();
            e.type = menuPath.back();
            menuPath.pop_back();
            g_game->broadcast_event(&e);
        } else if (menuPath[2] == EVENT_SHIP_CONFIG_REPAIR) {
            if (checkComponent()) {
                repairComponent();
                menuPath.pop_back();
            } else
                g_game->ShowMessageBoxWindow(
                    "", "You don't have one to repair!", 400, 200);

            menuPath.pop_back();
            e.type = menuPath.back();
            menuPath.pop_back();
            g_game->broadcast_event(&e);
        }
        break;

    case EVENT_SHIP_CONFIG_BACK:
        inputName = false;
        menuPath.pop_back();
        e.type = menuPath.back();
        menuPath.pop_back();
        g_game->broadcast_event(&e);
        break;

    // buy new class of component
    case EVENT_SHIP_CONFIG_CLASS1:
    case EVENT_SHIP_CONFIG_CLASS2:
    case EVENT_SHIP_CONFIG_CLASS3:
    case EVENT_SHIP_CONFIG_CLASS4:
    case EVENT_SHIP_CONFIG_CLASS5:
    case EVENT_SHIP_CONFIG_CLASS6:
        menuPath.push_back(static_cast<EventType>(event->type));
        if (menuPath[2] == EVENT_SHIP_CONFIG_BUY) {
            if (!checkComponent()) {
                buyComponent();
                menuPath.pop_back();
                menuPath.pop_back();
            }
        }
        menuPath.pop_back();
        e.type = menuPath.back();
        menuPath.pop_back();
        g_game->broadcast_event(&e);
        break;

    case EVENT_SHIP_CONFIG_CANCEL:
        menuPath.pop_back();
        e.type = menuPath.back();
        menuPath.pop_back();
        g_game->broadcast_event(&e);
        break;

    case EVENT_SHIP_CONFIG_SAVE_NAME:
        if (menuPath[2] == EVENT_SHIP_CONFIG_NAME) {
            if (shipName != "") {
                g_game->gameState->m_ship.setName(shipName);

                menuPath.pop_back();
                e.type = menuPath.back();
                menuPath.pop_back();
                g_game->broadcast_event(&e);
            } else {
                g_game->ShowMessageBoxWindow(
                    "", "You must first christen your ship!", 400, 200);
            }
        }
        break;

    case EVENT_SHIP_CONFIG_TV_CONFIG:
        menuPath.push_back(EVENT_SHIP_CONFIG_TV_CONFIG);
        buttonsActive = 2;
        configureButton(0, EVENT_SHIP_CONFIG_BUY_TV);
        configureButton(1, EVENT_SHIP_CONFIG_BACK);
        break;

    case EVENT_SHIP_CONFIG_BUY_TV:
        if (g_game->gameState->m_ship.getHasTV()) {
            g_game->ShowMessageBoxWindow(
                "", "You already own a Terrain Vehicle!", 400, 200);
        } else {
            if (g_game->gameState->getCredits() >= 2000) {
                g_game->gameState->m_ship.setHasTV(true);
                g_game->gameState->augCredits(-2000);

                menuPath.pop_back();
                e.type = menuPath.back();
                menuPath.pop_back();
                g_game->broadcast_event(&e);
            } else {
                g_game->ShowMessageBoxWindow(
                    "", "A new Terrain Vehicle costs 2000 credits.", 400, 200);
            }
        }
        break;
    }
    return true;
}

bool
ModuleShipConfig::on_close() {
    ALLEGRO_DEBUG("EVENT_SHIP_CONFIG_SYSTEMS Destroy\n");

    menuPath.clear();
    for (int a = 0; a < NUMBER_OF_BUTTONS; ++a) {
        buttons[a]->Destroy();
        buttons[a] = NULL;
    }

    m_sndClick.reset();
    m_sndErr.reset();

    // unload the data file
    m_resources.unload();
    return true;
}

std::string
ModuleShipConfig::convertMenuPathToString() const {
    if (menuPath.size() < 2)
        return "Error menuPath.size() < 1";
    std::string result = convertEventTypeToString(menuPath[1]);
    for (int i = 2; i < (int)menuPath.size(); ++i)
        result = result + "->" + convertEventTypeToString(menuPath[i]);
    return result;
}
std::string
ModuleShipConfig::convertEventTypeToString(EventType btnType) const {
    switch (btnType) {
    case EVENT_SHIP_CONFIG_SYSTEMS:
        return "Ship Systems";
    case EVENT_SHIP_CONFIG_LAUNCH:
        return "Launch";
    case EVENT_SHIP_CONFIG_BUY:
        return "Buy";
    case EVENT_SHIP_CONFIG_SELL:
        return "Sell";
    case EVENT_SHIP_CONFIG_REPAIR:
        return "Repair";
    case EVENT_SHIP_CONFIG_NAME:
        return "Name";
    case EVENT_SHIP_CONFIG_EXIT:
        return "Exit";
    case EVENT_SHIP_CONFIG_CARGO_PODS:
        return "Cargo Pods";
    case EVENT_SHIP_CONFIG_ENGINES:
        return "Engines";
    case EVENT_SHIP_CONFIG_SHIELDS:
        return "Shields";
    case EVENT_SHIP_CONFIG_ARMOR:
        return "Armor";
    case EVENT_SHIP_CONFIG_MISSILES:
        return "Missiles";
    case EVENT_SHIP_CONFIG_LASERS:
        return "Lasers";
    case EVENT_SHIP_CONFIG_HULL:
        return "Hull";
    case EVENT_SHIP_CONFIG_BACK:
        return "Back";
    case EVENT_SHIP_CONFIG_CLASS1:
        return "Class 1";
    case EVENT_SHIP_CONFIG_CLASS2:
        return "Class 2";
    case EVENT_SHIP_CONFIG_CLASS3:
        return "Class 3";
    case EVENT_SHIP_CONFIG_CLASS4:
        return "Class 4";
    case EVENT_SHIP_CONFIG_CLASS5:
        return "Class 5";
    case EVENT_SHIP_CONFIG_CLASS6:
        return "Class 6";
    case EVENT_SHIP_CONFIG_PAY:
        return "Pay";
    case EVENT_SHIP_CONFIG_CANCEL:
        return "Cancel";
    case EVENT_SHIP_CONFIG_SAVE_NAME:
        return "Save Name";
    case EVENT_SHIP_CONFIG_TV_CONFIG:
        return "Terrain Vehicle";
    case EVENT_SHIP_CONFIG_BUY_TV:
        return "Buy T.V. (2000)";
    default:
        return "";
    }
    return "";
}

void
ModuleShipConfig::configureButton(int btn, EventType btnType) {
    if (0 <= btn && btn < NUMBER_OF_BUTTONS) {
        buttons[btn]->SetClickEvent(btnType);
        if (EVENT_SHIP_CONFIG_CLASS1 <= btnType
            && btnType <= EVENT_SHIP_CONFIG_CLASS6) {
            buttons[btn]->SetButtonText(
                convertEventTypeToString(btnType) + "  "
                + Util::ToString(
                    ITEM_PRICES[menuPath[3] - ITEM_ENUM_DIF][btn]));

        } else if (btnType == EVENT_SHIP_CONFIG_CARGO_PODS)
            buttons[btn]->SetButtonText(
                convertEventTypeToString(btnType) + "  "
                + Util::ToString(CARGOPODS));
        else
            buttons[btn]->SetButtonText(convertEventTypeToString(btnType));

        if (btnType == EVENT_SHIP_CONFIG_EXIT
            || btnType == EVENT_SHIP_CONFIG_BACK) {
            buttons[btn]->SetX(BOTTOM_CORNER_X);
            buttons[btn]->SetY(BOTTOM_CORNER_Y);
        } else {
            buttons[btn]->SetX(BUTTON_X_START);
            buttons[btn]->SetY(
                BUTTON_Y_START + btn * (BUTTON_HEIGHT + PIXEL_BUFFER));
        }
    }
}
bool
ModuleShipConfig::checkComponent() const {
    if ((int)menuPath.size() < 4)
        return false;

    switch (menuPath[2]) {
    case EVENT_SHIP_CONFIG_BUY:
        {
            int itemIndex = menuPath[3] - ITEM_ENUM_DIF;
            int classIndex = menuPath[4] - CLASS_ENUM_DIF - 1;
            int cost = ITEM_PRICES[itemIndex][classIndex];

            // can player afford it?
            int cash = g_game->gameState->m_credits;
            if (cash - cost < 0)
                return true;

            switch (menuPath[3]) {
            case EVENT_SHIP_CONFIG_ENGINES:
                return g_game->gameState->m_ship.getEngineClass()
                       == menuPath[4] - CLASS_ENUM_DIF;
            case EVENT_SHIP_CONFIG_SHIELDS:
                return g_game->gameState->m_ship.getShieldClass()
                       == menuPath[4] - CLASS_ENUM_DIF;
            case EVENT_SHIP_CONFIG_ARMOR:
                return g_game->gameState->m_ship.getArmorClass()
                       == menuPath[4] - CLASS_ENUM_DIF;
            case EVENT_SHIP_CONFIG_MISSILES:
                return g_game->gameState->m_ship.getMissileLauncherClass()
                       == menuPath[3] - CLASS_ENUM_DIF;
            case EVENT_SHIP_CONFIG_LASERS:
                return g_game->gameState->m_ship.getLaserClass()
                       == menuPath[4] - CLASS_ENUM_DIF;
            default:
                break;
            }
        }
        break;
    case EVENT_SHIP_CONFIG_SELL:
        switch (menuPath[3]) {
        case EVENT_SHIP_CONFIG_ENGINES:
            return g_game->gameState->m_ship.getEngineClass()
                   != NotInstalledType;
        case EVENT_SHIP_CONFIG_SHIELDS:
            return g_game->gameState->m_ship.getShieldClass()
                   != NotInstalledType;
        case EVENT_SHIP_CONFIG_ARMOR:
            return g_game->gameState->m_ship.getArmorClass()
                   != NotInstalledType;
        case EVENT_SHIP_CONFIG_MISSILES:
            return g_game->gameState->m_ship.getMissileLauncherClass()
                   != NotInstalledType;
        case EVENT_SHIP_CONFIG_LASERS:
            return g_game->gameState->m_ship.getLaserClass()
                   != NotInstalledType;
        default:
            break;
        }
        break;

    case EVENT_SHIP_CONFIG_REPAIR:
        switch (menuPath[3]) {
        case EVENT_SHIP_CONFIG_ENGINES:
            return g_game->gameState->m_ship.getEngineClass()
                   != NotInstalledType;
        case EVENT_SHIP_CONFIG_SHIELDS:
            return g_game->gameState->m_ship.getShieldClass()
                   != NotInstalledType;
        case EVENT_SHIP_CONFIG_ARMOR:
            return g_game->gameState->m_ship.getArmorClass()
                   != NotInstalledType;
        case EVENT_SHIP_CONFIG_MISSILES:
            return g_game->gameState->m_ship.getMissileLauncherClass()
                   != NotInstalledType;
        case EVENT_SHIP_CONFIG_LASERS:
            return g_game->gameState->m_ship.getLaserClass()
                   != NotInstalledType;
        case EVENT_SHIP_CONFIG_HULL:
            return true;
        default:
            break;
        }

    default:
        break;
    }
    return false;
}

int
ModuleShipConfig::getEngineValue() {
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int engine = ship.getEngineClass();
    switch (engine) {
    case 0:
        value = 0;
        break;
    case 1:
        value = ENGINE_CLASS1;
        break;
    case 2:
        value = ENGINE_CLASS2;
        break;
    case 3:
        value = ENGINE_CLASS3;
        break;
    case 4:
        value = ENGINE_CLASS4;
        break;
    case 5:
        value = ENGINE_CLASS5;
        break;
    case 6:
        value = ENGINE_CLASS6;
        break;
    }
    float health = ship.getEngineIntegrity() * 0.01f;
    health *= 0.75f;
    value = (int)((float)value * health);
    return value;
}

int
ModuleShipConfig::getLaserValue() {
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int laser = ship.getLaserClass();
    switch (laser) {
    case 0:
        value = 0;
        break;
    case 1:
        value = LASER_CLASS1;
        break;
    case 2:
        value = LASER_CLASS2;
        break;
    case 3:
        value = LASER_CLASS3;
        break;
    case 4:
        value = LASER_CLASS4;
        break;
    case 5:
        value = LASER_CLASS5;
        break;
    case 6:
        value = LASER_CLASS6;
        break;
    }
    float health = ship.getLaserIntegrity() * 0.01f;
    health *= 0.75f;
    value = (int)((float)value * health);
    return value;
}

int
ModuleShipConfig::getMissileValue() {
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int missile = ship.getMissileLauncherClass();
    switch (missile) {
    case 0:
        value = 0;
        break;
    case 1:
        value = MISSILELAUNCHER_CLASS1;
        break;
    case 2:
        value = MISSILELAUNCHER_CLASS2;
        break;
    case 3:
        value = MISSILELAUNCHER_CLASS3;
        break;
    case 4:
        value = MISSILELAUNCHER_CLASS4;
        break;
    case 5:
        value = MISSILELAUNCHER_CLASS5;
        break;
    case 6:
        value = MISSILELAUNCHER_CLASS6;
        break;
    }
    float health = ship.getMissileLauncherIntegrity() * 0.01f;
    health *= 0.75f;
    value = (int)((float)value * health);
    return value;
}

int
ModuleShipConfig::getShieldValue() {
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int shield = ship.getShieldClass();
    switch (shield) {
    case 0:
        value = 0;
        break;
    case 1:
        value = SHIELD_CLASS1;
        break;
    case 2:
        value = SHIELD_CLASS2;
        break;
    case 3:
        value = SHIELD_CLASS3;
        break;
    case 4:
        value = SHIELD_CLASS4;
        break;
    case 5:
        value = SHIELD_CLASS5;
        break;
    case 6:
        value = SHIELD_CLASS6;
        break;
    }
    float health = ship.getShieldIntegrity() * 0.01f;
    health *= 0.75f;
    value = (int)((float)value * health);
    return value;
}

int
ModuleShipConfig::getArmorValue() {
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int armor = ship.getArmorClass();
    switch (armor) {
    case 0:
        value = 0;
        break;
    case 1:
        value = ARMOR_CLASS1;
        break;
    case 2:
        value = ARMOR_CLASS2;
        break;
    case 3:
        value = ARMOR_CLASS3;
        break;
    case 4:
        value = ARMOR_CLASS4;
        break;
    case 5:
        value = ARMOR_CLASS5;
        break;
    case 6:
        value = ARMOR_CLASS6;
        break;
    }

    float health = (ship.getArmorIntegrity() / (armor * 120.0f));
    value = (int)((float)value * health * 0.75f);
    return value;
}

int
ModuleShipConfig::getEngineRepair() {
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int engine = ship.getEngineClass();
    switch (engine) {
    case 0:
        value = 0;
        break;
    case 1:
        value = ENGINE_CLASS1;
        break;
    case 2:
        value = ENGINE_CLASS2;
        break;
    case 3:
        value = ENGINE_CLASS3;
        break;
    case 4:
        value = ENGINE_CLASS4;
        break;
    case 5:
        value = ENGINE_CLASS5;
        break;
    case 6:
        value = ENGINE_CLASS6;
        break;
    }
    float damage = (100 - ship.getEngineIntegrity()) * 0.01f;
    value = (int)((float)value * damage);
    return value;
}

int
ModuleShipConfig::getLaserRepair() {
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int laser = ship.getLaserClass();
    switch (laser) {
    case 0:
        value = 0;
        break;
    case 1:
        value = LASER_CLASS1;
        break;
    case 2:
        value = LASER_CLASS2;
        break;
    case 3:
        value = LASER_CLASS3;
        break;
    case 4:
        value = LASER_CLASS4;
        break;
    case 5:
        value = LASER_CLASS5;
        break;
    case 6:
        value = LASER_CLASS6;
        break;
    }
    float damage = (100 - ship.getLaserIntegrity()) * 0.01f;
    value = (int)((float)value * damage);
    return value;
}

int
ModuleShipConfig::getMissileRepair() {
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int missile = ship.getMissileLauncherClass();
    switch (missile) {
    case 0:
        value = 0;
        break;
    case 1:
        value = MISSILELAUNCHER_CLASS1;
        break;
    case 2:
        value = MISSILELAUNCHER_CLASS2;
        break;
    case 3:
        value = MISSILELAUNCHER_CLASS3;
        break;
    case 4:
        value = MISSILELAUNCHER_CLASS4;
        break;
    case 5:
        value = MISSILELAUNCHER_CLASS5;
        break;
    case 6:
        value = MISSILELAUNCHER_CLASS6;
        break;
    }
    float damage = (100 - ship.getMissileLauncherIntegrity()) * 0.01f;
    value = (int)((float)value * damage);
    return value;
}

int
ModuleShipConfig::getShieldRepair() {
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int shield = ship.getShieldClass();
    switch (shield) {
    case 0:
        value = 0;
        break;
    case 1:
        value = SHIELD_CLASS1;
        break;
    case 2:
        value = SHIELD_CLASS2;
        break;
    case 3:
        value = SHIELD_CLASS3;
        break;
    case 4:
        value = SHIELD_CLASS4;
        break;
    case 5:
        value = SHIELD_CLASS5;
        break;
    case 6:
        value = SHIELD_CLASS6;
        break;
    }
    float damage = (100 - ship.getShieldIntegrity()) * 0.01f;
    value = (int)((float)value * damage);
    return value;
}

int
ModuleShipConfig::getArmorRepair() {
    int value = 0;
    Ship ship = g_game->gameState->getShip();
    int armor = ship.getArmorClass();
    switch (armor) {
    case 0:
        value = 0;
        break;
    case 1:
        value = ARMOR_CLASS1;
        break;
    case 2:
        value = ARMOR_CLASS2;
        break;
    case 3:
        value = ARMOR_CLASS3;
        break;
    case 4:
        value = ARMOR_CLASS4;
        break;
    case 5:
        value = ARMOR_CLASS5;
        break;
    case 6:
        value = ARMOR_CLASS6;
        break;
    }
    float damage = (100 - ship.getArmorIntegrity()) * 0.01f;
    value = (int)((float)value * damage);
    return value;
}

int
ModuleShipConfig::getHullRepair() {
    Ship ship = g_game->gameState->getShip();
    int damage = (100 - ship.getHullIntegrity()) * 100;
    return damage;
}

void
ModuleShipConfig::repairComponent() {
    if ((int)menuPath.size() < 4)
        return;

    string noMsg = "You don't have enough credits to pay for the repairs.";
    int itemCost = 0;
    switch (menuPath[3]) {
    case EVENT_SHIP_CONFIG_HULL:
        itemCost = getHullRepair();
        if (itemCost > 0) {
            if (g_game->gameState->m_credits >= itemCost) {
                g_game->gameState->augCredits(-itemCost);
                g_game->gameState->m_ship.setHullIntegrity(100);
                g_game->ShowMessageBoxWindow(
                    "", "Hull breaches have been patched up.", 400, 200);
            } else
                g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
        }
        break;
    case EVENT_SHIP_CONFIG_ENGINES:
        itemCost = getEngineRepair();
        if (itemCost > 0) {
            if (g_game->gameState->m_credits >= itemCost) {
                g_game->gameState->augCredits(-itemCost);
                g_game->gameState->m_ship.setEngineIntegrity(100);
                g_game->ShowMessageBoxWindow(
                    "", "Engines now at peak operating efficiency.", 400, 200);
            } else
                g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
        }
        break;
    case EVENT_SHIP_CONFIG_SHIELDS:
        itemCost = getShieldRepair();
        if (itemCost > 0) {
            if (g_game->gameState->m_credits >= itemCost) {
                g_game->gameState->augCredits(-itemCost);
                g_game->gameState->m_ship.setShieldIntegrity(100);
                g_game->ShowMessageBoxWindow(
                    "", "Shield capability fully restored.", 400, 200);
            } else
                g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
        }
        break;
    case EVENT_SHIP_CONFIG_ARMOR:
        itemCost = getArmorRepair();
        if (itemCost > 0) {
            if (g_game->gameState->m_credits >= itemCost) {
                g_game->gameState->augCredits(-itemCost);
                g_game->gameState->m_ship.setArmorIntegrity(100);
                g_game->ShowMessageBoxWindow(
                    "", "Armor plating has been reinforced.", 400, 200);
            } else
                g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
        }
        break;
    case EVENT_SHIP_CONFIG_MISSILES:
        itemCost = getMissileRepair();
        if (itemCost > 0) {
            if (g_game->gameState->m_credits >= itemCost) {
                g_game->gameState->augCredits(-itemCost);
                g_game->gameState->m_ship.setMissileLauncherIntegrity(100);
                g_game->ShowMessageBoxWindow(
                    "", "Missile launcher fully repaired.", 400, 200);
            } else
                g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
        }
        break;
    case EVENT_SHIP_CONFIG_LASERS:
        itemCost = getLaserRepair();
        if (itemCost > 0) {
            if (g_game->gameState->m_credits >= itemCost) {
                g_game->gameState->augCredits(-itemCost);
                g_game->gameState->m_ship.setLaserIntegrity(100);
                g_game->ShowMessageBoxWindow(
                    "", "Lasers are ready for action.", 400, 200);
            } else
                g_game->ShowMessageBoxWindow("", noMsg, 400, 200);
        }
        break;
    default:
        ALLEGRO_ASSERT(0);
    }

    g_game->gameState->m_ship.setHullIntegrity(100);
}

void
ModuleShipConfig::sellComponent() {
    if ((int)menuPath.size() < 4)
        return;

    int salePrice = 0;
    string saleText = "You received ";
    string saleItem = "";

    Ship ship = g_game->gameState->getShip();
    switch (menuPath[3]) {
    case EVENT_SHIP_CONFIG_ENGINES:
        salePrice = getEngineValue();
        saleItem = "Class " + Util::ToString(ship.getEngineClass()) + " Engine";
        ship.setEngineClass(NotInstalledType);
        break;
    case EVENT_SHIP_CONFIG_SHIELDS:
        salePrice = getShieldValue();
        saleItem = "Class " + Util::ToString(ship.getShieldClass()) + " Shield";
        ship.setShieldClass(NotInstalledType);
        break;
    case EVENT_SHIP_CONFIG_ARMOR:
        salePrice = getArmorValue();
        saleItem = "Class " + Util::ToString(ship.getArmorClass()) + " Armor";
        ship.setArmorClass(NotInstalledType);
        break;
    case EVENT_SHIP_CONFIG_MISSILES:
        salePrice = getMissileValue();
        saleItem = "Class " + Util::ToString(ship.getMissileLauncherClass())
                   + " Missile Launcher";
        ship.setMissileLauncherClass(NotInstalledType);
        break;
    case EVENT_SHIP_CONFIG_LASERS:
        salePrice = getLaserValue();
        saleItem = "Class " + Util::ToString(ship.getLaserClass()) + " Laser";
        ship.setLaserClass(NotInstalledType);
        break;
    default:
        ALLEGRO_ASSERT(0);
    }

    // save ship changes
    g_game->gameState->setShip(ship);

    // add credits to player's account
    g_game->gameState->augCredits(salePrice);

    saleText = "You received " + Util::ToString(salePrice) + " credits for the "
               + saleItem + ".";
    g_game->ShowMessageBoxWindow("", saleText, 600, 200);
}

void
ModuleShipConfig::buyComponent() {
    if ((int)menuPath.size() < 5)
        return;

    int itemIndex = menuPath[3] - ITEM_ENUM_DIF;
    int classIndex = menuPath[4] - CLASS_ENUM_DIF - 1;
    int cost = ITEM_PRICES[itemIndex][classIndex];
    Ship ship = g_game->gameState->getShip();

    switch (menuPath[3]) {
    case EVENT_SHIP_CONFIG_ENGINES:
        if (ship.getEngineClass() == 0) {
            ship.setEngineClass(menuPath[4] - CLASS_ENUM_DIF);
            ship.setEngineIntegrity(100.0f);
            g_game->gameState->augCredits(-cost);
        } else
            g_game->ShowMessageBoxWindow(
                "", "Your ship already has an engine!", 450, 200);
        break;
    case EVENT_SHIP_CONFIG_SHIELDS:
        if (ship.getShieldClass() == 0) {
            ship.setShieldClass(menuPath[4] - CLASS_ENUM_DIF);
            ship.setShieldIntegrity(100.0f);
            ship.setShieldCapacity(ship.getMaxShieldCapacity());
            g_game->gameState->augCredits(-cost);
        } else
            g_game->ShowMessageBoxWindow(
                "", "Your ship already has a shield generator!", 450, 200);
        break;
    case EVENT_SHIP_CONFIG_ARMOR:
        if (ship.getArmorClass() == 0) {
            ship.setArmorClass(menuPath[4] - CLASS_ENUM_DIF);
            ship.setArmorIntegrity(ship.getMaxArmorIntegrity());
            g_game->gameState->augCredits(-cost);
        } else
            g_game->ShowMessageBoxWindow(
                "", "Your ship already has armor plating!", 450, 200);
        break;
    case EVENT_SHIP_CONFIG_MISSILES:
        if (ship.getMissileLauncherClass() == 0) {
            ship.setMissileLauncherClass(menuPath[4] - CLASS_ENUM_DIF);
            ship.setMissileLauncherIntegrity(100.0f);
            g_game->gameState->augCredits(-cost);
        } else
            g_game->ShowMessageBoxWindow(
                "", "Your ship already has a missile launcher!", 450, 200);
        break;
    case EVENT_SHIP_CONFIG_LASERS:
        if (ship.getLaserClass() == 0) {
            ship.setLaserClass(menuPath[4] - CLASS_ENUM_DIF); // jjh
            ship.setLaserIntegrity(100.0f);
            g_game->gameState->augCredits(-cost);
        } else
            g_game->ShowMessageBoxWindow(
                "", "Your ship already has a laser!", 450, 200);
        break;
    default:
        ALLEGRO_ASSERT(0);
    }
    g_game->gameState->setShip(ship);
}

void
ModuleShipConfig::display() const {
    al_set_target_bitmap(g_game->GetBackBuffer());
    // show menu path
    if (menuPath.back() == EVENT_SHIP_CONFIG_REPAIR) {
        std::string temp =
            "Total Repair Cost: " + Util::ToString(repairCost) + " MU";
        al_draw_text(
            g_game->font32, WHITE, MENU_PATH_X, MENU_PATH_Y, 0, temp.c_str());
    } else if (menuPath.back() == EVENT_SHIP_CONFIG_NAME) {
        int nlen;
        // print "MSS"
        al_draw_text(
            g_game->font22, WHITE, MENU_PATH_X, MENU_PATH_Y, 0, "MSS ");
        nlen = al_get_text_width(g_game->font22, "MSS ");

        // print ship name
        al_draw_text(
            g_game->font22,
            WHITE,
            MENU_PATH_X + nlen,
            MENU_PATH_Y,
            0,
            shipName.c_str());
        nlen += al_get_text_width(g_game->font22, shipName.c_str());

        al_draw_bitmap(m_cursor, MENU_PATH_X + nlen + 2, MENU_PATH_Y, 0);

    } else {
        if (menuPath.size() > 1)
            al_draw_text(
                g_game->font32,
                WHITE,
                MENU_PATH_X,
                MENU_PATH_Y,
                0,
                convertMenuPathToString().c_str());
    }

    // draw ship schematic
    al_draw_bitmap(shipImage, 586, 548, 0);

    // static
    int i = 0;
    al_draw_textf(
        g_game->font22,
        WHITE,
        STATIC_SHIPNAME_X - 10,
        SHIPNAME_Y,
        0,
        "Ship Name: MSS %s",
        g_game->gameState->m_ship.getName().c_str());
    al_draw_text(
        g_game->font32,
        WHITE,
        STATIC_READOUT_X,
        READOUT_Y + (i++) * READOUT_SPACING,
        0,
        "Cargo Pods");
    al_draw_text(
        g_game->font32,
        WHITE,
        STATIC_READOUT_X,
        READOUT_Y + (i++) * READOUT_SPACING,
        0,
        "Engine");
    al_draw_text(
        g_game->font32,
        WHITE,
        STATIC_READOUT_X,
        READOUT_Y + (i++) * READOUT_SPACING,
        0,
        "Shield");
    al_draw_text(
        g_game->font32,
        WHITE,
        STATIC_READOUT_X,
        READOUT_Y + (i++) * READOUT_SPACING,
        0,
        "Armor");
    al_draw_text(
        g_game->font32,
        WHITE,
        STATIC_READOUT_X,
        READOUT_Y + (i++) * READOUT_SPACING,
        0,
        "Missile");
    al_draw_text(
        g_game->font32,
        WHITE,
        STATIC_READOUT_X,
        READOUT_Y + (i++) * READOUT_SPACING,
        0,
        "Laser");
    al_draw_text(
        g_game->font32,
        WHITE,
        STATIC_READOUT_X,
        READOUT_Y + (i++) * READOUT_SPACING,
        0,
        "Hull Integrity");
    al_draw_text(
        g_game->font32, WHITE, STATIC_CREDITS_X, CREDITS_Y, 0, "Credits");

    // dynamic
    int j = 0;
    al_draw_textf(
        g_game->font32,
        WHITE,
        DYNAMIC_READOUT_X,
        READOUT_Y + (j++) * READOUT_SPACING,
        ALLEGRO_ALIGN_RIGHT,
        "%d",
        g_game->gameState->m_ship.getCargoPodCount());

    // display class level of engine
    ALLEGRO_COLOR color = WHITE;
    int health = g_game->gameState->m_ship.getEngineIntegrity();
    if (health < 50)
        color = RED;
    else if (health < 100)
        color = YELLOW;
    else
        color = WHITE;
    al_draw_text(
        g_game->font32,
        color,
        DYNAMIC_READOUT_X,
        READOUT_Y + (j++) * READOUT_SPACING,
        ALLEGRO_ALIGN_RIGHT,
        g_game->gameState->m_ship.getEngineClassString().c_str());

    // display class level of shield
    health = g_game->gameState->m_ship.getShieldIntegrity();
    if (health < 50)
        color = RED;
    else if (health < 100)
        color = YELLOW;
    else
        color = WHITE;
    al_draw_text(
        g_game->font32,
        color,
        DYNAMIC_READOUT_X,
        READOUT_Y + (j++) * READOUT_SPACING,
        ALLEGRO_ALIGN_RIGHT,
        g_game->gameState->m_ship.getShieldClassString().c_str());

    // display class level of armor
    health = g_game->gameState->m_ship.getArmorIntegrity();
    if (health < 50)
        color = RED;
    else if (health < 100)
        color = YELLOW;
    else
        color = WHITE;
    al_draw_text(
        g_game->font32,
        color,
        DYNAMIC_READOUT_X,
        READOUT_Y + (j++) * READOUT_SPACING,
        ALLEGRO_ALIGN_RIGHT,
        g_game->gameState->m_ship.getArmorClassString().c_str());

    // display class level of missile
    health = g_game->gameState->m_ship.getMissileLauncherIntegrity();
    if (health < 50)
        color = RED;
    else if (health < 100)
        color = YELLOW;
    else
        color = WHITE;
    al_draw_text(
        g_game->font32,
        color,
        DYNAMIC_READOUT_X,
        READOUT_Y + (j++) * READOUT_SPACING,
        ALLEGRO_ALIGN_RIGHT,
        g_game->gameState->m_ship.getMissileLauncherClassString().c_str());

    // display class level of laser
    health = g_game->gameState->m_ship.getLaserIntegrity();
    if (health < 50)
        color = RED;
    else if (health < 100)
        color = YELLOW;
    else
        color = WHITE;
    al_draw_text(
        g_game->font32,
        color,
        DYNAMIC_READOUT_X,
        READOUT_Y + (j++) * READOUT_SPACING,
        ALLEGRO_ALIGN_RIGHT,
        g_game->gameState->m_ship.getLaserClassString().c_str());

    // this should clear up any hull init problem
    health = g_game->gameState->m_ship.getHullIntegrity();
    if (health < 50)
        color = RED;
    else if (health < 100)
        color = YELLOW;
    else
        color = WHITE;

    if (g_game->gameState->m_ship.getHullIntegrity() > 100.0f)
        g_game->gameState->m_ship.setHullIntegrity(100.0f);

    // print hull integrity
    al_draw_textf(
        g_game->font32,
        WHITE,
        DYNAMIC_READOUT_X,
        READOUT_Y + (j++) * READOUT_SPACING,
        ALLEGRO_ALIGN_RIGHT,
        "%.0f",
        g_game->gameState->m_ship.getHullIntegrity());

    // print credits
    al_draw_textf(
        g_game->font32,
        WHITE,
        DYNAMIC_CREDITS_X,
        CREDITS_Y,
        ALLEGRO_ALIGN_RIGHT,
        "%d",
        g_game->gameState->m_credits);
    return;
}

bool
ModuleShipConfig::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);
    // blit the background image
    al_draw_bitmap(shipConfig, 0, 0, 0);

    // step through active buttons
    for (int i = 0; i < buttonsActive; ++i)
        buttons[i]->Run(target);

    display();

    return true;
}
