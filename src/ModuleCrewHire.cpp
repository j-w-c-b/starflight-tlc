///*
//	STARFLIGHT - THE LOST COLONY
//	ModuleCrewHire.cpp - This module gives the player the ability to hire,
// fire, and reassign crew members. 	Author: Justin Sargent 	Date: 9/21/07
// Mods: Jim Haga - JJH 	Date: 3/16/21
//*/

#include "ModuleCrewHire.h"
#include "AudioSystem.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "Label.h"
#include "ModeMgr.h"
#include "PauseMenu.h"
#include "QuestMgr.h"
#include "Util.h"
#include "crewhire_resources.h"

using namespace std;
using namespace crewhire_resources;

ALLEGRO_DEBUG_CHANNEL("ModuleCrewHire")

#define PERSONNEL_SCREEN 0
#define UNEMPLOYED_SCREEN 1

#define CREW_X 561
#define CREW_Y 509
#define CREW_HEIGHT 174
#define CREW_WIDTH 465

#define UNEMPLOYED_X 564
#define UNEMPLOYED_Y 68
#define UNEMPLOYED_HEIGHT 594
#define UNEMPLOYED_WIDTH 460

#define EXITBTN_X 16
#define EXITBTN_Y 698

#define HIREMOREBTN_X 217
#define HIREMOREBTN_Y 698

#define FIREBTN_X 815
#define FIREBTN_Y 698

#define HIREBTN_X 815
#define HIREBTN_Y 698

#define UNASSIGNBTN_X 606
#define UNASSIGNBTN_Y 698

#define CATBTN_X 531
#define CATBTN_Y 65
#define CATSPACING 59

#define SKILLBAR_X 73
#define SKILLBAR_Y 240

#define SKILLICONS_X 25
#define SKILLICONS_Y 220
#define SKILLSPACING 50

#define SKILLMAXIUM 200
#define ATTRIBUTEMAXIUM 65

#define CREWPOSITION_X 580
#define CREWPOSITION_Y 100
#define CREWSPACING 59

#define PORTRAITPOSITION_X 25
#define PORTRAITPOSITION_Y 170

#define EMPLOYEE_SPAWN_RATE 1

#define AVAILABLE_TEXT_COLOR WHITE
#define UNASSIGNED_TEXT_COLOR YELLOW

string ModuleCrewHire::c_directions =
    "Click on your crew members to the right to reassign or fire "
    "them. You can also browse for future employees by clicking "
    "on the Hire Crew button";

string ModuleCrewHire::c_hire_more_directions =
    "On the right is a list potential galactic faring employees. "
    "You can view their statistics by clicking on them.";

string ModuleCrewHire::c_title = "Welcome to Crew Match";
string ModuleCrewHire::c_statistics_title = "Statistics";

ModuleCrewHire::ModuleCrewHire()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), lastEmployeeSpawn(-1),
      m_exitBtn(nullptr), m_hireBtn(nullptr), m_hiremoreBtn(nullptr),
      m_fireBtn(nullptr), m_unassignBtn(nullptr), m_backBtn(nullptr),
      selectedOfficer(nullptr), unassignedCrew(nullptr), unemployed(nullptr),
      unemployedType(nullptr), resources(CREWHIRE_IMAGES) {

    for (int i = 0; i < 8; ++i)
        m_PositionBtns[i] = nullptr;

    // Load label for title
    m_title = new Label(
        c_title,
        28,
        170,
        456,
        30,
        false,
        0,
        g_game->font32,
        al_map_rgb(0, 255, 128));
    add_child_module(m_title);

    // Load label for slogan
    m_slogan = new Label(
        "Where you can hire the finest galactic crew!",
        28,
        200,
        456,
        80,
        true,
        0,
        g_game->font22,
        al_map_rgb(0, 255, 255));
    add_child_module(m_slogan);

    // Load label for directions
    m_directions = new Label(
        c_directions,
        28,
        280,
        456,
        408,
        true,
        0,
        g_game->font18,
        al_map_rgb(0, 255, 255));
    add_child_module(m_directions);
}

ModuleCrewHire::~ModuleCrewHire() {}

bool
ModuleCrewHire::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    switch (currentScreen) {
    case PERSONNEL_SCREEN:
        if (selectedPosition != -1) {
            m_fireBtn->OnMouseMove(x, y);
            m_unassignBtn->OnMouseMove(x, y);
        }
        m_exitBtn->OnMouseMove(x, y);
        m_hiremoreBtn->OnMouseMove(x, y);

        for (int i = 0; i < 8; i++)
            m_PositionBtns[i]->OnMouseMove(x, y);

        if (FALSEHover > -1 && FALSEHover < 8)
            m_PositionBtns[FALSEHover]->OnMouseMove(
                m_PositionBtns[FALSEHover]->GetX(),
                m_PositionBtns[FALSEHover]->GetY());

        if (selectedPosition == -1 || selectedPosition == 7)
            unassignedCrew->OnMouseMove(x, y);
        break;

    case UNEMPLOYED_SCREEN:
        if (is_mouse_wheel_down(event)) {
            unemployedType->OnMouseWheelDown(x, y);
            break;
        } else if (is_mouse_wheel_up(event)) {
            unemployedType->OnMouseWheelUp(x, y);
            break;
        }
        unemployedType->OnMouseMove(x, y);
        m_hireBtn->OnMouseMove(x, y);
        m_backBtn->OnMouseMove(x, y);
        break;
    }
    return true;
}

