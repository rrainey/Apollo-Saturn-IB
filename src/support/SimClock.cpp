#include "SimClock.h"
#include "Log.h"
#include "save_state.h"

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

// ---------------------------------------------------------------------
// Save-state (dskymatic-studio)
// ---------------------------------------------------------------------

static constexpr uint32_t kSimClockFormatVersion = 1;

void SimClock::SerializeState(dskymatic::BinaryWriter &w) const {
    w.write_u32(kSimClockFormatVersion);
    // m_dtOrigin_utc is a sim::Date which is a double-valued Julian/MJD
    // representation.  We serialize the double itself through a cast to
    // DATE (the Date class's primitive).
    w.write_f64((double)(DATE)m_dtOrigin_utc);
    w.write_f64(m_dTimeOffset_sec);
    w.write_f64(m_dCurrentTime_sec);
    w.write_f64(m_dLastTime_sec);
    w.write_bool(m_bRunning);
}

bool SimClock::DeserializeState(dskymatic::BinaryReader &r) {
    uint32_t v = r.read_u32();
    if (v != kSimClockFormatVersion) {
        LogError("SimClock::DeserializeState: version mismatch\n");
        return false;
    }
    m_dtOrigin_utc     = Date((DATE)r.read_f64());
    m_dTimeOffset_sec  = r.read_f64();
    (void)r.read_f64();  // saved m_dCurrentTime_sec (wall); see below
    (void)r.read_f64();  // saved m_dLastTime_sec   (wall); see below
    m_bRunning         = r.read_bool();

    // The wall-clock pair is persisted for diagnostics/completeness but
    // not restored verbatim: on resume there may be hours between the
    // save's wall time and now, and ClockTimeUpdate would then advance
    // sim time by that whole delta.  Zeroing m_dLastTime_sec triggers
    // the "first update = 0.05 s" special case in ClockTimeUpdate, so
    // pacing resumes from wall-clock *now* and sim-elapsed stays at
    // whatever was in m_dTimeOffset_sec.
    m_dCurrentTime_sec = 0.0;
    m_dLastTime_sec    = 0.0;
    return r.ok();
}

}  // namespace sim
