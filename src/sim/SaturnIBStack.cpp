#include "SaturnIBStack.h"
#include "SaturnIBPlantObliq.h"
#include "IAgcBridge.h"
#include "msvc_str.h"
#include "save_state.h"
#include "plant_serialize.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <limits>
#include <cstring>
#include <cstdio>

using namespace std;
using namespace sim;

/*
 * SaturnIBPlantObliq owns dynamics; ExternalInputs/Outputs live as members
 * on SaturnIBStack (see SaturnIBStack.h).  All CMC/yaAGC coupling lives in
 * the IAgcBridge pluggable interface; the default is a no-op null bridge
 * (batch_sim path).  apolloa constructs a SaturnIBAgcBridge which sets
 * itself as the active IAgcBridge in its ctor.
 */

/**
 *  OUT11 — S-IB Stage state values; Xe is engine-gimbal position in ECI.
 */
#define OUT11_lat_deg           0
#define OUT11_lon_deg           1
#define OUT11_h_m               2
#define OUT11_Xe_x_m            3
#define OUT11_Xe_y_m            4
#define OUT11_Xe_z_m            5
#define OUT11_Ve_x_mps          6
#define OUT11_Ve_y_mps          7
#define OUT11_Ve_z_mps          8

/**
 *  OUT12 — S-IVB Stage state values.
 */
#define OUT12_lat_deg           0
#define OUT12_lon_deg           1
#define OUT12_h_m               2
#define OUT12_Xe_x_m            3
#define OUT12_Xe_y_m            4
#define OUT12_Xe_z_m            5

/**
 *  OUT22 — on-the-pad state values (valid when bLiftoff == false).
 */
#define OUT22_lat_deg           0
#define OUT22_lon_deg           1
#define OUT22_h_m               2
#define OUT22_Xe_x_m            3
#define OUT22_Xe_y_m            4
#define OUT22_Xe_z_m            5
#define OUT22_Ve_x_mps          6
#define OUT22_Ve_y_mps          7
#define OUT22_Ve_z_mps          8

/**
 * This singleton makes it easy to reach the stack from host callbacks
 * (apolloa's GUI code uses g_lv, which is this pointer at construction).
 */
SaturnIBStack * g_pSaturnIBStack = nullptr;

static NullAgcBridge g_NullAgcBridge;

// Default BatchConfig -- all sentinels set to "keep hardcoded default".
SaturnIBStack::BatchConfig::BatchConfig()
    : pad_load_start_sec(-1.0),
      A_z_deg(std::numeric_limits<double>::quiet_NaN()),
      T_L_prime_sec(-1.0)
{
    for (int i = 0; i < 3; ++i)
        CG_LLA_deg_m[i] = std::numeric_limits<double>::quiet_NaN();
    for (int i = 0; i < 4; ++i)
        q_ECItoSM[i] = std::numeric_limits<double>::quiet_NaN();
}

ExternalInputs_saturn_ib_stack *
SaturnIBStack::GetModelInputs()
{
    return &m_inputs;
}

const ExternalOutputs_saturn_ib_stack *
SaturnIBStack::GetModelOutputs() const
{
    return &m_outputs;
}

const char *
SaturnIBStack::GetErrorMessage() const
{
    return m_pszErrorStatus;
}

#include "igm_trace.h"
#include "ic_trace.h"
#include "onpad_trace.h"
#include "sib_trace.h"
#include "lvdc_io_trace.h"

bool
SaturnIBStack::FillIgmTrace(struct IgmTraceSample_tag *out, double t_sec) const
{
    if (!out || !m_pPlant) return false;
    return SaturnIBPlantObliq_fill_igm_trace(m_pPlant, t_sec, out) == 0;
}

bool
SaturnIBStack::FillLvdcIoTrace(struct LvdcIoTraceSample_tag *out, double t_sec) const
{
    if (!out || !m_pPlant) return false;
    return SaturnIBPlantObliq_fill_lvdc_io_trace(m_pPlant, t_sec, out) == 0;
}

bool
SaturnIBStack::FillIcTrace(struct IcTraceSample_tag *out, double t_sec) const
{
    (void)t_sec;
    if (!out) return false;
    ic_trace_clear(out);
    return false;
}

bool
SaturnIBStack::FillOnPadTrace(struct OnPadTraceSample_tag *out, double t_sec) const
{
    (void)t_sec;
    if (!out) return false;
    onpad_trace_clear(out);
    return false;
}

bool
SaturnIBStack::FillSibTrace(struct SibTraceSample_tag *out, double t_sec) const
{
    (void)t_sec;
    if (!out) return false;
    sib_trace_clear(out);
    return false;
}

