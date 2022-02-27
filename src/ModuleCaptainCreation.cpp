#include "ModuleCaptainCreation.h"
#include "AudioSystem.h"
#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "Label.h"
#include "MessageBoxWindow.h"
#include "ModeMgr.h"
#include "captaincreation_resources.h"

using namespace std;
using namespace captaincreation;

ALLEGRO_DEBUG_CHANNEL("ModuleCaptainCreation")

#define BACKBTN_WIDTH 82
#define BACKBTN_HEIGHT 58

#define BACKBTN_X 12
#define BACKBTN_Y 698

ModuleCaptainCreation::ModuleCaptainCreation()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), m_wizPage(WP_NONE),
      m_profession_choice(make_shared<ModuleProfessionChoice>()),
      m_captain_details(make_shared<ModuleCaptainDetails>()),
      m_back_button(make_shared<Button>(
          BACKBTN_X,
          BACKBTN_Y,
          BACKBTN_WIDTH,
          BACKBTN_HEIGHT,
          EVENT_NONE,
          EVENT_CAPTAINCREATION_BACK,
          images[I_CAPTAINCREATION_BACK],
          images[I_CAPTAINCREATION_BACK_MOUSEOVER],
          nullptr,
          samples[S_BUTTONCLICK])) {

    add_child_module(m_profession_choice);
    add_child_module(m_captain_details);

    // last to draw so it is on top of other sub-modules
    add_child_module(m_back_button);
}

ModuleCaptainCreation::~ModuleCaptainCreation() {}

bool
ModuleCaptainCreation::on_init() {
    m_wizPage = WP_PROFESSION_CHOICE;
    m_profession_choice->set_active(true);
    m_captain_details->set_active(false);

    return true;
}

bool
ModuleCaptainCreation::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);
    switch (m_wizPage) {
    case WP_PROFESSION_CHOICE:
        {
            static bool help1 = true;
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
                set_modal_child(make_shared<MessageBoxWindow>(
                    "", str, 600, 400, 400, 300, YELLOW, false));
            }
        }
        break;

    case WP_DETAILS:

        break;
    case WP_NONE:
        break;
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
    g_game->gameState->setProfession(PROFESSION_FREELANCE);
    m_attributesInitial = g_game->gameState->base_skills();
    m_attributesMax = g_game->gameState->max_skills();

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
    g_game->gameState->setProfession(PROFESSION_MILITARY);
    m_attributesInitial = g_game->gameState->base_skills();
    m_attributesMax = g_game->gameState->max_skills();

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
    // set military attributes
    g_game->gameState->setProfession(PROFESSION_SCIENTIFIC);
    m_attributesInitial = g_game->gameState->base_skills();
    m_attributesMax = g_game->gameState->max_skills();

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
    if (m_wizPage == WP_PROFESSION_CHOICE) {
        switch (event->type) {
        case EVENT_CLOSE:
            set_modal_child(nullptr);
            return true;
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
        bool creationComplete = false;
        bool playBtnSnd = false;

        if (event->type == EVENT_CAPTAINCREATION_BACK) {
            m_wizPage = WP_PROFESSION_CHOICE;
            m_profession_choice->set_active(true);
            m_captain_details->set_active(false);
            return false;
        }

        if (event->type == EVENT_CAPTAINCREATION_FINISH) {
            playBtnSnd = true;

            g_game->gameState->create_captain(
                m_captain_details->get_name(),
                m_captain_details->get_attributes());
            g_game->gameState->m_captainSelected = true;
            g_game->gameState->SaveGame(GameState::GAME_SAVE_SLOT_NEW);

            creationComplete = true;
        }

        if (playBtnSnd) {
            g_game->audioSystem->Play(samples[S_BUTTONCLICK]);
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
