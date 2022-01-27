#ifndef MODULECAPTAINSLOUNGE_H
#define MODULECAPTAINSLOUNGE_H
#pragma once

#include "AudioSystem.h"
#include "GameState.h"
#include "Module.h"
#include "ResourceManager.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

class Button;

#include <vector>

#define CAPTAINSLOUNGE_NUMSLOTS GameState::GAME_SAVE_SLOT_MAX

class ModuleCaptainsLounge : public Module {
  public:
    ModuleCaptainsLounge(void);
    virtual ~ModuleCaptainsLounge(void);
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

  private:
    ALLEGRO_BITMAP *m_background;
    ALLEGRO_BITMAP *m_modalPromptBackground;

    std::shared_ptr<Sample> m_sndBtnClick;

    Button *m_backBtn;
    Button *m_launchBtn;
    Button *m_newCaptBtns[CAPTAINSLOUNGE_NUMSLOTS];
    Button *m_delCaptBtns[CAPTAINSLOUNGE_NUMSLOTS];
    Button *m_selCaptBtns[CAPTAINSLOUNGE_NUMSLOTS];
    Button *m_saveCaptBtns[CAPTAINSLOUNGE_NUMSLOTS];

    GameState *m_games[CAPTAINSLOUNGE_NUMSLOTS];
    Button *m_yesBtn;
    Button *m_noBtn;

    bool m_requestedCaptainCreation;
    GameState::GameSaveSlot m_requestedCaptainCreationSlotNum;

    bool m_modalPromptActive;
    std::vector<std::string> m_modalPromptStrings;
    int m_modalPromptYesEvent;
    int m_modalPromptNoEvent;
    GameState::GameSaveSlot m_modalPromptSlotNum;

    void LoadGames();

    bool displayHelp;
    // If the player has at least one saved game file, then the tutorial message
    // will not display in ModuleCaptainsLounge when the player goes to load a
    // saved game.
    ResourceManager<ALLEGRO_BITMAP> m_resources;
};

#endif