SaturnIBStack::SaturnIBStack(const BatchConfig & config)
    : m_pAgcBridge(&g_NullAgcBridge)
{
    g_pSaturnIBStack = this;

    memset (m_dP_feet, 0, sizeof (m_dP_feet));

    m_dResidualDeltaT_sec    = 0.0;
    m_dIMUResidualDeltaT_sec = 0.0;

    /*** Initialize Obliq plant ***/
    m_pszErrorStatus = nullptr;
    memset(&m_inputs, 0, sizeof(m_inputs));
    memset(&m_outputs, 0, sizeof(m_outputs));
    m_outputs.veh_q_ECI[0] = 1.0;

    m_pPlant = SaturnIBPlantObliq_create(MODEL_TIME_STEP_SEC);
    if (m_pPlant == nullptr) {
        (void)fprintf(stderr, "SaturnIBPlantObliq_create failed\n");
        exit(EXIT_FAILURE);
    }

    ExternalInputs_saturn_ib_stack * pModelInputs = &m_inputs;

    int  nYear, nMonth, nDay, nHour, nMin;
    double dSec;
    GetSimClock()->GetCurrentSimulationTime().GetDate( &nYear,  &nMonth,  &nDay, &nHour,  &nMin, &dSec );

    pModelInputs->LaunchDate[0] = nYear;
    pModelInputs->LaunchDate[1] = nMonth;
    pModelInputs->LaunchDate[2] = nDay;
    pModelInputs->LaunchDate[3] = nHour;
    pModelInputs->LaunchDate[4] = nMin;
    pModelInputs->LaunchDate[5] = dSec;

    pModelInputs->A_z_deg = std::isnan(config.A_z_deg) ? 72.0 : config.A_z_deg;

    pModelInputs->CG_LLA_deg_m[0] = std::isnan(config.CG_LLA_deg_m[0])
        ?  28.521963 : config.CG_LLA_deg_m[0];
    pModelInputs->CG_LLA_deg_m[1] = std::isnan(config.CG_LLA_deg_m[1])
        ? -80.561141 : config.CG_LLA_deg_m[1];
    pModelInputs->CG_LLA_deg_m[2] = std::isnan(config.CG_LLA_deg_m[2])
        ?  34.7      : config.CG_LLA_deg_m[2];

    if (std::isnan(config.q_ECItoSM[0])) {
        sim::Quaternion q( 0.0, 0.0, 0.0 );
        for( int i=0; i<4; ++i )
            pModelInputs->q_ECItoSM[0] = q.GetE( i );
    } else {
        for (int i = 0; i < 4; ++i)
            pModelInputs->q_ECItoSM[i] = config.q_ECItoSM[i];
    }

    pModelInputs->T_L_prime_sec = (config.T_L_prime_sec < 0) ? 300.0 : config.T_L_prime_sec;

    m_dFDAI_Roll_deg = 0.0;
    m_dFDAI_Pitch_deg = 0.0;
    m_dFDAI_Yaw_deg = 0.0;
    m_dFDAI_RollDot_dps = 0.0;
    m_dFDAI_PitchDot_dps = 0.0;
    m_dFDAI_YawDot_dps = 0.0;
    m_vCM_IMU_PIPA_Pulses = Vector3d(0,0,0);

    m_dIMU_OGAResolver_deg = 0.0;
    m_dIMU_IGAResolver_deg = 0.0;
    m_dIMU_MGAResolver_deg = 0.0;

    m_dLastSentIMU_OGAResolver_deg = 0.0;
    m_dLastSentIMU_IGAResolver_deg = 0.0;
    m_dLastSentIMU_MGAResolver_deg = 0.0;

    m_dTotalCDUPulseIncrements = Vector3d(0,0,0);

    m_bCoarseAlignFastRate[0] = false;
    m_bCoarseAlignFastRate[1] = false;
    m_bCoarseAlignFastRate[2] = false;

    m_bCoarseAlignEnable  = false;
    m_bErrorCounterEnable = false;

    m_eFineAlignAxis = FA_NO_OPERATION;
    m_dFineAlignPulseCount = 0;
    m_nFineAlignPulseSign = 1;

    m_bLiftoff   = false;
    m_bFirstPass = true;

    m_nCDUXErrorCounter_pulses = 0;
    m_nCDUYErrorCounter_pulses = 0;
    m_nCDUZErrorCounter_pulses = 0;

    m_eIMUMode = ISS_IMU_TURN_ON;
    m_vCoarseAlignCommand_deg = Vector3d( 0, 0, 0 );

    m_mLVtoCM.EulerToMatrix4d( M_PI, 0.0, 0.0 );

    strncpy_s( m_szDebugText, sizeof(m_szDebugText), "", 1 );

    UpdateDebugText();
}

SaturnIBStack::~SaturnIBStack(void)
{
    if (m_pPlant) {
        SaturnIBPlantObliq_destroy(m_pPlant);
        m_pPlant = nullptr;
    }
}

void
SaturnIBStack::SetAgcBridge(IAgcBridge *bridge)
{
    m_pAgcBridge = bridge ? bridge : &g_NullAgcBridge;
}

