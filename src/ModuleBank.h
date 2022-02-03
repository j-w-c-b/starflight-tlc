/*
        STARFLIGHT - THE LOST COLONY
        ModuleBank.h - The Bank module.
        Author: Keith "Daikaze" Patch
        Date: ?-?-2007
*/
#pragma once

#include <memory>
#include <vector>

#include <allegro5/allegro.h>

#include "Bitmap.h"
#include "Button.h"
#include "Loan.h"
#include "Module.h"
#include "ResourceManager.h"
#include "ScrollBox.h"
#include "Stardate.h"
#include "TextEntry.h"

// bank needs punishment
// bank needs credit scoring and interest rates influenced by credit scores
// bank needs time delay before taking new loan

// score will be based on player purchases with the trade depot, but majorly
// influenced by previous loans.
class ModuleBank : public Module {
  public:
    ModuleBank();
    ~ModuleBank();

    bool on_init() override;
    bool on_update() override;
    bool on_draw(ALLEGRO_BITMAP *target) override;
    bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    bool on_mouse_button_down(ALLEGRO_MOUSE_EVENT *event) override;
    bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    bool on_event(ALLEGRO_EVENT *event) override;
    bool on_close() override;

    void take_loan();
    void pay_loan();
    void render_text();
    void push_digit(int value);
    bool is_overdue();

    bool PerformCreditCheck();

  private:
    ResourceManager<ALLEGRO_BITMAP> resources;
    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<Bitmap> m_banner;

    std::shared_ptr<Loan> m_loan;

    int m_last_time;         // the last time the player visited the bank
    bool m_considering_pay;  // does the player wish to pay the loan?
    bool m_considering_take; // does the player wish to take a loan;

    std::shared_ptr<NewButton> m_exit_button;
    std::shared_ptr<NewButton> m_help_button;
    std::vector<std::shared_ptr<TextButton>> m_calc_buttons;
    std::shared_ptr<TextButton> m_pay_button;
    std::shared_ptr<TextButton> m_take_button;
    std::shared_ptr<TextButton> m_confirm_button;
    std::shared_ptr<NumericTextEntry> m_keypad_display;

    std::shared_ptr<ScrollBox::ScrollBox> m_help_window;
    bool b_help_visible;
    bool m_bWarned;
};
