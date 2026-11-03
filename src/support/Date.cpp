#include "Date.h"

#include <cmath>

/**
 * JD for Midnight Dec 31, 1899
 */
#define JD_DATE_ORIGIN 2415018.5

using namespace sim;

void
Date::SetDate (int nYear,
int nMonth,
int nDay,
int nHour,
int nMin,
double dSec)
{
    long A, B, C, D;
    DATE JD_1990;

    if (nYear < 1583 || nYear > 9999 ||
        nMonth < 0 || nMonth > 11 ||
        nDay < 1 || nDay > 31 ||
        nHour < 0 || nHour > 23 ||
        nMin < 0 || nMin > 59 || dSec < 0.0 || dSec >= 60.0 )
    {
        SetStatus (invalid);
    }
    else
    {

        if (nMonth == 1 || nMonth == 2)
        {
            nYear -= 1;
            nMonth += 12;
        }

        A = (long) (nYear / 100);
        B = (long) (2 - A + (int) (A / 4));

        if (nYear < 0)
        {
            C = (long) ((365.25 * nYear) - 0.75);
        }
        else
        {
            C = (long) (365.25 * nYear);
        }

        D = (long) (30.6001 * (nMonth + 1));

        JD_1990 = B + C + D + nDay + 1720994.5;

        m_dt = JD_1990 - JD_DATE_ORIGIN + nHour / 24.0 +
            nMin / (24.0 * 60) + dSec / (24.0 * 60.0 * 60.0);

        SetStatus (valid);
    }
}


void Date::GetDate (int *pnYear, int *pnMonth, int *pnDay,
int *pnHour, int *pnMin, double *pdSec)
{
    if (GetStatus() == valid)
    {

        long Z;
        double F;
        long alpha;
        long A, B, C, D, E;
        double dIntPart;

        F = modf( m_dt + JD_DATE_ORIGIN + 0.5, &dIntPart );
        Z = (long) ( dIntPart );

        if ( Z > 2299160 )
        {
            alpha = (long) ((Z - 1867216.25) / 36524.25 );
            A = Z + 1 + alpha - (long) (alpha / 4 );
        }
        else
        {
            A = Z;
        }

        B = A + 1524;
        C = (long) (( B - 122.1) / 365.25);
        D = (long) (365.25 * C);
        E = (long) (( B - D ) / 30.6001);

        *pnDay = (int) (B - D - (long) (30.6001 * E ) + F);

        if ( E < 14 )
        {
            *pnMonth = E - 1;
        }
        else
        {
            *pnMonth = E - 13;
        }

        if (*pnMonth > 2 )
        {
            *pnYear = C - 4716;
        }
        else
        {
            *pnYear = C - 4715;
        }

        *pnHour = (long) (F * 24.0);
        *pnMin = (long) ( F * 24.0 * 60.0 - *pnHour * 60.0 );
        *pdSec = fmod ( F * 24.0 * 3600.0, 60.0 );
    }
}