bool
SaturnIBStack::TimeStep( double dDeltaT_sec )
{
    double dActualDeltaT_sec = m_dResidualDeltaT_sec + dDeltaT_sec;
    ExternalOutputs_saturn_ib_stack * pModelOut = &m_outputs;

    // Pre-tick AGC hook: pad-load + state-vector uplink decisions.
    {
        const double elapsed = GetSimClock()->GetElapsedTime();
        const Vector3d pad_pos( pModelOut->OUT22[OUT22_Xe_x_m],
                                pModelOut->OUT22[OUT22_Xe_y_m],
                                pModelOut->OUT22[OUT22_Xe_z_m] );
        const Vector3d pad_vel( pModelOut->OUT22[OUT22_Ve_x_mps],
                                pModelOut->OUT22[OUT22_Ve_y_mps],
                                pModelOut->OUT22[OUT22_Ve_z_mps] );
        m_pAgcBridge->OnTimeStepBegin(elapsed, pad_pos, pad_vel);
    }

    // Recover current IMU Gimbal angles from the CM-to-SM transform.
    Quaternion qECItoLV( pModelOut->veh_q_ECI[0],
                         pModelOut->veh_q_ECI[1],
                         pModelOut->veh_q_ECI[2],
                         pModelOut->veh_q_ECI[3] );
    Matrix4d mECItoCM = m_mLVtoCM * qECItoLV.QuaternionToMatrix4d();
    Matrix4d mCMtoECI = mECItoCM.Transpose();
    Matrix4d mECItoSM = m_qECItoSM.QuaternionToMatrix4d();
    Matrix4d mCMtoSM = mECItoSM * mCMtoECI;

    double dOGA_rad, dIGA_rad, dMGA_rad;
    MIStoEuler( mCMtoSM, &dOGA_rad, &dIGA_rad, &dMGA_rad );
    m_dIMU_OGAResolver_deg = RADtoDEG( dOGA_rad );
    m_dIMU_IGAResolver_deg = RADtoDEG( dIGA_rad );
    m_dIMU_MGAResolver_deg = RADtoDEG( dMGA_rad );

    while ( dActualDeltaT_sec >= MODEL_TIME_STEP_SEC )
    {
        // Pump next uplink keycode into the CMC (if an uplink is active).
        m_pAgcBridge->UplinkTimeStep();

        double dIMUActualDeltaT_sec = m_dIMUResidualDeltaT_sec + MODEL_TIME_STEP_SEC;

        while ( dIMUActualDeltaT_sec >= STABLE_MEMBER_MODEL_TIME_STEP_SEC )
        {
            Vector3d vGimbalRates_deg_per_sec( 0.0, 0.0, 0.0 );
            UpdateIMUMode();

            switch ( m_eIMUMode )
            {
            default:
                break;

            case ISS_IMU_FINE_ALIGN:
                {
                    // Apollo IRIG scale: 0.549 deg/s at 3200 pulses/s
                    // = 0.000172 deg/pulse (~0.615 arcsec/pulse).
                    const double dFineRate_deg_per_sec = 0.54931644444444444444444444444444;
                    const double dPulseCoundownRate_pulses_per_step = 3200.0 * STABLE_MEMBER_MODEL_TIME_STEP_SEC;
                    bool bDone = false;

                    double dPulsesThisStep;
                    if ( m_dFineAlignPulseCount > dPulseCoundownRate_pulses_per_step )
                    {
                        dPulsesThisStep = dPulseCoundownRate_pulses_per_step;
                        m_dFineAlignPulseCount -= dPulseCoundownRate_pulses_per_step;
                    }
                    else
                    {
                        dPulsesThisStep = m_dFineAlignPulseCount;
                        m_dFineAlignPulseCount = 0.0;
                        bDone = true;
                    }

                    // Gyro torque pulses rotate SM about the pulsed axis in SM-body frame.
                    double dAxisAngle_rad = DEGtoRAD(
                        dPulsesThisStep * (dFineRate_deg_per_sec / 3200.0)
                        * m_nFineAlignPulseSign);
                    double dHalf = 0.5 * dAxisAngle_rad;
                    double dSinHalf = sin(dHalf);
                    double dCosHalf = cos(dHalf);
                    sim::Quaternion q_delta(dCosHalf, 0.0, 0.0, 0.0);
                    switch ( m_eFineAlignAxis )
                    {
                    case FA_X_AXIS:
                        q_delta = sim::Quaternion(dCosHalf, dSinHalf, 0.0, 0.0);
                        break;
                    case FA_Y_AXIS:
                        q_delta = sim::Quaternion(dCosHalf, 0.0, dSinHalf, 0.0);
                        break;
                    case FA_Z_AXIS:
                        q_delta = sim::Quaternion(dCosHalf, 0.0, 0.0, dSinHalf);
                        break;
                    default:
                        break;
                    }
                    m_qECItoSM = m_qECItoSM * q_delta;

                    // Recompute resolvers from the updated SM orientation.
                    {
                        Matrix4d m_newECItoSM = m_qECItoSM.QuaternionToMatrix4d();
                        Matrix4d m_newCMtoSM  = m_newECItoSM * mCMtoECI;
                        double dNewOGA_rad, dNewIGA_rad, dNewMGA_rad;
                        MIStoEuler(m_newCMtoSM, &dNewOGA_rad, &dNewIGA_rad, &dNewMGA_rad);
                        m_dIMU_OGAResolver_deg = RADtoDEG(dNewOGA_rad);
                        m_dIMU_IGAResolver_deg = RADtoDEG(dNewIGA_rad);
                        m_dIMU_MGAResolver_deg = RADtoDEG(dNewMGA_rad);
                    }

                    if ( bDone )
                        m_eFineAlignAxis = FA_NO_OPERATION;
                }
                break;

            case ISS_IMU_TURN_ON:
            case ISS_IMU_COARSE_ALIGN:
                {
                    Vector3d vQuantizedCurrentAngles_deg;
                    const double dMaxFastCoarseError_deg = 1.2;
                    const double dMaxSlowCoarseError_deg = 0.07;
                    const double dCoarseAlignFastRate_deg_per_sec = 35.0;
                    const double dCoarseAlignSlowRate_deg_per_sec = 4.45;

                    vQuantizedCurrentAngles_deg = Vector3d(
                        FineResolverQuantization(m_dIMU_OGAResolver_deg),
                        FineResolverQuantization(m_dIMU_IGAResolver_deg),
                        FineResolverQuantization(m_dIMU_MGAResolver_deg));

                    m_vCoarseAlignError_deg = m_vCoarseAlignCommand_deg - vQuantizedCurrentAngles_deg;

                    auto compute_rate = [&](double err, bool *fastOut) {
                        *fastOut = false;
                        if (err < -180.0) err += 360.0;
                        else if (err > 180.0) err -= 360.0;
                        double rate = 0.0;
                        if (err <= -dMaxFastCoarseError_deg) {
                            rate = -dCoarseAlignFastRate_deg_per_sec;
                            *fastOut = true;
                        }
                        if (err <= -dMaxSlowCoarseError_deg) {
                            rate = -dCoarseAlignSlowRate_deg_per_sec;
                        }
                        else if (err >= dMaxFastCoarseError_deg) {
                            rate = dCoarseAlignFastRate_deg_per_sec;
                            *fastOut = true;
                        }
                        else if (err >= dMaxSlowCoarseError_deg) {
                            rate = dCoarseAlignSlowRate_deg_per_sec;
                        }
                        return rate;
                    };

                    vGimbalRates_deg_per_sec.x = compute_rate(m_vCoarseAlignError_deg.x, &m_bCoarseAlignFastRate[0]);
                    vGimbalRates_deg_per_sec.y = compute_rate(m_vCoarseAlignError_deg.y, &m_bCoarseAlignFastRate[1]);
                    vGimbalRates_deg_per_sec.z = compute_rate(m_vCoarseAlignError_deg.z, &m_bCoarseAlignFastRate[2]);
                }
                break;
            }

            // Integrate IMU gimbal angle changes due to torquers.
            m_dIMU_OGAResolver_deg += vGimbalRates_deg_per_sec.x * STABLE_MEMBER_MODEL_TIME_STEP_SEC;
            m_dIMU_IGAResolver_deg += vGimbalRates_deg_per_sec.y * STABLE_MEMBER_MODEL_TIME_STEP_SEC;
            m_dIMU_MGAResolver_deg += vGimbalRates_deg_per_sec.z * STABLE_MEMBER_MODEL_TIME_STEP_SEC;

            // Push gimbal deltas to AGC CDU and step AGC for this stable-member step.
            m_pAgcBridge->InjectGimbalCDU();
            m_pAgcBridge->StepAGC(STABLE_MEMBER_MODEL_TIME_STEP_SEC);

            dIMUActualDeltaT_sec -= STABLE_MEMBER_MODEL_TIME_STEP_SEC;
        }

        m_dIMUResidualDeltaT_sec = dIMUActualDeltaT_sec;

        // Propagate SM orientation to vehicle for the next plant step.
        // Gate off during FINE_ALIGN (the quaternion is the ground truth there).
        if ( m_eIMUMode != ISS_IMU_FINE_ALIGN )
        {
            mCMtoSM = EulerToMIS( DEGtoRAD( m_dIMU_OGAResolver_deg ),
                                  DEGtoRAD( m_dIMU_IGAResolver_deg ),
                                  DEGtoRAD( m_dIMU_MGAResolver_deg ) );
            mECItoSM = mCMtoSM * mECItoCM;
            m_qECItoSM = (Quaternion) mECItoSM;
        }

        ExternalInputs_saturn_ib_stack * pModelInputs = &m_inputs;
        for (int i=0; i<4; ++i )
            pModelInputs->q_ECItoSM[i] = m_qECItoSM.GetE( i );

        StepPlant();

        // Inject PIPA pulse deltas to AGC.
        m_pAgcBridge->InjectPIPA();

        dActualDeltaT_sec -= MODEL_TIME_STEP_SEC;
    }

    m_dResidualDeltaT_sec = dActualDeltaT_sec;
    m_bFirstPass = false;

    m_pAgcBridge->OnTimeStepEnd();

    return false;
}

