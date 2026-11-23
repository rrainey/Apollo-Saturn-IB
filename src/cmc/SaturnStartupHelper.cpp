#include "SaturnStartupHelper.h"
#include "SaturnIBAgcBridge.h"

SaturnStartupHelper::SaturnStartupHelper() = default;

SaturnStartupHelper::~SaturnStartupHelper()
{
    delete m_pVehicle;
    delete m_pClock;
    delete m_pLogger;
}

void SaturnStartupHelper::SetBatchConfig(const SaturnIBStack::BatchConfig &cfg)
{
    m_cfg = cfg;
    m_cfg_set = true;
}

SaturnIBAgcBridge *SaturnStartupHelper::GetLaunchVehicle()
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

    m_pVehicle = m_cfg_set ? new SaturnIBAgcBridge(m_cfg)
                           : new SaturnIBAgcBridge();
    m_pClock->Register( m_pVehicle );
}
