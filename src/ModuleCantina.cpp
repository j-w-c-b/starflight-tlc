///*
//	STARFLIGHT - THE LOST COLONY
//	ModuleCantina.cpp - ??
//	Author: Justin Sargent
//	Date: 11/17/07
//*/

#include "ModuleCantina.h"
#include "AudioSystem.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "Label.h"
#include "ModeMgr.h"
#include "QuestMgr.h"
#include "Util.h"
#include "cantina_resources.h"

using namespace std;
using namespace cantina_resources;

ALLEGRO_DEBUG_CHANNEL("ModuleCantina")

#define EXITBTN_X 16
#define EXITBTN_Y 698

#define TURNINBTN_X EXITBTN_X + 176 + 40
#define TURNINBTN_Y EXITBTN_Y

#define WINDOW_X 198
#define WINDOW_Y 148
#define WINDOW_W 628
#define WINDOW_H 474

#define TITLE_X WINDOW_X
#define TITLE_Y WINDOW_Y
#define TITLE_H 60
#define TITLE_W WINDOW_W

#define REQUIREMENT_W 210

#define REWARD_X WINDOW_X
#define REWARD_Y TITLE_Y + TITLE_H
#define REWARD_W WINDOW_W
#define REWARD_H 75

#define LONG_X WINDOW_X
#define LONG_Y REWARD_Y + REWARD_H
#define LONG_H 300
#define LONG_W WINDOW_W

ModuleCantina::ModuleCantina()
    : Module(), m_background(nullptr), resources(CANTINA_IMAGES) {
    // create labels
    m_title_label = new Label(
        "",
        TITLE_X,
        TITLE_Y,
        TITLE_W - REQUIREMENT_W,
        TITLE_H,
        false,
        0,
        g_game->font24,
        WHITE);
    add_child_module(m_title_label);
    questTitle = new Label(
        "",
        TITLE_X,
        TITLE_Y + 23,
        TITLE_W,
        TITLE_H,
        false,
        0,
        g_game->font22,
        WHITE);
    add_child_module(questTitle);
    m_requirement_label = new Label(
        "",
        TITLE_X + WINDOW_W - REQUIREMENT_W,
        TITLE_Y,
        REQUIREMENT_W,
        TITLE_H,
        false,
        ALLEGRO_ALIGN_RIGHT,
        g_game->font24,
        WHITE);
    add_child_module(m_requirement_label);

    // description
    m_description_label = new Label(
        "", LONG_X, LONG_Y, LONG_W, LONG_H, true, 0, g_game->font24, WHITE);
    add_child_module(m_description_label);
    questLong = new Label(
        "",
        LONG_X,
        LONG_Y + 23,
        LONG_W,
        LONG_H,
        true,
        0,
        g_game->font22,
        WHITE);
    add_child_module(questLong);

    // reward
    m_reward_label = new Label(
        "",
        REWARD_X,
        REWARD_Y,
        REWARD_W,
        REWARD_H,
        true,
        0,
        g_game->font24,
        WHITE);
    add_child_module(m_reward_label);
    questReward = new Label(
        "",
        REWARD_X,
        REWARD_Y + 23,
        REWARD_W,
        REWARD_H,
        false,
        0,
        g_game->font22,
        WHITE);

    add_child_module(questReward);
}

ModuleCantina::~ModuleCantina(void) { ALLEGRO_DEBUG("ModuleCantina Dead\n"); }

bool
ModuleCantina::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    m_exitBtn->OnMouseMove(x, y);
    m_turninBtn->OnMouseMove(x, y);

    return true;
}

bool
ModuleCantina::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    int button = event->button - 1;
    int x = event->x;
    int y = event->y;

    m_turninBtn->OnMouseReleased(button, x, y);
    m_exitBtn->OnMouseReleased(button, x, y);

    return true;
}

bool
ModuleCantina::on_event(ALLEGRO_EVENT *event) {
    switch (event->type) {
    case EVENT_CANTINA_EXIT:
        g_game->LoadModule(MODULE_STARPORT);
        return false;

    case EVENT_CANTINA_CLICK:
        selectedQuestCompleted = true;
        break;
    }
    return true;
}

bool
ModuleCantina::on_close() {
    if (m_exitBtn != nullptr) {
        delete m_exitBtn;
        m_exitBtn = nullptr;
    }
    if (m_turninBtn != nullptr) {
        delete m_turninBtn;
        m_turninBtn = nullptr;
    }
    resources.unload();
    return true;
}

