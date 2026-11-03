#include "SaturnStartupHelper.h"
#include "SaturnIBStack.h"

SaturnStartupHelper::SaturnStartupHelper() = default;

SaturnStartupHelper::~SaturnStartupHelper()
{
    delete m_pVehicle;
    delete m_pClock;
    delete m_pLogger;
}

SaturnIBStack *SaturnStartupHelper::GetLaunchVehicle()
{
    if (!m_pVehicle) Initialize();
    return m_pVehicle;
}

void SaturnStartupHelper::Initialize()
{
    sim::Date dtStart_utc;
    dtStart_utc.SetDate( 1968, 10, 11, 14, 57, 45.0 );

    m_pClock = new sim::SimClock();
    m_pLogger = new sim::LogManager("saturn_a_log.txt");

    m_pClock->SetCurrentSimulationTime( dtStart_utc );
    m_pClock->Thaw();

    m_pVehicle = new SaturnIBStack();
    m_pClock->Register( m_pVehicle );
}
