#include "ModuleCaptainCreation.h"
#include "AudioSystem.h"
#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "Label.h"
#include "ModeMgr.h"
#include "captaincreation_resources.h"

using namespace std;
using namespace captaincreation_resources;

ALLEGRO_DEBUG_CHANNEL("ModuleCaptainCreation")

#define BACKBTN_WIDTH 82
#define BACKBTN_HEIGHT 58

#define BACKBTN_X 12
#define BACKBTN_Y 698

#define BASEATT_SCIENTIFIC_DURABILITY 5
#define BASEATT_SCIENTIFIC_LEARNRATE 5
#define BASEATT_SCIENTIFIC_SCIENCE 15
#define BASEATT_SCIENTIFIC_NAVIGATION 5
#define BASEATT_SCIENTIFIC_TACTICS 0
#define BASEATT_SCIENTIFIC_ENGINEERING 5
#define BASEATT_SCIENTIFIC_COMMUNICATION 15
#define BASEATT_SCIENTIFIC_MEDICAL 10

#define MAXATT_SCIENTIFIC_DURABILITY 10
#define MAXATT_SCIENTIFIC_LEARNRATE 10
#define MAXATT_SCIENTIFIC_SCIENCE 250
#define MAXATT_SCIENTIFIC_NAVIGATION 95
#define MAXATT_SCIENTIFIC_TACTICS 65
#define MAXATT_SCIENTIFIC_ENGINEERING 95
#define MAXATT_SCIENTIFIC_COMMUNICATION 250
#define MAXATT_SCIENTIFIC_MEDICAL 125

#define BASEATT_FREELANCE_DURABILITY 5
#define BASEATT_FREELANCE_LEARNRATE 5
#define BASEATT_FREELANCE_SCIENCE 5
#define BASEATT_FREELANCE_NAVIGATION 15
#define BASEATT_FREELANCE_TACTICS 5
#define BASEATT_FREELANCE_ENGINEERING 10
#define BASEATT_FREELANCE_COMMUNICATION 15
#define BASEATT_FREELANCE_MEDICAL 0

#define MAXATT_FREELANCE_DURABILITY 10
#define MAXATT_FREELANCE_LEARNRATE 10
#define MAXATT_FREELANCE_SCIENCE 95
#define MAXATT_FREELANCE_NAVIGATION 250
#define MAXATT_FREELANCE_TACTICS 95
#define MAXATT_FREELANCE_ENGINEERING 125
#define MAXATT_FREELANCE_COMMUNICATION 250
#define MAXATT_FREELANCE_MEDICAL 65

#define BASEATT_MILITARY_DURABILITY 5
#define BASEATT_MILITARY_LEARNRATE 5
#define BASEATT_MILITARY_SCIENCE 0
#define BASEATT_MILITARY_NAVIGATION 10
#define BASEATT_MILITARY_TACTICS 15
#define BASEATT_MILITARY_ENGINEERING 10
#define BASEATT_MILITARY_COMMUNICATION 15
#define BASEATT_MILITARY_MEDICAL 0

#define MAXATT_MILITARY_DURABILITY 10
#define MAXATT_MILITARY_LEARNRATE 10
#define MAXATT_MILITARY_SCIENCE 65
#define MAXATT_MILITARY_NAVIGATION 125
#define MAXATT_MILITARY_TACTICS 250
#define MAXATT_MILITARY_ENGINEERING 125
#define MAXATT_MILITARY_COMMUNICATION 250
#define MAXATT_MILITARY_MEDICAL 65

static string
resolve_sample_name(const string &name) {
    auto result = find_if(
        begin(CAPTAINCREATION_SAMPLES),
        end(CAPTAINCREATION_SAMPLES),
        [name](const ResourceName &rn) { return rn.name == name; });
    if (result != end(CAPTAINCREATION_SAMPLES)) {
        return result->path;
    }
    return "";
}

