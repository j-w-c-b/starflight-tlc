/*
        STARFLIGHT - THE LOST COLONY
        ModuleMedical.cpp
        Author: Keith Patch
        Date: April 2008
*/

#include <sstream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModuleMedical.h"
#include "Script.h"
#include "Util.h"
#include "medical_resources.h"

using namespace std;
using namespace medical_resources;

int gvl = 0, gvr = 0;                // global viewer right and left
const int RIGHT_TARGET_OFFSET = 600; // SCREEN_WIDTH - 436;
const int LEFT_TARGET_OFFSET = 396;
const int VIEWER_MOVE_RATE = 16;
#define CATBTN_X 633
#define CATBTN_Y 120
#define CATSPACING 40

ALLEGRO_DEBUG_CHANNEL("ModuleMedical")

int right_offset = SCREEN_WIDTH, left_offset = -LEFT_TARGET_OFFSET,
    left_offset2 = -LEFT_TARGET_OFFSET;

ModuleMedical::ModuleMedical() : resources(MEDICAL_IMAGES) {}

ModuleMedical::~ModuleMedical() {}

void
ModuleMedical::disable_others(int officer) {
    if (selected_officer != NULL || officer != -1) {
        for (int i = 0; i < 7; i++) {
            if (i != officer) // this officer is not me
            {
                OfficerBtns[i]->SetEnabled(false);
                HealBtns[i]->SetEnabled(false);
                HealBtns[i]->SetVisible(false);
            }
        }
    } else {
        for (int i = 0; i < 7; i++) {
            OfficerBtns[i]->SetEnabled(true);
            HealBtns[i]->SetEnabled(true);
            HealBtns[i]->SetVisible(true);
        }
    }
}

void
ModuleMedical::cease_healing() {
    g_game->gameState->officerCap->Recovering(false);
    g_game->gameState->officerSci->Recovering(false);
    g_game->gameState->officerEng->Recovering(false);
    g_game->gameState->officerNav->Recovering(false);
    g_game->gameState->officerDoc->Recovering(false);
    g_game->gameState->officerTac->Recovering(false);
    g_game->gameState->officerCom->Recovering(false);
}

