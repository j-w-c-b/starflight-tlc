//	STARFLIGHT - THE LOST COLONY
//	ModuleCantina.cpp - ??
//	Author: Justin Sargent
//	Date: 11/17/07
//

#include "ModuleCantina.h"
#include "AudioSystem.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "Label.h"
#include "MessageBoxWindow.h"
#include "ModeMgr.h"
#include "QuestMgr.h"
#include "Util.h"
#include "cantina_resources.h"

using namespace std;
using namespace cantina;

ALLEGRO_DEBUG_CHANNEL("ModuleCantina")

#define EXITBTN_X 16
#define EXITBTN_Y 698
#define EXITBTN_W 176
#define EXITBTN_H 74

#define TURNINBTN_X EXITBTN_X + EXITBTN_W + 40
#define TURNINBTN_Y EXITBTN_Y
#define TURNINBTN_W 278
#define TURNINBTN_H 78

#define WINDOW_X 198
#define WINDOW_Y 148
#define WINDOW_W 628
#define WINDOW_H 474

#define TITLE_X WINDOW_X
#define TITLE_Y WINDOW_Y
#define TITLE_H 60
#define TITLE_W WINDOW_W

#define REQUIREMENT_W 210

#define STATUS_X WINDOW_X
#define STATUS_Y TITLE_Y + TITLE_H
#define STATUS_W WINDOW_W
#define STATUS_H 75

#define LONG_X WINDOW_X
#define LONG_Y STATUS_Y + STATUS_H
#define LONG_H 300
#define LONG_W WINDOW_W

ModuleCantina::ModuleCantina()
    : Module(),
      m_background(
          make_shared<Bitmap>(nullptr, 0, 0, SCREEN_WIDTH, SCREEN_WIDTH)),
      m_title_heading_label(make_shared<Label>(
          "",
          TITLE_X,
          TITLE_Y,
          TITLE_W - REQUIREMENT_W,
          TITLE_H,
          false,
          0,
          g_game->font24,
          WHITE)),
      m_title_label(make_shared<Label>(
          "",
          TITLE_X,
          TITLE_Y + 23,
          TITLE_W,
          TITLE_H,
          false,
          0,
          g_game->font22,
          WHITE)),
      m_status_label(make_shared<Label>(
          "",
          TITLE_X + WINDOW_W - REQUIREMENT_W,
          TITLE_Y,
          REQUIREMENT_W,
          TITLE_H,
          false,
          ALLEGRO_ALIGN_RIGHT,
          g_game->font24,
          WHITE)),
      m_description_title_label(make_shared<Label>(
          "",
          LONG_X,
          LONG_Y,
          LONG_W,
          LONG_H,
          true,
          0,
          g_game->font24,
          WHITE)),
      m_description_label(make_shared<Label>(
          "",
          LONG_X,
          LONG_Y + 23,
          LONG_W,
          LONG_H,
          true,
          0,
          g_game->font22,
          WHITE)),
      m_reward_title_label(make_shared<Label>(
          "",
          STATUS_X,
          STATUS_Y,
          STATUS_W,
          STATUS_H,
          true,
          0,
          g_game->font24,
          WHITE)),
      m_reward_label(make_shared<Label>(
          "",
          STATUS_X,
          STATUS_Y + 23,
          STATUS_W,
          STATUS_H,
          false,
          0,
          g_game->font22,
          WHITE)),
      m_exit_button(make_shared<TextButton>(
          make_shared<Label>(
              "Exit",
              EXITBTN_X,
              EXITBTN_Y
                  + (EXITBTN_H - al_get_font_line_height(g_game->font24.get()))
                        / 2,
              EXITBTN_W,
              EXITBTN_H
                  - (EXITBTN_H - al_get_font_line_height(g_game->font24.get()))
                        / 2,
              false,
              ALLEGRO_ALIGN_CENTER,
              g_game->font24,
              BLACK),
          EXITBTN_X,
          EXITBTN_Y,
          EXITBTN_W,
          EXITBTN_H,
          EVENT_NONE,
          EVENT_CANTINA_EXIT,
          images[I_CANTINA_EXIT_BTN_NORM],
          images[I_CANTINA_EXIT_BTN_OVER],
          nullptr,
          samples[S_BUTTONCLICK])),
      m_turn_in_button(make_shared<TextButton>(
          make_shared<Label>(
              "Submit",
              TURNINBTN_X,
              TURNINBTN_Y
                  + (TURNINBTN_H
                     - al_get_font_line_height(g_game->font24.get()))
                        / 2,
              TURNINBTN_W,
              TURNINBTN_H
                  - (TURNINBTN_H
                     - al_get_font_line_height(g_game->font24.get()))
                        / 2,
              false,
              ALLEGRO_ALIGN_CENTER,
              g_game->font24,
              BLACK),
          TURNINBTN_X,
          TURNINBTN_Y,
          TURNINBTN_W,
          TURNINBTN_H,
          EVENT_NONE,
          EVENT_CANTINA_CLICK,
          images[I_CANTINA_BTN],
          images[I_CANTINA_BTN_HOV],
          images[I_CANTINA_BTN_DIS],
          samples[S_BUTTONCLICK])),
      m_quest_status(QUEST_ACTIVE) {
    add_child_module(m_background);

    add_child_module(m_title_heading_label);
    add_child_module(m_title_label);
    add_child_module(m_status_label);

    add_child_module(m_description_title_label);
    add_child_module(m_description_label);

    add_child_module(m_reward_title_label);
    add_child_module(m_reward_label);

    add_child_module(m_exit_button);
    add_child_module(m_turn_in_button);
}