ModuleCaptainCreation::ModuleCaptainCreation()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
      m_resources(CAPTAINCREATION_IMAGES),
      m_click_loaded(
          g_game->audioSystem->Load(resolve_sample_name(S_CLICK), S_CLICK)),
      m_buttonclick_loaded(g_game->audioSystem->Load(
          resolve_sample_name(S_BUTTONCLICK),
          S_BUTTONCLICK)),
      m_error_loaded(
          g_game->audioSystem->Load(resolve_sample_name(S_ERROR), S_ERROR)),
      m_wizPage(WP_NONE),
      m_profession_choice(make_shared<ModuleProfessionChoice>(m_resources)),
      m_captain_details(make_shared<ModuleCaptainDetails>(m_resources)),
      m_back_button(make_shared<NewButton>(
          BACKBTN_X,
          BACKBTN_Y,
          BACKBTN_WIDTH,
          BACKBTN_HEIGHT,
          EVENT_NONE,
          EVENT_CAPTAINCREATION_BACK,
          m_resources[I_CAPTAINCREATION_BACK],
          m_resources[I_CAPTAINCREATION_BACK_MOUSEOVER],
          nullptr,
          S_BUTTONCLICK)) {

    add_child_module(m_profession_choice);
    add_child_module(m_captain_details);

    // last to draw so it is on top of other sub-modules
    add_child_module(m_back_button);
}

ModuleCaptainCreation::~ModuleCaptainCreation() {}

bool
ModuleCaptainCreation::on_init() {
    if (!m_resources.load()) {
        g_game->message("CaptainCreation: Error loading resources");
        return false;
    }

    // load audio files
    m_sndBtnClick =
        g_game->audioSystem->Load("data/captaincreation/buttonclick.ogg");
    if (!m_sndBtnClick) {
        g_game->message("Error loading data/captaincreation_buttonclick.ogg");
        return false;
    }

    m_sndClick = g_game->audioSystem->Load("data/captaincreation/click.ogg");
    if (!m_sndClick) {
        g_game->message("Error loading data/captaincreation_click.ogg");
        return false;
    }

    m_sndErr = g_game->audioSystem->Load("data/captaincreation/error.ogg");
    if (!m_sndErr) {
        g_game->message("Error loading data/captaincreation/error.ogg");
        return false;
    }

    m_wizPage = WP_PROFESSION_CHOICE;
    m_profession_choice->set_active(true);
    m_captain_details->set_active(false);

    return true;
}

bool
ModuleCaptainCreation::on_draw(ALLEGRO_BITMAP *target) {
    static bool help1 = true;
    static bool help2 = true;

    al_set_target_bitmap(target);
    switch (m_wizPage) {
    case WP_PROFESSION_CHOICE:
        {
            // display tutorial help messages for beginners
            if ((!g_game->gameState->firstTimeVisitor
                 || g_game->gameState->getActiveQuest() > 1))
                help1 = false;
            if (help1) {
                help1 = false;
                string str =
                    "Okay, let's create a new character for you, starting "
                    "with your choice of profession. Choose a Science, "
                    "Freelance, or Military career.";
                g_game->ShowMessageBoxWindow(
                    "", str, 400, 300, YELLOW, 600, 400, false);
            }
        }
        break;

    case WP_DETAILS:
        {
            // display tutorial help messages for beginners
            if ((!g_game->gameState->firstTimeVisitor
                 || g_game->gameState->getActiveQuest() > 1))
                help2 = false;
            if (help2) {
                help2 = false;
                string str = "Next, you need to enter a name for your "
                             "captain, and then set "
                             "your attribute points: 5 points to "
                             "Durability or Learning "
                             "Rate, and 25 points to all the rest. You "
                             "must allocate all of "
                             "the points before continuing.";
                g_game->ShowMessageBoxWindow(
                    "", str, 400, 300, YELLOW, 10, 250, false);
            }
        }

        break;
    case WP_NONE:
        break;
    }
    return true;
}

bool
ModuleCaptainCreation::on_close() {
    if (m_sndBtnClick != nullptr) {
        m_sndBtnClick = nullptr;
    }

    if (m_sndClick != nullptr) {
        m_sndClick = nullptr;
    }

    if (m_sndErr != nullptr) {
        m_sndErr = nullptr;
    }

    return true;
}

bool
ModuleCaptainCreation::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    if (event->keycode == ALLEGRO_KEY_ESCAPE) {
        ALLEGRO_EVENT e = {.type = EVENT_CAPTAINCREATION_BACK};
        g_game->broadcast_event(&e);
        return false;
    }
    return true;
}

