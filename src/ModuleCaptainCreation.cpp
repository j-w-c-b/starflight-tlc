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

#define PROFBTN_SCIENTIFIC_X 13
#define PROFBTN_SCIENTIFIC_Y 219
#define PROFBTN_FREELANCE_X 349
#define PROFBTN_FREELANCE_Y 219
#define PROFBTN_MILITARY_X 686
#define PROFBTN_MILITARY_Y 219
#define BACKBTN_WIDTH 82
#define BACKBTN_HEIGHT 58

#define BACKBTN_X 12
#define BACKBTN_Y 698

#define FINISHBTN_X 860
#define FINISHBTN_Y 585

#define PROFESSION_BOX_X 72
#define PROFESSION_BOX_WIDTH 880
#define PROFESSION_BOX_Y 390
#define PROFESSION_BOX_HEIGHT 260

#define DETAILS_BOX_X 72
#define DETAILS_BOX_WIDTH 880
#define DETAILS_BOX_Y 170
#define DETAILS_BOX_HEIGHT 480

#define TEXTCOL al_map_rgb(0, 255, 255)

#define TEXTHEIGHT_TITLES 60
#define TEXTHEIGHT_PROFESSIONNAMES 40
#define TEXTHEIGHT_NAME 30
#define TEXTHEIGHT_ATTRIBUTES 30

#define PROFESSIONAMES_VERT_SPACE 10

#define DETAILS_FONT_SIZE 32
#define NAME_X DETAILS_BOX_X + DETAILS_FONT_SIZE
#define NAME_Y DETAILS_BOX_Y + DETAILS_FONT_SIZE
#define NAME_MAXLEN 15

#define CURSOR_Y NAME_Y
#define CURSOR_DELAY 10

#define ATTS_X 287
#define ATTS_Y 144

#define ATTR_LABEL_X 32
#define ATTR_LABEL_WIDTH 300
#define ATTR_LABEL_HEIGHT 40
#define ATTR_VALUE_WIDTH 48
#define ATTR_VALUE_PADDING 32
#define ATTR_PLUS_WIDTH 37
#define ATTR_PLUS_HEIGHT 37
#define ATTR_PLUS_PADDING 5
#define ATTR_MINUS_WIDTH 37
#define ATTR_MINUS_HEIGHT 37
#define ATTR_MINUS_PADDING 32
#define ATTR_AVAILABLE_WIDTH 270

#define ATTS_COMMON_X NAME_X
// NOTE: right-align relative to NAME_X
#define ATTS_VALS_RIGHT_OFFSET_X 380 - DETAILS_FONT_SIZE
// NOTE: VALS are right aligned
#define ATTS_VALS_COMMON_X DETAILS_BOX_X + 380
#define ATTS_PLUS_COMMON_X ATTS_VALS_COMMON_X + DETAILS_FONT_SIZE
#define ATTS_MAX_COMMON_X ATTS_PLUS_COMMON_X + 42 * 2 + DETAILS_FONT_SIZE
#define ATTS_MINUS_COMMON_X ATTS_PLUS_COMMON_X + 42
// NOTE: right aligned
#define ATTS_AVAILPTS_COMMON_X DETAILS_BOX_X + 700
#define ATTS_Y_BASE NAME_Y + 2 * DETAILS_FONT_SIZE
#define ATTS_Y_SPACING 40
#define ATTS_PLUS_Y_BASE (ATTS_Y_BASE - 7)

#define RESET_X 868
#define RESET_Y 542

#define DURABILITY_X ATTS_COMMON_X
#define DURABILITY_Y (ATTS_Y_BASE + (ATTS_Y_SPACING * 0))
#define PLUS_DURABILITY_X ATTS_PLUS_COMMON_X
#define PLUS_DURABILITY_Y (ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING * 0))
#define LEARNRATE_X ATTS_COMMON_X
#define LEARNRATE_Y (ATTS_Y_BASE + (ATTS_Y_SPACING * 1))
#define PLUS_LEARNRATE_X ATTS_PLUS_COMMON_X
#define PLUS_LEARNRATE_Y (ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING * 1))

