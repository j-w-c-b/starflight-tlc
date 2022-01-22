#include "ModuleCaptainsLounge.h"
#include "AudioSystem.h"
#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "Util.h"
#include "captainslounge_resources.h"

#include <sstream>
#include <string>

using namespace std;
using namespace captainslounge_resources;

#define TEXTHEIGHT_TITLES 60
#define TEXTHEIGHT_GAME_NAME 30
#define TEXTHEIGHT_GAME_PROFESSION 20
#define TEXTHEIGHT_BTN_TITLES 30
#define TEXTCOL al_map_rgb(0, 255, 255)

#define BACKBTN_X 52
#define BACKBTN_Y 698

#define EVENT_NONE 0
#define EVENT_BACK_CLICK 1
#define EVENT_LAUNCH_CLICK 2
#define EVENT_NEWCAPTAIN_SLOT0 100
#define EVENT_DELCAPTAIN_SLOT0                                                 \
    (EVENT_NEWCAPTAIN_SLOT0 + CAPTAINSLOUNGE_NUMSLOTS)
#define EVENT_SELCAPTAIN_SLOT0                                                 \
    (EVENT_DELCAPTAIN_SLOT0 + CAPTAINSLOUNGE_NUMSLOTS)
#define EVENT_SAVECAPTAIN_SLOT0                                                \
    (EVENT_SELCAPTAIN_SLOT0 + CAPTAINSLOUNGE_NUMSLOTS)

#define EVENT_CONFIRMDELCAPTAIN_YES 200
#define EVENT_CONFIRMDELCAPTAIN_NO 201

#define EVENT_CONFIRMSELCAPTAIN_YES 300
#define EVENT_CONFIRMSELCAPTAIN_NO 301

#define EVENT_CONFIRMSAVECAPTAIN_YES 400
#define EVENT_CONFIRMSAVECAPTAIN_NO 401

#define BTN_BASE_Y 200
#define BTN_DELTA_Y 96
#
#define TEXT_BOX_X_LEFT 580
#define TEXT_BOX_WIDTH 440

#define BTN_NEWCAPTAIN_X 361
#define BTN_DELCAPTAIN_X 425
#define BTN_SELCAPTAIN_X 489
#define BTN_SAVECAPTAIN_X TEXT_BOX_X_LEFT + 20

#define GAME_BASE_Y 192
#define GAME_DELTA_Y 97
#define GAME_X 36

#define GAMES_TITLE_X 64
#define GAMES_TITLE_Y 100
#define TEXTHEIGHT_GAMES_TITLE 50

#define CURGAME_TITLE_X BTN_SAVECAPTAIN_X + 37 + 20
#define CURGAME_TITLE_Y 182

#define YES_X 317
#define YES_Y 533
#define NO_X 620
#define NO_Y 533
#define EVENT_YES 1000
#define EVENT_NO 1001
#define TEXTHEIGHT_MODALPROMPT 40
#define MODALPROMPT_START_Y 230

#define MODALPROMPT_BG_X 69
#define MODALPROMPT_BG_Y 157

ALLEGRO_DEBUG_CHANNEL("ModuleCaptainsLounge")

ModuleCaptainsLounge::ModuleCaptainsLounge(void)
    : m_resources(CAPTAINSLOUNGE_IMAGES) {
    m_requestedCaptainCreation = false;
    m_requestedCaptainCreationSlotNum = GameState::GAME_SAVE_SLOT0;
    displayHelp = true;
}

ModuleCaptainsLounge::~ModuleCaptainsLounge(void) {}