void
ModuleCaptainCreation::chooseFreelance() {
    // set freelance attributes
    m_profession = PROFESSION_FREELANCE;
    m_attributesInitial.durability = BASEATT_FREELANCE_DURABILITY;
    m_attributesInitial.learnRate = BASEATT_FREELANCE_LEARNRATE;
    m_attributesInitial.science = BASEATT_FREELANCE_SCIENCE;
    m_attributesInitial.navigation = BASEATT_FREELANCE_NAVIGATION;
    m_attributesInitial.tactics = BASEATT_FREELANCE_TACTICS;
    m_attributesInitial.engineering = BASEATT_FREELANCE_ENGINEERING;
    m_attributesInitial.communication = BASEATT_FREELANCE_COMMUNICATION;
    m_attributesInitial.medical = BASEATT_FREELANCE_MEDICAL;

    // set attribute max values
    m_attributesMax.durability = MAXATT_FREELANCE_DURABILITY;
    m_attributesMax.learnRate = MAXATT_FREELANCE_LEARNRATE;
    m_attributesMax.science = MAXATT_FREELANCE_SCIENCE;
    m_attributesMax.navigation = MAXATT_FREELANCE_NAVIGATION;
    m_attributesMax.tactics = MAXATT_FREELANCE_TACTICS;
    m_attributesMax.engineering = MAXATT_FREELANCE_ENGINEERING;
    m_attributesMax.communication = MAXATT_FREELANCE_COMMUNICATION;
    m_attributesMax.medical = MAXATT_FREELANCE_MEDICAL;

    // store attributes in gamestate
    g_game->gameState->setProfession(m_profession);
    g_game->gameState->officerCap->attributes = m_attributesInitial;

    // set ship name and properties based on profession
    Ship ship = g_game->gameState->getShip();
    ship.setName("Acquisition");
    int value = g_game->getGlobalNumber("PROF_FREELANCE_ARMOR");
    ship.setArmorClass(value);
    ship.setArmorIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_FREELANCE_ENGINE");
    ship.setEngineClass(value);
    ship.setEngineIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_FREELANCE_SHIELD");
    ship.setShieldClass(value);
    ship.setShieldIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_FREELANCE_LASER");
    ship.setLaserClass(value);
    ship.setLaserIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_FREELANCE_MISSILE");
    ship.setMissileLauncherClass(value);
    ship.setMissileLauncherIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_FREELANCE_PODS");
    ship.setCargoPodCount(value);

    int maxEngineClass = 0, maxShieldClass = 0, maxArmorClass = 0,
        maxMissileLauncherClass = 0, maxLaserClass = 0;
    maxEngineClass = g_game->getGlobalNumber("PROF_FREELANCE_ENGINE_MAX");
    maxShieldClass = g_game->getGlobalNumber("PROF_FREELANCE_SHIELD_MAX");
    maxArmorClass = g_game->getGlobalNumber("PROF_FREELANCE_ARMOR_MAX");
    maxMissileLauncherClass =
        g_game->getGlobalNumber("PROF_FREELANCE_MISSILE_MAX");
    maxLaserClass = g_game->getGlobalNumber("PROF_FREELANCE_LASER_MAX");
    ship.setMaxEngineClass(maxEngineClass);
    ship.setMaxShieldClass(maxShieldClass);
    ship.setMaxArmorClass(maxArmorClass);
    ship.setMaxMissileLauncherClass(maxMissileLauncherClass);
    ship.setMaxLaserClass(maxLaserClass);

    // Roll random repair minerals and set the repair counters
    ship.initializeRepair();

    g_game->gameState->setShip(ship);
}