/*
 * Mission-time clock formatter used by UpdateDebugText.
 *
 * Liftoff occurs at sim time + dLiftoffOffset_sec (default 300 s).
 *   Before liftoff: "- HH:MM:SS"  (countdown, two-digit hours)
 *   At/after liftoff: "+ H:MM:SS" (hours grow to as many digits as needed)
 *
 * Returns a pointer to a static buffer; not reentrant.
 */
static const char *GenerateMissionTimeString(double dSimTime_sec,
                                             double dLiftoffOffset_sec = 300.0)
{
    static char szBuf[32];

    double dMissionTime_sec = dSimTime_sec - dLiftoffOffset_sec;
    bool   bPrelaunch       = dMissionTime_sec < 0.0;

    long nTotal = (long) std::floor( std::fabs( dMissionTime_sec ) );
    long nHours = nTotal / 3600;
    int  nMin   = (int) ((nTotal / 60) % 60);
    int  nSec   = (int) (nTotal % 60);

    if (bPrelaunch)
        snprintf( szBuf, sizeof(szBuf), "- %02ld:%02d:%02d", nHours, nMin, nSec );
    else
        snprintf( szBuf, sizeof(szBuf), "+ %ld:%02d:%02d", nHours, nMin, nSec );
    return szBuf;
}

ISS_IMU_Mode
SaturnIBStack::UpdateIMUMode()
{
    ISS_IMU_Mode eIMUMode;

    if ( m_eIMUMode == ISS_IMU_TURN_ON )
    {
        eIMUMode = ISS_IMU_TURN_ON;
    }
    else if ( GetErrorCounterEnable() )
    {
        if ( GetCoarseAlignEnable() )
            eIMUMode = ISS_IMU_COARSE_ALIGN;
        else if ( m_eFineAlignAxis != FA_NO_OPERATION )
            eIMUMode = ISS_IMU_FINE_ALIGN;
        else
            eIMUMode = ISS_IMU_ATTITUDE_ERROR_DISPLAY;
    }
    else
    {
        if ( m_eFineAlignAxis != FA_NO_OPERATION )
            eIMUMode = ISS_IMU_FINE_ALIGN;
        else
            eIMUMode = ISS_IMU_INERTIAL_REFERENCE;
    }

    m_eIMUMode = eIMUMode;
    return eIMUMode;
}

double
SaturnIBStack::CoarseResolverQuantization( double dAngle_deg )
{
    return (double) ((int) ( dAngle_deg / 360.0 * 128.0 ) * 360.0 / 128.0 );
}

double
SaturnIBStack::FineResolverQuantization( double dAngle_deg )
{
    return (double) ((int) ( dAngle_deg / 360.0 * 32768.0 ) * 360.0 / 32768.0 );
}