bool
ModuleCantina::on_init() {
    ALLEGRO_COLOR labelcolor, textcolor;
    string title, description, reward;

    ALLEGRO_DEBUG("  Cantina/Research Lab/Military Ops Initialize\n");

    m_quest_status = QUEST_ACTIVE;

    // Load the background image based on profession
    switch (g_game->gameState->getProfession()) {
    case PROFESSION_SCIENTIFIC:
        m_background->set_bitmap(images[I_RESEARCHLAB_BACKGROUND]);
        m_turn_in_button->set_text("Breakthrough!");
        m_exit_button->set_text("Terminate");

        title = "PROJECT TITLE";
        description = "DESCRIPTION";
        reward = "REWARD";

        labelcolor = LTBLUE;
        textcolor = DODGERBLUE;
        break;
    case PROFESSION_MILITARY:
        m_background->set_bitmap(images[I_MILITARYOPS_BACKGROUND]);
        m_turn_in_button->set_text("Accomplished!");
        m_exit_button->set_text("Dismissed");

        title = "MISSION CODENAME";
        description = "DESCRIPTION";
        reward = "REWARD";
        labelcolor = ORANGE;
        textcolor = DKORANGE;
        break;
    default:
        m_background->set_bitmap(images[I_CANTINA_BACKGROUND]);
        m_turn_in_button->set_text("Pay Up!");
        m_exit_button->set_text("Scram");
        title = "JOB NAME";
        description = "DESCRIPTION";
        reward = "REWARD";
        labelcolor = LTYELLOW;
        textcolor = YELLOW;
        break;
    }
    m_title_heading_label->set_text(title);
    m_title_heading_label->set_color(labelcolor);
    m_title_label->set_color(textcolor);

    m_description_title_label->set_text(description);
    m_description_title_label->set_color(labelcolor);
    m_description_label->set_color(textcolor);

    m_reward_title_label->set_text(reward);
    m_reward_title_label->set_color(labelcolor);
    m_reward_label->set_color(textcolor);

    return true;
}

bool
ModuleCantina::on_event(ALLEGRO_EVENT *event) {
    switch (event->type) {
    case EVENT_CLOSE:
        if (m_quest_status == QUEST_DEBRIEF) {
            set_modal_child(nullptr);
            m_quest_status = QUEST_DEBRIEF_COMPLETE;
        } else if (m_quest_status == QUEST_REWARD) {
            set_modal_child(nullptr);
            m_quest_status = QUEST_REWARD_COMPLETE;
        }
        return false;
    case EVENT_CANTINA_EXIT:
        g_game->LoadModule(MODULE_STARPORT);
        return false;

    case EVENT_CANTINA_CLICK:
        m_quest_status = QUEST_COMPLETE;
        break;
    }
    return true;
}

