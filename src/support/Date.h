// Calendar date stored as days since midnight Dec 31, 1899.  Good to dates
// as early as Jan 1, 1583 (the Gregorian calendar starts October 15, 1582).

#pragma once

typedef double DATE;

namespace sim {

class Date {
public:
    enum DateTimeStatus { valid = 0, invalid = 1, nulldate = 2 };

    Date() : m_dt(0.0), m_status(valid) {}
    Date(DATE d) : m_dt(d), m_status(valid) {}

    // nYear: 4-digit year; nMonth 1-12; nDay 1-31; nHour 0-23; nMin 0-59;
    // dSec: seconds, modulo 60.0
    void SetDate(int nYear, int nMonth, int nDay, int nHour, int nMin, double dSec);

    // If the date is invalid or null, no action is taken.
    void GetDate(int *pnYear, int *pnMonth, int *pnDay, int *pnHour, int *pnMin, double *pdSec);

    enum DateTimeStatus GetStatus() const { return m_status; }
    void SetStatus(enum DateTimeStatus s) { m_status = s; }

    operator DATE() const { return m_dt; }

private:
    double m_dt;
    enum DateTimeStatus m_status;
};

}  // namespace sim