bool
ModuleCaptainsLounge::Init() {
    ALLEGRO_BITMAP *btnNorm, *btnOver, *btnDis;

    g_game->SetTimePaused(true); // game-time frozen in this module.

    // load the resources
    if (!m_resources.load()) {
        g_game->message("CaptainsLounge: Error loading resources");
        return false;
    }

    // load the background
    m_background = m_resources[I_CAPTAINSLOUNGE_BACKGROUND];

    btnNorm = m_resources[I_GENERIC_EXIT_BTN_NORM];
    btnOver = m_resources[I_GENERIC_EXIT_BTN_OVER];

    // create exit button
    m_backBtn = new Button(btnNorm,
                           btnOver,
                           NULL,
                           BACKBTN_X,
                           BACKBTN_Y,
                           EVENT_NONE,
                           EVENT_BACK_CLICK,
                           g_game->font32,
                           "Exit",
                           BLACK);
    if (m_backBtn == NULL)
        return false;
    if (!m_backBtn->IsInitialized())
        return false;

    // create launch button
    m_launchBtn = new Button(btnNorm,
                             btnOver,
                             NULL,
                             BACKBTN_X + 180,
                             BACKBTN_Y,
                             EVENT_NONE,
                             EVENT_LAUNCH_CLICK,
                             g_game->font32,
                             "Launch",
                             BLACK);
    if (m_launchBtn == NULL)
        return false;
    if (!m_launchBtn->IsInitialized())
        return false;

    int y = BTN_BASE_Y;
    for (int i = 0; i < CAPTAINSLOUNGE_NUMSLOTS; i++) {
        btnNorm = m_resources[I_CAPTAINSLOUNGE_PLUS];
        btnOver = m_resources[I_CAPTAINSLOUNGE_PLUS_MOUSEOVER];
        btnDis = m_resources[I_CAPTAINSLOUNGE_PLUS_DISABLED];
        m_newCaptBtns[i] = new Button(btnNorm,
                                      btnOver,
                                      btnDis,
                                      BTN_NEWCAPTAIN_X,
                                      y,
                                      EVENT_NONE,
                                      EVENT_NEWCAPTAIN_SLOT0 + i);
        if (m_newCaptBtns[i] == NULL)
            return false;
        if (!m_newCaptBtns[i]->IsInitialized())
            return false;

        btnNorm = m_resources[I_CAPTAINSLOUNGE_DEL];
        btnOver = m_resources[I_CAPTAINSLOUNGE_DEL_MOUSEOVER];
        btnDis = m_resources[I_CAPTAINSLOUNGE_DEL_DISABLED];
        m_delCaptBtns[i] = new Button(btnNorm,
                                      btnOver,
                                      btnDis,
                                      BTN_DELCAPTAIN_X,
                                      y,
                                      EVENT_NONE,
                                      EVENT_DELCAPTAIN_SLOT0 + i);
        if (m_delCaptBtns[i] == NULL)
            return false;
        if (!m_delCaptBtns[i]->IsInitialized())
            return false;

        btnNorm = m_resources[I_CAPTAINSLOUNGE_SEL];
        btnOver = m_resources[I_CAPTAINSLOUNGE_SEL_MOUSEOVER];
        btnDis = m_resources[I_CAPTAINSLOUNGE_SEL_DISABLED];
        m_selCaptBtns[i] = new Button(btnNorm,
                                      btnOver,
                                      btnDis,
                                      BTN_SELCAPTAIN_X,
                                      y,
                                      EVENT_NONE,
                                      EVENT_SELCAPTAIN_SLOT0 + i);
        if (m_selCaptBtns[i] == NULL)
            return false;
        if (!m_selCaptBtns[i]->IsInitialized())
            return false;

        btnNorm = m_resources[I_CAPTAINSLOUNGE_SAVE];
        btnOver = m_resources[I_CAPTAINSLOUNGE_SAVE_MOUSEOVER];
        m_saveCaptBtns[i] = new Button(btnNorm,
                                       btnOver,
                                       NULL,
                                       BTN_SAVECAPTAIN_X,
                                       y,
                                       EVENT_NONE,
                                       EVENT_SAVECAPTAIN_SLOT0 + i);
        if (m_saveCaptBtns[i] == NULL)
            return false;
        if (!m_saveCaptBtns[i]->IsInitialized())
            return false;

        m_games[i] = NULL;

        y += BTN_DELTA_Y;
    }

    btnNorm = m_resources[I_CAPTAINSLOUNGE_YES];
    btnOver = m_resources[I_CAPTAINSLOUNGE_YES_MOUSEOVER];
    m_yesBtn =
        new Button(btnNorm, btnOver, NULL, YES_X, YES_Y, EVENT_NONE, EVENT_YES);
    if (!m_yesBtn->IsInitialized())
        return false;

    btnNorm = m_resources[I_CAPTAINSLOUNGE_NO];
    btnOver = m_resources[I_CAPTAINSLOUNGE_NO_MOUSEOVER];
    m_noBtn =
        new Button(btnNorm, btnOver, NULL, NO_X, NO_Y, EVENT_NONE, EVENT_NO);
    if (!m_noBtn->IsInitialized())
        return false;

    m_backBtn->OnMouseMove(0, 0);
    m_launchBtn->OnMouseMove(0, 0);
    for (int i = 0; i < CAPTAINSLOUNGE_NUMSLOTS; i++) {
        m_newCaptBtns[i]->OnMouseMove(0, 0);
        m_delCaptBtns[i]->OnMouseMove(0, 0);
        m_selCaptBtns[i]->OnMouseMove(0, 0);
        m_saveCaptBtns[i]->OnMouseMove(0, 0);
    }
    m_yesBtn->OnMouseMove(0, 0);
    m_noBtn->OnMouseMove(0, 0);

    if (m_requestedCaptainCreation) {
        GameState *gs = GameState::LoadGame(GameState::GAME_SAVE_SLOT_NEW);
        if (gs != NULL) {
            gs->SaveGame(m_requestedCaptainCreationSlotNum);
        }
        GameState::DeleteGame(GameState::GAME_SAVE_SLOT_NEW);
        m_requestedCaptainCreation = false;
    }

    // load audio files
    m_sndBtnClick =
        g_game->audioSystem->Load("data/captainslounge/buttonclick.ogg");
    if (!m_sndBtnClick) {
        g_game->message("Lounge: Error loading buttonclick");
        return false;
    }

    m_modalPromptActive = false;

    m_modalPromptBackground =
        m_resources[I_CAPTAINSLOUNGE_MODALPROMPT_BACKGROUND];

    LoadGames();

    for (int i = 0; i < CAPTAINSLOUNGE_NUMSLOTS; i++) {
        m_saveCaptBtns[i]->SetVisible(g_game->gameState->m_captainSelected);
    }

    return true;
}

