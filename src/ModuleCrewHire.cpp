//
//	STARFLIGHT - THE LOST COLONY
//	ModuleCrewHire.cpp - This module gives the player the ability to hire,
// fire, and reassign crew members. 	Author: Justin Sargent 	Date: 9/21/07
// Mods: Jim Haga - JJH 	Date: 3/16/21
//

#include <utility>

#include <allegro5/allegro.h>

#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "Label.h"
#include "MessageBoxWindow.h"
#include "ModeMgr.h"
#include "ModuleCrewHire.h"
#include "PauseMenu.h"
#include "QuestMgr.h"
#include "Util.h"
#include "crewhire_resources.h"

using namespace std;
using namespace crewhire;

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

#define UNASSIGNBTN_X 606
#define UNASSIGNBTN_Y 698

#define CATBTN_X 531
#define CATBTN_Y 65
#define CATSPACING 59

#define UNASSIGNED_CREW_X 561
#define UNASSIGNED_CREW_Y 478

#define SKILLBAR_X 73
#define SKILLBAR_Y 240

#define SKILLICONS_X 25
#define SKILLICONS_Y 220
#define SKILLSPACING 50

#define CREWPOSITION_X 580
#define CREWPOSITION_Y 100
#define CREWSPACING 59

string ModuleCrewHire::c_directions =
    "Click on your crew members to the right to reassign or fire "
    "them. You can also browse for future employees by clicking "
    "on the Hire Crew button";

string ModuleCrewHire::c_hire_more_directions =
    "On the right is a list potential galactic faring employees. "
    "You can view their statistics by clicking on them.";

string ModuleCrewHire::c_title = "Welcome to Crew Match";
string ModuleCrewHire::c_statistics_title = "Statistics";

ModuleCrewHire::ModuleCrewHire() : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT) {}