bool
ModuleCantina::on_update() {
    // HACK!!!!!
    // if player acquires Hypercube, then skip to the 25th quest as a shortcut
    // note: this is dangerous since the quest script could change the quest
    // numbering if already at quest 25, then skip the hypercube check again...
    if (g_game->gameState->getActiveQuest() < 25) {
        Item newitem;
        int amt;
        g_game->gameState->m_items.Get_Item_By_ID(
            2 /* Hypercube */, newitem, amt);

        if (amt > 0) {
            g_game->questMgr->getQuestByID(25);
            g_game->gameState->m_items.RemoveItems(2, 1);
            m_quest_status = QUEST_COMPLETE;
        }
    }

    g_game->questMgr->getActiveQuest();
    m_title_label->set_text(g_game->questMgr->getName());
    string desc = g_game->questMgr->getLong();
    int len = desc.length();

    // dynamically change font size for long descriptions
    if (len > 600) {
        m_description_label->set_font(g_game->font18);
    } else if (len > 800) {
        m_description_label->set_font(g_game->font20);
    } else {
        m_description_label->set_font(g_game->font22);
    }

    m_description_label->set_text(desc);

    // show requirement status
    if (m_quest_status == QUEST_ACTIVE) {
        int reqAmt = static_cast<int>(g_game->questMgr->questReqAmt);
        g_game->questMgr->VerifyRequirements(
            g_game->questMgr->questReqCode,
            g_game->questMgr->questReqType,
            reqAmt);
    }
    if (g_game->gameState->getQuestCompleted()) {
        m_status_label->set_text("(COMPLETE)");
        m_status_label->set_color(GREEN);
        m_turn_in_button->set_enabled(true);
    } else {
        m_status_label->set_text("(INCOMPLETE)");
        m_status_label->set_color(RED);
        m_turn_in_button->set_enabled(false);
    }

    // display reward info
    string reward = "???";
    int amt = 0;
    switch (g_game->questMgr->questRewCode) {
    // 1 = money
    case 1:
        amt = static_cast<int>(g_game->questMgr->questRewAmt);
        reward = Util::ToString(amt, 1, 0) + " MU";
        break;

    // 2 = item
    case 2:
        {
            amt = static_cast<int>(g_game->questMgr->questRewAmt);
            int id = g_game->questMgr->questRewType;
            Item *item = g_game->dataMgr->GetItemByID(id);

            if (item) {
                reward = item->name + " - " + Util::ToString(amt, 1, 2)
                         + " cubic meters";
            } else
                reward = "";
            break;
        }
    }

    m_reward_label->set_text(reward);

    // do we need to show the debriefing and reward?
    if (m_quest_status == QUEST_COMPLETE) {
        if (g_game->questMgr->getDebrief().length() > 0) {
            set_modal_child(make_shared<MessageBoxWindow>(
                "",
                g_game->questMgr->getDebrief(),
                650,
                440,
                350,
                300,
                WHITE,
                false));
            m_quest_status = QUEST_DEBRIEF;
        } else {
            m_quest_status = QUEST_DEBRIEF_COMPLETE;
        }
    } else if (m_quest_status == QUEST_DEBRIEF_COMPLETE) {
        auto reward_msg = g_game->questMgr->giveReward();
        auto msg = make_shared<MessageBoxWindow>("", reward_msg);

        set_modal_child(msg);
        m_quest_status = QUEST_REWARD;
    } else if (m_quest_status == QUEST_REWARD_COMPLETE) {
        g_game->gameState->setQuestCompleted(false);
        g_game->questMgr->getNextQuest();
        m_quest_status = QUEST_ACTIVE;
    }
    return true;
}