bool
ModuleCrewHire::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    int button = event->button - 1;
    int x = event->x;
    int y = event->y;

    switch (currentScreen) {
    case PERSONNEL_SCREEN:
        if (selectedPosition != -1) {
            m_fireBtn->OnMouseReleased(button, x, y);
            m_unassignBtn->OnMouseReleased(button, x, y);
        }

        m_hiremoreBtn->OnMouseReleased(button, x, y);

        for (int i = 0; i < 8; i++)
            m_PositionBtns[i]->OnMouseReleased(button, x, y);

        if (selectedPosition == -1 || selectedPosition == 7)
            unassignedCrew->OnMouseReleased(button, x, y);

        // Always call the backBtn "Exit" last
        m_exitBtn->OnMouseReleased(button, x, y);

        if (is_mouse_click(event)) {
            if (selectedPosition == -1 || selectedPosition == 7)
                unassignedCrew->OnMouseClick(button, x, y);
        }
        break;
    case UNEMPLOYED_SCREEN:
        if (is_mouse_click(event)) {
            unemployedType->OnMouseClick(button, x, y);
        }
        unemployedType->OnMouseReleased(button, x, y);
        m_hireBtn->OnMouseReleased(button, x, y);
        m_backBtn->OnMouseReleased(button, x, y);
        break;
    }
    return true;
}