void
ModuleCaptainsLounge::Close() {
    // continue the stardate updates

    if (m_sndBtnClick != NULL) {
        m_sndBtnClick = NULL;
    }

    if (m_backBtn != NULL) {
        delete m_backBtn;
        m_backBtn = NULL;
    }
    if (m_launchBtn != NULL) {
        delete m_launchBtn;
        m_launchBtn = NULL;
    }

    for (int i = 0; i < CAPTAINSLOUNGE_NUMSLOTS; i++) {
        if (m_newCaptBtns[i] != NULL) {
            delete m_newCaptBtns[i];
            m_newCaptBtns[i] = NULL;
        }

        if (m_delCaptBtns[i] != NULL) {
            delete m_delCaptBtns[i];
            m_delCaptBtns[i] = NULL;
        }

        if (m_selCaptBtns[i] != NULL) {
            delete m_selCaptBtns[i];
            m_selCaptBtns[i] = NULL;
        }

        if (m_saveCaptBtns[i] != NULL) {
            delete m_saveCaptBtns[i];
            m_saveCaptBtns[i] = NULL;
        }

        if (m_games[i] != NULL) {
            delete m_games[i];
            m_games[i] = NULL;
        }
    }

    if (m_yesBtn != NULL) {
        delete m_yesBtn;
        m_yesBtn = NULL;
    }

    if (m_noBtn != NULL) {
        delete m_noBtn;
        m_noBtn = NULL;
    }

    // unload the data
    m_resources.unload();
}

void
ModuleCaptainsLounge::Update() {}