void
SaturnIBStack::StepPlant()
{
    SaturnIBPlantObliq_step(m_pPlant, &m_inputs, &m_outputs);

    ExternalOutputs_saturn_ib_stack * pModelOut = &m_outputs;

    if ( pModelOut->bLiftoff == false )
    {
        m_wcGCLocation.m_latitude_rad = DEGtoRAD( pModelOut->OUT22[OUT22_lat_deg] );
        m_wcGCLocation.m_longitude_rad = DEGtoRAD( pModelOut->OUT22[OUT22_lon_deg] );
        m_wcGCLocation.m_altitude_meters = pModelOut->OUT22[OUT22_h_m];
        m_dP_feet[0] = METERStoFEET (pModelOut->OUT22[OUT22_Xe_x_m]);
        m_dP_feet[1] = METERStoFEET (pModelOut->OUT22[OUT22_Xe_y_m]);
        m_dP_feet[2] = METERStoFEET (pModelOut->OUT22[OUT22_Xe_z_m]);
    }
    else if ( pModelOut->bStageSep == false )
    {
        m_wcGCLocation.m_latitude_rad = DEGtoRAD( pModelOut->OUT11[OUT11_lat_deg] );
        m_wcGCLocation.m_longitude_rad = DEGtoRAD( pModelOut->OUT11[OUT11_lon_deg] );
        m_wcGCLocation.m_altitude_meters = pModelOut->OUT11[OUT11_h_m];
        m_dP_feet[0] = METERStoFEET (pModelOut->OUT11[OUT11_Xe_x_m]);
        m_dP_feet[1] = METERStoFEET (pModelOut->OUT11[OUT11_Xe_y_m]);
        m_dP_feet[2] = METERStoFEET (pModelOut->OUT11[OUT11_Xe_z_m]);
    }
    else
    {
        m_wcGCLocation.m_latitude_rad = DEGtoRAD( pModelOut->OUT12[OUT12_lat_deg] );
        m_wcGCLocation.m_longitude_rad = DEGtoRAD( pModelOut->OUT12[OUT12_lon_deg] );
        m_wcGCLocation.m_altitude_meters = pModelOut->OUT12[OUT12_h_m];
        m_dP_feet[0] = METERStoFEET (pModelOut->OUT12[OUT12_Xe_x_m]);
        m_dP_feet[1] = METERStoFEET (pModelOut->OUT12[OUT12_Xe_y_m]);
        m_dP_feet[2] = METERStoFEET (pModelOut->OUT12[OUT12_Xe_z_m]);
    }

    // CM +Z opposes L/V -Z (180° roll), so Y/Z rate axes invert.
    m_dFDAI_RollDot_dps  = pModelOut->FDAI_RollDot_dps;
    m_dFDAI_PitchDot_dps = - pModelOut->FDAI_PitchDot_dps;
    m_dFDAI_YawDot_dps   = - pModelOut->FDAI_YawDot_dps;

    m_vCM_IMU_PIPA_Pulses = Vector3d(pModelOut->CM_IMU_PIPA_Pulses[0],
                                     pModelOut->CM_IMU_PIPA_Pulses[1],
                                     pModelOut->CM_IMU_PIPA_Pulses[2]);

    m_dFDAI_Yaw_deg   = m_dIMU_MGAResolver_deg;
    m_dFDAI_Pitch_deg = m_dIMU_IGAResolver_deg;
    m_dFDAI_Roll_deg  = m_dIMU_OGAResolver_deg;

    m_bLiftoff = pModelOut->bLiftoff ? true : false;
}

void
SaturnIBStack::GetFDAIState( sim::Vector3d * pvAngles_deg,
                             sim::Vector3d * pvRates_dps,
                             sim::Vector3d * pvErrorNeedles_deg )
{
    pvAngles_deg->x = m_dIMU_OGAResolver_deg;
    pvAngles_deg->y = m_dIMU_IGAResolver_deg;
    pvAngles_deg->z = m_dIMU_MGAResolver_deg;
    pvRates_dps->x = m_dFDAI_RollDot_dps;
    pvRates_dps->y = m_dFDAI_PitchDot_dps;
    pvRates_dps->z = m_dFDAI_YawDot_dps;

    if ( GetErrorCounterEnable() )
    {
        pvErrorNeedles_deg->x = ERROR_COUNTER_STEPS_TO_DEG( m_nCDUXErrorCounter_pulses ) / 22.5;
        pvErrorNeedles_deg->y = ERROR_COUNTER_STEPS_TO_DEG( m_nCDUYErrorCounter_pulses ) / 22.5;
        pvErrorNeedles_deg->z = ERROR_COUNTER_STEPS_TO_DEG( m_nCDUZErrorCounter_pulses ) / 22.5;
    }
    else
    {
        *pvErrorNeedles_deg = Vector3d(0,0,0);
    }
}

void
SaturnIBStack::UpdateDebugText()
{
    int  nYear, nMonth, nDay, nHour, nMin;
    double dSec;
    GetSimClock()->GetCurrentSimulationTime().GetDate( &nYear,  &nMonth,  &nDay, &nHour,  &nMin, &dSec );

    sprintf( m_szDebugText,
        "    T %s\r\n"
        "SIM_T %8.2f   %04.4d-%02.2d-%02.2d %02.2d:%02.2d:%05.2f\r\n"
        " CMDX %8.3f   CMDY %8.3f   CMDZ %8.3f\r\n"
        "  OGX %8.3f    IGY %8.3f    MGZ %8.3f\r\n"
        "  LSX %8.3f    LSY %8.3f    LSZ %8.3f\r\n"
        " FAPS %8.8d   FAPC %8.3f     CA  %d  ERRCEN %d\r\n"
        "  ERX %8.3f    ERY %8.3f    ERZ %8.3f\r\n"
        "  LAT %8.3f    LON %8.3f   H(M) %8.0f\r\n"
        " ECIX %8.0f   ECIY %8.0f   ECIZ %8.0f\r\n",
        GenerateMissionTimeString(GetSimClock()->GetElapsedTime()),
        GetSimClock()->GetElapsedTime(),
        nYear, nMonth,  nDay, nHour, nMin, dSec,
        m_vCoarseAlignCommand_deg.x,
        m_vCoarseAlignCommand_deg.y,
        m_vCoarseAlignCommand_deg.z,
        m_dIMU_OGAResolver_deg,
        m_dIMU_IGAResolver_deg,
        m_dIMU_MGAResolver_deg,
        m_dLastSentIMU_OGAResolver_deg,
        m_dLastSentIMU_IGAResolver_deg,
        m_dLastSentIMU_MGAResolver_deg,
        m_nFineAlignPulseSign,
        m_dFineAlignPulseCount,
        GetCoarseAlignEnable() ? 1 : 0,
        GetErrorCounterEnable() ? 1 : 0,
        ERROR_COUNTER_STEPS_TO_DEG( m_nCDUXErrorCounter_pulses ),
        ERROR_COUNTER_STEPS_TO_DEG( m_nCDUYErrorCounter_pulses ),
        ERROR_COUNTER_STEPS_TO_DEG( m_nCDUZErrorCounter_pulses ),
        RADtoDEG(m_wcGCLocation.m_latitude_rad),
        RADtoDEG(m_wcGCLocation.m_longitude_rad),
        m_wcGCLocation.m_altitude_meters,
        FEETtoMETERS(m_dP_feet[0]),
        FEETtoMETERS(m_dP_feet[1]),
        FEETtoMETERS(m_dP_feet[2])
    );
}