bool
ModuleCrewHire::on_event(ALLEGRO_EVENT *event) {
    bool exitToStarportCommons = false;
    string escape = "";

    switch (event->type) {
    case EVENT_SAVE_GAME: // save game
        g_game->gameState->AutoSave();
        break;
    case EVENT_LOAD_GAME: // load game
        g_game->gameState->AutoLoad();
        break;
    case EVENT_QUIT_GAME: // quit game
        g_game->setVibration(0);
        escape = g_game->getGlobalString("ESCAPEMODULE");
        g_game->LoadModule(escape);
        break;

    case EVENT_CREWHIRE_EXIT:
        {
            bool passedCheck = true;
            for (int i = 0; i < (int)tOfficers.size(); i++) {
                if (tOfficers[i]->GetOfficerType() == OFFICER_NONE) {
                    passedCheck = false;
                }
            }

            if (passedCheck)
                exitToStarportCommons = true;
        }
        break;

    case EVENT_CREWHIRE_HIRE_MORE:
        currentScreen = UNEMPLOYED_SCREEN;
        selectedOfficer = nullptr;
        selectedPosition = 1;
        RefreshUnemployedCrewBox();
        m_directions->set_text(c_hire_more_directions);
        break;

    case EVENT_CREWHIRE_HIRE:
        if (tOfficers.size() <= 6) {
            for (int i = 0;
                 i < (int)g_game->gameState->m_unemployedOfficers.size();
                 i++) {
                if (g_game->gameState->m_unemployedOfficers[i]
                    == selectedOfficer) {
                    g_game->gameState->m_unemployedOfficers.erase(
                        g_game->gameState->m_unemployedOfficers.begin() + i);
                    tOfficers.push_back(selectedOfficer);
                    selectedPosition = -1;
                    selectedOfficer = nullptr;
                    m_title->set_active(true);
                    m_slogan->set_active(true);
                    m_directions->set_active(true);
                    RefreshUnemployedCrewBox();
                }
            }
        } else {
            g_game->ShowMessageBoxWindow(
                "", "You can't hire any more crew members!", 400, 150);
        }
        break;

    case EVENT_CREWHIRE_BACK:
        currentScreen = PERSONNEL_SCREEN;
        selectedOfficer = nullptr;
        selectedPosition = -1;
        RefreshUnassignedCrewBox();
        m_directions->set_text(c_directions);
        break;

    case EVENT_CREWHIRE_FIRE:

        for (int i = 0; i < (int)tOfficers.size(); i++) {
            if (tOfficers[i] == selectedOfficer) {
                if (selectedOfficer->GetOfficerType() != OFFICER_CAPTAIN) {
                    tOfficers.erase(tOfficers.begin() + i);
                    g_game->gameState->m_unemployedOfficers.push_back(
                        selectedOfficer);
                    selectedPosition = -1;
                    selectedOfficer = nullptr;
                    RefreshUnassignedCrewBox();
                } else {
                    g_game->ShowMessageBoxWindow(
                        "", "You can't fire yourself, Captain!");
                }
            }
        }
        break;

    case EVENT_CREWHIRE_UNASSIGN:
        for (int i = 0; i < (int)tOfficers.size();
             i++) // Find the officer we are clicking on and unassign him
        {
            if (tOfficers[i]->GetOfficerType() - 1 == selectedPosition) {
                if (selectedOfficer->GetOfficerType() != OFFICER_CAPTAIN) {
                    tOfficers[i]->SetOfficerType(
                        OFFICER_NONE);     // unassign the officer
                    selectedPosition = -1; // set the selected position to none
                    RefreshUnassignedCrewBox();
                    break;
                } else {
                    g_game->ShowMessageBoxWindow(
                        "", "You can't unassign yourself, Captain!");
                }
            }
        }
        selectedPositionLastRun =
            -2; // This forces a refresh in the run function
        break;

    case EVENT_CREWHIRE_CREWLIST_BOX_CLICK:
        {
            // Find the selected officer
            int j = 0;
            for (int i = 0; i < (int)tOfficers.size();
                 i++) // Loop through all the officers
            {
                if (tOfficers[i]->GetOfficerType()
                    == OFFICER_NONE) // Count the officers that aren't assigned
                                     // a position
                {
                    selectedOfficer = nullptr;
                    selectedPosition = -1;
                    // If there are at least as many unassigned officers as
                    // the selected Index then its valid
                    if (j == unassignedCrew->GetSelectedIndex()) {
                        selectedOfficer = tOfficers[i];
                        selectedPosition = 7;
                        break;
                    } else {
                        j++; // apparently it wasn't this officer perhaps
                             // the next one
                    }
                }
            }
            if (!selectedOfficer) {
                m_title->set_text(c_title);
                m_title->set_flags(ALLEGRO_ALIGN_LEFT);
                m_directions->set_active(true);
                m_slogan->set_active(true);
            } else {
                m_title->set_text(c_statistics_title);
                m_title->set_flags(ALLEGRO_ALIGN_CENTER);
                m_directions->set_active(false);
                m_slogan->set_active(false);
            }
        }
        break;

    case EVENT_CREWHIRE_UNEMPLOYEDLISTBOX_CLICK:
        if (unemployed->GetSelectedIndex() != -1) {
            // If there are at least as many officers as the selected
            // Index then its valid
            if ((int)g_game->gameState->m_unemployedOfficers.size()
                > unemployed->GetSelectedIndex()) {
                selectedOfficer =
                    g_game->gameState
                        ->m_unemployedOfficers[unemployed->GetSelectedIndex()];
                selectedPosition = -1;
            } else {
                selectedOfficer = nullptr;
                selectedPosition = -1;
            }
        } else {
            selectedOfficer = nullptr;
            selectedPosition = -1;
        }
        if (!selectedOfficer) {
            m_title->set_text(c_title);
            m_title->set_flags(ALLEGRO_ALIGN_LEFT);
            m_directions->set_active(true);
        } else {
            m_title->set_text(c_statistics_title);
            m_title->set_flags(ALLEGRO_ALIGN_CENTER);
            m_directions->set_active(false);
        }
        break;

    case EVENT_CREWHIRE_CAPTAIN:
    case EVENT_CREWHIRE_NAVIGATION:
    case EVENT_CREWHIRE_MEDICAL:
    case EVENT_CREWHIRE_ENGINEERING:
    case EVENT_CREWHIRE_COMMUNICATIONS:
    case EVENT_CREWHIRE_TACTICAL:
    case EVENT_CREWHIRE_SCIENCE:
    case EVENT_CREWHIRE_UNKNOWN:
        if (currentScreen == PERSONNEL_SCREEN) {
            int crew_type = event->type - EVENT_CREWHIRE_CAPTAIN;

#pragma region Personel Screen functions

            // no position selected yet, so we need to highlight it
            if (selectedPosition == -1) {
                // check to see if there is an officer in this position
                for (int i = 0; i < (int)tOfficers.size(); i++) {
                    if (tOfficers[i]->GetOfficerType() - 1
                        == crew_type) // If there is
                    {
                        selectedPosition = crew_type; // Then select him
                        selectedOfficer = tOfficers[i];
                        break;
                    }
                }
            } else {
                if (selectedPosition
                    == crew_type) { // Did we click on the same officer we
                                    // already had selected?
                    if (selectedPosition == 7)
                        RefreshUnassignedCrewBox();

                    selectedPosition = -1; // Then Deselect him
                    selectedOfficer = nullptr;
                } else {
                    // If we are clicking on a new position, we can assume
                    // this position is open and the player wants it filled
                    // with the selected officer

                    // First check to see if the position clicked is the
                    // unassigned button
                    if (crew_type == EVENT_CREWHIRE_UNKNOWN) {
                        // if it is...
                        for (int i = 0; i < (int)tOfficers.size();
                             i++) // Find the officer we are clicking on and
                                  // unassign him
                        {
                            if (tOfficers[i]->GetOfficerType() - 1
                                == selectedPosition) {
                                tOfficers[i]->SetOfficerType(
                                    OFFICER_NONE); // unassign the officer
                                selectedPosition =
                                    -1; // set the selected position to none
                                selectedOfficer = nullptr;
                                RefreshUnassignedCrewBox(); // refresh the
                                                            // crew listbox
                                break;
                            }
                        }
                        selectedPositionLastRun =
                            -2; // This forces a refresh in the run function
                    } else {
                        for (int i = 0; i < (int)tOfficers.size();
                             i++) // Find the officer we have selected
                                  // already, and assign him to the new
                                  // position
                        {
                            if (selectedPosition
                                == 7) // If an unassigned officer was selected
                                      // we have to do it different
                            {
                                // Find the selected officer
                                int j = 0;
                                for (int i = 0; i < (int)tOfficers.size();
                                     i++) // Loop through all the officers
                                {
                                    if (tOfficers[i]->GetOfficerType()
                                        == OFFICER_NONE) // Count the officers
                                                         // that aren't assign
                                                         // a position
                                    {
                                        // If there are at least as many
                                        // unassigned officers as the
                                        // selected Index then its valid
                                        if (j
                                            == unassignedCrew
                                                   ->GetSelectedIndex()) {
                                            // assign that officer to the
                                            // newly selected position
                                            tOfficers[i]->SetOfficerType(
                                                (OfficerType)(crew_type + 1));
                                            selectedPosition =
                                                crew_type; // Set the
                                                           // selectedPosition
                                                           // to the
                                                           // newly
                                                           // assigned
                                                           // position
                                            selectedOfficer = tOfficers[i];
                                            RefreshUnassignedCrewBox();
                                            break;
                                        } else {
                                            j++; // apparently it wasn't
                                                 // this officer perhaps the
                                                 // next one
                                        }
                                    }
                                }
                            } else {
                                if (tOfficers[i]->GetOfficerType() - 1
                                    == selectedPosition) {
                                    tOfficers[i]->SetOfficerType((
                                        OfficerType)(crew_type + 1)); // assign
                                                                      // the
                                                                      // officer
                                    selectedPosition =
                                        crew_type; // Set the
                                                   // selectedPosition
                                                   // to the newly
                                                   // assigned
                                                   // position
                                    selectedOfficer = tOfficers[i];
                                    m_PositionBtns[FALSEHover]->OnMouseMove(
                                        0, 0); // Remove the FALSEHover glow
                                               // from the old position
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if (!selectedOfficer) {
                m_title->set_text(c_title);
                m_title->set_flags(ALLEGRO_ALIGN_LEFT);
                m_directions->set_active(true);
                m_slogan->set_active(true);
            } else {
                m_title->set_text(c_statistics_title);
                m_title->set_flags(ALLEGRO_ALIGN_CENTER);
                m_directions->set_active(false);
                m_slogan->set_active(false);
            }
#pragma endregion
        }
        selectedPositionLastRun =
            -2; // This forces a refresh in the run function
        break;
    }

    if (exitToStarportCommons) {
        g_game->LoadModule(MODULE_STARPORT);
        return false;
    }
    return true;
}

bool
ModuleCrewHire::on_close() {
    ALLEGRO_DEBUG("CrewHire Close\n");

    // We must save all the officers to the game state class before closing
    g_game->gameState->officerSci = nullptr;
    g_game->gameState->officerNav = nullptr;
    g_game->gameState->officerEng = nullptr;
    g_game->gameState->officerCom = nullptr;
    g_game->gameState->officerDoc = nullptr;
    g_game->gameState->officerTac = nullptr;

    for (int i = 0; i < (int)tOfficers.size(); ++i) {
        // No need to save the captain as they shouldn't of made any changes to
        // begin with
        if (tOfficers[i]->GetOfficerType() == OFFICER_SCIENCE)
            g_game->gameState->officerSci = tOfficers[i];
        if (tOfficers[i]->GetOfficerType() == OFFICER_NAVIGATION)
            g_game->gameState->officerNav = tOfficers[i];
        if (tOfficers[i]->GetOfficerType() == OFFICER_ENGINEER)
            g_game->gameState->officerEng = tOfficers[i];
        if (tOfficers[i]->GetOfficerType() == OFFICER_COMMUNICATION)
            g_game->gameState->officerCom = tOfficers[i];
        if (tOfficers[i]->GetOfficerType() == OFFICER_MEDICAL)
            g_game->gameState->officerDoc = tOfficers[i];
        if (tOfficers[i]->GetOfficerType() == OFFICER_TACTICAL)
            g_game->gameState->officerTac = tOfficers[i];
    }

    if (g_game->gameState->officerSci == nullptr)
        g_game->gameState->officerSci = new Officer(OFFICER_NONE);
    if (g_game->gameState->officerNav == nullptr)
        g_game->gameState->officerNav = new Officer(OFFICER_NONE);
    if (g_game->gameState->officerEng == nullptr)
        g_game->gameState->officerEng = new Officer(OFFICER_NONE);
    if (g_game->gameState->officerCom == nullptr)
        g_game->gameState->officerCom = new Officer(OFFICER_NONE);
    if (g_game->gameState->officerDoc == nullptr)
        g_game->gameState->officerDoc = new Officer(OFFICER_NONE);
    if (g_game->gameState->officerTac == nullptr)
        g_game->gameState->officerTac = new Officer(OFFICER_NONE);

    // delete the crew position button images
    for (int i = 0; i < 8; i++) {
        al_destroy_bitmap(posNormImages[i]);
        al_destroy_bitmap(posOverImages[i]);
        al_destroy_bitmap(posDisImages[i]);
    }

    if (m_exitBtn) {
        delete m_exitBtn;
        m_exitBtn = nullptr;
    }

    if (m_hireBtn) {
        delete m_hireBtn;
        m_hireBtn = nullptr;
    }

    if (m_hiremoreBtn) {
        delete m_hiremoreBtn;
        m_hiremoreBtn = nullptr;
    }

    if (m_fireBtn) {
        delete m_fireBtn;
        m_fireBtn = nullptr;
    }

    if (m_unassignBtn) {
        delete m_unassignBtn;
        m_unassignBtn = nullptr;
    }

    if (m_backBtn) {
        delete m_backBtn;
        m_backBtn = nullptr;
    }

    for (int i = 0; i < 8; i++) {
        delete m_PositionBtns[i];
        m_PositionBtns[i] = nullptr;
    }

    if (unassignedCrew) {
        delete unassignedCrew;
        unassignedCrew = nullptr;
    }

    // This will also delete unemployed
    if (unemployedType) {
        delete unemployedType;
        unemployedType = nullptr;
    }

    resources.unload();
    return true;
}

bool
ModuleCrewHire::on_init() {
    ALLEGRO_BITMAP *btnNorm, *btnOver, *btnDis;

    ALLEGRO_DEBUG("  Crew Hire Initialize\n");

    // load the resources
    if (!resources.load()) {
        g_game->message("CrewHire: Error loading resources");
        return false;
    }

    // enable the Pause Menu
    g_game->pauseMenu->setEnabled(true);

    currentScreen = PERSONNEL_SCREEN;
    selectedPosition = -1; // Set the selectedPosition to none
    selectedEntryLastRun =
        -1; // Set the crew listbox selection previous run to unselected
    selectedOfficer = nullptr;

    g_game->audioSystem->Load("data/crewhire/buttonclick.ogg", "click");

    // setup unassignedCrew scrollbox
    unassignedCrew = new ScrollBox::ScrollBox(
        g_game->font24,
        ScrollBox::SB_LIST,
        CREW_X,
        CREW_Y,
        CREW_WIDTH,
        CREW_HEIGHT,
        EVENT_CREWHIRE_CREWLIST_BOX_CLICK);
    unassignedCrew->DrawScrollBar(false);
    unassignedCrew->setLines(6);

    // setup unemployed scrollbox
    unemployed = new ScrollBox::ScrollBox(
        g_game->font24,
        ScrollBox::SB_LIST,
        UNEMPLOYED_X,
        UNEMPLOYED_Y,
        UNEMPLOYED_WIDTH,
        UNEMPLOYED_HEIGHT,
        EVENT_CREWHIRE_UNEMPLOYEDLISTBOX_CLICK);
    unemployed->DrawScrollBar(false);
    unemployed->setLines(25);

    // setup unemployed type column scrollbox
    unemployedType = new ScrollBox::ScrollBox(
        g_game->font24,
        ScrollBox::SB_LIST,
        (int)(UNEMPLOYED_X + UNEMPLOYED_WIDTH * .66),
        UNEMPLOYED_Y,
        UNEMPLOYED_WIDTH / 3,
        UNEMPLOYED_HEIGHT,
        EVENT_CREWHIRE_UNEMPLOYEDLISTBOX_CLICK);
    unemployedType->DrawScrollBar(false);
    unemployedType->setLines(25);
    unemployedType->LinkBox(unemployed);

    // Must clear the vectors incase this isn't the first time this module
    // loaded
    tOfficers.clear();

    // Load current officers into the officer array
    //  NOTE: Explicitly defined officers (i.e. officerCap) will ALWAYS exist
    //  and NEVER be null
    if (g_game->gameState->officerCap->name.length() > 0)
        tOfficers.push_back(g_game->gameState->officerCap);
    if (g_game->gameState->officerSci->name.length() > 0)
        tOfficers.push_back(g_game->gameState->officerSci);
    if (g_game->gameState->officerNav->name.length() > 0)
        tOfficers.push_back(g_game->gameState->officerNav);
    if (g_game->gameState->officerEng->name.length() > 0)
        tOfficers.push_back(g_game->gameState->officerEng);
    if (g_game->gameState->officerCom->name.length() > 0)
        tOfficers.push_back(g_game->gameState->officerCom);
    if (g_game->gameState->officerTac->name.length() > 0)
        tOfficers.push_back(g_game->gameState->officerTac);
    if (g_game->gameState->officerDoc->name.length() > 0)
        tOfficers.push_back(g_game->gameState->officerDoc);

    // if PERSONS FOR HIRE list is empty, fill it
    if (g_game->gameState->m_unemployedOfficers.size() == 0) {
        for (int i = 0; g_game->gameState->m_unemployedOfficers.size() <= 18;
             i++) {
            // create a random dude
            Officer *dude = new Officer(OFFICER_NONE);
            dude->name = g_game->dataMgr->GetRandMixedName();
            for (int att = 0; att < 6; att++)
                dude->attributes[att] = Util::Random(5, 50);

            // specialization in a random skill
            dude->attributes[Util::Random(0, 5)] = Util::Random(50, 75);

            dude->attributes[6] = 5;
            dude->attributes[7] = 5;

            // add this dude to the FOR HIRE list
            g_game->gameState->m_unemployedOfficers.push_back(dude);
        }
    }

    if (lastEmployeeSpawn == -1) { // it hasn't been initialized
        lastEmployeeSpawn =
            g_game->gameState->stardate.get_current_date_in_days();
    }

    currentVisit = g_game->gameState->stardate.get_current_date_in_days();

    // refresh the list of random employees (we do it unconditionally for the
    // time being) if ( (currentVisit - lastEmployeeSpawn)/EMPLOYEE_SPAWN_RATE
    // >= 1 )
    if (true) {
        // Save the new time for Employee Spawn
        lastEmployeeSpawn =
            g_game->gameState->stardate.get_current_date_in_days();

        // Remove some old faces
        int facesToRemove = Util::Random(2, 6);
        for (int i = 0; i < facesToRemove; ++i) {
            g_game->gameState->m_unemployedOfficers.erase(
                g_game->gameState->m_unemployedOfficers.begin()
                + Util::Random(
                    0,
                    (int)g_game->gameState->m_unemployedOfficers.size() - 1));
        }

        // Add some new ones (exactly has many has we removed for the time
        // being; so we don't have to worry about the list growing or shrinking)
        int facesToAdd = facesToRemove;
        for (int i = 0; i < facesToAdd; ++i) {
            // create a random dude
            Officer *dude = new Officer(OFFICER_NONE);
            dude->name = g_game->dataMgr->GetRandMixedName();
            for (int att = 0; att < 6; att++)
                dude->attributes[att] = Util::Random(5, 50);

            // specialization in a random skill
            dude->attributes[Util::Random(0, 5)] = Util::Random(50, 75);

            dude->attributes[6] = 5;
            dude->attributes[7] = 5;

            // add this dude to the FOR HIRE list
            g_game->gameState->m_unemployedOfficers.push_back(dude);
        }
    }

    for (int i = 0; i < (int)g_game->gameState->m_unemployedOfficers.size();
         ++i) {
        // add this person to the AVAILABLE FOR HIRE list
        unemployed->Write(
            g_game->gameState->m_unemployedOfficers[i]->name,
            AVAILABLE_TEXT_COLOR);

        unemployedType->Write(
            g_game->gameState->m_unemployedOfficers[i]
                ->GetPreferredProfession(),
            AVAILABLE_TEXT_COLOR);
    }

    // Create escape button for the module
    btnNorm = resources[I_GENERIC_EXIT_BTN_NORM];
    btnOver = resources[I_GENERIC_EXIT_BTN_OVER];
    m_exitBtn = new Button(
        btnNorm,
        btnOver,
        nullptr,
        EXITBTN_X,
        EXITBTN_Y,
        EVENT_NONE,
        EVENT_CREWHIRE_EXIT,
        g_game->font24,
        "Exit",
        al_map_rgb(255, 0, 0),
        "click");
    if (m_exitBtn == nullptr)
        return false;
    if (!m_exitBtn->IsInitialized())
        return false;

    // Create and initialize the Back button for the module
    m_backBtn = new Button(
        btnNorm,
        btnOver,
        nullptr,
        EXITBTN_X,
        EXITBTN_Y,
        EVENT_NONE,
        EVENT_CREWHIRE_BACK,
        g_game->font24,
        "Back",
        al_map_rgb(255, 0, 0),
        "click");
    if (m_backBtn == nullptr)
        return false;
    if (!m_backBtn->IsInitialized())
        return false;

    // Create and initialize the HireMore button for the module
    btnNorm = resources[I_PERSONNEL_BTN2];
    btnOver = resources[I_PERSONNEL_BTN2_HOV];
    btnDis = resources[I_PERSONNEL_BTN2_DIS];
    m_hiremoreBtn = new Button(
        btnNorm,
        btnOver,
        btnDis,
        HIREMOREBTN_X,
        HIREMOREBTN_Y,
        EVENT_NONE,
        EVENT_CREWHIRE_HIRE_MORE,
        g_game->font24,
        "Hire Crew",
        al_map_rgb(0, 255, 255),
        "click");
    if (m_hiremoreBtn == nullptr)
        return false;
    if (!m_hiremoreBtn->IsInitialized())
        return false;

    // Create and initialize the Hire button for the module
    btnNorm = resources[I_PERSONNEL_BTN];
    btnOver = resources[I_PERSONNEL_BTN_HOV];
    btnDis = resources[I_PERSONNEL_BTN_DIS];
    m_hireBtn = new Button(
        btnNorm,
        btnOver,
        btnDis,
        HIREBTN_X,
        HIREBTN_Y,
        EVENT_NONE,
        EVENT_CREWHIRE_HIRE,
        g_game->font24,
        "Hire",
        al_map_rgb(0, 255, 255),
        "click");
    if (m_hireBtn == nullptr)
        return false;
    if (!m_hireBtn->IsInitialized())
        return false;

    // Create and initialize the Fire button for the module
    m_fireBtn = new Button(
        btnNorm,
        btnOver,
        btnDis,
        FIREBTN_X,
        FIREBTN_Y,
        EVENT_NONE,
        EVENT_CREWHIRE_FIRE,
        g_game->font24,
        "Fire",
        al_map_rgb(0, 255, 255),
        "click");
    if (m_fireBtn == nullptr)
        return false;
    if (!m_fireBtn->IsInitialized())
        return false;

    // Create and initialize the Assign Position button for the module
    m_unassignBtn = new Button(
        btnNorm,
        btnOver,
        btnDis,
        UNASSIGNBTN_X,
        UNASSIGNBTN_Y,
        EVENT_NONE,
        EVENT_CREWHIRE_UNASSIGN,
        g_game->font24,
        "Unassign",
        al_map_rgb(0, 255, 255),
        "click");
    if (m_unassignBtn == nullptr)
        return false;
    if (!m_unassignBtn->IsInitialized())
        return false;

    ALLEGRO_BITMAP *blackIcons = resources[I_ICONS_SMALL];
    ALLEGRO_BITMAP *greenIcons = resources[I_ICONS_SMALL_GREEN];
    ALLEGRO_BITMAP *redIcons = resources[I_ICONS_SMALL_RED];

    btnNorm = resources[I_PERSONNEL_CATBTN];
    btnOver = resources[I_PERSONNEL_CATBTN_HOV];
    btnDis = resources[I_PERSONNEL_CATBTN_DIS];

    // create crew buttons
    char positions[8][20] = {
        "- Captain - ",
        "- Science -",
        "- Navigation -",
        "- Engineering -",
        "- Communication -",
        "- Medical -",
        "- Tactical -",
        "- Unassigned -"};

    for (int i = 0; i < 8; i++) {
        // create a normal image for each crew position button
        posNormImages[i] = al_create_bitmap(
            al_get_bitmap_width(btnNorm), al_get_bitmap_height(btnNorm));
        al_set_target_bitmap(posNormImages[i]);
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_draw_bitmap(btnNorm, 0, 0, 0);
        // create an over image for each crew position button
        posOverImages[i] = al_create_bitmap(
            al_get_bitmap_width(btnOver), al_get_bitmap_height(btnOver));
        al_set_target_bitmap(posOverImages[i]);
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_draw_bitmap(btnOver, 0, 0, 0);
        // create a disabled image for each crew position button
        posDisImages[i] = al_create_bitmap(
            al_get_bitmap_width(btnDis), al_get_bitmap_height(btnDis));
        al_set_target_bitmap(posDisImages[i]);
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_draw_bitmap(btnDis, 0, 0, 0);

        // Create and initialize the new button
        m_PositionBtns[i] = new Button(
            posNormImages[i],
            posOverImages[i],
            posDisImages[i],
            CATBTN_X,
            CATBTN_Y + (i * CATSPACING),
            EVENT_NONE,
            EVENT_CREWHIRE_CAPTAIN + i,
            "click");

        if (m_PositionBtns[i] == nullptr)
            return false;
        if (!m_PositionBtns[i]->IsInitialized())
            return false;

        al_set_target_bitmap(m_PositionBtns[i]->GetImgNormal());
        al_draw_bitmap_region(blackIcons, 30 * i, 0, 30, 30, 0, 0, 0);

        al_set_target_bitmap(m_PositionBtns[i]->GetImgMouseOver());
        al_draw_bitmap_region(greenIcons, 30 * i, 0, 30, 30, 0, 0, 0);
        al_draw_bitmap_region(blackIcons, 30 * i, 0, 30, 30, 1, 1, 0);

        al_set_target_bitmap(m_PositionBtns[i]->GetImgDisabled());
        al_draw_bitmap_region(blackIcons, 30 * i, 0, 30, 30, 1, 1, 0);
        al_draw_bitmap_region(redIcons, 30 * i, 0, 30, 30, 0, 0, 0);

        al_set_target_bitmap(m_PositionBtns[i]->GetImgNormal());
        al_draw_text(
            g_game->font24, al_map_rgb(0, 255, 255), 35, 4, 0, positions[i]);
        al_set_target_bitmap(m_PositionBtns[i]->GetImgMouseOver());
        al_draw_text(
            g_game->font24, al_map_rgb(0, 255, 255), 35, 4, 0, positions[i]);
        al_set_target_bitmap(m_PositionBtns[i]->GetImgDisabled());
        al_draw_text(
            g_game->font24, al_map_rgb(0, 255, 255), 35, 4, 0, positions[i]);
    }

    // tell questmgr that Personnel event has occurred
    g_game->questMgr->raiseEvent(18);

    return true;
}

bool
ModuleCrewHire::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);
    al_draw_bitmap(resources[I_PERSONNEL_BACKGROUND], 0, 0, 0);

    switch (currentScreen) {
    case PERSONNEL_SCREEN:
#pragma region Personel Screen
        if (selectedEntryLastRun != unassignedCrew->GetSelectedIndex()) {
            selectedEntryLastRun = unassignedCrew->GetSelectedIndex();
            if (selectedEntryLastRun == -1) {
                selectedOfficer = nullptr;
                selectedPosition = -1;
            } else {
                int j = 0;
                for (int i = 0; i < (int)tOfficers.size();
                     i++) // Loop through all the officers
                {
                    if (tOfficers[i]->GetOfficerType()
                        == OFFICER_NONE) // Count the officers that aren't
                                         // assigned a position
                    {
                        // If there are at least as many unassigned officers
                        // as the selected Index then its valid
                        if (j == unassignedCrew->GetSelectedIndex()) {
                            selectedOfficer = tOfficers[i];
                            // Can't fire the captain
                            if (selectedOfficer->GetOfficerType()
                                != OFFICER_CAPTAIN)
                                m_fireBtn->SetEnabled(true);
                            else
                                m_fireBtn->SetEnabled(false);

                            break;
                        } else {
                            j++; // apparently it wasn't this officer
                                 // perhaps the next one
                        }
                    }
                }

                if (selectedOfficer != nullptr
                    && selectedOfficer->GetOfficerType() != OFFICER_CAPTAIN) {
                    m_PositionBtns[7]->SetEnabled(true);
                    selectedPosition = 7;
                }
            }
        }

        if (selectedPosition == -1) {
            if (selectedPositionLastRun != -1) {
                for (int i = 0; i < 8; i++)
                    m_PositionBtns[i]->SetEnabled(true);

                m_fireBtn->SetEnabled(false);
                m_unassignBtn->SetEnabled(false);

                FALSEHover = -1;
            }
            selectedPositionLastRun = -1;
        } else {
            if (selectedPositionLastRun != selectedPosition) {
                if (selectedOfficer != nullptr) {
                    if (selectedOfficer->GetOfficerType() != OFFICER_CAPTAIN) {
                        // Turn all the Position buttons on, except the no
                        // position button and the captain button
                        for (int i = 0; i < 8; i++)
                            m_PositionBtns[i]->SetEnabled(true);

                        // Turn off only the ones that already have officers
                        for (int i = 0; i < (int)tOfficers.size(); i++) {
                            if (tOfficers[i]->GetOfficerType() != OFFICER_NONE
                                && tOfficers[i]->GetOfficerType() - 1
                                       != selectedPosition) {
                                m_PositionBtns
                                    [tOfficers[i]->GetOfficerType() - 1]
                                        ->SetEnabled(false);
                            }
                        }

                        FALSEHover = selectedPosition; // Set the FALSEhover for
                                                       // the selectedPosition

                        if (selectedOfficer != nullptr) {
                            if (selectedOfficer->GetOfficerType()
                                != OFFICER_CAPTAIN) {
                                if (selectedOfficer->GetOfficerType()
                                    == OFFICER_NONE) {
                                    m_fireBtn->SetEnabled(true);
                                    m_unassignBtn->SetEnabled(false);
                                } else {
                                    m_fireBtn->SetEnabled(false);
                                    m_unassignBtn->SetEnabled(true);
                                }
                            }
                        }
                    } else {
                        FALSEHover = selectedPosition; // Set the FALSEhover for
                                                       // the selectedPosition

                        // Turn all the other Position buttons off,this
                        // prevents the Captain from ever moving
                        for (int i = 1; i < 8; i++)
                            m_PositionBtns[i]->SetEnabled(false);
                    }
                }
                selectedPositionLastRun = selectedPosition;
            }

            m_fireBtn->Run(target);
            m_unassignBtn->Run(target);

            if (selectedOfficer != nullptr)
                DrawOfficerInfo(selectedOfficer);
        }

        for (int i = 0; i < 8; i++)
            m_PositionBtns[i]->Run(target);

        m_exitBtn->Run(target);
        m_hiremoreBtn->Run(target);
        unassignedCrew->Draw(target);

        for (int i = 0; i < (int)tOfficers.size(); i++) {
            if (tOfficers[i]->GetOfficerType() != OFFICER_NONE)
                al_draw_text(
                    g_game->font24,
                    ((tOfficers[i]->GetOfficerType() - 1) == selectedPosition
                         ? al_map_rgb(0, 255, 255)
                         : al_map_rgb(255, 255, 255)),
                    CREWPOSITION_X,
                    CREWPOSITION_Y
                        + ((tOfficers[i]->GetOfficerType() - 1) * CREWSPACING),
                    0,
                    tOfficers[i]->name.c_str());
        }
#pragma endregion
        break;

    case UNEMPLOYED_SCREEN:
#pragma region Unemployed Screen
        unemployedType->Draw(target);

        m_backBtn->Run(target);

        if (selectedOfficer != nullptr) {
            m_slogan->set_active(false);

            m_hireBtn->Run(target);
            DrawOfficerInfo(selectedOfficer);
        } else {
            m_title->set_active(true);
            m_slogan->set_active(true);
        }
#pragma endregion
        break;
    }
    return true;
}

void
ModuleCrewHire::DrawOfficerInfo(Officer *officer) {
    static const string skillnames[] = {
        "science",
        "navigation",
        "engineering",
        "communication",
        "medical",
        "tactical",
        "learning",
        "durability"};

    al_draw_bitmap(
        resources[I_PERSONNEL_MINIPOSITIONS], SKILLICONS_X, SKILLICONS_Y, 0);

    // draw names and skills
    ostringstream os;

    // Print Captain jjh
    string offtitle = "Officer ";
    if (officer->GetOfficerType() == OFFICER_CAPTAIN) {
        offtitle = "Captain ";
    }
    os << offtitle << officer->getFirstName() << " " << officer->getLastName();
    g_game->Print22(
        g_game->GetBackBuffer(),
        SKILLBAR_X + 70,
        SKILLBAR_Y - 30,
        os.str(),
        WHITE);
    os.str("");

    // find officer's highest skill jjh
    int tempi = 0;
    int tempval = 0;
    for (int i = 0; i < 6; i++) {
        if (officer->attributes[i] > tempval) {
            tempval = officer->attributes[i];
            tempi = i;
        }
    }

    // print skill name and skill level JJH (using 2 prints in hopes of
    // highlighting one of the skills & readabiltiy) jjh
    for (int i = 0; i < 6; i++) {
        os << skillnames[i];
        g_game->Print22(
            g_game->GetBackBuffer(),
            SKILLBAR_X + 20,
            SKILLBAR_Y + (SKILLSPACING * i) + 2,
            os.str(),
            STEEL);
        os.str("");
        os << officer->attributes[i];
        if (tempi != i) {
            g_game->Print22(
                g_game->GetBackBuffer(),
                SKILLBAR_X + 180,
                SKILLBAR_Y + (SKILLSPACING * i) + 2,
                os.str(),
                STEEL);
        } else {
            g_game->Print22(
                g_game->GetBackBuffer(),
                SKILLBAR_X + 180,
                SKILLBAR_Y + (SKILLSPACING * i) + 2,
                os.str(),
                RED);
        }
        os.str("");
    }
    // print attribute name and level JJH (using 2 prints for readabiltiy) jjh
    for (int i = 6; i < 8; i++) {
        os << skillnames[i];
        g_game->Print22(
            g_game->GetBackBuffer(),
            SKILLBAR_X + 20,
            SKILLBAR_Y + (SKILLSPACING * i) + 2,
            os.str(),
            STEEL);
        os.str("");
        os << officer->attributes[i];
        g_game->Print22(
            g_game->GetBackBuffer(),
            SKILLBAR_X + 180,
            SKILLBAR_Y + (SKILLSPACING * i) + 2,
            os.str(),
            STEEL);
        os.str("");
    }
}

void
ModuleCrewHire::RefreshUnassignedCrewBox() {
    unassignedCrew->Clear();

    for (int i = 0; i < (int)tOfficers.size(); i++) {
        if (tOfficers[i]->GetOfficerType() == OFFICER_NONE) {
            unassignedCrew->Write(tOfficers[i]->name, UNASSIGNED_TEXT_COLOR);
        }
    }
    unassignedCrew->OnMouseMove(unassignedCrew->GetX(), unassignedCrew->GetY());
    unassignedCrew->OnMouseMove(0, 0);
}

void
ModuleCrewHire::RefreshUnemployedCrewBox() {
    unemployedType->Clear();

    for (int i = 0; i < (int)g_game->gameState->m_unemployedOfficers.size();
         i++) {
        unemployed->Write(
            g_game->gameState->m_unemployedOfficers[i]->name,
            UNASSIGNED_TEXT_COLOR);
        unemployedType->Write(
            g_game->gameState->m_unemployedOfficers[i]
                ->GetPreferredProfession(),
            UNASSIGNED_TEXT_COLOR);
    }
    unemployedType->OnMouseMove(unemployedType->GetX(), unemployedType->GetY());
    unemployedType->OnMouseMove(0, 0);
}