void
ModuleMedical::OnEvent(Event *event) {
    Officer *currentDoc = g_game->gameState->getCurrentDoc();
    string med = currentDoc->getLastName() + "-> ";
    string other = "";

    switch (event->getEventType()) {
    /**
            CREW SELECTED EVENTS
    **/
    case -100: // captain selected
        if (selected_officer != g_game->gameState->officerCap) {
            selected_officer = g_game->gameState->officerCap;
            disable_others(0);
        } else {
            selected_officer = NULL;
            disable_others(-1);
        }
        break;
    case -101: // science selected
        if (selected_officer != g_game->gameState->officerSci) {
            selected_officer = g_game->gameState->officerSci;
            disable_others(1);
        } else {
            selected_officer = NULL;
            disable_others(-1);
        }
        break;
    case -102: // navigation selected
        if (selected_officer != g_game->gameState->officerNav) {
            selected_officer = g_game->gameState->officerNav;
            disable_others(2);
        } else {
            selected_officer = NULL;
            disable_others(-1);
        }
        break;
    case -103: // tactical selected
        if (selected_officer != g_game->gameState->officerTac) {
            selected_officer = g_game->gameState->officerTac;
            disable_others(3);
        } else {
            selected_officer = NULL;
            disable_others(-1);
        }
        break;
    case -104: // engineer selected
        if (selected_officer != g_game->gameState->officerEng) {
            selected_officer = g_game->gameState->officerEng;
            disable_others(4);
        } else {
            selected_officer = NULL;
            disable_others(-1);
        }
        break;
    case -105: // communications selected
        if (selected_officer != g_game->gameState->officerCom) {
            selected_officer = g_game->gameState->officerCom;
            disable_others(5);
        } else {
            selected_officer = NULL;
            disable_others(-1);
        }
        break;
    case -106: // medical selected
        if (selected_officer != g_game->gameState->officerDoc) {
            selected_officer = g_game->gameState->officerDoc;
            disable_others(6);
        } else {
            selected_officer = NULL;
            disable_others(-1);
        }
        break;

    /**
            CREW TREATMENT EVENTS
    **/
    case -200: // captain treat
        cease_healing();
        if (g_game->gameState->officerCap->isBeingHealed() == false &&
            g_game->gameState->officerCap->attributes.getVitality() < 100 &&
            g_game->gameState->officerCap->attributes.getVitality() > 0) {
            g_game->gameState->officerCap->Recovering(true);
            other = "Captain " + g_game->gameState->officerCap->getLastName();
            (currentDoc == g_game->gameState->officerCap)
                ? g_game->printout(g_game->g_scrollbox,
                                   med + "Okay, I'll patch myself up",
                                   GREEN,
                                   1000)
                : g_game->printout(g_game->g_scrollbox,
                                   med + "Okay, I'm treating " + other,
                                   GREEN,
                                   1000);
        }
        break;
    case -201: // science treat
        cease_healing();
        if (g_game->gameState->officerSci->isBeingHealed() == false &&
            g_game->gameState->officerSci->attributes.getVitality() < 100 &&
            g_game->gameState->officerSci->attributes.getVitality() > 0) {
            g_game->gameState->officerSci->Recovering(true);
            other = "Science Officer " +
                    g_game->gameState->officerSci->getLastName();
            g_game->printout(g_game->g_scrollbox,
                             med + "Okay, I'm treating " + other,
                             GREEN,
                             1000);
        }
        break;
    case -202: // navigation treat
        cease_healing();
        if (g_game->gameState->officerNav->isBeingHealed() == false &&
            g_game->gameState->officerNav->attributes.getVitality() < 100 &&
            g_game->gameState->officerNav->attributes.getVitality() > 0) {
            g_game->gameState->officerNav->Recovering(true);
            other = "Navigator " + g_game->gameState->officerNav->getLastName();
            g_game->printout(g_game->g_scrollbox,
                             med + "Okay, I'm treating " + other,
                             GREEN,
                             1000);
        }
        break;
    case -203: // tactical treat
        cease_healing();
        if (g_game->gameState->officerTac->isBeingHealed() == false &&
            g_game->gameState->officerTac->attributes.getVitality() < 100 &&
            g_game->gameState->officerTac->attributes.getVitality() > 0) {
            g_game->gameState->officerTac->Recovering(true);
            other = "Tactical Officer " +
                    g_game->gameState->officerTac->getLastName();
            g_game->printout(g_game->g_scrollbox,
                             med + "Okay, I'm treating " + other,
                             GREEN,
                             1000);
        }
        break;
    case -204: // engineer treat
        cease_healing();
        if (g_game->gameState->officerEng->isBeingHealed() == false &&
            g_game->gameState->officerEng->attributes.getVitality() < 100 &&
            g_game->gameState->officerEng->attributes.getVitality() > 0) {
            g_game->gameState->officerEng->Recovering(true);
            other = "Engineer " + g_game->gameState->officerEng->getLastName();
            g_game->printout(g_game->g_scrollbox,
                             med + "Okay, I'm treating " + other,
                             GREEN,
                             1000);
        }
        break;
    case -205: // communications treat
        cease_healing();
        if (g_game->gameState->officerCom->isBeingHealed() == false &&
            g_game->gameState->officerCom->attributes.getVitality() < 100 &&
            g_game->gameState->officerCom->attributes.getVitality() > 0) {
            g_game->gameState->officerCom->Recovering(true);
            other =
                "Comm Officer " + g_game->gameState->officerCom->getLastName();
            g_game->printout(g_game->g_scrollbox,
                             med + "Okay, I'm treating " + other,
                             GREEN,
                             1000);
        }
        break;
    case -206: // medical treat
        cease_healing();
        if (g_game->gameState->officerDoc->isBeingHealed() == false &&
            g_game->gameState->officerDoc->attributes.getVitality() < 100 &&
            g_game->gameState->officerDoc->attributes.getVitality() > 0) {
            g_game->gameState->officerDoc->Recovering(true);
            other = "Doctor " + g_game->gameState->officerDoc->getLastName();
            (currentDoc == g_game->gameState->officerDoc)
                ? g_game->printout(g_game->g_scrollbox,
                                   med + "Okay, I'll patch myself up",
                                   GREEN,
                                   1000)
                : g_game->printout(g_game->g_scrollbox,
                                   med + "Okay, I'm treating " + other,
                                   GREEN,
                                   1000);
        }
        break;

    case 7001: // EVENT_DOCTOR_TREAT
        if (b_examine == false && viewer_active == false) {
            viewer_active = true;
        } else if (viewer_active == true && b_examine == true) {
            b_examine = false;
        } else {
            viewer_active = false;
            b_examine = true;
        }
        break;

    case 7000: // EVENT_DOCTOR_EXAMINE
        if (b_examine == true) {
            viewer_active = false;
        } else {
            viewer_active = true;
            b_examine = true;
        }
        break;
    case 0:
    default:
        break;
    }
}