void
SaturnIBStack::UpdateIMUXGimbal( short nSign, unsigned short nPulseCount )
{
    ISS_IMU_Mode eIMUMode = UpdateIMUMode();
    if ( eIMUMode == ISS_IMU_COARSE_ALIGN )
        m_vCoarseAlignCommand_deg.x += nSign * nPulseCount * READ_COUNTER_PULSES_TO_DEG(4);
    else
        m_nCDUXErrorCounter_pulses += nSign * nPulseCount;
}

void
SaturnIBStack::UpdateIMUYGimbal( short nSign, unsigned short nPulseCount )
{
    ISS_IMU_Mode eIMUMode = UpdateIMUMode();
    if ( eIMUMode == ISS_IMU_COARSE_ALIGN )
        m_vCoarseAlignCommand_deg.y += nSign * nPulseCount * READ_COUNTER_PULSES_TO_DEG(4);
    else
        m_nCDUYErrorCounter_pulses += nSign * nPulseCount;
}

void
SaturnIBStack::UpdateIMUZGimbal( short nSign, unsigned short nPulseCount )
{
    ISS_IMU_Mode eIMUMode = UpdateIMUMode();
    if ( eIMUMode == ISS_IMU_COARSE_ALIGN )
        m_vCoarseAlignCommand_deg.z += nSign * nPulseCount * READ_COUNTER_PULSES_TO_DEG(4);
    else
        m_nCDUZErrorCounter_pulses += nSign * nPulseCount;
}

void
SaturnIBStack::IncrementFineAlignPulseCount( ISS_Fine_Align_Axis eAxis,
                                             int nSign,
                                             unsigned short usPulseCount )
{
    if ( eAxis != FA_NO_OPERATION )
    {
        m_eFineAlignAxis = eAxis;

        if ( m_dFineAlignPulseCount == 0.0 )
        {
            m_dFineAlignPulseCount += usPulseCount;
            m_nFineAlignPulseSign = nSign;
        }
        else
        {
            if ( m_nFineAlignPulseSign == nSign )
            {
                m_dFineAlignPulseCount += usPulseCount;
            }
            else
            {
                m_dFineAlignPulseCount -= usPulseCount;
                if ( m_dFineAlignPulseCount < 0.0 )
                {
                    m_dFineAlignPulseCount = - m_dFineAlignPulseCount;
                    m_nFineAlignPulseSign = - nSign;
                }
            }
        }
        if ( m_dFineAlignPulseCount == 0 )
            m_eFineAlignAxis = FA_NO_OPERATION;
    }
    else if ( nSign == 0 )
    {
        m_eFineAlignAxis = FA_NO_OPERATION;
        m_dFineAlignPulseCount = 0.0;
        m_nFineAlignPulseSign = 1;
    }

    UpdateIMUMode();
}

void
SaturnIBStack::ResetCDUInternalState()
{
    ExternalOutputs_saturn_ib_stack * pModelOut = &m_outputs;

    m_dLastSentIMU_OGAResolver_deg = 0.0;
    m_dLastSentIMU_IGAResolver_deg = 0.0;
    m_dLastSentIMU_MGAResolver_deg = 0.0;

    Quaternion qECItoLV( pModelOut->veh_q_ECI[0],
                         pModelOut->veh_q_ECI[1],
                         pModelOut->veh_q_ECI[2],
                         pModelOut->veh_q_ECI[3] );

    m_qECItoSM = (Quaternion) (m_mLVtoCM * (Matrix4d) qECItoLV);

    m_vCoarseAlignCommand_deg = Vector3d( 0, 0, 0 );

    m_nCDUXErrorCounter_pulses = 0;
    m_nCDUYErrorCounter_pulses = 0;
    m_nCDUZErrorCounter_pulses = 0;
}

void
SaturnIBStack::SaveState( const std::string & strName )
{
    char szName[1024];
    strcpy_s( szName, sizeof(szName), strName.c_str() );
    strcat_s( szName, sizeof(szName), "_state.txt" );

    Quaternion qECItoSM = m_qECItoSM;
    ExternalInputs_saturn_ib_stack * pModelInputs = &m_inputs;

    std::ofstream out( szName );
    out << "Saturn-IB Stack State Dump" << endl;
    out << "T=" << GetSimClock()->GetElapsedTime() << endl;
    out << "StartDate=( " << pModelInputs->LaunchDate[0] << ", "
        << pModelInputs->LaunchDate[1] << ", "
        << pModelInputs->LaunchDate[2] << ", "
        << pModelInputs->LaunchDate[3] << ", "
        << pModelInputs->LaunchDate[4] << ", "
        << pModelInputs->LaunchDate[5] << " )" << endl;
    out << "qECItoSM=( "
        << qECItoSM.GetE(0) << ", "
        << qECItoSM.GetE(1) << ", "
        << qECItoSM.GetE(2) << ", "
        << qECItoSM.GetE(3) << " )" << endl;
    out.close();
}

