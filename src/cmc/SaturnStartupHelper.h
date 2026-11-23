// Brings up the clock, log and launch vehicle (with CMC bridge) for the
// interactive application.

#pragma once

#include "SimSupport.h"
#include "SaturnIBStack.h"

class SaturnIBAgcBridge;

class SaturnStartupHelper
{
public:
    SaturnStartupHelper();
    ~SaturnStartupHelper();

    /** Supply an explicit BatchConfig for the launch vehicle.  Must be
     *  called before GetLaunchVehicle() to take effect.  If omitted, the
     *  bridge is constructed with a default BatchConfig and relies on the
     *  BatchConfig sentinel defaults (NaN / <0) to pick TMX AS-205
     *  values. */
    void SetBatchConfig(const SaturnIBStack::BatchConfig &cfg);

    SaturnIBAgcBridge *GetLaunchVehicle();

private:
    void Initialize();

    sim::SimClock *m_pClock = nullptr;
    sim::LogManager *m_pLogger = nullptr;
    SaturnIBAgcBridge *m_pVehicle = nullptr;
    SaturnIBStack::BatchConfig m_cfg;
    bool m_cfg_set = false;
};
