/*
        STARFLIGHT - THE LOST COLONY
        ModuleBank.cpp - The Bank module.
        Author: Keith "Daikaze" Patch
        Date: ?-?-2007
*/

#include "ModuleBank.h"
#include "AudioSystem.h"
#include "ModeMgr.h"
#include "QuestMgr.h"
#include "bank_resources.h"

using namespace bank_resources;
using namespace std;

ALLEGRO_DEBUG_CHANNEL("ModuleBank")

const int EXITBTN_X = 50;
const int EXITBTN_Y = 688;

const int INFO_FONT_SIZE = 18;
const int INFO_OUTPUT_X = 190;
const int INFO_OUTPUT_Y = 190;

// square buttons, so this counts for both Height & Width
const int CALC_BTN_SIZE = 62;
const int CALC_PADDING_X = CALC_BTN_SIZE + 20;
const int CALC_PADDING_Y = CALC_BTN_SIZE + 10;
const int CALC_LEFT_X = 635;
const int CALC_TOP_Y = 279;
const int CALC_TEXT_W = 270;
const int CALC_TEXT_H = 32;

const int CONFIRM_BTN_X = 700;
const int CONFIRM_BTN_Y = 255;

const int TAKE_BTN_X = 750;
const int TAKE_BTN_Y = 175;

const int PAY_BTN_X = 630;
const int PAY_BTN_Y = 175;

const int BANK_BANNER_X = 92;
const int BANK_BANNER_Y = 0;

/* const unsigned int MAX_DIGITS = 5; */

const int CALC_TEXT_X = 620;
const int CALC_TEXT_Y = 214;

const int HELP_BTN_X = 952;
const int HELP_BTN_Y = 10;
const int HELP_WINDOW_X = 338;
const int HELP_WINDOW_Y = 160;