void
ModuleCaptainCreation::chooseMilitary() {
    // set military attributes
    m_profession = PROFESSION_MILITARY;
    m_attributesInitial.durability = BASEATT_MILITARY_DURABILITY;
    m_attributesInitial.learnRate = BASEATT_MILITARY_LEARNRATE;
    m_attributesInitial.science = BASEATT_MILITARY_SCIENCE;
    m_attributesInitial.navigation = BASEATT_MILITARY_NAVIGATION;
    m_attributesInitial.tactics = BASEATT_MILITARY_TACTICS;
    m_attributesInitial.engineering = BASEATT_MILITARY_ENGINEERING;
    m_attributesInitial.communication = BASEATT_MILITARY_COMMUNICATION;
    m_attributesInitial.medical = BASEATT_MILITARY_MEDICAL;

    // maximum attribute values
    m_attributesMax.durability = MAXATT_MILITARY_DURABILITY;
    m_attributesMax.learnRate = MAXATT_MILITARY_LEARNRATE;
    m_attributesMax.science = MAXATT_MILITARY_SCIENCE;
    m_attributesMax.navigation = MAXATT_MILITARY_NAVIGATION;
    m_attributesMax.tactics = MAXATT_MILITARY_TACTICS;
    m_attributesMax.engineering = MAXATT_MILITARY_ENGINEERING;
    m_attributesMax.communication = MAXATT_MILITARY_COMMUNICATION;
    m_attributesMax.medical = MAXATT_MILITARY_MEDICAL;

    // store attributes in gamestate
    g_game->gameState->setProfession(m_profession);
    g_game->gameState->officerCap->attributes = m_attributesInitial;

    // set ship name and properties based on profession
    Ship ship = g_game->gameState->getShip();
    ship.setName("Devastator");
    int value = g_game->getGlobalNumber("PROF_MILITARY_ARMOR");
    ship.setArmorClass(value);
    ship.setArmorIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_MILITARY_ENGINE");
    ship.setEngineClass(value);
    ship.setEngineIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_MILITARY_SHIELD");
    ship.setShieldClass(value);
    ship.setShieldIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_MILITARY_LASER");
    ship.setLaserClass(value);
    ship.setLaserIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_MILITARY_MISSILE");
    ship.setMissileLauncherClass(value);
    ship.setMissileLauncherIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_MILITARY_PODS");
    ship.setCargoPodCount(value);

    int maxEngineClass = 0, maxShieldClass = 0, maxArmorClass = 0,
        maxMissileLauncherClass = 0, maxLaserClass = 0;
    maxEngineClass = g_game->getGlobalNumber("PROF_MILITARY_ENGINE_MAX");
    maxShieldClass = g_game->getGlobalNumber("PROF_MILITARY_SHIELD_MAX");
    maxArmorClass = g_game->getGlobalNumber("PROF_MILITARY_ARMOR_MAX");
    maxMissileLauncherClass =
        g_game->getGlobalNumber("PROF_MILITARY_MISSILE_MAX");
    maxLaserClass = g_game->getGlobalNumber("PROF_MILITARY_LASER_MAX");
    ship.setMaxEngineClass(maxEngineClass);
    ship.setMaxShieldClass(maxShieldClass);
    ship.setMaxArmorClass(maxArmorClass);
    ship.setMaxMissileLauncherClass(maxMissileLauncherClass);
    ship.setMaxLaserClass(maxLaserClass);

    // Roll random repair minerals and set the repair counters
    ship.initializeRepair();

    g_game->gameState->setShip(ship);
}

