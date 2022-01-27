/*
        STARFLIGHT - THE LOST COLONY
        ModuleBank.cpp - The Bank module.
        Author: Keith "Daikaze" Patch
        Date: ?-?-2007
*/
#include "ModuleBank.h"
#include "AudioSystem.h"
#include "QuestMgr.h"
#include "bank_resources.h"

using namespace bank_resources;

ALLEGRO_DEBUG_CHANNEL("ModuleBank")

const int EXITBTN_X = 50;
const int EXITBTN_Y = 688;

const int INFO_FONT_SIZE = 18;
const int INFO_OUTPUT_X = 190;
const int INFO_OUTPUT_Y = 190;

const int MAX_LOAN = 10000;
const int LOAN_BRONZE = 1000;
const int LOAN_GOLD = 5000;
const int LOAN_PLATINUM = 9000;

const int CALC_BTN_SIZE =
    62; // square buttons, so this counts for both Height & Width
const int CALC_PADDING_X = CALC_BTN_SIZE + 20;
const int CALC_PADDING_Y = CALC_BTN_SIZE + 10;
const int CALC_ZERO_X = 635;
const int CALC_ZERO_Y = 495;
const int CALC_OUT_X = 850;
const int CALC_OUT_Y = 215;

const int CONFIRM_BTN_X = 700;
const int CONFIRM_BTN_Y = 255;

const int TAKE_BTN_X = 750;
const int TAKE_BTN_Y = 175;

const int PAY_BTN_X = 630;
const int PAY_BTN_Y = 175;

const int BANK_BANNER_X = 92;
const int BANK_BANNER_Y = 0;

const unsigned int MAX_DIGITS = 5;

const int CALC_TEXT_X = 620;
const int CALC_TEXT_Y = 214;

const int HELP_BTN_X = 952;
const int HELP_BTN_Y = 10;
const int HELP_WINDOW_X = 338;
const int HELP_WINDOW_Y = 160;

ModuleBank::ModuleBank()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), resources(BANK_IMAGES) {
    i_max_loan = 0;
    i_amount_owed = 0;
    i_time_lapsed = 0;
    b_has_loan = false;
    i_last_time = -1;
    f_interest_rate = 0.0f;
    i_original_loan = 0;
    i_minimum_payment = 0;
}

ModuleBank::~ModuleBank(void) { ALLEGRO_DEBUG("ModuleBank Dead\n"); }

bool
ModuleBank::on_init() {
    ALLEGRO_DEBUG("ModuleBank Initialize\n");

    if (!resources.load()) {
        g_game->message("Bank: Error loading resources");
        return false;
    }

    if (i_last_time == -1) { // it hasn't been initialized
        i_last_time = g_game->gameState->stardate.get_current_date_in_days();
    }

    if (!init_buttons()) {
        return false;
    }

    {
        b_help_visible = false;
        m_help_window = new ScrollBox::ScrollBox(
            g_game->font18,
            ScrollBox::SB_TEXT,
            HELP_WINDOW_X + 27,
            HELP_WINDOW_Y + 15,
            287,
            318);
        if (m_help_window == NULL) {
            return false;
        }

        m_help_window->setLines(32);
        m_help_window->DrawScrollBar(true);
        m_help_window->Write("Bank Help:", al_map_rgb(255, 255, 0));
        m_help_window->Write("", al_map_rgb(255, 255, 255));
        m_help_window->Write(
            "Use the 'take' and 'pay' buttons to select "
            "between paying and taking a loan.",
            al_map_rgb(255, 255, 255));
        m_help_window->Write(
            "By clicking the keys on the keypad you can type in a number for "
            "withdrawal. You can use up to (and including) 6 digits, for a max "
            "value of 999,999.",
            al_map_rgb(255, 255, 255));
        m_help_window->Write(
            "Use the button, labeled 'confirm', to pay or "
            "take the value listed on the keypad screen.",
            al_map_rgb(255, 255, 255));
        m_help_window->Write("", al_map_rgb(255, 255, 255));
        m_help_window->Write("A word of caution:", al_map_rgb(255, 0, 0));
        m_help_window->Write(
            "High bank loans have high interest rates. Be "
            "wary of taking loans larger than you need.",
            al_map_rgb(255, 255, 255));
        m_help_window->Write(
            "A minimum payment must be payed every 7 days. By paying in excess "
            "you can cover additional payments ahead of time.",
            al_map_rgb(255, 255, 255));
        m_help_window->Write(
            "Failure to pay the loan will result in severe punishment. It is "
            "not recommended that you test the bank's generosity.",
            al_map_rgb(255, 255, 255));
    }
    m_bWarned = false;
    digit_list.clear();

    // tell questmgr that this module has been entered
    g_game->questMgr->raiseEvent(24);

    return true;
}