#define SCIENCE_X ATTS_COMMON_X
#define SCIENCE_Y (ATTS_Y_BASE + (ATTS_Y_SPACING * 3))
#define PLUS_SCIENCE_X ATTS_PLUS_COMMON_X
#define PLUS_SCIENCE_Y (ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING * 3))
#define NAVIGATION_X ATTS_COMMON_X
#define NAVIGATION_Y (ATTS_Y_BASE + (ATTS_Y_SPACING * 4))
#define PLUS_NAVIGATION_X ATTS_PLUS_COMMON_X
#define PLUS_NAVIGATION_Y (ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING * 4))
#define TACTICS_X ATTS_COMMON_X
#define TACTICS_Y (ATTS_Y_BASE + (ATTS_Y_SPACING * 5))
#define PLUS_TACTICS_X ATTS_PLUS_COMMON_X
#define PLUS_TACTICS_Y (ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING * 5))
#define ENGINEERING_X ATTS_COMMON_X
#define ENGINEERING_Y (ATTS_Y_BASE + (ATTS_Y_SPACING * 6))
#define PLUS_ENGINEERING_X ATTS_PLUS_COMMON_X
#define PLUS_ENGINEERING_Y (ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING * 6))
#define COMMUNICATION_X ATTS_COMMON_X
#define COMMUNICATION_Y (ATTS_Y_BASE + (ATTS_Y_SPACING * 7))
#define PLUS_COMMUNICATION_X ATTS_PLUS_COMMON_X
#define PLUS_COMMUNICATION_Y (ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING * 7))
#define MEDICAL_X ATTS_COMMON_X
#define MEDICAL_Y (ATTS_Y_BASE + (ATTS_Y_SPACING * 8))
#define PLUS_MEDICAL_X ATTS_PLUS_COMMON_X
#define PLUS_MEDICAL_Y (ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING * 8))

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

#define INITIAL_AVAIL_PTS 5
#define INITIAL_AVAIL_PROF_PTS 25

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
      m_click_loaded(g_game->audioSystem->Load(
          resolve_sample_name(S_BUTTONCLICK),
          S_BUTTONCLICK)),
      m_wizPage(WP_NONE),
      m_profession_choice(new ModuleProfessionChoice(m_resources)),
      m_physical_points(new ModuleCaptainAttributeGroup(
          DETAILS_BOX_X,
          DETAILS_BOX_Y,
          DETAILS_BOX_WIDTH,
          DETAILS_BOX_HEIGHT,
          INITIAL_AVAIL_PTS,
          m_resources)),
      m_profession_choice(new ModuleCaptainAttributeGroup(
          DETAILS_BOX_X,
          DETAILS_BOX_Y,
          DETAILS_BOX_WIDTH,
          DETAILS_BOX_HEIGHT,
          INITIAL_AVAIL_PROF_PTS,
          m_resources)),
      m_back_button(new NewButton(
          BACKBTN_X,
          BACKBTN_Y,
          BACKBTN_WIDTH,
          BACKBTN_HEIGHT,
          EVENT_NONE,
          EVENT_CAPTAINCREATION_BACK,
          m_resources[I_CAPTAINCREATION_BACK],
          m_resources[I_CAPTAINCREATION_BACK_MOUSEOVER],
          nullptr,
          S_BUTTONCLICK)),
      m_cursorIdx(0), m_cursorIdxDelay(0), m_mouseOverImg(nullptr) {

    add_child_module(m_profession_choice);

    add_child_module(m_physical_points);
    add_child_module(m_profession_points);

    m_physical_points->set_active(false);
    m_profession_points->set_active(false);

    // last to draw over child backgrounds
    add_child_module(m_back_button);
}

ModuleCaptainCreation::~ModuleCaptainCreation() {}