ModuleBank::ModuleBank()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), resources(BANK_IMAGES) {
    m_background = make_shared<Bitmap>(resources[I_BANK_BACKGROUND]);
    add_child_module(m_background);
    m_banner = make_shared<Bitmap>(
        resources[I_BANK_BANNER], BANK_BANNER_X, BANK_BANNER_Y);
    add_child_module(m_banner);

    m_keypad_display = make_shared<NumericTextEntry>(
        g_game->font22,
        WHITE,
        "TAKE: ",
        6,
        Loan::MAX_LOAN,
        CALC_TEXT_X,
        CALC_TEXT_Y,
        CALC_TEXT_W,
        CALC_TEXT_H);
    add_child_module(m_keypad_display);

    g_game->audioSystem->Load("data/cantina/buttonclick.ogg", S_BUTTONCLICK);

    m_exit_button = make_shared<NewButton>(
        EXITBTN_X,
        EXITBTN_Y,
        EVENT_NONE,
        EVENT_BANK_EXIT,
        resources[I_BANK_BUTTON_EXIT],
        resources[I_BANK_BUTTON_EXIT_HOVER],
        nullptr,
        S_BUTTONCLICK);
    add_child_module(m_exit_button);

    m_help_button = make_shared<NewButton>(
        HELP_BTN_X,
        HELP_BTN_Y,
        EVENT_NONE,
        EVENT_BANK_HELP,
        resources[I_BANK_BUTTON_HELP],
        resources[I_BANK_BUTTON_HELP_HOVER],
        nullptr,
        S_BUTTONCLICK);
    add_child_module(m_help_button);

    m_confirm_button = make_shared<TextButton>(
        string("Confirm"),
        g_game->font10,
        WHITE,
        ALLEGRO_ALIGN_CENTER,
        CONFIRM_BTN_X,
        CONFIRM_BTN_Y,
        EVENT_NONE,
        EVENT_BANK_CONFIRM,
        resources[I_BANK_BUTTON_CONFIRM_NORMAL],
        resources[I_BANK_BUTTON_CONFIRM_HOVER],
        nullptr,
        S_BUTTONCLICK);
    add_child_module(m_confirm_button);

    m_pay_button = make_shared<TextButton>(
        "Pay",
        g_game->font10,
        WHITE,
        ALLEGRO_ALIGN_CENTER,
        PAY_BTN_X,
        PAY_BTN_Y,
        EVENT_NONE,
        EVENT_BANK_PAY,
        resources[I_BANK_BUTTON_PAY_NORMAL],
        resources[I_BANK_BUTTON_PAY_HOVER],
        nullptr,
        S_BUTTONCLICK);
    add_child_module(m_pay_button);

    m_take_button = make_shared<TextButton>(
        "Take",
        g_game->font10,
        WHITE,
        ALLEGRO_ALIGN_CENTER,
        TAKE_BTN_X,
        TAKE_BTN_Y,
        EVENT_NONE,
        EVENT_BANK_TAKE,
        resources[I_BANK_BUTTON_TAKE_NORMAL],
        resources[I_BANK_BUTTON_TAKE_HOVER],
        nullptr,
        S_BUTTONCLICK);
    add_child_module(m_take_button);

    // Calc layout
    // 7 8 9
    // 4 5 6
    // 1 2 3
    // 0   C
    using calc_button = tuple<string, int, int, EventType>;
    vector<calc_button> calc_button_layout = {
        {"7", 0, 0, EVENT_BANK_SEVEN},
        {"8", 1, 0, EVENT_BANK_EIGHT},
        {"9", 2, 0, EVENT_BANK_NINE},
        {"4", 0, 1, EVENT_BANK_FOUR},
        {"5", 1, 1, EVENT_BANK_FIVE},
        {"6", 2, 1, EVENT_BANK_SIX},
        {"1", 0, 2, EVENT_BANK_ONE},
        {"2", 1, 2, EVENT_BANK_TWO},
        {"3", 2, 2, EVENT_BANK_THREE},
        {"0", 0, 3, EVENT_BANK_ZERO},
        {"Clear", 2, 3, EVENT_BANK_CLEAR},
    };

    for (vector<calc_button>::size_type i = 0; i < calc_button_layout.size();
         i++) {
        string &text = get<0>(calc_button_layout[i]);
        int x_position = get<1>(calc_button_layout[i]);
        int y_position = get<2>(calc_button_layout[i]);
        EventType event = get<3>(calc_button_layout[i]);
        auto button = make_shared<TextButton>(
            text,
            g_game->font10,
            GREEN,
            ALLEGRO_ALIGN_CENTER,
            CALC_LEFT_X + x_position * CALC_PADDING_X,
            CALC_TOP_Y + y_position * CALC_PADDING_Y,
            EVENT_NONE,
            event,
            resources[I_BANK_CALC_BUTTON_NORMAL],
            resources[I_BANK_CALC_BUTTON_HOVER],
            resources[I_BANK_CALC_BUTTON_DEACTIVATE],
            S_BUTTONCLICK);

        m_calc_buttons.push_back(button);
        add_child_module(button);
    }
    m_help_window = make_shared<ScrollBox::ScrollBox>(
        g_game->font18,
        ScrollBox::SB_TEXT,
        HELP_WINDOW_X + 27,
        HELP_WINDOW_Y + 15,
        287,
        318);
    set_draw_after_children(true);
}

ModuleBank::~ModuleBank(void) { ALLEGRO_DEBUG("ModuleBank Dead\n"); }

bool
ModuleBank::on_init() {
    ALLEGRO_DEBUG("ModuleBank Initialize\n");

    {
        b_help_visible = false;
        if (m_help_window == NULL) {
            return false;
        }

        m_help_window->setLines(32);
        m_help_window->DrawScrollBar(true);
        m_help_window->Write("Bank Help:", LTYELLOW);
        m_help_window->Write("", WHITE);
        m_help_window->Write(
            "Use the 'take' and 'pay' buttons to select "
            "between paying and taking a loan.",
            WHITE);
        m_help_window->Write(
            "By clicking the keys on the keypad you can type in a number for "
            "withdrawal. You can use up to (and including) 6 digits, for a max "
            "value of 999,999.",
            WHITE);
        m_help_window->Write(
            "Use the button, labeled 'confirm', to pay or "
            "take the value listed on the keypad screen.",
            WHITE);
        m_help_window->Write("", WHITE);
        m_help_window->Write("A word of caution:", RED);
        m_help_window->Write(
            "High bank loans have high interest rates. Be "
            "wary of taking loans larger than you need.",
            WHITE);
        m_help_window->Write(
            "A minimum payment must be payed every 7 days. By paying in excess "
            "you can cover additional payments ahead of time.",
            WHITE);
        m_help_window->Write(
            "Failure to pay the loan will result in severe punishment. It is "
            "not recommended that you test the bank's generosity.",
            WHITE);
    }
    m_bWarned = false;
    m_keypad_display->reset();

    // tell questmgr that this module has been entered
    g_game->questMgr->raiseEvent(24);

    return true;
}