bool
ModuleMedical::Init() {
    ALLEGRO_DEBUG("  ModuleMedical::Init()\n");

    b_examine = false;
    selected_officer = NULL;

    //
    // GUI stuff
    //

    // sound effects
    g_game->audioSystem->Load("data/medical/buttonclick.ogg", "click");

    // load data file
    if (!resources.load()) {
        g_game->message("Medical: Error loading resources");
        return false;
    }

    img_left_viewer2 = resources[I_GUI_VIEWER_RIGHT];
    img_right_viewer = resources[I_GUI_VIEWER_RIGHT];
    img_right_bg = resources[I_RIGHT_VIEWER_BG];
    img_left_viewer = resources[I_GUI_VIEWER];

    img_left_bg = resources[I_MED_WINDOW_DATA];
    img_health_bar = resources[I_MED_BAR_HEALTH];
    img_science_bar = resources[I_MED_BAR_SCIENCE];
    img_nav_bar = resources[I_MED_BAR_NAVIGATION];
    img_medical_bar = resources[I_MED_BAR_MEDICAL];
    img_engineer_bar = resources[I_MED_BAR_ENGINEER];
    img_dur_bar = resources[I_MED_BAR_DURABILITY];
    img_learn_bar = resources[I_MED_BAR_LEARN];
    img_comm_bar = resources[I_MED_BAR_COMMUNICATION];
    img_tac_bar = resources[I_MED_BAR_TACTICAL];

    // load crew button images
    img_button_crew = resources[I_MEDICAL_CAPTBTN];
    img_button_crew_hov = resources[I_MEDICAL_CAPTBTN_HOV];
    img_button_crew_dis = resources[I_MEDICAL_CAPTBTN_DIS];

    for (int i = 0; i < 7; i++) {
        // Create and initialize the crew buttons
        OfficerBtns[i] = new Button(img_button_crew,
                                    img_button_crew_hov,
                                    img_button_crew_dis,
                                    CATBTN_X,
                                    CATBTN_Y + (i * CATSPACING),
                                    0,
                                    -100 - i,
                                    g_game->font22,
                                    "",
                                    al_map_rgb(255, 255, 255),
                                    "click");

        if (OfficerBtns[i] == NULL) {
            return false;
        }
        if (!OfficerBtns[i]->IsInitialized()) {
            return false;
        }
    }

    // set labels for each crew member
    OfficerBtns[0]->SetButtonText("CAP. " +
                                  g_game->gameState->officerCap->name);
    OfficerBtns[1]->SetButtonText("SCI. " +
                                  g_game->gameState->officerSci->name);
    OfficerBtns[2]->SetButtonText("NAV. " +
                                  g_game->gameState->officerNav->name);
    OfficerBtns[3]->SetButtonText("TAC. " +
                                  g_game->gameState->officerTac->name);
    OfficerBtns[4]->SetButtonText("ENG. " +
                                  g_game->gameState->officerEng->name);
    OfficerBtns[5]->SetButtonText("COM. " +
                                  g_game->gameState->officerCom->name);
    OfficerBtns[6]->SetButtonText("DOC. " +
                                  g_game->gameState->officerDoc->name);

    // load plus button images
    img_treat = resources[I_BTN_NORM];
    img_treat_hov = resources[I_BTN_HOV];
    img_treat_hov = resources[I_BTN_DIS];

    // Create and initialize the heal buttons
    // they share the same location but are unique for each crew to make events
    // simpler
    for (int i = 0; i < 7; i++) {
        HealBtns[i] = new Button(img_treat,
                                 img_treat_hov,
                                 img_treat_dis,
                                 CATBTN_X + 142,
                                 2 + CATBTN_Y + (i * CATSPACING),
                                 0,
                                 -200 - i,
                                 g_game->font24,
                                 "TREAT",
                                 BLACK,
                                 "click");
        if (HealBtns[i] == NULL) {
            return false;
        }
        if (!HealBtns[i]->IsInitialized()) {
            return false;
        }
    }

    return true;
}

