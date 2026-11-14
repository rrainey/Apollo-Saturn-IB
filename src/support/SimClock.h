// Simulation clock.  Holds the simulated epoch and elapsed time and steps
// the registered model, either on a fixed interval (batch) or from the
// wall clock (GUI).

#pragma once

#include "Date.h"

namespace dskymatic { class BinaryWriter; class BinaryReader; }

namespace sim {

class SimModel {
public:
    virtual ~SimModel() = default;
    // Advance the model by dDeltaT_sec.  Return true to be removed from the clock.
    virtual bool TimeStep(double dDeltaT_sec) = 0;
};

class SimClock {
public:
    SimClock();

    void SetCurrentSimulationTime(const Date &dtSimDate_utc);
    Date GetCurrentSimulationTime() const;
    double GetElapsedTime() const { return m_dTimeOffset_sec; }

    // The clock starts frozen; nothing advances until Thaw().
    void Thaw() { m_bRunning = true; }

    void Register(SimModel *pModel) { m_pModel = pModel; }

    void FixedTimeUpdate(double dDeltaT_sec);
    void ClockTimeUpdate();

    /** Binary save-state hooks (dskymatic-studio). */
    void SerializeState  (dskymatic::BinaryWriter &w) const;
    bool DeserializeState(dskymatic::BinaryReader &r);

private:
    void TimeStep(double dDeltaT_sec);

    Date m_dtOrigin_utc;
    double m_dTimeOffset_sec = 0.0;
    double m_dCurrentTime_sec = 0.0;
    double m_dLastTime_sec = 0.0;
    bool m_bRunning = false;
    SimModel *m_pModel = nullptr;
};

// The most recently constructed clock.
SimClock *GetSimClock();

}  // namespace sim