void
ModuleCaptainsLounge::Draw() {
    al_set_target_bitmap(g_game->GetBackBuffer());

    // draw background
    al_draw_bitmap(m_background, 0, 0, 0);

    m_backBtn->Run(g_game->GetBackBuffer());
    m_launchBtn->Run(g_game->GetBackBuffer());

    int x = CURGAME_TITLE_X, y = CURGAME_TITLE_Y;

    if (g_game->gameState->m_captainSelected) {
        g_game->Print24(g_game->GetBackBuffer(),
                        x,
                        y,
                        g_game->gameState->officerCap->name.c_str(),
                        TEXTCOL);

        ostringstream str;
        str << "Profession: ";

        if (g_game->gameState->m_profession == PROFESSION_SCIENTIFIC)
            str << "Scientific";
        else if (g_game->gameState->m_profession == PROFESSION_FREELANCE)
            str << "Freelance";
        else if (g_game->gameState->m_profession == PROFESSION_MILITARY)
            str << "Military";

        y += TEXTHEIGHT_GAME_NAME + 2;
        g_game->Print18(
            g_game->GetBackBuffer(), x, y, str.str().c_str(), TEXTCOL);

        y += TEXTHEIGHT_GAME_PROFESSION + 2;
        {
            y += TEXTHEIGHT_GAME_PROFESSION + 2;
            g_game->Print18(g_game->GetBackBuffer(),
                            x,
                            y,
                            "Stardate: " +
                                g_game->gameState->stardate.GetFullDateString(),
                            TEXTCOL);
        }

        {
            ostringstream str;
            str << "Credits: " << g_game->gameState->m_credits;
            y += TEXTHEIGHT_GAME_PROFESSION + 2;
            g_game->Print18(
                g_game->GetBackBuffer(), x, y, str.str().c_str(), TEXTCOL);
        }

        {
            string str = g_game->gameState->getSavedModule();
            if (str == MODULE_CAPTAINCREATION)
                str = "Captain Creation";
            else if (str == MODULE_CAPTAINSLOUNGE)
                str = "Captain's Lounge";
            else if (str == MODULE_HYPERSPACE)
                str = "Hyperspace";
            else if (str == MODULE_INTERPLANETARY)
                str = "Interplanetary space";
            else if (str == MODULE_ORBIT)
                str = "Planet Orbit";
            else if (str == MODULE_SURFACE)
                str = "Planet Surface";
            else if (str == MODULE_PORT)
                str = "Starport";
            else if (str == MODULE_STARPORT)
                str = "Starport";

            str = "Current location: " + str;
            y += TEXTHEIGHT_GAME_PROFESSION + 2;
            g_game->Print18(
                g_game->GetBackBuffer(), x, y, str.c_str(), TEXTCOL);
        }

        y += TEXTHEIGHT_GAME_PROFESSION + 4;
        {
            ostringstream str;
            str << "Ship: "
                << "MSS " << g_game->gameState->m_ship.getName();
            y += TEXTHEIGHT_GAME_PROFESSION + 2;
            g_game->Print18(
                g_game->GetBackBuffer(), x, y, str.str().c_str(), TEXTCOL);
        }

        {
            ostringstream str;
            str << "Cargo Pods: "
                << Util::ToString(g_game->gameState->m_ship.getCargoPodCount());
            y += TEXTHEIGHT_GAME_PROFESSION + 2;
            g_game->Print18(
                g_game->GetBackBuffer(), x, y, str.str().c_str(), TEXTCOL);
        }

        {
            ostringstream str;
            str << "Engine: "
                << g_game->gameState->m_ship.getEngineClassString();
            g_game->Print18(g_game->GetBackBuffer(),
                            x + 150,
                            y,
                            str.str().c_str(),
                            TEXTCOL);
        }

        {
            ostringstream str;
            str << "Armor: " << g_game->gameState->m_ship.getArmorClassString();
            y += TEXTHEIGHT_GAME_PROFESSION + 2;
            g_game->Print18(
                g_game->GetBackBuffer(), x, y, str.str().c_str(), TEXTCOL);
        }

        {
            ostringstream str;
            str << "Shields: "
                << g_game->gameState->m_ship.getShieldClassString();
            g_game->Print18(g_game->GetBackBuffer(),
                            x + 150,
                            y,
                            str.str().c_str(),
                            TEXTCOL);
        }

        {
            ostringstream str;
            str << "Laser: " << g_game->gameState->m_ship.getLaserClassString();
            y += TEXTHEIGHT_GAME_PROFESSION + 2;
            g_game->Print18(
                g_game->GetBackBuffer(), x, y, str.str().c_str(), TEXTCOL);
        }

        {
            ostringstream str;
            str << "Missile: "
                << g_game->gameState->m_ship.getMissileLauncherClassString();
            g_game->Print18(g_game->GetBackBuffer(),
                            x + 150,
                            y,
                            str.str().c_str(),
                            TEXTCOL);
        }

        y += 2 * TEXTHEIGHT_GAME_PROFESSION;
        g_game->Print20(g_game->GetBackBuffer(), x, y, "CREW:", TEXTCOL);
        {
            int dy = TEXTHEIGHT_GAME_PROFESSION + 2;
            string str = g_game->gameState->officerSci->name;
            y += 2 * dy;
            g_game->Print12(
                g_game->GetBackBuffer(), x, y, "Science: ", TEXTCOL);
            g_game->Print12(
                g_game->GetBackBuffer(), x + 140, y, str.c_str(), TEXTCOL);

            str = g_game->gameState->officerNav->name;
            y += dy;
            g_game->Print12(
                g_game->GetBackBuffer(), x, y, "Navigation: ", TEXTCOL);
            g_game->Print12(
                g_game->GetBackBuffer(), x + 140, y, str.c_str(), TEXTCOL);

            str = g_game->gameState->officerTac->name;
            y += dy;
            g_game->Print12(
                g_game->GetBackBuffer(), x, y, "Tactical: ", TEXTCOL);
            g_game->Print12(
                g_game->GetBackBuffer(), x + 140, y, str.c_str(), TEXTCOL);

            str = g_game->gameState->officerEng->name;
            y += dy;
            g_game->Print12(
                g_game->GetBackBuffer(), x, y, "Engineering: ", TEXTCOL);
            g_game->Print12(
                g_game->GetBackBuffer(), x + 140, y, str.c_str(), TEXTCOL);

            str = g_game->gameState->officerCom->name;
            y += dy;
            g_game->Print12(
                g_game->GetBackBuffer(), x, y, "Communications: ", TEXTCOL);
            g_game->Print12(
                g_game->GetBackBuffer(), x + 140, y, str.c_str(), TEXTCOL);

            str = g_game->gameState->officerDoc->name;
            y += dy;
            g_game->Print12(
                g_game->GetBackBuffer(), x, y, "Medical: ", TEXTCOL);
            g_game->Print12(
                g_game->GetBackBuffer(), x + 140, y, str.c_str(), TEXTCOL);
        }

        al_draw_text(g_game->font20,
                     TEXTCOL,
                     BTN_SAVECAPTAIN_X + (m_selCaptBtns[0]->GetWidth() / 2),
                     CURGAME_TITLE_Y - 40,
                     ALLEGRO_ALIGN_CENTER,
                     "SAVE");
    } else {
        string none_selected =
            "You may load an existing captain by "
            "clicking on the target (load) button beside a slot which "
            "contains an existing captain.\n\n"
            "Or, you may create a new captain by "
            "clicking on the + (new) button beside "
            "an empty slot.";

        al_draw_multiline_text(g_game->font24,
                               TEXTCOL,
                               TEXT_BOX_X_LEFT + 20,
                               y,
                               TEXT_BOX_WIDTH - 40,
                               0,
                               0,
                               none_selected.c_str());
    }

    y = GAME_BASE_Y;
    for (int i = 0; i < CAPTAINSLOUNGE_NUMSLOTS; i++) {
        m_newCaptBtns[i]->Run(g_game->GetBackBuffer());
        m_delCaptBtns[i]->Run(g_game->GetBackBuffer());
        m_selCaptBtns[i]->Run(g_game->GetBackBuffer());
        m_saveCaptBtns[i]->Run(g_game->GetBackBuffer());

        if (m_games[i] != NULL) {
            g_game->Print32(g_game->GetBackBuffer(),
                            GAME_X,
                            y,
                            m_games[i]->officerCap->name.c_str(),
                            TEXTCOL);

            string profName;
            if (m_games[i]->m_profession == PROFESSION_SCIENTIFIC)
                profName = "Scientific";
            else if (m_games[i]->m_profession == PROFESSION_FREELANCE)
                profName = "Freelance";
            else if (m_games[i]->m_profession == PROFESSION_MILITARY)
                profName = "Military";

            g_game->Print20(g_game->GetBackBuffer(),
                            GAME_X,
                            y + TEXTHEIGHT_GAME_NAME + 2,
                            profName.c_str(),
                            TEXTCOL);
        }
        y += GAME_DELTA_Y;
    }

    // modules should not be calling alfont functions directly--use the engine

    al_draw_text(g_game->font20,
                 TEXTCOL,
                 BTN_NEWCAPTAIN_X + (m_newCaptBtns[0]->GetWidth() / 2),
                 CURGAME_TITLE_Y - 40,
                 ALLEGRO_ALIGN_CENTER,
                 "NEW");
    al_draw_text(g_game->font20,
                 TEXTCOL,
                 BTN_DELCAPTAIN_X + (m_delCaptBtns[0]->GetWidth() / 2),
                 CURGAME_TITLE_Y - 40,
                 ALLEGRO_ALIGN_CENTER,
                 "DEL");
    al_draw_text(g_game->font20,
                 TEXTCOL,
                 BTN_SELCAPTAIN_X + (m_selCaptBtns[0]->GetWidth() / 2),
                 CURGAME_TITLE_Y - 40,
                 ALLEGRO_ALIGN_CENTER,
                 "LOAD");

    if (m_modalPromptActive) {
        al_draw_bitmap(
            m_modalPromptBackground, MODALPROMPT_BG_X, MODALPROMPT_BG_Y, 0);

        int y = MODALPROMPT_START_Y;
        for (vector<string>::iterator i = m_modalPromptStrings.begin();
             i != m_modalPromptStrings.end();
             ++i) {
            al_draw_text(g_game->font32,
                         TEXTCOL,
                         SCREEN_WIDTH / 2,
                         y,
                         ALLEGRO_ALIGN_CENTER,
                         (*i).c_str());
            y += TEXTHEIGHT_MODALPROMPT + 2;
        }

        m_yesBtn->Run(g_game->GetBackBuffer());
        m_noBtn->Run(g_game->GetBackBuffer());

        return;
    }

    // display tutorial help messages for beginners
    if (displayHelp) {
        if ((g_game->gameState->firstTimeVisitor &&
             g_game->gameState->getActiveQuest() <= 1)) {
            displayHelp = false;
            string str = "Welcome to the lounge, captain! This is where you "
                         "can load and save your game. The panel on the right "
                         "shows information about the game currently in play.";
            g_game->ShowMessageBoxWindow("", str, 400, 300, WHITE, 600, 400);
        }
    }
}

