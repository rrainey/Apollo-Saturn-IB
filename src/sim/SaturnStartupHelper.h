// Brings up the clock, log and launch vehicle for the interactive application.

#pragma once

#include "SimSupport.h"

class SaturnIBStack;

class SaturnStartupHelper
{
public:
    SaturnStartupHelper();
    ~SaturnStartupHelper();

    SaturnIBStack *GetLaunchVehicle();

private:
    void Initialize();

    sim::SimClock *m_pClock = nullptr;
    sim::LogManager *m_pLogger = nullptr;
    SaturnIBStack *m_pVehicle = nullptr;
};