bool
ModuleBank::on_update() {
    if (m_loan && m_loan->is_paid_off()) {
        m_loan = nullptr;
    }
    if (m_loan) {
        m_loan->compute_interest();
    }
    return true;
}

bool
ModuleBank::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);

    render_text();

    if (b_help_visible) {
        al_draw_bitmap(
            resources[I_BANK_HELP_WINDOW], HELP_WINDOW_X, HELP_WINDOW_Y, 0);
        m_help_window->Draw(g_game->GetBackBuffer());
    }
    return true;
}

void
ModuleBank::render_text() {
    if (m_considering_pay) {
        m_keypad_display->set_prompt("PAY:");
    } else if (m_considering_take) {
        m_keypad_display->set_prompt("TAKE:");
    }

    al_draw_textf(
        g_game->font12,
        WHITE,
        INFO_OUTPUT_X,
        INFO_OUTPUT_Y,
        0,
        "Date: %s",
        g_game->gameState->stardate.GetFullDateString().c_str());

    al_draw_textf(
        g_game->font18,
        WHITE,
        INFO_OUTPUT_X,
        INFO_OUTPUT_Y + INFO_FONT_SIZE,
        0,
        "Credits: %i",
        g_game->gameState->getCredits());

    if (m_loan) {
        al_draw_textf(
            g_game->font12,
            WHITE,
            INFO_OUTPUT_X,
            INFO_OUTPUT_Y + INFO_FONT_SIZE * 2,
            0,
            "Date Taken: %s",
            m_loan->get_issued_date_string().c_str());

        al_draw_textf(
            g_game->font12,
            WHITE,
            INFO_OUTPUT_X,
            INFO_OUTPUT_Y + INFO_FONT_SIZE * 3,
            0,
            "Amount Owed: %i",
            m_loan->get_amount_owed());

        al_draw_textf(
            g_game->font12,
            WHITE,
            INFO_OUTPUT_X,
            INFO_OUTPUT_Y + INFO_FONT_SIZE * 4,
            0,
            "Interest Rate: %.2f",
            m_loan->get_interest_rate());

        ALLEGRO_COLOR due_color;

        if (m_loan->is_overdue()) {
            due_color = ORANGE;
        } else {
            due_color = WHITE;
        }

        al_draw_textf(
            g_game->font12,
            due_color,
            INFO_OUTPUT_X,
            INFO_OUTPUT_Y + INFO_FONT_SIZE * 6,
            0,
            "Payment: %i",
            m_loan->get_minimum_payment());

        al_draw_textf(
            g_game->font12,
            due_color,
            INFO_OUTPUT_X,
            INFO_OUTPUT_Y + INFO_FONT_SIZE * 7,
            0,
            "Due: %s",
            m_loan->get_due_date_string().c_str());

        if (m_loan->is_overdue()) {
            al_draw_text(
                g_game->font12,
                ORANGE,
                INFO_OUTPUT_X,
                INFO_OUTPUT_Y + INFO_FONT_SIZE * 8,
                0,
                "PAYMENT OVERDUE!");

            if (!m_bWarned) {
                g_game->ShowMessageBoxWindow(
                    "", " - Your payment is overdue! - ", 400, 200);
                m_bWarned = true;
            }
        }
    }
}