void
ModuleCaptainsLounge::OnMouseMove(int x, int y) {
    if (m_modalPromptActive) {
        m_yesBtn->OnMouseMove(x, y);
        m_noBtn->OnMouseMove(x, y);

        return;
    }

    m_backBtn->OnMouseMove(x, y);
    m_launchBtn->OnMouseMove(x, y);

    for (int i = 0; i < CAPTAINSLOUNGE_NUMSLOTS; i++) {
        m_newCaptBtns[i]->OnMouseMove(x, y);
        m_delCaptBtns[i]->OnMouseMove(x, y);
        m_selCaptBtns[i]->OnMouseMove(x, y);
        m_saveCaptBtns[i]->OnMouseMove(x, y);
    }
}

void
ModuleCaptainsLounge::OnMouseReleased(int button, int x, int y) {

    if (m_modalPromptActive) {
        m_yesBtn->OnMouseReleased(button, x, y);
        m_noBtn->OnMouseReleased(button, x, y);

        return;
    }

    // heinous anus - avoiding -> on bad variables after leaving the module
    for (int i = 0; i < CAPTAINSLOUNGE_NUMSLOTS; i++) {
        m_delCaptBtns[i]->OnMouseReleased(button, x, y);
        m_selCaptBtns[i]->OnMouseReleased(button, x, y);
        m_saveCaptBtns[i]->OnMouseReleased(button, x, y);
        if (m_newCaptBtns[i]->OnMouseReleased(button, x, y))
            return;
    }
    m_backBtn->OnMouseReleased(button, x, y);
    m_launchBtn->OnMouseReleased(button, x, y);
}