bool
ModuleCrewHire::on_init() {
    ALLEGRO_DEBUG("  Crew Hire Initialize\n");
    g_game->enable_pause_menu(true);

    auto background = make_shared<Bitmap>(images[I_PERSONNEL_BACKGROUND]);
    add_child_module(background);

    m_title = make_shared<Label>(
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

    m_slogan = make_shared<Label>(
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

    m_directions = make_shared<Label>(
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

    m_selected_slot = nullopt;

    m_unemployed_panel = make_shared<UnemployedPanel>();
    add_child_module(m_unemployed_panel);

    m_officer_info = make_shared<OfficerInfo>(15, 155);
    add_child_module(m_officer_info);
    m_officer_info->set_active(false);

    // Create escape button for the module
    shared_ptr<ALLEGRO_BITMAP> btnNorm, btnOver, btnDis;

    m_personnel_panel = make_shared<Module>();
    add_child_module(m_personnel_panel);

    m_personnel_unassigned_panel = make_shared<Module>();
    m_personnel_panel->add_child_module(m_personnel_unassigned_panel);
    btnNorm = images[I_GENERIC_EXIT_BTN_NORM];
    btnOver = images[I_GENERIC_EXIT_BTN_OVER];
    m_exit_button = make_shared<TextButton>(
        "Exit",
        g_game->font24,
        al_map_rgb(255, 0, 0),
        ALLEGRO_ALIGN_CENTER,
        EXITBTN_X,
        EXITBTN_Y,
        EVENT_NONE,
        EVENT_CREWHIRE_EXIT,
        btnNorm,
        btnOver,
        nullptr,
        samples[S_BUTTONCLICK]);
    add_child_module(m_exit_button);

    // Create and initialize the HireMore button for the module
    btnNorm = images[I_PERSONNEL_BTN2];
    btnOver = images[I_PERSONNEL_BTN2_HOV];
    btnDis = images[I_PERSONNEL_BTN2_DIS];
    auto personnel_hire_more = make_shared<TextButton>(
        "Hire Crew",
        g_game->font24,
        al_map_rgb(0, 255, 255),
        ALLEGRO_ALIGN_CENTER,
        HIREMOREBTN_X,
        HIREMOREBTN_Y,
        EVENT_NONE,
        EVENT_CREWHIRE_HIRE_MORE,
        btnNorm,
        btnOver,
        btnDis,
        samples[S_BUTTONCLICK]);
    m_personnel_panel->add_child_module(personnel_hire_more);

    // Create and initialize the Fire button for the module
    m_personnel_fire = make_shared<TextButton>(
        "Fire",
        g_game->font24,
        al_map_rgb(0, 255, 255),
        ALLEGRO_ALIGN_CENTER,
        FIREBTN_X,
        FIREBTN_Y,
        EVENT_NONE,
        EVENT_CREWHIRE_FIRE,
        images[I_PERSONNEL_BTN2],
        images[I_PERSONNEL_BTN2_HOV],
        images[I_PERSONNEL_BTN2_DIS],
        samples[S_BUTTONCLICK]);
    m_personnel_panel->add_child_module(m_personnel_fire);
    m_personnel_fire->set_enabled(false);

    // Create and initialize the Assign Position button for the module
    m_personnel_unassign = make_shared<TextButton>(
        "Unassign",
        g_game->font24,
        al_map_rgb(0, 255, 255),
        ALLEGRO_ALIGN_CENTER,
        UNASSIGNBTN_X,
        UNASSIGNBTN_Y,
        EVENT_NONE,
        EVENT_CREWHIRE_UNASSIGN,
        images[I_PERSONNEL_BTN2],
        images[I_PERSONNEL_BTN2_HOV],
        images[I_PERSONNEL_BTN2_DIS],
        samples[S_BUTTONCLICK]);
    m_personnel_panel->add_child_module(m_personnel_unassign);
    m_personnel_unassign->set_enabled(false);

    int i = 0;
    for (auto &type : OfficerIterator) {
        auto button = make_shared<PersonnelSlotButton>(
            CATBTN_X, CATBTN_Y + (i * PersonnelSlotButton::spacing), type);
        m_personnel_slot_buttons[button->get_id()] = button;
        auto officer = g_game->gameState->get_officer(type);
        if (officer) {
            button->set_officer(officer);
        }
        m_personnel_panel->add_child_module(button);
        ++i;
    }

    // tell questmgr that Personnel event has occurred
    g_game->questMgr->raiseEvent(18);
    m_personnel_panel->set_active(true);
    m_unemployed_panel->set_active(false);

    return true;
}

bool
ModuleCrewHire::on_event(ALLEGRO_EVENT *event) {
    bool exitToStarportCommons = false;
    string escape = "";

    switch (event->type) {
    case EVENT_CLOSE:
        set_modal_child(nullptr);
        break;
    case EVENT_SAVE_GAME:
        g_game->gameState->AutoSave();
        break;
    case EVENT_LOAD_GAME:
        g_game->gameState->AutoLoad();
        break;
    case EVENT_QUIT_GAME:
        g_game->set_vibration(0);
        escape = g_game->getGlobalString("ESCAPEMODULE");
        g_game->LoadModule(escape);
        break;

    case EVENT_CREWHIRE_EXIT:
        {
            bool passedCheck = true;
            for (auto type : OfficerIterator) {
                if (!g_game->gameState->has_officer(type)) {
                    passedCheck = false;
                    break;
                }
            }
            if (passedCheck)
                exitToStarportCommons = true;
        }
        break;

    case EVENT_CREWHIRE_HIRE_MORE:
        m_unemployed_panel->set_active(true);
        m_personnel_panel->set_active(false);
        m_exit_button->set_active(false);
        m_directions->set_text(c_hire_more_directions);
        m_selected_slot = nullopt;
        break;

    case EVENT_CREWHIRE_HIRE:
        {
            auto officer = m_unemployed_panel->hire();
            if (officer) {
                auto pref = officer->get_preferred_profession();

                if (!g_game->gameState->has_officer(pref)) {
                    // If no officer in the preferred slot, hire into that
                    // position
                    g_game->gameState->set_officer(pref, officer);

                    for (auto &i : m_personnel_slot_buttons) {
                        if (i.second->get_officer_type() == pref) {
                            i.second->set_officer(officer);
                            officer = nullptr;
                            break;
                        }
                    }
                    ALLEGRO_ASSERT(officer == nullptr);
                } else {
                    // Hire into unassigned
                    auto unassigned_button = make_shared<UnemployedSlotButton>(
                        UNASSIGNED_CREW_X,
                        UNASSIGNED_CREW_Y
                            + m_personnel_unassigned_slot_buttons.size()
                                  * UnemployedSlotButton::spacing,
                        officer,
                        EVENT_CREWHIRE_UNASSIGNED_CREW_CLICK);
                    m_personnel_unassigned_slot_buttons[unassigned_button
                                                            ->get_id()] =
                        unassigned_button;
                    m_personnel_unassigned_panel->add_child_module(
                        unassigned_button);
                }
            }
            m_selected_slot = nullopt;
        }
        break;

    case EVENT_CREWHIRE_BACK:
        m_unemployed_panel->set_active(false);
        m_exit_button->set_active(true);
        m_personnel_panel->set_active(true);
        m_directions->set_text(c_directions);
        m_selected_slot = nullopt;
        break;

    case EVENT_CREWHIRE_FIRE:
        if (m_selected_slot) {
            const Officer *officer = nullptr;
            if (m_selected_slot->slot_type == CREW) {
                auto button = m_personnel_slot_buttons[m_selected_slot->slot];
                OfficerType type = button->get_officer_type();
                if (type != OFFICER_CAPTAIN) {
                    officer = button->fire_officer();
                }
                button->set_highlight(false);
            } else {
                auto button =
                    m_personnel_unassigned_slot_buttons[m_selected_slot->slot];
                officer = button->fire_officer();

                m_personnel_unassigned_panel->remove_child_module(button);
                m_personnel_unassigned_slot_buttons.erase(
                    m_selected_slot->slot);
                for (auto &b : m_personnel_unassigned_slot_buttons) {
                    auto [x, y] = b.second->get_position();
                    b.second->move(x, y - UnemployedSlotButton::spacing);
                }
            }
            if (officer) {
                m_unemployed_panel->add(officer);
            }
            m_selected_slot = nullopt;
        }
        break;

    case EVENT_CREWHIRE_UNASSIGN:
        if (m_selected_slot) {
            if (m_selected_slot->slot_type == CREW) {
                auto button = m_personnel_slot_buttons[m_selected_slot->slot];
                OfficerType type = button->get_officer_type();
                if (type != OFFICER_CAPTAIN) {
                    auto unassigned_button = make_shared<UnemployedSlotButton>(
                        UNASSIGNED_CREW_X,
                        UNASSIGNED_CREW_Y
                            + m_personnel_unassigned_slot_buttons.size()
                                  * UnemployedSlotButton::spacing,
                        button->unassign_officer(),
                        EVENT_CREWHIRE_UNASSIGNED_CREW_CLICK);
                    m_personnel_unassigned_slot_buttons[unassigned_button
                                                            ->get_id()] =
                        unassigned_button;
                    m_personnel_unassigned_panel->add_child_module(
                        unassigned_button);
                }
                button->set_highlight(false);
            }
            m_selected_slot = nullopt;
        }
        break;

    case EVENT_CREWHIRE_UNASSIGNED_CREW_CLICK:
        {
            int slot_id = static_cast<int>(event->user.data1);
            auto button = m_personnel_unassigned_slot_buttons[slot_id];
            if (m_selected_slot && slot_id == m_selected_slot->slot) {
                // Select the same slot, clear highlight and selection
                button->set_highlight(false);
                m_selected_slot = nullopt;
            } else if (
                m_selected_slot && m_selected_slot->slot_type == CREW
                && m_personnel_slot_buttons[m_selected_slot->slot]
                           ->get_officer_type()
                       == OFFICER_CAPTAIN) {
                // Previously selected officer was captain; clear selection and
                // highlight on the captain. highlight and select the new
                // officer
                m_personnel_slot_buttons[m_selected_slot->slot]->set_highlight(
                    false);
                m_selected_slot = {UNASSIGNED, slot_id};
                button->set_highlight(true);
            } else if (m_selected_slot) {
                // Swap officer
                if (m_selected_slot->slot_type == CREW) {
                    auto old_button =
                        m_personnel_slot_buttons[m_selected_slot->slot];

                    button->set_officer(
                        old_button->set_officer(button->get_officer()));
                    old_button->set_highlight(false);
                    m_selected_slot = nullopt;
                } else {
                    // Old selection was also unassigned; no point in swapping
                    // so just set new selection.
                    auto old_slot_button =
                        m_personnel_unassigned_slot_buttons[m_selected_slot
                                                                ->slot];
                    old_slot_button->set_highlight(false);
                    button->set_highlight(true);
                    m_selected_slot = {UNASSIGNED, slot_id};
                }
            } else {
                // no officer selected, set selection to this one
                button->set_highlight(true);
                m_selected_slot = {UNASSIGNED, slot_id};
            }
        }
        break;

    case EVENT_CREWHIRE_PERSONNEL_CLICK:
        {
            int slot_id = static_cast<int>(event->user.data1);
            auto personnel_slot_button = m_personnel_slot_buttons[slot_id];
            OfficerType slot_officer_type =
                personnel_slot_button->get_officer_type();

            if (m_selected_slot && slot_id == m_selected_slot->slot) {
                // Select the same slot, clear highlight and selection
                personnel_slot_button->set_highlight(false);
                m_selected_slot = nullopt;
            } else if (slot_officer_type == OFFICER_CAPTAIN) {
                // clear previous highlight;
                // set highlight and selection to the captain slot
                if (m_selected_slot && m_selected_slot->slot_type == CREW) {
                    m_personnel_slot_buttons[m_selected_slot->slot]
                        ->set_highlight(false);
                } else if (
                    m_selected_slot
                    && m_selected_slot->slot_type == UNASSIGNED) {
                    m_personnel_unassigned_slot_buttons[m_selected_slot->slot]
                        ->set_highlight(false);
                }
                m_selected_slot = {CREW, slot_id};
                personnel_slot_button->set_highlight(true);
            } else if (
                m_selected_slot && m_selected_slot->slot_type == CREW
                && m_personnel_slot_buttons[m_selected_slot->slot]
                           ->get_officer_type()
                       == OFFICER_CAPTAIN) {
                // Previously selected officer was captain; clear
                // highlight from the captain. Highlight and select the new
                // officer
                m_personnel_slot_buttons[m_selected_slot->slot]->set_highlight(
                    false);
                personnel_slot_button->set_highlight(true);
                m_selected_slot = {CREW, slot_id};
            } else if (m_selected_slot) {
                // Swap officer
                if (m_selected_slot->slot_type == CREW) {
                    auto old_slot_button =
                        m_personnel_slot_buttons[m_selected_slot->slot];
                    personnel_slot_button->set_officer(
                        old_slot_button->set_officer(
                            personnel_slot_button->get_officer()));
                    old_slot_button->set_highlight(false);
                } else if (m_selected_slot->slot_type == UNASSIGNED) {
                    auto old_button =
                        m_personnel_unassigned_slot_buttons[m_selected_slot
                                                                ->slot];
                    personnel_slot_button->set_officer(old_button->set_officer(
                        personnel_slot_button->get_officer()));

                    if (old_button->has_officer()) {
                        old_button->set_highlight(false);
                    } else {
                        m_personnel_unassigned_panel->remove_child_module(
                            old_button);
                        m_personnel_unassigned_slot_buttons.erase(
                            m_selected_slot->slot);
                        for (auto &i : m_personnel_unassigned_slot_buttons) {
                            auto [x, y] = i.second->get_position();
                            i.second->move(
                                x, y - UnemployedSlotButton::spacing);
                        }
                    }
                }
                if (personnel_slot_button->has_officer()) {
                    personnel_slot_button->set_highlight(true);
                    m_selected_slot = {CREW, personnel_slot_button->get_id()};
                } else {
                    m_selected_slot = nullopt;
                }
            } else {
                // no officer selected, set selection to this one
                personnel_slot_button->set_highlight(true);
                m_selected_slot = {CREW, slot_id};
            }
        }
        break;
    }

    if (exitToStarportCommons) {
        g_game->LoadModule(MODULE_STARPORT);
        return false;
    } else if (
        m_selected_slot && m_selected_slot->slot_type == CREW
        && m_personnel_slot_buttons[m_selected_slot->slot]->get_officer_type()
               != OFFICER_CAPTAIN
        && m_personnel_slot_buttons[m_selected_slot->slot]->has_officer()) {
        m_title->set_active(false);
        m_slogan->set_active(false);
        m_directions->set_active(false);
        m_officer_info->set_officer(
            m_personnel_slot_buttons[m_selected_slot->slot]->get_officer());
        m_officer_info->set_active(true);
        m_personnel_fire->set_enabled(true);
        m_personnel_unassign->set_enabled(true);
    } else if (m_selected_slot && m_selected_slot->slot_type == UNASSIGNED) {
        auto button =
            m_personnel_unassigned_slot_buttons[m_selected_slot->slot];
        auto officer = button->get_officer();
        m_title->set_active(false);
        m_slogan->set_active(false);
        m_directions->set_active(false);
        m_officer_info->set_officer(officer);
        m_officer_info->set_active(true);
        m_personnel_fire->set_enabled(true);
    } else {
        m_title->set_active(true);
        m_slogan->set_active(true);
        m_officer_info->set_active(false);
        m_directions->set_active(true);
        m_personnel_fire->set_enabled(false);
        m_personnel_unassign->set_enabled(false);
    }
    return true;
}

bool
ModuleCrewHire::on_close() {
    ALLEGRO_DEBUG("CrewHire Close\n");

    remove_child_module(m_exit_button);
    m_exit_button = nullptr;

    m_personnel_fire = nullptr;
    m_personnel_unassign = nullptr;
    m_personnel_slot_buttons.clear();

    remove_child_module(m_personnel_panel);
    m_personnel_panel = nullptr;

    remove_child_module(m_officer_info);
    m_officer_info = nullptr;

    remove_child_module(m_title);
    m_title = nullptr;

    remove_child_module(m_slogan);
    m_slogan = nullptr;

    remove_child_module(m_directions);
    m_directions = nullptr;

    return true;
}
