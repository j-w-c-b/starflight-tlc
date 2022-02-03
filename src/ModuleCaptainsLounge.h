#ifndef MODULECAPTAINSLOUNGE_H
#define MODULECAPTAINSLOUNGE_H
#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <vector>

#include "AudioSystem.h"
#include "Button.h"
#include "GameState.h"
#include "Module.h"
#include "ResourceManager.h"

#define CAPTAINSLOUNGE_NUMSLOTS GameState::GAME_SAVE_SLOT_MAX

class CaptainsLoungeSlot : public Module {
  public:
    CaptainsLoungeSlot(
        GameState::GameSaveSlot slot,
        EventType new_event,
        EventType del_event,
        EventType load_event,
        EventType save_event,
        ResourceManager<ALLEGRO_BITMAP> &resources);
    virtual ~CaptainsLoungeSlot();

    virtual bool on_init() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

    bool is_empty() const {
        return !(m_game_state && m_game_state->m_captainSelected);
    }
    std::string get_captain_name() const;

  private:
    std::shared_ptr<Label> m_name_label;
    std::shared_ptr<Label> m_profession_label;
    std::shared_ptr<NewButton> m_new_button;
    std::shared_ptr<NewButton> m_del_button;
    std::shared_ptr<NewButton> m_load_button;
    std::shared_ptr<NewButton> m_save_button;
    std::shared_ptr<GameState> m_game_state;
    GameState::GameSaveSlot m_slot;
};

class CaptainsLoungeDetails : public Module {
  public:
    explicit CaptainsLoungeDetails(const GameState *state);
    virtual ~CaptainsLoungeDetails() {}

    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    void reset_labels(const GameState *state);

    std::shared_ptr<Label> m_name_label;
    std::shared_ptr<Label> m_profession_label;
    std::shared_ptr<Label> m_stardate_label;
    std::shared_ptr<Label> m_credits_label;
    std::shared_ptr<Label> m_location_label;

    std::shared_ptr<Label> m_ship_label;
    std::shared_ptr<Label> m_ship_cargo_label;
    std::shared_ptr<Label> m_ship_engine_label;
    std::shared_ptr<Label> m_ship_armor_label;
    std::shared_ptr<Label> m_ship_shields_label;
    std::shared_ptr<Label> m_ship_laser_label;
    std::shared_ptr<Label> m_ship_missile_label;

    std::shared_ptr<Label> m_science_title_label;
    std::shared_ptr<Label> m_science_name_label;
    std::shared_ptr<Label> m_navigation_title_label;
    std::shared_ptr<Label> m_navigation_name_label;
    std::shared_ptr<Label> m_tactics_title_label;
    std::shared_ptr<Label> m_tactics_name_label;
    std::shared_ptr<Label> m_engineering_title_label;
    std::shared_ptr<Label> m_engineering_name_label;
    std::shared_ptr<Label> m_communications_title_label;
    std::shared_ptr<Label> m_communications_name_label;
    std::shared_ptr<Label> m_medical_title_label;
    std::shared_ptr<Label> m_medical_name_label;
};

class CaptainsLoungeConfirmationBox : public Module {
  public:
    CaptainsLoungeConfirmationBox(
        const std::string &text,
        GameState::GameSaveSlot slot,
        EventType yes_event,
        EventType no_event,
        ResourceManager<ALLEGRO_BITMAP> &resources);
    virtual ~CaptainsLoungeConfirmationBox();

    GameState::GameSaveSlot get_slot() const { return m_slot; }

  private:
    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<Label> m_details;
    std::shared_ptr<NewButton> m_yes_button;
    std::shared_ptr<NewButton> m_no_button;
    GameState::GameSaveSlot m_slot;
};

class ModuleCaptainsLounge : public Module {
  public:
    ModuleCaptainsLounge();
    virtual ~ModuleCaptainsLounge();
    virtual bool on_init() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    ResourceManager<ALLEGRO_BITMAP> m_resources;
    std::shared_ptr<Bitmap> m_background;

    std::shared_ptr<TextButton> m_back_button;
    std::shared_ptr<TextButton> m_launch_button;
    std::shared_ptr<CaptainsLoungeSlot>
        m_captain_slots[CAPTAINSLOUNGE_NUMSLOTS];
    std::shared_ptr<CaptainsLoungeDetails> m_captain_details;
    std::shared_ptr<Label> m_detail_ui_help;

    GameState::GameSaveSlot m_selected_slot;

    bool m_display_help;
};

#endif
// vi: ft=cpp