void
ModuleCaptainsLounge::OnEvent(Event *event) {
    bool playBtnClick = false;
    bool exitToStarportCommons = false;
    bool launchSavedModule = false;
    bool exitToCaptCreation = false;

    switch (event->getEventType()) {
    case EVENT_BACK_CLICK:
        playBtnClick = true;
        exitToStarportCommons = true;
        break;
    case EVENT_LAUNCH_CLICK:
        playBtnClick = true;
        launchSavedModule = true;
        break;
    }

    if ((event->getEventType() >= EVENT_NEWCAPTAIN_SLOT0) &&
        (event->getEventType() <
         (EVENT_NEWCAPTAIN_SLOT0 + CAPTAINSLOUNGE_NUMSLOTS))) {
        // NEW captain
        m_requestedCaptainCreation = true;
        m_requestedCaptainCreationSlotNum =
            static_cast<GameState::GameSaveSlot>(event->getEventType() -
                                                 EVENT_NEWCAPTAIN_SLOT0);

        playBtnClick = true;
        exitToCaptCreation = true;
    }

    if ((event->getEventType() >= EVENT_DELCAPTAIN_SLOT0) &&
        (event->getEventType() <
         (EVENT_DELCAPTAIN_SLOT0 + CAPTAINSLOUNGE_NUMSLOTS))) {
        // DEL captain
        playBtnClick = true;

        // activate the confirmation modal prompt
        m_modalPromptActive = true;
        m_modalPromptStrings.clear();
        m_modalPromptStrings.push_back(
            "Are you sure you want to delete this Captain?");
        m_modalPromptStrings.push_back("");
        m_modalPromptStrings.push_back(
            m_games[event->getEventType() - EVENT_DELCAPTAIN_SLOT0]
                ->officerCap->name);
        m_modalPromptYesEvent = EVENT_CONFIRMDELCAPTAIN_YES;
        m_modalPromptNoEvent = EVENT_CONFIRMDELCAPTAIN_NO;
        m_modalPromptSlotNum = static_cast<GameState::GameSaveSlot>(
            event->getEventType() - EVENT_DELCAPTAIN_SLOT0);

        m_yesBtn->OnMouseMove(0, 0);
        m_noBtn->OnMouseMove(0, 0);
    }

    // modal prompt responses
    if (event->getEventType() == EVENT_YES) {
        m_modalPromptActive = false;
        playBtnClick = true;
        Event e(m_modalPromptYesEvent);
        g_game->modeMgr->BroadcastEvent(&e);

        m_backBtn->OnMouseMove(0, 0);
        m_launchBtn->OnMouseMove(0, 0);
        for (int i = 0; i < CAPTAINSLOUNGE_NUMSLOTS; i++) {
            m_newCaptBtns[i]->OnMouseMove(0, 0);
            m_delCaptBtns[i]->OnMouseMove(0, 0);
            m_selCaptBtns[i]->OnMouseMove(0, 0);
        }
    }

    if (event->getEventType() == EVENT_NO) {
        m_modalPromptActive = false;
        playBtnClick = true;
        Event e(m_modalPromptNoEvent);
        g_game->modeMgr->BroadcastEvent(&e);

        m_backBtn->OnMouseMove(0, 0);
        m_launchBtn->OnMouseMove(0, 0);
        for (int i = 0; i < CAPTAINSLOUNGE_NUMSLOTS; i++) {
            m_newCaptBtns[i]->OnMouseMove(0, 0);
            m_delCaptBtns[i]->OnMouseMove(0, 0);
            m_selCaptBtns[i]->OnMouseMove(0, 0);
        }
    }

    if (event->getEventType() == EVENT_CONFIRMDELCAPTAIN_YES) {
        GameState::DeleteGame(m_modalPromptSlotNum);
        LoadGames();
    }

    if (event->getEventType() == EVENT_CONFIRMDELCAPTAIN_NO) {
        // user cancelled captain deletion request
    }

    if ((event->getEventType() >= EVENT_SELCAPTAIN_SLOT0) &&
        (event->getEventType() <
         (EVENT_SELCAPTAIN_SLOT0 + CAPTAINSLOUNGE_NUMSLOTS))) {
        // SEL captain
        playBtnClick = true;

        // activate the confirmation modal prompt if needed:
        // game state becomes dirty when we leave the captain's lounge
        //(and potentially go anywhere).
        if (g_game->gameState->dirty == true) {
            m_modalPromptActive = true;
            m_modalPromptStrings.clear();
            m_modalPromptStrings.push_back("Select this Captain?");
            m_modalPromptStrings.push_back(
                "Progress in the Current Game will be lost.");
            m_modalPromptStrings.push_back("");
            m_modalPromptStrings.push_back(
                m_games[event->getEventType() - EVENT_SELCAPTAIN_SLOT0]
                    ->officerCap->name);
            m_modalPromptYesEvent = EVENT_CONFIRMSELCAPTAIN_YES;
            m_modalPromptNoEvent = EVENT_CONFIRMSELCAPTAIN_NO;
            m_modalPromptSlotNum = static_cast<GameState::GameSaveSlot>(
                event->getEventType() - EVENT_SELCAPTAIN_SLOT0);

            m_yesBtn->OnMouseMove(0, 0);
            m_noBtn->OnMouseMove(0, 0);
        } else {
            m_modalPromptSlotNum = static_cast<GameState::GameSaveSlot>(
                event->getEventType() - EVENT_SELCAPTAIN_SLOT0);
            event->setEventType(EVENT_CONFIRMSELCAPTAIN_YES);
        }
    }

    if (event->getEventType() == EVENT_CONFIRMSELCAPTAIN_YES) {
        GameState *lgs = GameState::LoadGame(m_modalPromptSlotNum);
        if (lgs == NULL) {
            g_game->message("CaptainsLounge: Error loading game save file.");
        } else {

            for (int i = 0; i < CAPTAINSLOUNGE_NUMSLOTS; i++) {
                m_saveCaptBtns[i]->SetVisible(true);
            }
        }
    }

    if (event->getEventType() == EVENT_CONFIRMSELCAPTAIN_NO) {
        // user cancelled captain selection request
    }

    if ((event->getEventType() >= EVENT_SAVECAPTAIN_SLOT0) &&
        (event->getEventType() <
         (EVENT_SAVECAPTAIN_SLOT0 + CAPTAINSLOUNGE_NUMSLOTS))) {
        playBtnClick = true;

        // SEL captain
        int slot = (event->getEventType() - EVENT_SAVECAPTAIN_SLOT0);

        if (m_games[slot] == nullptr) {
            g_game->gameState->SaveGame(
                static_cast<GameState::GameSaveSlot>(slot));
            LoadGames();
        } else {
            // activate the confirmation modal prompt
            m_modalPromptActive = true;
            m_modalPromptStrings.clear();
            m_modalPromptStrings.push_back("Save this Captain?");
            m_modalPromptStrings.push_back(
                "Existing Captain will be overwritten");
            m_modalPromptStrings.push_back("");
            m_modalPromptStrings.push_back(g_game->gameState->officerCap->name);
            m_modalPromptYesEvent = EVENT_CONFIRMSAVECAPTAIN_YES;
            m_modalPromptNoEvent = EVENT_CONFIRMSAVECAPTAIN_NO;
            m_modalPromptSlotNum = static_cast<GameState::GameSaveSlot>(
                event->getEventType() - EVENT_SAVECAPTAIN_SLOT0);

            m_yesBtn->OnMouseMove(0, 0);
            m_noBtn->OnMouseMove(0, 0);
        }
    }

    if (event->getEventType() == EVENT_CONFIRMSAVECAPTAIN_YES) {
        GameState::DeleteGame(m_modalPromptSlotNum);
        g_game->gameState->SaveGame(m_modalPromptSlotNum);
        LoadGames();
    }

    if (event->getEventType() == EVENT_CONFIRMSAVECAPTAIN_NO) {
        // user cancelled captain save request
    }

    if (playBtnClick) {
        g_game->audioSystem->Play(m_sndBtnClick);
    }

    // issue 181 resolved
    // this launches the module where the game was saved
    if (launchSavedModule) {
        // return player to previous saved mode:
        if (g_game->gameState->m_captainSelected == true) {
            g_game->gameState->dirty = true;
            if (g_game->gameState->getSavedModule() == MODULE_CAPTAINSLOUNGE)
                g_game->LoadModule(MODULE_STARPORT);
            else
                g_game->LoadModule(g_game->gameState->getSavedModule());
        }
    }

    // issue 181 problem
    // this exits to the starport or the title screen
    else if (exitToStarportCommons) {
        if (g_game->gameState->m_captainSelected == true) {
            g_game->gameState->dirty = true;
            g_game->LoadModule(MODULE_STARPORT);
        } else
            g_game->LoadModule(MODULE_TITLESCREEN);

        return;
    } else if (exitToCaptCreation) {
        g_game->LoadModule(MODULE_CAPTAINCREATION);
        return;
    }
}

void
ModuleCaptainsLounge::LoadGames() {
    for (int i = 0; i < CAPTAINSLOUNGE_NUMSLOTS; i++) {
        if (m_games[i] != NULL) {
            delete m_games[i];
            m_games[i] = NULL;
        }

        GameState *loadedGame =
            GameState::ReadGame(static_cast<GameState::GameSaveSlot>(i));
        m_games[i] = loadedGame;

        if (m_games[i] != NULL) {
            m_newCaptBtns[i]->SetEnabled(false);
            m_delCaptBtns[i]->SetEnabled(true);
            m_selCaptBtns[i]->SetEnabled(true);
            displayHelp = false;
        } else {
            m_newCaptBtns[i]->SetEnabled(true);
            m_delCaptBtns[i]->SetEnabled(false);
            m_selCaptBtns[i]->SetEnabled(false);
        }
    }
}
