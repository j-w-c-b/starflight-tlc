#include "Loan.h"
#include "Game.h"

using namespace std;

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
    m_overdue = false;

    g_game->gameState->augCredits(principal);
}

Loan::Loan(InputArchive &ar) { ar >> *this; }

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
        }
    }
    return (m_amount_owed == 0);
}

bool
Loan::is_overdue() const {
    auto [amount_owed, amount_due, overdue] = _compute_interest();

    return overdue;
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
Loan::get_amount_owed() const {
    auto [amount_owed, amount_due, overdue] = _compute_interest();

    return amount_owed;
}

int
Loan::get_minimum_payment() {
    compute_interest();

    return m_minimum_payment;
}

int
Loan::get_minimum_payment() const {
    auto [amount_owed, amount_due, overdue] = _compute_interest();

    return amount_due;
}

float
Loan::get_interest_rate() const {
    return m_interest_rate;
}

void
Loan::compute_interest() {
    auto [amount_owed, amount_due, overdue] = _compute_interest();

    m_amount_owed = amount_owed;
    m_amount_due = amount_due;
    m_overdue = overdue;
}

tuple<float, float, bool>
Loan::_compute_interest() const {
    auto now = g_game->gameState->stardate;
    auto next_due_date = m_due_date;
    int overdue = -1;
    float amount_owed = m_amount_owed;
    float amount_due = m_amount_due;
    next_due_date.add_days(7);

    if (now < next_due_date) {
        return {0, 0, false};
    }

    while (next_due_date <= now) {
        amount_owed += amount_owed * m_interest_rate;
        amount_due += m_minimum_payment;
        if (amount_due > amount_owed) {
            amount_due = amount_owed;
        }
        ++overdue;
        next_due_date.add_days(7);
    }
    return {amount_owed, amount_due, overdue > 0};
}

InputArchive
operator>>(InputArchive &ar, Loan &loan) {
    string class_name = string(Loan::class_name);
    int schema = 0;

    string load_class_name;
    ar >> load_class_name;
    if (load_class_name != class_name) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: expected " + class_name + ", got "
                + load_class_name);
    }

    int load_schema;
    ar >> load_schema;
    if (load_schema > schema) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: unknown schema: " + class_name + " version "
                + std::to_string(load_schema));
    }

    ar >> loan.m_initial_loan;
    ar >> loan.m_interest_rate;
    ar >> loan.m_amount_owed;
    ar >> loan.m_date_taken;
    ar >> loan.m_due_date;
    ar >> loan.m_amount_due;
    ar >> loan.m_minimum_payment;
    ar >> loan.m_overdue;

    return ar;
}

OutputArchive
operator<<(OutputArchive &ar, const Loan &loan) {
    string class_name = string(Loan::class_name);
    int schema = 0;

    ar << class_name;
    ar << schema;
    ar << loan.m_initial_loan;
    ar << loan.m_interest_rate;
    ar << loan.m_amount_owed;
    ar << loan.m_date_taken;
    ar << loan.m_due_date;
    ar << loan.m_amount_due;
    ar << loan.m_minimum_payment;
    ar << loan.m_overdue;

    return ar;
}

// vi: ft=cpp