bool
ModuleBank::init_buttons() {
    ALLEGRO_BITMAP *imgNorm, *imgOver, *imgDis;
    g_game->audioSystem->Load("data/cantina/buttonclick.ogg", "click");

    imgNorm = resources[I_BANK_BUTTON_EXIT];
    imgOver = resources[I_BANK_BUTTON_EXIT_HOVER];
    exit_button = new Button( // exit button
        imgNorm,
        imgOver,
        NULL,
        EXITBTN_X,
        EXITBTN_Y,
        0,
        EVENT_BANK_EXIT,
        g_game->font10,
        "",
        al_map_rgb(255, 255, 255),
        "click");
    if (exit_button) {
        if (!exit_button->IsInitialized()) {
            return false;
        }
    } else {
        return false;
    }

    imgNorm = resources[I_BANK_BUTTON_HELP];
    imgOver = resources[I_BANK_BUTTON_HELP_HOVER];
    help_button = new Button( // help button
        imgNorm,
        imgOver,
        NULL,
        HELP_BTN_X,
        HELP_BTN_Y,
        0,
        EVENT_BANK_HELP,
        g_game->font10,
        "",
        al_map_rgb(255, 255, 255),
        "click");
    if (help_button) {
        if (!help_button->IsInitialized()) {
            return false;
        }
    } else {
        return false;
    }

    imgNorm = resources[I_BANK_BUTTON_CONFIRM_NORMAL];
    imgOver = resources[I_BANK_BUTTON_CONFIRM_HOVER];
    confirm_button = new Button( // confirm button
        imgNorm,
        imgOver,
        NULL,
        CONFIRM_BTN_X,
        CONFIRM_BTN_Y,
        0,
        EVENT_BANK_CONFIRM,
        g_game->font10,
        "Confirm",
        al_map_rgb(255, 255, 255),
        "click");
    if (confirm_button) {
        if (!confirm_button->IsInitialized()) {
            return false;
        }
    } else {
        return false;
    }

    imgNorm = resources[I_BANK_BUTTON_PAY_NORMAL];
    imgOver = resources[I_BANK_BUTTON_PAY_HOVER];
    pay_button = new Button( // pay button
        imgNorm,
        imgOver,
        NULL,
        PAY_BTN_X,
        PAY_BTN_Y,
        0,
        EVENT_BANK_PAY,
        g_game->font10,
        "Pay",
        al_map_rgb(255, 255, 255),
        "click");
    if (pay_button) {
        if (!pay_button->IsInitialized()) {
            return false;
        }
    } else {
        return false;
    }

    imgNorm = resources[I_BANK_BUTTON_TAKE_NORMAL];
    imgOver = resources[I_BANK_BUTTON_TAKE_HOVER];
    take_button = new Button( // take button
        imgNorm,
        imgOver,
        NULL,
        TAKE_BTN_X,
        TAKE_BTN_Y,
        0,
        EVENT_BANK_TAKE,
        g_game->font10,
        "Take",
        al_map_rgb(255, 255, 255),
        "click");
    if (take_button) {
        if (!take_button->IsInitialized()) {
            return false;
        }
    } else {
        return false;
    }

    imgNorm = resources[I_BANK_CALC_BUTTON_NORMAL];
    imgOver = resources[I_BANK_CALC_BUTTON_HOVER];
    imgDis = resources[I_BANK_CALC_BUTTON_DEACTIVATE];
    calc_buttons[0] = new Button(
        imgNorm,
        imgOver,
        imgDis,
        /*345, 572,*/ CALC_ZERO_X,
        CALC_ZERO_Y,
        0,
        EVENT_BANK_ZERO,
        g_game->font22,
        "0",
        al_map_rgb(0, 255, 0),
        "click");

    calc_buttons[1] = new Button(
        imgNorm,
        imgOver,
        imgDis,
        /*345, 500,*/ CALC_ZERO_X,
        CALC_ZERO_Y - CALC_PADDING_Y,
        0,
        EVENT_BANK_ONE,
        g_game->font22,
        "1",
        al_map_rgb(0, 255, 0),
        "click");

    calc_buttons[2] = new Button(
        imgNorm,
        imgOver,
        imgDis,
        /*427, 500,*/ CALC_ZERO_X + (CALC_PADDING_X),
        CALC_ZERO_Y - (CALC_PADDING_Y),
        0,
        EVENT_BANK_TWO,
        g_game->font22,
        "2",
        al_map_rgb(0, 255, 0),
        "click");

    calc_buttons[3] = new Button(
        imgNorm,
        imgOver,
        imgDis,
        /*509, 500,*/ CALC_ZERO_X + (CALC_PADDING_X)*2,
        CALC_ZERO_Y - (CALC_PADDING_Y),
        0,
        EVENT_BANK_THREE,
        g_game->font22,
        "3",
        al_map_rgb(0, 255, 0),
        "click");

    calc_buttons[4] = new Button(
        imgNorm,
        imgOver,
        imgDis,
        /*345, 428,*/ CALC_ZERO_X,
        CALC_ZERO_Y - (CALC_PADDING_Y)*2,
        0,
        EVENT_BANK_FOUR,
        g_game->font22,
        "4",
        al_map_rgb(0, 255, 0),
        "click");

    calc_buttons[5] = new Button(
        imgNorm,
        imgOver,
        imgDis,
        /*427, 428,*/ CALC_ZERO_X + (CALC_PADDING_X),
        CALC_ZERO_Y - (CALC_PADDING_Y)*2,
        0,
        EVENT_BANK_FIVE,
        g_game->font22,
        "5",
        al_map_rgb(0, 255, 0),
        "click");

    calc_buttons[6] = new Button(
        imgNorm,
        imgOver,
        imgDis,
        /*509, 428,*/ CALC_ZERO_X + (CALC_PADDING_X)*2,
        CALC_ZERO_Y - (CALC_PADDING_Y)*2,
        0,
        EVENT_BANK_SIX,
        g_game->font22,
        "6",
        al_map_rgb(0, 255, 0),
        "click");

    calc_buttons[7] = new Button(
        imgNorm,
        imgOver,
        imgDis,
        /*345, 356,*/ CALC_ZERO_X,
        CALC_ZERO_Y - (CALC_PADDING_Y)*3,
        0,
        EVENT_BANK_SEVEN,
        g_game->font22,
        "7",
        al_map_rgb(0, 255, 0),
        "click");

    calc_buttons[8] = new Button(
        imgNorm,
        imgOver,
        imgDis,
        /*427, 356,*/ CALC_ZERO_X + (CALC_PADDING_X),
        CALC_ZERO_Y - (CALC_PADDING_Y)*3,
        0,
        EVENT_BANK_EIGHT,
        g_game->font22,
        "8",
        al_map_rgb(0, 255, 0),
        "click");

    calc_buttons[9] = new Button(
        imgNorm,
        imgOver,
        imgDis,
        /*509, 356,*/ CALC_ZERO_X + (CALC_PADDING_X)*2,
        CALC_ZERO_Y - (CALC_PADDING_Y)*3,
        0,
        EVENT_BANK_NINE,
        g_game->font22,
        "9",
        al_map_rgb(0, 255, 0),
        "click");

    calc_buttons[10] = new Button(
        imgNorm,
        imgOver,
        imgDis,
        /*509, 572,*/ CALC_ZERO_X + (CALC_PADDING_X)*2,
        CALC_ZERO_Y,
        0,
        EVENT_BANK_CLEAR,
        g_game->font22,
        "Clear",
        al_map_rgb(0, 255, 0),
        "click");

    for (int i = 0; i < NUM_CALC_BUTTONS; i++) {
        if (calc_buttons[i]) {
            if (!calc_buttons[i]->IsInitialized()) {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}

bool
ModuleBank::on_update() {
    if (b_has_loan) {
        int i_current_time =
            g_game->gameState->stardate.get_current_date_in_days();

        if (i_current_time > i_last_time) {
            i_time_lapsed = i_current_time - i_last_time;
            i_last_time = i_current_time;
            i_amount_owed +=
                i_original_loan * ((int)(f_interest_rate * i_time_lapsed));
        }
    }

    if (i_amount_owed == 0) {
        i_original_loan = 0;
        b_has_loan = false;
        if (g_game->gameState->player->hasHyperspacePermit() == false) {
            g_game->gameState->player->set_HyperspacePermit(true);
        }
    } else {
        b_has_loan = true;
    }
    return false;
}

bool
ModuleBank::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);

    render_images();
    render_text();

    if (b_help_visible) {
        al_draw_bitmap(
            resources[I_BANK_HELP_WINDOW], HELP_WINDOW_X, HELP_WINDOW_Y, 0);
        m_help_window->Draw(g_game->GetBackBuffer());
    }
    return true;
}

void
ModuleBank::render_images() {
    al_draw_bitmap(resources[I_BANK_BACKGROUND], 0, 0, 0);
    al_draw_bitmap(
        resources[I_BANK_BANNER],
        BANK_BANNER_X,
        BANK_BANNER_Y,
        0); // render background
    exit_button->Run(g_game->GetBackBuffer());
    help_button->Run(g_game->GetBackBuffer());
    confirm_button->Run(g_game->GetBackBuffer());
    pay_button->Run(g_game->GetBackBuffer());
    take_button->Run(g_game->GetBackBuffer());
    for (int i = 0; i < NUM_CALC_BUTTONS; i++) {
        calc_buttons[i]->Run(g_game->GetBackBuffer());
    }
}

void
ModuleBank::render_text() {
    char c_output[256];

    if (b_considering_pay) {
        sprintf(c_output, "PAY:");
        al_draw_text(
            g_game->font22,
            al_map_rgb(255, 255, 255),
            CALC_TEXT_X,
            CALC_TEXT_Y,
            0,
            c_output);
    } else if (b_considering_take) {
        sprintf(c_output, "TAKE:");
        al_draw_text(
            g_game->font22,
            al_map_rgb(255, 255, 255),
            CALC_TEXT_X,
            CALC_TEXT_Y,
            0,
            c_output);
    }
    int x = CALC_OUT_X, y = CALC_OUT_Y;
    if (!digit_list.empty()) {
        int total = 0;
        int digit = 1;
        for (auto i : digit_list) {
            total += i * digit;
            sprintf(c_output, "%i", i);
            al_draw_text(
                g_game->font22, al_map_rgb(255, 255, 255), x, y, 0, c_output);
            x -= 16;
            digit *= 10;
        }
        if (total > MAX_LOAN) {
            digit_list.clear();
            digit_list.push_front(1);
            for (unsigned int i = 1; i < MAX_DIGITS; i++) {
                digit_list.push_front(0);
            }
        }
    } else {
        sprintf(c_output, "0");
        al_draw_text(
            g_game->font22, al_map_rgb(255, 255, 255), x, y, 0, c_output);
    }

    sprintf(
        c_output,
        "Date: %s",
        g_game->gameState->stardate.GetFullDateString()
            .c_str()); // display date
    al_draw_text(
        g_game->font12,
        al_map_rgb(255, 255, 255),
        INFO_OUTPUT_X,
        INFO_OUTPUT_Y,
        0,
        c_output);

    sprintf(
        c_output,
        "Credits: %i",
        g_game->gameState->getCredits()); // display credits
    al_draw_text(
        g_game->font18,
        al_map_rgb(255, 255, 255),
        INFO_OUTPUT_X,
        INFO_OUTPUT_Y + INFO_FONT_SIZE,
        0,
        c_output);

    if (this->b_has_loan == true) { // does the player have a loan?
        sprintf(c_output, "Date Taken: %s", date_taken.GetDateString().c_str());
        al_draw_text(
            g_game->font12,
            al_map_rgb(255, 255, 255),
            INFO_OUTPUT_X,
            INFO_OUTPUT_Y + INFO_FONT_SIZE * 2,
            0,
            c_output);

        sprintf(c_output, "Amount Owed: %i", i_amount_owed);
        al_draw_text(
            g_game->font12,
            al_map_rgb(255, 255, 255),
            INFO_OUTPUT_X,
            INFO_OUTPUT_Y + INFO_FONT_SIZE * 3,
            0,
            c_output);

        if (is_overdue()) {
            sprintf(c_output, "Payment: %i", i_minimum_payment);
            al_draw_text(
                g_game->font12,
                ORANGE,
                INFO_OUTPUT_X,
                INFO_OUTPUT_Y + INFO_FONT_SIZE * 6,
                0,
                c_output);

            sprintf(c_output, "Due: %s", m_due_date.GetDateString().c_str());
            al_draw_text(
                g_game->font12,
                ORANGE,
                INFO_OUTPUT_X,
                INFO_OUTPUT_Y + INFO_FONT_SIZE * 7,
                0,
                c_output);

            sprintf(c_output, "PAYMENT OVERDUE!");
            al_draw_text(
                g_game->font12,
                ORANGE,
                INFO_OUTPUT_X,
                INFO_OUTPUT_Y + INFO_FONT_SIZE * 8,
                0,
                c_output);

            if (!m_bWarned) {
                g_game->ShowMessageBoxWindow(
                    "", " - Your payment is overdue! - ", 400, 200);
                m_bWarned = true;
            }

        } else {
            sprintf(c_output, "Payment: %i", i_minimum_payment);
            al_draw_text(
                g_game->font12,
                al_map_rgb(255, 255, 255),
                INFO_OUTPUT_X,
                INFO_OUTPUT_Y + INFO_FONT_SIZE * 6,
                0,
                c_output);

            sprintf(c_output, "Due: %s", m_due_date.GetDateString().c_str());
            al_draw_text(
                g_game->font12,
                al_map_rgb(255, 255, 255),
                INFO_OUTPUT_X,
                INFO_OUTPUT_Y + INFO_FONT_SIZE * 7,
                0,
                c_output);
        }

        sprintf(c_output, "Interest Rate: %.2f", f_interest_rate);
        al_draw_text(
            g_game->font12,
            al_map_rgb(255, 255, 255),
            INFO_OUTPUT_X,
            INFO_OUTPUT_Y + INFO_FONT_SIZE * 4,
            0,
            c_output);
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
        if (!digit_list.empty()) {
            push_digit(0);
        }
        break;
    case EVENT_BANK_ONE:
        push_digit(1);
        break;
    case EVENT_BANK_TWO:
        push_digit(2);
        break;
    case EVENT_BANK_THREE:
        push_digit(3);
        break;
    case EVENT_BANK_FOUR:
        push_digit(4);
        break;
    case EVENT_BANK_FIVE:
        push_digit(5);
        break;
    case EVENT_BANK_SIX:
        push_digit(6);
        break;
    case EVENT_BANK_SEVEN:
        push_digit(7);
        break;
    case EVENT_BANK_EIGHT:
        push_digit(8);
        break;
    case EVENT_BANK_NINE:
        push_digit(9);
        break;
    case EVENT_BANK_CLEAR:
        digit_list.clear();
        break;
    case EVENT_BANK_CONFIRM:

        if (!PerformCreditCheck())
            return true;

        if (!digit_list.empty()) {
            if (b_considering_pay && b_has_loan) {
                pay_loan();
            } else if (b_considering_take && !b_has_loan) {
                take_loan();
            }
        }
        break;
    case EVENT_BANK_PAY:
        if (!PerformCreditCheck())
            return true;
        b_considering_pay = true;
        b_considering_take = false;
        pay_button->SetEnabled(false);
        take_button->SetEnabled(true);
        break;
    case EVENT_BANK_TAKE:
        if (!PerformCreditCheck())
            return true;
        b_considering_take = true;
        b_considering_pay = false;
        take_button->SetEnabled(false);
        pay_button->SetEnabled(true);
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

void
ModuleBank::push_digit(int value) {
    if (digit_list.size() < MAX_DIGITS) {
        digit_list.push_front(value);
    }
}

void
ModuleBank::take_loan() {
    int total = 0, digit = 1;
    for (auto i : digit_list) {
        total += i * digit;
        digit *= 10;
    }
    i_amount_owed = i_original_loan = total;

    if (total > 0 && total < LOAN_BRONZE) {
        this->f_interest_rate = 0.04;
        i_minimum_payment = 50;
        if (total < i_minimum_payment) {
            i_minimum_payment = total;
        }
    } else if (total >= LOAN_BRONZE && total < LOAN_GOLD) {
        this->f_interest_rate = 0.06;
        i_minimum_payment = 150;
    } else if (total >= LOAN_GOLD && total < LOAN_PLATINUM) {
        this->f_interest_rate = 0.08;
        i_minimum_payment = 250;
    } else if (total >= LOAN_PLATINUM) {
        this->f_interest_rate = 0.1;
        i_minimum_payment = 350;
    }
    m_due_date = g_game->gameState->stardate;
    m_due_date.add_days(7);

    g_game->gameState->augCredits(total);
    digit_list.clear();
}

void
ModuleBank::pay_loan() {
    int total = 0, digit = 1;
    for (std::list<int>::iterator i = digit_list.begin(); i != digit_list.end();
         i++, digit *= 10) {
        total += (*i) * digit;
    }
    if (g_game->gameState->getCredits() >= total && total > 0) {
        if (total > i_amount_owed) {
            total = i_amount_owed;
        }
        g_game->gameState->augCredits(-total);
        this->i_amount_owed -= total;

        i_minimum_payment -= total;

        if (i_amount_owed > 0 && i_minimum_payment <= 0) {
            if (i_original_loan > 0 && i_original_loan < LOAN_BRONZE) {
                i_minimum_payment = 50;
                if (i_amount_owed < i_minimum_payment) {
                    i_minimum_payment = i_amount_owed;
                }
            } else if (i_original_loan >= LOAN_BRONZE && total < LOAN_GOLD) {
                i_minimum_payment = 150;
            } else if (i_original_loan >= LOAN_GOLD && total < LOAN_PLATINUM) {
                i_minimum_payment = 250;
            } else if (i_original_loan >= LOAN_PLATINUM) {
                i_minimum_payment = 350;
            }

            m_due_date.add_days(7);
            if (g_game->gameState->player->hasOverdueLoan() == true) {
                g_game->gameState->player->set_OverdueLoan(false);
            }
        }
        if (i_amount_owed == 0) {
            m_due_date = g_game->gameState->stardate;
            if (g_game->gameState->player->hasOverdueLoan() == true) {
                g_game->gameState->player->set_OverdueLoan(false);
            }
        }
        digit_list.clear();
    }
}

bool
ModuleBank::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    int x = event->x;
    int y = event->y;

    help_button->OnMouseMove(x, y);
    exit_button->OnMouseMove(x, y);

    if (b_help_visible) {
        if (is_mouse_wheel_up(event)) {
            m_help_window->OnMouseWheelUp(x, y);
        } else if (is_mouse_wheel_down(event)) {
            m_help_window->OnMouseWheelDown(x, y);
        } else {
            m_help_window->OnMouseMove(x, y);
        }
    } else {
        confirm_button->OnMouseMove(x, y);
        pay_button->OnMouseMove(x, y);
        take_button->OnMouseMove(x, y);
        for (int i = 0; i < NUM_CALC_BUTTONS; i++) {
            calc_buttons[i]->OnMouseMove(x, y);
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
    if (exit_button->OnMouseReleased(button, x, y)) {
        return false;
    }
    if (help_button->OnMouseReleased(button, x, y)) {
        return false;
    }
    if (b_help_visible) {
        m_help_window->OnMouseReleased(button, x, y);
    } else {
        if (confirm_button->OnMouseReleased(button, x, y)) {
            return false;
        }
        if (pay_button->OnMouseReleased(button, x, y)) {
            return false;
        }
        if (take_button->OnMouseReleased(button, x, y)) {
            return false;
        }
        for (int i = 0; i < NUM_CALC_BUTTONS; i++) {
            if (calc_buttons[i]->OnMouseReleased(button, x, y)) {
                return false;
            }
        }
    }
    return true;
}

bool
ModuleBank::on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) {
    ALLEGRO_EVENT e = {.type = static_cast<int>(EVENT_NONE)};

    if (!b_help_visible) {
        switch (event->unichar) {
        case '0':
            e.type = EVENT_BANK_ZERO;
            g_game->broadcast_event(&e);
            break;
        case '1':
            e.type = EVENT_BANK_ONE;
            g_game->broadcast_event(&e);
            break;
        case '2':
            e.type = EVENT_BANK_TWO;
            g_game->broadcast_event(&e);
            break;
        case '3':
            e.type = EVENT_BANK_THREE;
            g_game->broadcast_event(&e);
            break;
        case '4':
            e.type = EVENT_BANK_FOUR;
            g_game->broadcast_event(&e);
            break;
        case '5':
            e.type = EVENT_BANK_FIVE;
            g_game->broadcast_event(&e);
            break;
        case '6':
            e.type = EVENT_BANK_SIX;
            g_game->broadcast_event(&e);
            break;
        case '7':
            e.type = EVENT_BANK_SEVEN;
            g_game->broadcast_event(&e);
            break;
        case '8':
            e.type = EVENT_BANK_EIGHT;
            g_game->broadcast_event(&e);
            break;
        case '9':
            e.type = EVENT_BANK_NINE;
            g_game->broadcast_event(&e);
            break;
        case 0:
            switch (event->keycode) {
            case ALLEGRO_KEY_PAD_ENTER:
                e.type = EVENT_BANK_CONFIRM;
                g_game->broadcast_event(&e);
                break;
            case ALLEGRO_KEY_DELETE:
            case ALLEGRO_KEY_PAD_DELETE:
                e.type = EVENT_BANK_CLEAR;
                g_game->broadcast_event(&e);
                break;
            case ALLEGRO_KEY_BACKSPACE:
                if (!digit_list.empty()) {
                    digit_list.pop_front();
                }
            }
            break;
        }
    }
    return false;
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

    if (m_help_window != NULL) {
        delete m_help_window;
        m_help_window = NULL;
    }
    if (exit_button != NULL) {
        exit_button->Destroy();
        exit_button = NULL;
    }
    if (help_button != NULL) {
        help_button->Destroy();
        help_button = NULL;
    }
    if (confirm_button != NULL) {
        confirm_button->Destroy();
        confirm_button = NULL;
    }
    if (pay_button != NULL) {
        pay_button->Destroy();
        pay_button = NULL;
    }
    if (take_button != NULL) {
        take_button->Destroy();
        take_button = NULL;
    }
    for (int i = 0; i < NUM_CALC_BUTTONS; i++) {
        if (calc_buttons[i] != NULL) {
            calc_buttons[i]->Destroy();
            calc_buttons[i] = NULL;
        }
    }

    // unload the data file
    resources.unload();
    return true;
}

bool
ModuleBank::is_overdue() {
    if (g_game->gameState->stardate.get_current_date_in_days()
        > m_due_date.get_current_date_in_days()) {
        if (g_game->gameState->player->hasOverdueLoan() == false) {
            g_game->gameState->player->set_OverdueLoan(true);
        }
        return true;
    }
    return false;
}