bool
ModuleCantina::on_init() {
    ALLEGRO_COLOR labelcolor, textcolor;
    string title, description, reward;

    ALLEGRO_DEBUG("  Cantina/Research Lab/Military Ops Initialize\n");

    // load the datafile
    if (!resources.load()) {
        g_game->message("Cantina: Error loading datafile");
        return false;
    }

    selectedQuestCompleted = false;

    g_game->audioSystem->Load("data/cantina/buttonclick.ogg", "click");

    // Create and initialize the ESC button for the module
    ALLEGRO_BITMAP *btnNorm, *btnOver, *btnDis;

    btnNorm = resources[I_CANTINA_EXIT_BTN_NORM];
    btnOver = resources[I_CANTINA_EXIT_BTN_OVER];
    m_exitBtn = new Button(
        btnNorm,
        btnOver,
        NULL,
        EXITBTN_X,
        EXITBTN_Y,
        EVENT_NONE,
        EVENT_CANTINA_EXIT,
        g_game->font24,
        "Exit",
        BLACK,
        "click");
    if (m_exitBtn == NULL)
        return false;
    if (!m_exitBtn->IsInitialized())
        return false;

    // load button images
    btnNorm = resources[I_CANTINA_BTN];
    btnOver = resources[I_CANTINA_BTN_HOV];
    btnDis = resources[I_CANTINA_BTN_DIS];

    // Create and initialize the TURNIN button for the module
    m_turninBtn = new Button(
        btnNorm,
        btnOver,
        btnDis,
        TURNINBTN_X,
        TURNINBTN_Y,
        EVENT_NONE,
        EVENT_CANTINA_CLICK,
        g_game->font24,
        "SUBMIT",
        BLACK,
        "click");
    if (m_turninBtn == NULL)
        return false;
    if (!m_turninBtn->IsInitialized())
        return false;
    m_turninBtn->SetEnabled(true);

    // Load the background image based on profession
    switch (g_game->gameState->getProfession()) {
    case PROFESSION_SCIENTIFIC:
        m_background = resources[I_RESEARCHLAB_BACKGROUND];
        m_turninBtn->SetButtonText("Breakthrough!");
        m_exitBtn->SetButtonText("Terminate");

        title = "PROJECT TITLE";
        description = "DESCRIPTION";
        reward = "REWARD";

        labelcolor = LTBLUE;
        textcolor = DODGERBLUE;
        break;
    case PROFESSION_MILITARY:
        m_background = resources[I_MILITARYOPS_BACKGROUND];
        m_turninBtn->SetButtonText("Accomplished!");
        m_exitBtn->SetButtonText("Dismissed");

        title = "MISSION CODENAME";
        description = "DESCRIPTION";
        reward = "REWARD";
        labelcolor = ORANGE;
        textcolor = DKORANGE;
        break;
    default:
        m_background = resources[I_CANTINA_BACKGROUND];
        m_turninBtn->SetButtonText("Pay Up!");
        m_exitBtn->SetButtonText("Scram");
        title = "JOB NAME";
        description = "DESCRIPTION";
        reward = "REWARD";
        labelcolor = LTYELLOW;
        textcolor = YELLOW;
        break;
    }
    m_title_label->set_text(title);
    m_title_label->set_color(labelcolor);
    questTitle->set_color(textcolor);

    m_description_label->set_text(description);
    m_description_label->set_color(labelcolor);
    questLong->set_color(textcolor);

    m_reward_label->set_text(reward);
    m_reward_label->set_color(labelcolor);
    questReward->set_color(textcolor);

    if (!m_background) {
        g_game->message("Error loading cantina background");
        return false;
    }

    return true;
}

bool
ModuleCantina::on_update() {
    static int debriefStatus = 0;

    // HACK!!!!!
    // if player acquires Hypercube, then skip to the 25th quest as a shortcut
    // note: this is dangerous since the quest script could change the quest
    // numbering if already at quest 25, then skip the hypercube check again...

    // FIXed: If Player obtained a 2nd HyperCube, the old code was throwing
    // Player back to 25/1. Not now.  jjh 2nd Cube could be used in the genetic
    // samples quest if (g_game->questMgr->getId() != 25) was always returning
    // -1.

    if (g_game->gameState->getActiveQuest() < 25) // jjh
    {
        Item newitem;
        int amt;
        g_game->gameState->m_items.Get_Item_By_ID(
            2 /* Hypercube */, newitem, amt);

        if (amt > 0) {
            g_game->questMgr->getQuestByID(25);
            g_game->gameState->m_items.RemoveItems(2, 1);
            selectedQuestCompleted = true;
        }
    }

    g_game->questMgr->getActiveQuest();
    questTitle->set_text(g_game->questMgr->getName());
    string desc = g_game->questMgr->getLong();
    int len = desc.length();

    // dynamically change font size for long descriptions
    if (len > 600) {
        questLong->set_font(g_game->font18);
    } else if (len > 800) {
        questLong->set_font(g_game->font20);
    } else {
        questLong->set_font(g_game->font22);
    }

    questLong->set_text(desc);

    // show requirement status
    if (!selectedQuestCompleted) {
        int reqAmt = static_cast<int>(g_game->questMgr->questReqAmt);
        g_game->questMgr->VerifyRequirements(
            g_game->questMgr->questReqCode,
            g_game->questMgr->questReqType,
            reqAmt);
    }
    if (g_game->gameState->getQuestCompleted()) {
        m_requirement_label->set_text("(COMPLETE)");
        m_requirement_label->set_color(GREEN);
        m_turninBtn->SetEnabled(true);
    } else {
        m_requirement_label->set_text("(INCOMPLETE)");
        m_requirement_label->set_color(RED);
        m_turninBtn->SetEnabled(false);
    }

    // display reward info
    string reward = "";
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

    questReward->set_text(reward);

    // do we need to show the debriefing and reward?
    if (selectedQuestCompleted)
        debriefStatus = 1;
    if (debriefStatus == 1) {
        if (g_game->questMgr->getDebrief().length() > 0) {
            g_game->ShowMessageBoxWindow(
                "",
                g_game->questMgr->getDebrief(),
                350,
                300,
                WHITE,
                650,
                440,
                false);
        }
        selectedQuestCompleted = false;
        debriefStatus = 2;
    } else if (debriefStatus == 2) {
        g_game->questMgr->giveReward();
        debriefStatus = 3;
    } else if (debriefStatus == 3) {
        g_game->gameState->setQuestCompleted(false);
        g_game->questMgr->getNextQuest();
        debriefStatus = 0;
    }
    return true;
}

bool
ModuleCantina::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);

    // draw background
    al_draw_bitmap(m_background, 0, 0, 0);

    // draw buttons
    m_exitBtn->Run(target);
    m_turninBtn->Run(target);

    return true;
}
