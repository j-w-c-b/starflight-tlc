#ifndef LOAN_H
#define LOAN_H

#include <string>

#include "Archive.h"
#include "Stardate.h"

class Loan {
  public:
    explicit Loan(int principal);
    explicit Loan(InputArchive &archive);

    static constexpr std::string_view class_name = "Loan";

    // returns true if the loan is paid off entirely
    bool pay(int amount);

    bool is_overdue();
    bool is_overdue() const;
    bool is_paid_off() const;
    std::string get_issued_date_string() const;
    std::string get_due_date_string() const;
    int get_amount_owed();
    int get_amount_owed() const;
    int get_minimum_payment();
    int get_minimum_payment() const;
    float get_interest_rate() const;
    void compute_interest();

    static const int MAX_LOAN;

    friend InputArchive operator>>(InputArchive &ar, Loan &loan);
    friend OutputArchive operator<<(OutputArchive &ar, const Loan &loan);

  private:
    std::tuple<float, float, bool> _compute_interest() const;

    int m_initial_loan;    // value of the loan before interest
    float m_interest_rate; // interest rate of loan
    int m_amount_owed;     // value of loan with interest

    Stardate m_date_taken; // date that the loan was taken
    Stardate m_due_date;   // when is the next payment due
    int m_amount_due;      // amount due at the due date

    int m_minimum_payment; // minimum amount to be paid each period
    bool m_overdue;

    static const int LOAN_BRONZE;
    static const int LOAN_GOLD;
    static const int LOAN_PLATINUM;
};

#endif // LOAN_H
// vi: ft=cpp
