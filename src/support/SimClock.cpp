#include "SimClock.h"
#include "Log.h"

#include <sys/time.h>

#define SECtoDAYS(s) ((s) / (24.0 * 3600.0))

namespace sim {

static SimClock *g_pSimClock = nullptr;

SimClock *GetSimClock() { return g_pSimClock; }

SimClock::SimClock() { g_pSimClock = this; }

void SimClock::SetCurrentSimulationTime(const Date &dtSimDate_utc) {
    m_dtOrigin_utc = dtSimDate_utc;
    m_dTimeOffset_sec = 0.0;
}

Date SimClock::GetCurrentSimulationTime() const {
    return Date((DATE)m_dtOrigin_utc + SECtoDAYS(m_dTimeOffset_sec));
}

void SimClock::FixedTimeUpdate(double dDeltaT_sec) {
    if (dDeltaT_sec >= 0.0) TimeStep(dDeltaT_sec);
}

void SimClock::ClockTimeUpdate() {
    struct timeval tp;
    gettimeofday(&tp, nullptr);
    const double dCurTime_sec = (double)tp.tv_sec + (double)tp.tv_usec / 1000000.0;

    // The first update is treated as one-twentieth of a second.
    if (m_dLastTime_sec == 0.0) {
        m_dLastTime_sec = dCurTime_sec - 0.05;
    } else {
        m_dLastTime_sec = m_dCurrentTime_sec;
    }

    if (dCurTime_sec == m_dLastTime_sec) {
        LogError("ClockTimeUpdate() was called too rapidly; ignored\n");
        return;
    }

    m_dCurrentTime_sec = dCurTime_sec;

    // The step may be large; the model catches up in smaller increments.
    TimeStep(m_dCurrentTime_sec - m_dLastTime_sec);
}

void SimClock::TimeStep(double dDeltaT_sec) {
    if (!m_bRunning) return;

    // Advance time first: the model sees the time at the end of this step.
    m_dTimeOffset_sec += dDeltaT_sec;

    if (m_pModel && m_pModel->TimeStep(dDeltaT_sec)) {
        m_pModel = nullptr;
    }
}

}  // namespace sim