void
SaturnIBStack::MIStoEuler( const sim::Matrix4d & mMIS,
                           double *pdOGA_rad,
                           double *pdIGA_rad,
                           double *pdMGA_rad)
{
    *pdOGA_rad = atan2( - mMIS.mat(1,2), mMIS.mat(1,1) );
    *pdIGA_rad = atan2( - mMIS.mat(2,0), mMIS.mat(0,0) );
    *pdMGA_rad = asin( mMIS.mat(1,0) );
}

sim::Matrix4d
SaturnIBStack::EulerToMIS( double dOGA_rad, double dIGA_rad, double dMGA_rad )
{
    double sinPhi   = sin (dOGA_rad), cosPhi   = cos (dOGA_rad);
    double sinTheta = sin (dIGA_rad), cosTheta = cos (dIGA_rad);
    double sinPsi   = sin (dMGA_rad), cosPsi   = cos (dMGA_rad);
    Matrix4d mResult;

    mResult.mat (0, 0) = cosTheta * cosPsi;
    mResult.mat (0, 1) = - cosTheta * sinPsi * cosPhi + sinTheta * sinPhi;
    mResult.mat (0, 2) = sinPhi * cosTheta * sinPsi + cosPhi * sinTheta;
    mResult.mat (1, 0) = sinPsi;
    mResult.mat (1, 1) =   cosPhi * cosPsi;
    mResult.mat (1, 2) = - sinPhi * cosPsi;
    mResult.mat (2, 0) = - sinTheta * cosPsi;
    mResult.mat (2, 1) = sinTheta * sinPsi * cosPhi + cosTheta * sinPhi;
    mResult.mat (2, 2) = - sinTheta * sinPsi * sinPhi + cosTheta * cosPhi;
    mResult.mat (3, 0) = mResult.mat (3, 1) = mResult.mat (3, 2) = 0.0;
    mResult.mat (0, 3) = mResult.mat (1, 3) = mResult.mat (2, 3) = 0.0;
    mResult.mat (3, 3) = 1.0;
    return mResult;
}

// =====================================================================
// Binary save-state (dskymatic-studio --save-state-time / --resume)
// =====================================================================

namespace {
constexpr uint32_t kStackFormatVersion = 1;

inline void write_vec3(dskymatic::BinaryWriter &w, const sim::Vector3d &v) {
    w.write_f64(v.x); w.write_f64(v.y); w.write_f64(v.z);
}
inline void read_vec3(dskymatic::BinaryReader &r, sim::Vector3d &v) {
    v.x = r.read_f64(); v.y = r.read_f64(); v.z = r.read_f64();
}
inline void write_quat(dskymatic::BinaryWriter &w, const sim::Quaternion &q) {
    for (int i = 0; i < 4; ++i) w.write_f64(q.GetE(i));
}
inline void read_quat(dskymatic::BinaryReader &r, sim::Quaternion &q) {
    q.SetW(r.read_f64()); q.SetX(r.read_f64());
    q.SetY(r.read_f64()); q.SetZ(r.read_f64());
}
} // namespace

void SaturnIBStack::SerializeState(dskymatic::BinaryWriter &w) const
{
    w.write_u32(kStackFormatVersion);

    // FDAI telemetry (recoverable but cheap to persist for display fidelity
    // immediately on resume, before the first plant tick fills them in).
    w.write_f64(m_dFDAI_Roll_deg);
    w.write_f64(m_dFDAI_Pitch_deg);
    w.write_f64(m_dFDAI_Yaw_deg);
    w.write_f64(m_dFDAI_RollDot_dps);
    w.write_f64(m_dFDAI_PitchDot_dps);
    w.write_f64(m_dFDAI_YawDot_dps);

    write_vec3(w, m_vCM_IMU_PIPA_Pulses);

    write_quat(w, m_qECItoSM);

    w.write_f64(m_dIMU_OGAResolver_deg);
    w.write_f64(m_dIMU_IGAResolver_deg);
    w.write_f64(m_dIMU_MGAResolver_deg);

    w.write_f64(m_dLastSentIMU_OGAResolver_deg);
    w.write_f64(m_dLastSentIMU_IGAResolver_deg);
    w.write_f64(m_dLastSentIMU_MGAResolver_deg);

    write_vec3(w, m_dTotalCDUPulseIncrements);
    w.write_bool(m_bFirstPass);
    w.write_i32((int32_t)m_eIMUMode);

    write_vec3(w, m_vCoarseAlignCommand_deg);
    write_vec3(w, m_vCoarseAlignError_deg);
    for (int i = 0; i < 3; ++i) w.write_bool(m_bCoarseAlignFastRate[i]);

    w.write_i32((int32_t)m_eFineAlignAxis);
    w.write_f64(m_dFineAlignPulseCount);
    w.write_i32(m_nFineAlignPulseSign);

    w.write_i16(m_nCDUXErrorCounter_pulses);
    w.write_i16(m_nCDUYErrorCounter_pulses);
    w.write_i16(m_nCDUZErrorCounter_pulses);

    w.write_bool(m_bCoarseAlignEnable);
    w.write_bool(m_bErrorCounterEnable);
    w.write_bool(m_bLiftoff);

    w.write_f64(m_dResidualDeltaT_sec);
    w.write_f64(m_dIMUResidualDeltaT_sec);

    // External plant outputs copy.  These feed the first post-resume
    // TimeStep (which reads veh_q_ECI / OUT22 BEFORE StepPlant runs),
    // so without this the first tick computes gimbal resolvers from a
    // zeroed quaternion and the FDAI ball flips to identity.
    {
        const auto &o = m_outputs;
        w.write_f64(o.FDAI_Roll_deg);
        w.write_f64(o.FDAI_Pitch_deg);
        w.write_f64(o.FDAI_Yaw_deg);
        w.write_f64(o.FDAI_RollDot_dps);
        w.write_f64(o.FDAI_PitchDot_dps);
        w.write_f64(o.FDAI_YawDot_dps);
        for (int i = 0; i < 3; ++i) w.write_f64(o.CM_IMU_PIPA_Pulses[i]);
        w.write_f64(o.CM_IMU_IGA_rad);
        w.write_f64(o.CM_IMU_MGA_Rad);
        w.write_f64(o.CM_IMU_OGA_rad);
        for (int i = 0; i < 25; ++i) w.write_f64(o.OUT11[i]);
        for (int i = 0; i < 24; ++i) w.write_f64(o.OUT12[i]);
        w.write_i32(o.bStageSep);
        w.write_i32(o.bIECO);
        w.write_i32(o.bOECO);
        w.write_i32(o.bS_IVB_EngineStart);
        for (int i = 0; i < 4; ++i) w.write_f64(o.veh_q_ECI[i]);
        w.write_i32(o.bLiftoff);
        w.write_f64(o.BodyToSM_Phi_deg);
        w.write_f64(o.BodyToSM_Theta_deg);
        w.write_f64(o.BodyToSM_Psi_deg);
        for (int i = 0; i < 9; ++i) w.write_f64(o.OUT22[i]);
    }

    // Plant persistent state.
    dskymatic::plant_serialize(w, m_pPlant);
}