void
ModuleMedical::Close() {
    ALLEGRO_DEBUG("*** ModuleMedical::Close()\n");
    resources.unload();

    for (int i = 0; i < 7; i++) {
        delete OfficerBtns[i];
        OfficerBtns[i] = NULL;
    }

    for (int i = 0; i < 7; i++) {
        delete HealBtns[i];
        HealBtns[i] = NULL;
    }
    selected_officer = NULL;
}

void
ModuleMedical::Update() {}

void
ModuleMedical::MedicalUpdate() {
    Officer *currentDoc = g_game->gameState->getCurrentDoc();
    string med = currentDoc->getLastName() + "-> ";
    string other = "";

    // heal medical_skill/50 vitality point every iteration
    float heal_rate =
        g_game->gameState->CalcEffectiveSkill(SKILL_MEDICAL) / 50.0;

    // award one skill point every 12 iterations
    if (currentDoc->attributes.extra_variable >= 12) {
        currentDoc->attributes.extra_variable = 0;

        if (currentDoc->SkillUp(SKILL_MEDICAL))
            g_game->printout(g_game->g_scrollbox,
                             med + "I think I'm getting better at this.",
                             PURPLE,
                             5000);
    }

    if (g_game->gameState->getCurrentSelectedOfficer() != OFFICER_MEDICAL)
        viewer_active = false;

#pragma region Captain
    if (g_game->gameState->officerCap->attributes.getVitality() <= 0) {
        OfficerBtns[0]->SetTextColor(BLACK);
    } else if (g_game->gameState->officerCap->isBeingHealed() == true) {
        OfficerBtns[0]->SetTextColor(LTGREEN);
        if (currentDoc->CanSkillCheck()) {
            currentDoc->FakeSkillCheck();
            currentDoc->attributes.extra_variable++;
            g_game->gameState->officerCap->attributes.augVitality(heal_rate);
        }
        if (g_game->gameState->officerCap->attributes.getVitality() == 100) {
            g_game->gameState->officerCap->Recovering(false);
            other = "Captain " + g_game->gameState->officerCap->getLastName();
            (currentDoc == g_game->gameState->officerCap)
                ? g_game->printout(g_game->g_scrollbox,
                                   med + "I'm feeling much better now.",
                                   BLUE,
                                   1000)
                : g_game->printout(g_game->g_scrollbox,
                                   other + " has fully recovered.",
                                   BLUE,
                                   1000);
        }
    } else if (g_game->gameState->officerCap->attributes.getVitality() < 30) {
        OfficerBtns[0]->SetTextColor(RED2);
    } else if (g_game->gameState->officerCap->attributes.getVitality() < 70) {
        OfficerBtns[0]->SetTextColor(YELLOW2);
    } else {
        OfficerBtns[0]->SetTextColor(WHITE);
    }
#pragma endregion
#pragma region Science Officer
    if (g_game->gameState->officerSci->attributes.getVitality() <= 0) {
        OfficerBtns[1]->SetTextColor(BLACK);
    } else if (g_game->gameState->officerSci->isBeingHealed() == true) {
        OfficerBtns[1]->SetTextColor(LTGREEN);
        if (currentDoc->CanSkillCheck() == true) {
            currentDoc->FakeSkillCheck();
            currentDoc->attributes.extra_variable++;
            g_game->gameState->officerSci->attributes.augVitality(heal_rate);
        }
        if (g_game->gameState->officerSci->attributes.getVitality() == 100) {
            g_game->gameState->officerSci->Recovering(false);
            other = "Science Officer " +
                    g_game->gameState->officerSci->getLastName();
            g_game->printout(g_game->g_scrollbox,
                             other + " has fully recovered.",
                             BLUE,
                             1000);
        }
    } else if (g_game->gameState->officerSci->attributes.getVitality() < 30) {
        OfficerBtns[1]->SetTextColor(RED2);
    } else if (g_game->gameState->officerSci->attributes.getVitality() < 70) {
        OfficerBtns[1]->SetTextColor(YELLOW2);
    } else {
        OfficerBtns[1]->SetTextColor(WHITE);
    }
#pragma endregion
#pragma region Navigation Officer
    if (g_game->gameState->officerNav->attributes.getVitality() <= 0) {
        OfficerBtns[2]->SetTextColor(BLACK);
    } else if (g_game->gameState->officerNav->isBeingHealed() == true) {
        OfficerBtns[2]->SetTextColor(LTGREEN);
        if (currentDoc->CanSkillCheck() == true) {
            currentDoc->FakeSkillCheck();
            currentDoc->attributes.extra_variable++;
            g_game->gameState->officerNav->attributes.augVitality(heal_rate);
        }
        if (g_game->gameState->officerNav->attributes.getVitality() == 100) {
            g_game->gameState->officerNav->Recovering(false);
            other = "Navigator " + g_game->gameState->officerNav->getLastName();
            g_game->printout(g_game->g_scrollbox,
                             other + " has fully recovered.",
                             BLUE,
                             1000);
        }
    } else if (g_game->gameState->officerNav->attributes.getVitality() < 30) {
        OfficerBtns[2]->SetTextColor(RED2);
    } else if (g_game->gameState->officerNav->attributes.getVitality() < 70) {
        OfficerBtns[2]->SetTextColor(YELLOW2);
    } else {
        OfficerBtns[2]->SetTextColor(WHITE);
    }
#pragma endregion
#pragma region Tactical Officer
    if (g_game->gameState->officerTac->attributes.getVitality() <= 0) {
        OfficerBtns[3]->SetTextColor(BLACK);
    } else if (g_game->gameState->officerTac->isBeingHealed() == true) {
        OfficerBtns[3]->SetTextColor(LTGREEN);
        if (currentDoc->CanSkillCheck() == true) {
            currentDoc->FakeSkillCheck();
            currentDoc->attributes.extra_variable++;
            g_game->gameState->officerTac->attributes.augVitality(heal_rate);
        }
        if (g_game->gameState->officerTac->attributes.getVitality() == 100) {
            g_game->gameState->officerTac->Recovering(false);
            other = "Tactical Officer " +
                    g_game->gameState->officerTac->getLastName();
            g_game->printout(g_game->g_scrollbox,
                             other + " has fully recovered.",
                             BLUE,
                             1000);
        }
    } else if (g_game->gameState->officerTac->attributes.getVitality() < 30) {
        OfficerBtns[3]->SetTextColor(RED2);
    } else if (g_game->gameState->officerTac->attributes.getVitality() < 70) {
        OfficerBtns[3]->SetTextColor(YELLOW2);
    } else {
        OfficerBtns[3]->SetTextColor(WHITE);
    }
#pragma endregion
#pragma region Engineer Officer
    if (g_game->gameState->officerEng->attributes.getVitality() <= 0) {
        OfficerBtns[4]->SetTextColor(BLACK);
    } else if (g_game->gameState->officerEng->isBeingHealed() == true) {

        OfficerBtns[4]->SetTextColor(LTGREEN);
        if (currentDoc->CanSkillCheck() == true) {
            currentDoc->FakeSkillCheck();
            currentDoc->attributes.extra_variable++;
            g_game->gameState->officerEng->attributes.augVitality(heal_rate);
        }
        if (g_game->gameState->officerEng->attributes.getVitality() == 100) {
            g_game->gameState->officerEng->Recovering(false);
            other = "Engineer " + g_game->gameState->officerEng->getLastName();
            g_game->printout(g_game->g_scrollbox,
                             other + " has fully recovered.",
                             BLUE,
                             1000);
        }
    } else if (g_game->gameState->officerEng->attributes.getVitality() < 30) {
        OfficerBtns[4]->SetTextColor(RED2);
    } else if (g_game->gameState->officerEng->attributes.getVitality() < 70) {
        OfficerBtns[4]->SetTextColor(YELLOW2);
    } else {
        OfficerBtns[4]->SetTextColor(WHITE);
    }
#pragma endregion
#pragma region Comms Officer
    if (g_game->gameState->officerCom->attributes.getVitality() <= 0) {
        OfficerBtns[5]->SetTextColor(BLACK);
    } else if (g_game->gameState->officerCom->isBeingHealed() == true) {
        OfficerBtns[5]->SetTextColor(LTGREEN);
        if (currentDoc->CanSkillCheck() == true) {
            currentDoc->FakeSkillCheck();
            currentDoc->attributes.extra_variable++;
            g_game->gameState->officerCom->attributes.augVitality(heal_rate);
        }
        if (g_game->gameState->officerCom->attributes.getVitality() == 100) {
            g_game->gameState->officerCom->Recovering(false);
            other =
                "Comm Officer " + g_game->gameState->officerCom->getLastName();
            g_game->printout(g_game->g_scrollbox,
                             other + " has fully recovered.",
                             BLUE,
                             1000);
        }
    } else if (g_game->gameState->officerCom->attributes.getVitality() < 30) {
        OfficerBtns[5]->SetTextColor(RED2);
    } else if (g_game->gameState->officerCom->attributes.getVitality() < 70) {
        OfficerBtns[5]->SetTextColor(YELLOW2);
    } else {
        OfficerBtns[5]->SetTextColor(WHITE);
    }
#pragma endregion
#pragma region Doctor
    if (g_game->gameState->officerDoc->attributes.getVitality() <= 0) {
        OfficerBtns[6]->SetTextColor(BLACK);
    } else if (g_game->gameState->officerDoc->isBeingHealed()) {
        OfficerBtns[6]->SetTextColor(LTGREEN);
        if (currentDoc->CanSkillCheck() == true) {
            currentDoc->FakeSkillCheck();
            currentDoc->attributes.extra_variable++;
            g_game->gameState->officerDoc->attributes.augVitality(heal_rate);
        }
        if (g_game->gameState->officerDoc->attributes.getVitality() == 100) {
            g_game->gameState->officerDoc->Recovering(false);
            other = "Doctor " + g_game->gameState->officerDoc->getLastName();
            (currentDoc == g_game->gameState->officerDoc)
                ? g_game->printout(g_game->g_scrollbox,
                                   med + "I'm feeling much better now.",
                                   BLUE,
                                   1000)
                : g_game->printout(g_game->g_scrollbox,
                                   other + " has fully recovered.",
                                   BLUE,
                                   1000);
        }
    } else if (g_game->gameState->officerDoc->attributes.getVitality() < 30) {
        OfficerBtns[6]->SetTextColor(RED2);
    } else if (g_game->gameState->officerDoc->attributes.getVitality() < 70) {
        OfficerBtns[6]->SetTextColor(YELLOW2);
    } else {
        OfficerBtns[6]->SetTextColor(WHITE);
    }
#pragma endregion
}