bool
ModuleCaptainCreation::on_init() {
    if (!m_resources.load()) {
        g_game->message("CaptainCreation: Error loading resources");
        return false;
    }

    m_detailsBackground = m_resources[I_CAPTAINCREATION_DETAILSBACKGROUND];
    m_resetBtn = m_resources[I_CAPTAINCREATION_RESET];
    m_resetBtnMouseOver = m_resources[I_CAPTAINCREATION_RESET_MOUSEOVER];

    ALLEGRO_BITMAP *btnNorm, *btnOver, *btnDis;

    btnNorm = m_resources[I_CAPTAINCREATION_FINISH];
    btnOver = m_resources[I_CAPTAINCREATION_FINISH_MOUSEOVER];
    btnDis = m_resources[I_CAPTAINCREATION_FINISH_DISABLED];

    m_finishBtn = new Button(
        btnNorm,
        btnOver,
        btnDis,
        FINISHBTN_X,
        FINISHBTN_Y,
        EVENT_NONE,
        EVENT_CAPTAINCREATION_FINISH,
        "",
        false,
        true);
    if (m_finishBtn == nullptr)
        return false;
    if (!m_finishBtn->IsInitialized())
        return false;

    m_cursor[0] = m_resources[I_CAPTAINCREATION_CURSOR0];
    m_cursor[1] = m_resources[I_CAPTAINCREATION_CURSOR1];

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

    m_mouseOverImg = nullptr;
    m_name = "";
    m_wizPage = WP_PROFESSION_CHOICE;
    m_profession_choice->set_active(true);

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
            al_draw_bitmap(m_detailsBackground, 0, 0, 0);

            al_draw_text(
                g_game->font60,
                TEXTCOL,
                SCREEN_WIDTH / 2,
                30,
                ALLEGRO_ALIGN_CENTER,
                "Captain Details");

            string name = "Name: " + m_name;
            al_draw_text(
                g_game->font32, TEXTCOL, NAME_X, NAME_Y, 0, name.c_str());

            int nlen = al_get_text_width(g_game->font32, name.c_str());
            al_draw_bitmap(
                m_cursor[m_cursorIdx], NAME_X + nlen + 2, CURSOR_Y, 0);

            if (++m_cursorIdxDelay > CURSOR_DELAY) {
                m_cursorIdxDelay = 0;
                m_cursorIdx++;
                if (m_cursorIdx > 1)
                    m_cursorIdx = 0;
            }

            al_draw_bitmap(m_resetBtn, RESET_X, RESET_Y, 0);

            if (m_physical_points->get_available_points() == 0
                && m_profession_points->get_available_points() == 0
                && (m_name.size() > 0)) {
                m_finishBtn->SetEnabled(true);
            } else {
                m_finishBtn->SetEnabled(false);
            }

            m_finishBtn->Run(g_game->GetBackBuffer());

            if (m_mouseOverImg != nullptr) {
                al_draw_bitmap(
                    m_mouseOverImg, m_mouseOverImgX, m_mouseOverImgY, 0);
            }

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
    if (m_finishBtn != nullptr) {
        delete m_finishBtn;
        m_finishBtn = nullptr;
    }

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
    if (m_wizPage == WP_DETAILS) {
        bool playKeySnd = false;
        bool playErrSnd = false;

        if (isalnum(event->unichar) || event->unichar == ' ') {
            if (m_name.size() < NAME_MAXLEN) {
                char c = event->unichar;
                m_name.push_back(c);

                playKeySnd = true;
            } else
                playErrSnd = true;
        } else if (event->keycode == ALLEGRO_KEY_BACKSPACE) {
            if (m_name.size() > 0) {
                m_name.erase(--(m_name.end()));

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
ModuleCaptainCreation::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    switch (m_wizPage) {
    case WP_PROFESSION_CHOICE:
        {
        }
        break;

    case WP_DETAILS:
        {
            if ((x >= RESET_X)
                && (x < (RESET_X + al_get_bitmap_width(m_resetBtn)))
                && (y >= RESET_Y)
                && (y < (RESET_Y + al_get_bitmap_height(m_resetBtn)))) {
                m_mouseOverImg = m_resetBtnMouseOver;
                m_mouseOverImgX = RESET_X;
                m_mouseOverImgY = RESET_Y;
            } else {
                m_mouseOverImg = nullptr;
            }

            m_finishBtn->OnMouseMove(x, y);
        }
        break;
    case WP_NONE:
        break;
    }
    return true;
}

void
ModuleCaptainCreation::chooseFreelance() {
    // set freelance attributes
    m_profession = PROFESSION_FREELANCE;
    m_attributes.durability = BASEATT_FREELANCE_DURABILITY;
    m_attributes.learnRate = BASEATT_FREELANCE_LEARNRATE;
    m_attributes.science = BASEATT_FREELANCE_SCIENCE;
    m_attributes.navigation = BASEATT_FREELANCE_NAVIGATION;
    m_attributes.tactics = BASEATT_FREELANCE_TACTICS;
    m_attributes.engineering = BASEATT_FREELANCE_ENGINEERING;
    m_attributes.communication = BASEATT_FREELANCE_COMMUNICATION;
    m_attributes.medical = BASEATT_FREELANCE_MEDICAL;
    m_attributesInitial = m_attributes;

    // set attribute max values
    m_attributesMax.durability = MAXATT_FREELANCE_DURABILITY;
    m_attributesMax.learnRate = MAXATT_FREELANCE_LEARNRATE;
    m_attributesMax.science = MAXATT_FREELANCE_SCIENCE;
    m_attributesMax.navigation = MAXATT_FREELANCE_NAVIGATION;
    m_attributesMax.tactics = MAXATT_FREELANCE_TACTICS;
    m_attributesMax.engineering = MAXATT_FREELANCE_ENGINEERING;
    m_attributesMax.communication = MAXATT_FREELANCE_COMMUNICATION;
    m_attributesMax.medical = MAXATT_FREELANCE_MEDICAL;

    m_availPts = INITIAL_AVAIL_PTS;
    m_availProfPts = INITIAL_AVAIL_PROF_PTS;

    // store attributes in gamestate
    g_game->gameState->setProfession(m_profession);
    g_game->gameState->officerCap->attributes = m_attributes;

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
    m_attributes.durability = BASEATT_MILITARY_DURABILITY;
    m_attributes.learnRate = BASEATT_MILITARY_LEARNRATE;
    m_attributes.science = BASEATT_MILITARY_SCIENCE;
    m_attributes.navigation = BASEATT_MILITARY_NAVIGATION;
    m_attributes.tactics = BASEATT_MILITARY_TACTICS;
    m_attributes.engineering = BASEATT_MILITARY_ENGINEERING;
    m_attributes.communication = BASEATT_MILITARY_COMMUNICATION;
    m_attributes.medical = BASEATT_MILITARY_MEDICAL;
    m_attributesInitial = m_attributes;

    // maximum attribute values
    m_attributesMax.durability = MAXATT_MILITARY_DURABILITY;
    m_attributesMax.learnRate = MAXATT_MILITARY_LEARNRATE;
    m_attributesMax.science = MAXATT_MILITARY_SCIENCE;
    m_attributesMax.navigation = MAXATT_MILITARY_NAVIGATION;
    m_attributesMax.tactics = MAXATT_MILITARY_TACTICS;
    m_attributesMax.engineering = MAXATT_MILITARY_ENGINEERING;
    m_attributesMax.communication = MAXATT_MILITARY_COMMUNICATION;
    m_attributesMax.medical = MAXATT_MILITARY_MEDICAL;

    m_availPts = INITIAL_AVAIL_PTS;
    m_availProfPts = INITIAL_AVAIL_PROF_PTS;

    // store attributes in gamestate
    g_game->gameState->setProfession(m_profession);
    g_game->gameState->officerCap->attributes = m_attributes;

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
    m_attributes.durability = BASEATT_SCIENTIFIC_DURABILITY;
    m_attributes.learnRate = BASEATT_SCIENTIFIC_LEARNRATE;
    m_attributes.science = BASEATT_SCIENTIFIC_SCIENCE;
    m_attributes.navigation = BASEATT_SCIENTIFIC_NAVIGATION;
    m_attributes.tactics = BASEATT_SCIENTIFIC_TACTICS;
    m_attributes.engineering = BASEATT_SCIENTIFIC_ENGINEERING;
    m_attributes.communication = BASEATT_SCIENTIFIC_COMMUNICATION;
    m_attributes.medical = BASEATT_SCIENTIFIC_MEDICAL;
    m_attributesInitial = m_attributes;

    // maximum attribute values
    m_attributesMax.durability = MAXATT_SCIENTIFIC_DURABILITY;
    m_attributesMax.learnRate = MAXATT_SCIENTIFIC_LEARNRATE;
    m_attributesMax.science = MAXATT_SCIENTIFIC_SCIENCE;
    m_attributesMax.navigation = MAXATT_SCIENTIFIC_NAVIGATION;
    m_attributesMax.tactics = MAXATT_SCIENTIFIC_TACTICS;
    m_attributesMax.engineering = MAXATT_SCIENTIFIC_ENGINEERING;
    m_attributesMax.communication = MAXATT_SCIENTIFIC_COMMUNICATION;
    m_attributesMax.medical = MAXATT_SCIENTIFIC_MEDICAL;

    m_availPts = INITIAL_AVAIL_PTS;
    m_availProfPts = INITIAL_AVAIL_PROF_PTS;

    // store attributes in gamestate
    g_game->gameState->setProfession(m_profession);
    g_game->gameState->officerCap->attributes = m_attributes;

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
ModuleCaptainCreation::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;
    int button = event->button - 1;

    if (!is_mouse_click(event)) {
        return true;
    }

    switch (m_wizPage) {
    case WP_PROFESSION_CHOICE:
        break;

    case WP_DETAILS:
        {
            bool playSnd = false;
            bool playErrSnd = false;

            if ((x >= RESET_X)
                && (x < (RESET_X + al_get_bitmap_width(m_resetBtn)))
                && (y >= RESET_Y)
                && (y < (RESET_Y + al_get_bitmap_height(m_resetBtn)))) {
                playSnd = true;
                m_attributes = m_attributesInitial;
                m_availPts = INITIAL_AVAIL_PTS;
                m_availProfPts = INITIAL_AVAIL_PROF_PTS;
            }

            m_finishBtn->OnMouseReleased(button, x, y);

            if (playSnd) {
                g_game->audioSystem->Play(m_sndBtnClick);
            }

            if (playErrSnd) {
                g_game->audioSystem->Play(m_sndErr);
            }
        }
        break;
    case WP_NONE:
        break;
    }
    return true;
}

bool
ModuleCaptainCreation::on_event(ALLEGRO_EVENT *event) {
    bool playBtnSnd = false;
    bool creationComplete = false;
    bool playErrSnd = false;

    if (m_wizPage == WP_PROFESSION_CHOICE) {
        switch (event->type) {
        case EVENT_PROFESSION_SCIENTIFIC:
            chooseScience();
            m_wizPage = WP_DETAILS;
            m_profession_choice->set_active(false);
            m_physical_points->set_active(true);
            m_profession_points->set_active(true);
            return false;
        case EVENT_PROFESSION_FREELANCE:
            chooseFreelance();
            m_wizPage = WP_DETAILS;
            m_profession_choice->set_active(false);
            m_physical_points->set_active(true);
            m_profession_points->set_active(true);
            return false;
        case EVENT_PROFESSION_MILITARY:
            chooseMilitary();
            m_wizPage = WP_DETAILS;
            m_profession_choice->set_active(false);
            m_physical_points->set_active(true);
            m_profession_points->set_active(true);
            return false;
        case EVENT_CAPTAINCREATION_BACK:
            m_wizPage = WP_PROFESSION_CHOICE;
            // return player to previous screen
            g_game->LoadModule(g_game->modeMgr->GetPrevModuleName());
            return false;
        }
        return true;
    }
    if (event->type == EVENT_CAPTAINCREATION_BACK) {
        m_wizPage = WP_PROFESSION_CHOICE;
        m_profession_choice->set_active(true);
        return false;
    }
    if (m_availPts < INITIAL_AVAIL_PTS) {
        if (event->type == EVENT_CAPTAINCREATION_MINUS_DURABILITY) {
            // durability
            if (m_attributes.durability > m_attributesInitial.durability) {
                playBtnSnd = true;
                m_attributes.durability--;
                m_availPts++;
            } else {
                playErrSnd = true;
            }
        } else if (event->type == EVENT_CAPTAINCREATION_MINUS_LEARNRATE) {
            // learn rate
            if (m_attributes.learnRate > m_attributesInitial.learnRate) {
                playBtnSnd = true;
                m_attributes.learnRate--;
                m_availPts++;
            } else {
                playErrSnd = true;
            }
        }
    } else {
        if (event->type == EVENT_CAPTAINCREATION_MINUS_DURABILITY
            || event->type == EVENT_CAPTAINCREATION_MINUS_LEARNRATE) {
            playErrSnd = true;
        }
    }
    if (m_availProfPts < INITIAL_AVAIL_PROF_PTS) {
        if (event->type == EVENT_CAPTAINCREATION_MINUS_SCIENCE) {
            // science
            if (m_attributes.science > m_attributesInitial.science) {
                playBtnSnd = true;
                m_attributes.science--;
                m_availProfPts++;
            } else {
                playErrSnd = true;
            }
        } else if (event->type == EVENT_CAPTAINCREATION_MINUS_NAVIGATION) {
            // navigation
            if (m_attributes.navigation > m_attributesInitial.navigation) {
                playBtnSnd = true;
                m_attributes.navigation--;
                m_availProfPts++;
            } else {
                playErrSnd = true;
            }
        } else if (event->type == EVENT_CAPTAINCREATION_MINUS_TACTICS) {
            // tactics
            if (m_attributes.tactics > m_attributesInitial.tactics) {
                playBtnSnd = true;
                m_attributes.tactics--;
                m_availProfPts++;
            } else {
                playErrSnd = true;
            }
        } else if (event->type == EVENT_CAPTAINCREATION_MINUS_ENGINEERING) {
            // engineering
            if (m_attributes.engineering > m_attributesInitial.engineering) {
                playBtnSnd = true;
                m_attributes.engineering--;
                m_availProfPts++;
            } else {
                playErrSnd = true;
            }
        } else if (event->type == EVENT_CAPTAINCREATION_MINUS_COMMUNICATION) {
            // communication
            if (m_attributes.communication
                > m_attributesInitial.communication) {
                playBtnSnd = true;
                m_attributes.communication--;
                m_availProfPts++;
            } else {
                playErrSnd = true;
            }
        } else if (event->type == EVENT_CAPTAINCREATION_MINUS_MEDICAL) {
            // medical
            if (m_attributes.medical > m_attributesInitial.medical) {
                playBtnSnd = true;
                m_attributes.medical--;
                m_availProfPts++;
            } else {
                playErrSnd = true;
            }
        }
    } else {
        switch (event->type) {
        case EVENT_CAPTAINCREATION_MINUS_SCIENCE:
        case EVENT_CAPTAINCREATION_MINUS_NAVIGATION:
        case EVENT_CAPTAINCREATION_MINUS_TACTICS:
        case EVENT_CAPTAINCREATION_MINUS_ENGINEERING:
        case EVENT_CAPTAINCREATION_MINUS_COMMUNICATION:
        case EVENT_CAPTAINCREATION_MINUS_MEDICAL:
            playErrSnd = true;
        }
    }

    if (playErrSnd) {
        g_game->audioSystem->Play(m_sndErr);
    }

    if (event->type == EVENT_CAPTAINCREATION_FINISH) {
        playBtnSnd = true;

        // this ends up calling g_game->gameState->m_ship.Reset()
        // so most of the changes we did on the ship are thrown out
        g_game->gameState->Reset();
        g_game->gameState->m_profession = m_profession;
        g_game->gameState->officerCap->name = m_name;
        g_game->gameState->officerCap->attributes = m_attributes;
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
    return true;
}
// vi: ft=cpp