bool
ModuleBank::PerformCreditCheck() {
    // prevent player from taking a loan until tutorial missions are completed
    if (g_game->gameState->getActiveQuest() < 20) {
        g_game->ShowMessageBoxWindow(
            "",
            "I'm sorry, but you are not yet authorized to use the bank's loan "
            "system. Please come back after you have starflight experience.",
            400,
            300,
            WHITE);
        return false;
    } else
        return true;
}

bool
ModuleBank::on_event(ALLEGRO_EVENT *event) {
    switch (event->type) {
    case EVENT_NONE:
        break;
    case EVENT_BANK_EXIT:
        g_game->LoadModule(MODULE_STARPORT);
        return false;
    case EVENT_BANK_ZERO:
    case EVENT_BANK_ONE:
    case EVENT_BANK_TWO:
    case EVENT_BANK_THREE:
    case EVENT_BANK_FOUR:
    case EVENT_BANK_FIVE:
    case EVENT_BANK_SIX:
    case EVENT_BANK_SEVEN:
    case EVENT_BANK_EIGHT:
    case EVENT_BANK_NINE:
        {
            int digit = static_cast<int>(event->type)
                        - static_cast<int>(EVENT_BANK_ZERO);
            ALLEGRO_KEYBOARD_EVENT faked_keypress = {
                .type = ALLEGRO_EVENT_KEY_CHAR,
                .keycode = ALLEGRO_KEY_0 + digit,
                .unichar = '0' + digit,
            };
            m_keypad_display->on_key_pressed(&faked_keypress);
            break;
        }
    case EVENT_BANK_CLEAR:
        m_keypad_display->reset();
        break;
    case EVENT_BANK_CONFIRM:
        {
            if (!PerformCreditCheck())
                return true;

            int entered_value = m_keypad_display->get_value();

            if (entered_value != 0) {
                if (m_considering_pay && m_loan) {
                    m_loan->pay(entered_value);
                } else if (m_considering_take && !m_loan) {
                    m_loan = make_shared<Loan>(entered_value);
                }
            }
        }
        break;
    case EVENT_BANK_PAY:
        if (!PerformCreditCheck())
            return true;
        m_considering_pay = true;
        m_considering_take = false;
        m_pay_button->set_active(false);
        m_take_button->set_active(true);
        break;
    case EVENT_BANK_TAKE:
        if (!PerformCreditCheck())
            return true;
        m_considering_take = true;
        m_considering_pay = false;
        m_take_button->set_active(false);
        m_pay_button->set_active(true);
        break;
    case EVENT_BANK_HELP:
        if (b_help_visible) {
            b_help_visible = false;
        } else {
            b_help_visible = true;
        }
        break;
    }
    return true;
}

bool
ModuleBank::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    if (b_help_visible) {
        if (is_mouse_wheel_up(event)) {
            m_help_window->OnMouseWheelUp(x, y);
        } else if (is_mouse_wheel_down(event)) {
            m_help_window->OnMouseWheelDown(x, y);
        } else {
            m_help_window->OnMouseMove(x, y);
        }
    }
    return true;
}

bool
ModuleBank::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;
    int button = event->button - 1;

    if (is_mouse_click(event) && b_help_visible) {
        m_help_window->OnMouseClick(button, x, y);
    }
    if (b_help_visible) {
        m_help_window->OnMouseReleased(button, x, y);
    }
    return true;
}

bool
ModuleBank::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    ALLEGRO_EVENT e = {.type = static_cast<int>(EVENT_NONE)};

    if (!b_help_visible) {
        switch (event->keycode) {
        case ALLEGRO_KEY_PAD_ENTER:
            e.type = EVENT_BANK_CONFIRM;
            g_game->broadcast_event(&e);
            return false;
        }
    }
    return true;
}

bool
ModuleBank::on_mouse_button_down(ALLEGRO_MOUSE_EVENT *event) {
    int button = event->button - 1;
    int x = event->x;
    int y = event->y;

    if (b_help_visible) {
        m_help_window->OnMousePressed(button, x, y);
    }
    return true;
}

bool
ModuleBank::on_close() {
    ALLEGRO_DEBUG("ModuleBank Closing\n");
    return true;
}