void
ModuleMedical::Draw() {
    static const string skillnames[] = {"science",
                                        "navigation",
                                        "engineering",
                                        "communication",
                                        "medical",
                                        "tactical",
                                        "learning",
                                        "durability"};

    al_set_target_bitmap(g_game->GetBackBuffer());

    float percentile = 0.00f;
    char t_buffer[20];

    // update medical status
    MedicalUpdate();

    if (right_offset < SCREEN_WIDTH) {
        // draw background
        al_draw_bitmap(img_right_viewer, right_offset, 85, 0);

        // draw crew gui
        al_draw_bitmap(img_right_bg, right_offset + 34, 119, 0);

        // crew buttons
        for (int i = 0; i < 7; i++) {
            OfficerBtns[i]->SetX(right_offset + 34);
            OfficerBtns[i]->Run(g_game->GetBackBuffer());
        }
    }

#pragma region Left Window 2
    if (left_offset2 > -LEFT_TARGET_OFFSET) {
        al_draw_bitmap(
            img_left_viewer2, left_offset2 - 43, 85, ALLEGRO_FLIP_HORIZONTAL);
        al_draw_bitmap(img_right_bg, left_offset2 + 64, 119, 0);
        if (selected_officer != NULL) {
            // print crew person's name
            g_game->Print22(g_game->GetBackBuffer(),
                            left_offset2 + 75,
                            130,
                            selected_officer->GetTitle() + ": " +
                                selected_officer->name,
                            WHITE);

            // print health status
            ALLEGRO_COLOR health_color = BLACK;
            std::string status = "";
            if (selected_officer->attributes.getVitality() <= 0) {
                health_color = BLACK;
                status = "DEAD";
            } else if (selected_officer->attributes.getVitality() < 30) {
                health_color = RED2;
                status = "CRITICAL";
            } else if (selected_officer->attributes.getVitality() < 70) {
                health_color = YELLOW2;
                status = "INJURED";
            } else {
                health_color = GREEN2;
                status = "HEALTHY";
            }
            g_game->Print20(g_game->GetBackBuffer(),
                            left_offset2 + 75,
                            200,
                            "STATUS: ",
                            WHITE);
            g_game->Print20(g_game->GetBackBuffer(),
                            left_offset2 + 175,
                            200,
                            status,
                            health_color);

            // enable treat button based on crew health
            int officer = selected_officer->GetOfficerType() - 1;
            if (selected_officer->attributes.getVitality() < 100)
                HealBtns[officer]->SetEnabled(true);
            else
                HealBtns[officer]->SetEnabled(false);

            // draw the health bar
            percentile = selected_officer->attributes.getVitality();
            al_draw_filled_rectangle(left_offset2 + 75,
                                     260,
                                     left_offset2 + 75 + 260 * percentile / 100,
                                     315,
                                     health_color);

            // print health percentage
            sprintf(t_buffer, "%.0f%%%%", percentile);
            g_game->Print22(g_game->GetBackBuffer(),
                            left_offset2 + 185,
                            280,
                            t_buffer,
                            WHITE);

            // display heal button
            int buttonx = left_offset2 + 144;
            int buttony = 341;
            for (int i = 0; i < 7; i++) {
                HealBtns[i]->SetX(buttonx);
                HealBtns[i]->SetY(buttony);
                HealBtns[i]->Run(g_game->GetBackBuffer());
            }
        }
    }
#pragma endregion

#pragma region Left Window

    if (left_offset > -LEFT_TARGET_OFFSET) {
        al_draw_bitmap(img_left_viewer, left_offset - 43, 10, 0);
        al_draw_bitmap(img_left_bg, left_offset + 67, 43, 0);

        // display officer stat bars
        if (selected_officer != NULL) {
            int x = left_offset + 110;

            // vitality bar
            percentile = selected_officer->attributes.getVitality();
            percentile /= 100;
            al_draw_bitmap_region(img_health_bar,
                                  0,
                                  0,
                                  al_get_bitmap_width(img_health_bar) *
                                      percentile,
                                  al_get_bitmap_height(img_health_bar),
                                  x,
                                  46,
                                  0);
            g_game->Print22(
                g_game->GetBackBuffer(), x + 10, 46 + 2, "health", BLACK);

            // science bar
            percentile = selected_officer->attributes.getScience();
            percentile /= 250;
            al_draw_bitmap_region(img_science_bar,
                                  0,
                                  0,
                                  al_get_bitmap_width(img_science_bar) *
                                      percentile,
                                  al_get_bitmap_height(img_science_bar),
                                  x,
                                  85,
                                  0);
            g_game->Print22(g_game->GetBackBuffer(),
                            x + 10,
                            85 + 2,
                            "science skill",
                            BLACK);

            // navigation bar
            percentile = selected_officer->attributes.getNavigation();
            percentile /= 250;
            al_draw_bitmap_region(img_nav_bar,
                                  0,
                                  0,
                                  al_get_bitmap_width(img_nav_bar) * percentile,
                                  al_get_bitmap_height(img_nav_bar),
                                  x,
                                  129,
                                  0);
            g_game->Print22(g_game->GetBackBuffer(),
                            x + 10,
                            129 + 2,
                            "navigation skill",
                            BLACK);

            // engineering bar
            percentile = selected_officer->attributes.getEngineering();
            percentile /= 250;
            al_draw_bitmap_region(img_engineer_bar,
                                  0,
                                  0,
                                  al_get_bitmap_width(img_engineer_bar) *
                                      percentile,
                                  al_get_bitmap_height(img_engineer_bar),
                                  x,
                                  177,
                                  0);
            g_game->Print22(g_game->GetBackBuffer(),
                            x + 10,
                            177 + 2,
                            "engineering skill",
                            BLACK);

            // communications bar
            percentile = selected_officer->attributes.getCommunication();
            percentile /= 250;
            al_draw_bitmap_region(img_comm_bar,
                                  0,
                                  0,
                                  al_get_bitmap_width(img_comm_bar) *
                                      percentile,
                                  al_get_bitmap_height(img_comm_bar),
                                  x,
                                  221,
                                  0);
            g_game->Print22(g_game->GetBackBuffer(),
                            x + 10,
                            221 + 2,
                            "communications skill",
                            BLACK);

            // medical bar
            percentile = selected_officer->attributes.getMedical();
            percentile /= 250;
            al_draw_bitmap_region(img_medical_bar,
                                  0,
                                  0,
                                  al_get_bitmap_width(img_medical_bar) *
                                      percentile,
                                  al_get_bitmap_height(img_medical_bar),
                                  x,
                                  266,
                                  0);
            g_game->Print22(g_game->GetBackBuffer(),
                            x + 10,
                            266 + 2,
                            "medical skill",
                            BLACK);

            // tactical bar
            percentile = selected_officer->attributes.getTactics();
            percentile /= 250;
            al_draw_bitmap_region(img_tac_bar,
                                  0,
                                  0,
                                  al_get_bitmap_width(img_tac_bar) * percentile,
                                  al_get_bitmap_height(img_tac_bar),
                                  x,
                                  311,
                                  0);
            g_game->Print22(g_game->GetBackBuffer(),
                            x + 10,
                            311 + 2,
                            "tactical skill",
                            BLACK);

            // learning rate bar
            percentile = selected_officer->attributes.getLearnRate();
            percentile /= 65;
            al_draw_bitmap_region(img_learn_bar,
                                  0,
                                  0,
                                  al_get_bitmap_width(img_learn_bar) *
                                      percentile,
                                  al_get_bitmap_height(img_learn_bar),
                                  x,
                                  357,
                                  0);
            g_game->Print22(
                g_game->GetBackBuffer(), x + 10, 357 + 2, "learning", BLACK);

            // durability
            percentile = selected_officer->attributes.getDurability();
            percentile /= 65;
            al_draw_bitmap_region(img_dur_bar,
                                  0,
                                  0,
                                  al_get_bitmap_width(img_dur_bar) * percentile,
                                  al_get_bitmap_height(img_dur_bar),
                                  x,
                                  401,
                                  0);
            g_game->Print22(
                g_game->GetBackBuffer(), x + 10, 401 + 2, "durability", BLACK);
        }
    }
#pragma endregion
    if (viewer_active) {
        if (right_offset > RIGHT_TARGET_OFFSET) {
            right_offset -= VIEWER_MOVE_RATE;
        }
        if (b_examine == true) {
            if (left_offset < 0) {
                left_offset += VIEWER_MOVE_RATE;
            }
            if (left_offset2 > -LEFT_TARGET_OFFSET) {
                left_offset2 -= VIEWER_MOVE_RATE;
            }
        } else {
            if (left_offset2 < 0) {
                left_offset2 += VIEWER_MOVE_RATE;
            }
            if (left_offset > -LEFT_TARGET_OFFSET) {
                left_offset -= VIEWER_MOVE_RATE;
            }
        }
    } else {
        if (right_offset < SCREEN_WIDTH) {
            right_offset += VIEWER_MOVE_RATE;
        }
        if (left_offset > -LEFT_TARGET_OFFSET) {
            left_offset -= VIEWER_MOVE_RATE;
        }
        if (left_offset2 > -LEFT_TARGET_OFFSET) {
            left_offset2 -= VIEWER_MOVE_RATE;
        }
        disable_others(-1);
        selected_officer = NULL;
        b_examine = false;
    }
}

void
ModuleMedical::OnMouseMove(int x, int y) {
    if (g_game->gameState->getCurrentSelectedOfficer() != OFFICER_MEDICAL)
        return;

    for (int i = 0; i < 7; i++) {
        OfficerBtns[i]->OnMouseMove(x, y);
        HealBtns[i]->OnMouseMove(x, y);
    }
}

void
ModuleMedical::OnMouseReleased(int button, int x, int y) {
    if (g_game->gameState->getCurrentSelectedOfficer() != OFFICER_MEDICAL)
        return;

    for (int i = 0; i < 7; i++) {
        OfficerBtns[i]->OnMouseReleased(button, x, y);
        HealBtns[i]->OnMouseReleased(button, x, y);
    }
}