void
ModuleCaptainCreation::chooseScience() {
    // set science attributes
    m_profession = PROFESSION_SCIENTIFIC;
    m_attributesInitial.durability = BASEATT_SCIENTIFIC_DURABILITY;
    m_attributesInitial.learnRate = BASEATT_SCIENTIFIC_LEARNRATE;
    m_attributesInitial.science = BASEATT_SCIENTIFIC_SCIENCE;
    m_attributesInitial.navigation = BASEATT_SCIENTIFIC_NAVIGATION;
    m_attributesInitial.tactics = BASEATT_SCIENTIFIC_TACTICS;
    m_attributesInitial.engineering = BASEATT_SCIENTIFIC_ENGINEERING;
    m_attributesInitial.communication = BASEATT_SCIENTIFIC_COMMUNICATION;
    m_attributesInitial.medical = BASEATT_SCIENTIFIC_MEDICAL;

    // maximum attribute values
    m_attributesMax.durability = MAXATT_SCIENTIFIC_DURABILITY;
    m_attributesMax.learnRate = MAXATT_SCIENTIFIC_LEARNRATE;
    m_attributesMax.science = MAXATT_SCIENTIFIC_SCIENCE;
    m_attributesMax.navigation = MAXATT_SCIENTIFIC_NAVIGATION;
    m_attributesMax.tactics = MAXATT_SCIENTIFIC_TACTICS;
    m_attributesMax.engineering = MAXATT_SCIENTIFIC_ENGINEERING;
    m_attributesMax.communication = MAXATT_SCIENTIFIC_COMMUNICATION;
    m_attributesMax.medical = MAXATT_SCIENTIFIC_MEDICAL;

    // store attributes in gamestate
    g_game->gameState->setProfession(m_profession);
    g_game->gameState->officerCap->attributes = m_attributesInitial;

    // set ship name and properties based on profession
    Ship ship = g_game->gameState->getShip();
    ship.setName("Expedition");
    int value = g_game->getGlobalNumber("PROF_SCIENCE_ARMOR");
    ship.setArmorClass(value);
    ship.setArmorIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_SCIENCE_ENGINE");
    ship.setEngineClass(value);
    ship.setEngineIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_SCIENCE_SHIELD");
    ship.setShieldClass(value);
    ship.setShieldIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_SCIENCE_LASER");
    ship.setLaserClass(value);
    ship.setLaserIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_SCIENCE_MISSILE");
    ship.setMissileLauncherClass(value);
    ship.setMissileLauncherIntegrity(100.0);
    value = g_game->getGlobalNumber("PROF_SCIENCE_PODS");
    ship.setCargoPodCount(value);

    int maxEngineClass = 0, maxShieldClass = 0, maxArmorClass = 0,
        maxMissileLauncherClass = 0, maxLaserClass = 0;
    maxEngineClass = g_game->getGlobalNumber("PROF_SCIENCE_ENGINE_MAX");
    maxShieldClass = g_game->getGlobalNumber("PROF_SCIENCE_SHIELD_MAX");
    maxArmorClass = g_game->getGlobalNumber("PROF_SCIENCE_ARMOR_MAX");
    maxMissileLauncherClass =
        g_game->getGlobalNumber("PROF_SCIENCE_MISSILE_MAX");
    maxLaserClass = g_game->getGlobalNumber("PROF_SCIENCE_LASER_MAX");
    ship.setMaxEngineClass(maxEngineClass);
    ship.setMaxShieldClass(maxShieldClass);
    ship.setMaxArmorClass(maxArmorClass);
    ship.setMaxMissileLauncherClass(maxMissileLauncherClass);
    ship.setMaxLaserClass(maxLaserClass);

    // Roll random repair minerals and set the repair counters
    ship.initializeRepair();

    g_game->gameState->setShip(ship);
}

bool
ModuleCaptainCreation::on_event(ALLEGRO_EVENT *event) {
    bool playBtnSnd = false;
    bool creationComplete = false;

    if (m_wizPage == WP_PROFESSION_CHOICE) {
        switch (event->type) {
        case EVENT_PROFESSION_SCIENTIFIC:
            chooseScience();
            break;
        case EVENT_PROFESSION_FREELANCE:
            chooseFreelance();
            break;
        case EVENT_PROFESSION_MILITARY:
            chooseMilitary();
            break;
        case EVENT_CAPTAINCREATION_BACK:
            m_wizPage = WP_PROFESSION_CHOICE;
            // return player to previous screen
            g_game->LoadModule(g_game->modeMgr->GetPrevModuleName());
            return false;
        default:
            return true;
        }
        m_wizPage = WP_DETAILS;
        m_profession_choice->set_active(false);
        m_captain_details->setup_attributes(
            m_attributesInitial, m_attributesMax);
        m_captain_details->reset();
        m_captain_details->set_active(true);
        return false;
    } else {
        if (event->type == EVENT_CAPTAINCREATION_BACK) {
            m_wizPage = WP_PROFESSION_CHOICE;
            m_profession_choice->set_active(true);
            m_captain_details->set_active(false);
            return false;
        }

        if (event->type == EVENT_CAPTAINCREATION_FINISH) {
            playBtnSnd = true;

            // this ends up calling g_game->gameState->m_ship.Reset()
            // so most of the changes we did on the ship are thrown out
            g_game->gameState->Reset();
            g_game->gameState->m_profession = m_profession;
            g_game->gameState->officerCap->name = m_captain_details->get_name();
            g_game->gameState->officerCap->attributes =
                m_captain_details->get_attributes();
            g_game->gameState->m_captainSelected = true;
            g_game->gameState->SaveGame(GameState::GAME_SAVE_SLOT_NEW);

            creationComplete = true;
        }

        if (playBtnSnd) {
            g_game->audioSystem->Play(m_sndBtnClick);
        }

        if (creationComplete) {
            g_game->gameState->m_captainSelected = true;
            g_game->LoadModule(MODULE_CAPTAINSLOUNGE);
            return false;
        }
    }
    return true;
}
// vi: ft=cpp
