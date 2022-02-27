/*
        STARFLIGHT - THE LOST COLONY
        Stardate.h - A robust Stardate class.
        Author: nybble
        Date: Jan-29-2007
*/

#ifndef STARDATE_H
#define STARDATE_H

#include <string>

#include "Archive.h"

class Stardate {
  public:
    Stardate();
    Stardate(const Stardate &stardate);
    explicit Stardate(const std::string &fullDateString); // dd.hh-mm-yyyy

    static constexpr std::string_view class_name = "Stardate";

    Stardate &operator=(const Stardate &stardate);
    int GetDay() const;
    int GetHour() const;
    int GetMonth() const;
    int GetYear() const;
    void SetDay(int day);
    void SetHour(int hour);
    void SetMonth(int month);
    void SetYear(int year);
    void SetDate(int day, int hour, int month, int year);
    std::string GetDateString() const;     // dd-mm-yyyy
    std::string GetFullDateString() const; // dd.hh-mm-yyyy

    // the following compare full date strings, including hours
    bool operator==(const Stardate &stardate) const;
    bool operator!=(const Stardate &stardate) const;
    bool operator<(const Stardate &stardate) const;
    bool operator<=(const Stardate &stardate) const;
    bool operator>(const Stardate &stardate) const;
    bool operator>=(const Stardate &stardate) const;

    // updates Stardate based on time since last update
    void Update(double gameTimeInSecs, double update_interval);

    void Reset();

    int get_current_date_in_days() const;
    void add_days(int days);

    int totalHours;
    friend InputArchive &operator>>(InputArchive &ar, Stardate &stardate);
    friend OutputArchive &
    operator<<(OutputArchive &ar, const Stardate &stardate);

  private:
    bool IsValidDate(int day, int month, int year);
    void initHours();

    static const int DEFAULT_DAY = 1;
    static const int DEFAULT_HOUR = 0;
    static const int DEFAULT_MONTH = 1;
    static const int DEFAULT_YEAR = 4622;

    int m_day;
    int m_hour;
    int m_month;
    int m_year;
};

#endif // STARDATE_H