bool SaturnIBStack::DeserializeState(dskymatic::BinaryReader &r)
{
    uint32_t v = r.read_u32();
    if (v != kStackFormatVersion) {
        std::fprintf(stderr,
            "SaturnIBStack::DeserializeState: version mismatch (file=%u, "
            "expected=%u)\n", v, kStackFormatVersion);
        return false;
    }

    m_dFDAI_Roll_deg     = r.read_f64();
    m_dFDAI_Pitch_deg    = r.read_f64();
    m_dFDAI_Yaw_deg      = r.read_f64();
    m_dFDAI_RollDot_dps  = r.read_f64();
    m_dFDAI_PitchDot_dps = r.read_f64();
    m_dFDAI_YawDot_dps   = r.read_f64();

    read_vec3(r, m_vCM_IMU_PIPA_Pulses);
    read_quat(r, m_qECItoSM);

    m_dIMU_OGAResolver_deg = r.read_f64();
    m_dIMU_IGAResolver_deg = r.read_f64();
    m_dIMU_MGAResolver_deg = r.read_f64();
    m_dLastSentIMU_OGAResolver_deg = r.read_f64();
    m_dLastSentIMU_IGAResolver_deg = r.read_f64();
    m_dLastSentIMU_MGAResolver_deg = r.read_f64();

    read_vec3(r, m_dTotalCDUPulseIncrements);
    m_bFirstPass = r.read_bool();
    m_eIMUMode   = (ISS_IMU_Mode)r.read_i32();

    read_vec3(r, m_vCoarseAlignCommand_deg);
    read_vec3(r, m_vCoarseAlignError_deg);
    for (int i = 0; i < 3; ++i) m_bCoarseAlignFastRate[i] = r.read_bool();

    m_eFineAlignAxis         = (ISS_Fine_Align_Axis)r.read_i32();
    m_dFineAlignPulseCount   = r.read_f64();
    m_nFineAlignPulseSign    = r.read_i32();

    m_nCDUXErrorCounter_pulses = r.read_i16();
    m_nCDUYErrorCounter_pulses = r.read_i16();
    m_nCDUZErrorCounter_pulses = r.read_i16();

    m_bCoarseAlignEnable   = r.read_bool();
    m_bErrorCounterEnable  = r.read_bool();
    m_bLiftoff             = r.read_bool();
    m_dResidualDeltaT_sec     = r.read_f64();
    m_dIMUResidualDeltaT_sec  = r.read_f64();

    {
        auto &o = m_outputs;
        o.FDAI_Roll_deg     = r.read_f64();
        o.FDAI_Pitch_deg    = r.read_f64();
        o.FDAI_Yaw_deg      = r.read_f64();
        o.FDAI_RollDot_dps  = r.read_f64();
        o.FDAI_PitchDot_dps = r.read_f64();
        o.FDAI_YawDot_dps   = r.read_f64();
        for (int i = 0; i < 3; ++i) o.CM_IMU_PIPA_Pulses[i] = r.read_f64();
        o.CM_IMU_IGA_rad = r.read_f64();
        o.CM_IMU_MGA_Rad = r.read_f64();
        o.CM_IMU_OGA_rad = r.read_f64();
        for (int i = 0; i < 25; ++i) o.OUT11[i] = r.read_f64();
        for (int i = 0; i < 24; ++i) o.OUT12[i] = r.read_f64();
        o.bStageSep          = r.read_i32();
        o.bIECO              = r.read_i32();
        o.bOECO              = r.read_i32();
        o.bS_IVB_EngineStart = r.read_i32();
        for (int i = 0; i < 4; ++i) o.veh_q_ECI[i] = r.read_f64();
        o.bLiftoff             = r.read_i32();
        o.BodyToSM_Phi_deg     = r.read_f64();
        o.BodyToSM_Theta_deg   = r.read_f64();
        o.BodyToSM_Psi_deg     = r.read_f64();
        for (int i = 0; i < 9; ++i) o.OUT22[i] = r.read_f64();
    }

    if (!dskymatic::plant_deserialize(r, m_pPlant)) return false;
    return r.ok();
}
