#include "Loan.h"
#include "Game.h"

const int Loan::MAX_LOAN = 100000;
const int Loan::LOAN_BRONZE = 1000;
const int Loan::LOAN_GOLD = 5000;
const int Loan::LOAN_PLATINUM = 9000;

Loan::Loan(int principal) {
    if (principal > MAX_LOAN) {
        principal = MAX_LOAN;
    } else if (principal < 0) {
        principal = 0;
    }
    m_initial_loan = principal;
    m_amount_owed = principal;

    if (principal > 0 && principal < LOAN_BRONZE) {
        m_interest_rate = 0.04;
        m_minimum_payment = 50;
        if (principal < m_minimum_payment) {
            m_minimum_payment = principal;
        }
    } else if (principal >= LOAN_BRONZE && principal < LOAN_GOLD) {
        m_interest_rate = 0.06;
        m_minimum_payment = 150;
    } else if (principal >= LOAN_GOLD && principal < LOAN_PLATINUM) {
        m_interest_rate = 0.08;
        m_minimum_payment = 250;
    } else {
        m_interest_rate = 0.1;
        m_minimum_payment = 350;
    }
    m_due_date = g_game->gameState->stardate;
    m_due_date.add_days(7);

    g_game->gameState->augCredits(principal);
}

bool
Loan::pay(int amount) {
    compute_interest();

    if (g_game->gameState->getCredits() >= amount && amount > 0) {
        if (amount > m_amount_owed) {
            amount = m_amount_owed;
        }
        g_game->gameState->augCredits(-amount);
        m_amount_owed -= amount;

        m_amount_due -= amount;

        if (m_amount_due < 0) {
            m_amount_due = 0;
        }

        if (m_amount_due == 0 || m_amount_owed == 0) {
            m_overdue = false;
            g_game->gameState->player->set_OverdueLoan(false);
        }
    }
    return (m_amount_owed == 0);
}

bool
Loan::is_overdue() {
    compute_interest();

    return m_overdue;
}

bool
Loan::is_paid_off() const {
    return m_amount_owed == 0;
}

std::string
Loan::get_issued_date_string() const {
    return m_date_taken.GetDateString();
}

std::string
Loan::get_due_date_string() const {
    return m_due_date.GetDateString();
}

int
Loan::get_amount_owed() {
    compute_interest();

    return m_amount_owed;
}

int
Loan::get_minimum_payment() {
    compute_interest();

    return m_minimum_payment;
}

float
Loan::get_interest_rate() const {
    return m_interest_rate;
}

void
Loan::compute_interest() {
    auto now = g_game->gameState->stardate;
    auto next_due_date = m_due_date;
    int overdue = -1;
    next_due_date.add_days(7);

    if (now < next_due_date) {
        return;
    }

    while (next_due_date <= now) {
        m_amount_owed += m_amount_owed * m_interest_rate;
        m_amount_due += m_minimum_payment;
        if (m_amount_due > m_amount_owed) {
            m_amount_due = m_amount_owed;
        }
        ++overdue;
        next_due_date.add_days(7);
    }
    if (overdue > 0) {
        m_overdue = true;
    }
}

// vi: ft=cpp
