#include "SaturnIBStack.h"
#include "SaturnIBPlantObliq.h"
#include "msvc_str.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <limits>
#include <cstring>
#include <cstdio>

using namespace std;

#include <agc_engine.h>
#include "UplinkMessage.h"
#include "ChannelQueue.h"

/*
 * Obliq backend: no RTW plant. SaturnIBPlantObliq owns dynamics;
 * ExternalInputs/Outputs live as members (see SaturnIBStack.h).
 */

using namespace sim;

/**
 *  OUT11 contains of a number of S-IB Stage state values
 *  Xe position is the X Sta. 100 (engine gibmals) in the ECI frame
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
#define OUT11_IMU_theta_x_deg   9
#define OUT11_IMU_theta_y_deg   10
#define OUT11_IMU_theta_z_deg   11
#define OUT11_alpha_deg         12
#define OUT11_Vb_mps            13
#define OUT11_q_ECI_q0          14
#define OUT11_q_ECI_q1          15
#define OUT11_q_ECI_q2          16
#define OUT11_q_ECI_q3          17
#define OUT11_omega_b_x_rps     18
#define OUT11_omega_b_y_rps     19
#define OUT11_omega_b_z_rps     20
#define OUT11_Vb_x_mps          21
#define OUT11_Vb_y_mps          22
#define OUT11_Vb_z_mps          23

/**
 *  OUT11 contains of a number of S-IB Stage state values
 *  Xe position is the X Sta. 100 (engine gimbals) in the ECI frame
 */
#define OUT12_lat_deg           0
#define OUT12_lon_deg           1
#define OUT12_h_m               2
#define OUT12_Xe_x_m            3
#define OUT12_Xe_y_m            4
#define OUT12_Xe_z_m            5
#define OUT12_Ve_x_mps          6
#define OUT12_Ve_y_mps          7
#define OUT12_Ve_z_mps          8
#define OUT12_IMU_theta_x_deg   9
#define OUT12_IMU_theta_y_deg   10
#define OUT12_IMU_theta_z_deg   11
#define OUT12_alpha_deg         12
#define OUT12_Vb_mps            13
#define OUT12_q_ECI_q0          14
#define OUT12_q_ECI_q1          15
#define OUT12_q_ECI_q2          16
#define OUT12_q_ECI_q3          17
#define OUT12_omega_b_x_rps     18
#define OUT12_omega_b_y_rps     19
#define OUT12_omega_b_z_rps     20
#define OUT12_Vb_x_mps          21
#define OUT12_Vb_y_mps          22
#define OUT12_Vb_z_mps          23

/**
 *  OUT22 contains of a number of S-IB Stage "on-the-pad" state values
 *  Xe position is the X Sta. 100 (engine gibmals) in the ECI frame
 *  NOTE: these values are only valid so long as bLiftoff is "FALSE".
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
 * yaAGC_COARSE_SMOOTH is a scaling factor that is an artifact of yaAGC's interactions with more
 * primitive IMU simulators than we're using here.  This value is defined in agc_engine.c in
 * the yaAGC code, and the value used there must be propagated here.  It is simply called
 * "COARSE_SMOOTH" in that module.
 */
#define yaAGC_COARSE_SMOOTH 8

/**
 * An expedient way to expose AGC Output operations
 */
ChannelIOList g_AGCChannelOutputQueue;

/**
 * An expedient way to expose AGC Input operations
 */
ChannelIOList g_AGCChannelInputQueue;

/**
 * This object is a singleton in this simulation to make it easer to handle yaAGC callbacks
 */
SaturnIBStack * g_pSaturnIBStack;

bool bSentFirstUplink = false;
bool bSentStateVector = false;

/**
 * Forward declaration of downlink output handler
 */
static void SwriteDownlinkWriter ();

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

bool
SaturnIBStack::FillIgmTrace(struct IgmTraceSample_tag *out, double t_sec) const
{
    if (!out || !m_pPlant)
        return false;
    return SaturnIBPlantObliq_fill_igm_trace(m_pPlant, t_sec, out) == 0;
}

bool
SaturnIBStack::FillIcTrace(struct IcTraceSample_tag *out, double t_sec) const
{
    (void)t_sec;
    if (!out)
        return false;
    ic_trace_clear(out);
    /* Obliq plant does not yet expose live <S5> BlockIO; use RTW backend. */
    return false;
}

bool
SaturnIBStack::FillOnPadTrace(struct OnPadTraceSample_tag *out,
                              double t_sec) const
{
    (void)t_sec;
    if (!out)
        return false;
    onpad_trace_clear(out);
    return false;
}

bool
SaturnIBStack::FillSibTrace(struct SibTraceSample_tag *out, double t_sec) const
{
    (void)t_sec;
    if (!out)
        return false;
    sib_trace_clear(out);
    return false;
}

SaturnIBStack::SaturnIBStack(const BatchConfig & config)
    : m_pad_load_file(config.pad_load_file.empty()
                        ? std::string("TM-X-62831_PAD_LOAD.txt")
                        : config.pad_load_file),
      m_pad_load_start_sec(config.pad_load_start_sec < 0
                            ? 15.0
                            : config.pad_load_start_sec)
{
    int nResult;

    g_pSaturnIBStack = this;

    memset (m_dP_feet, 0, sizeof (m_dP_feet));

    // Initialize the Apollo Guidance Computer simulator

    SwritePtr = SwriteDownlinkWriter;

    m_pAGC = (agc_t *) malloc( sizeof( agc_t ) );
    memset( m_pAGC, 0, sizeof(agc_t) );
    nResult = agc_engine_init ( m_pAGC, NULL, NULL, 0 );

    if ( nResult != 0) {
        // log error and exit
    }

    {
        const char * rom = config.rom_path.empty()
                             ? "Colossus249.bin"
                             : config.rom_path.c_str();
        agc_load_binfile( m_pAGC, rom );

        // Upstream yaAGC (2025+) loads downlink list specs from CWD TSV files
        // via dddConfigure(); without this, DecodeDigitalDownlink is a no-op
        // and the simulated downlink screen stays blank.  Expect
        // ddd-version-aliases.tsv and ddd-*-<software>.tsv next to the ROM.
        char soft[64];
        const char *base = strrchr(rom, '/');
        base = base ? base + 1 : rom;
        strncpy(soft, base, sizeof(soft) - 1);
        soft[sizeof(soft) - 1] = '\0';
        size_t n = strlen(soft);
        if (n > 4 && strcmp(soft + n - 4, ".bin") == 0)
            soft[n - 4] = '\0';
        if (soft[0] == '\0')
            strcpy(soft, "Colossus249");
        const int ddd = dddConfigure(soft, "");
        if (ddd < 0)
            std::fprintf(stderr,
                "AGC downlink: dddConfigure(%s) failed — "
                "place ddd-*-%s.tsv and ddd-version-aliases.tsv in CWD\n",
                soft, soft);
        else
            std::fprintf(stderr,
                "AGC downlink: configured for %s (CmOrLm=%d)\n", soft, ddd);
    }

    // Restart phase tables (Colossus -PHASE1..PHASE6 at 00752..00765).
    // GOPROG3 requires (-PHASEn) XOR PHASEn == -0.  Virgin zeros give
    // 0 XOR 0 == +0 → alarm 01107 (phase table failure) then DOFSTART.
    // Match MR.KLEAN idle: -PHASEn = -0 (077777), PHASEn = +0.
    for (int g = 0; g < 6; ++g) {
        const int addr = 00752 + 2 * g;  // -PHASEn
        const int bank = (addr >> 8) & 07;
        const int off  = addr & 0377;
        m_pAGC->Erasable[bank][off]     = 077777;  // -0
        m_pAGC->Erasable[bank][off + 1] = 000000;  // +0
    }
    // MODREG = -0 so GOPROG3 (no active groups) takes ENDRSTRT instead of
    // GOTOP00H.  GOTOP00H flashes V50N07 (MM change request); uplink pad
    // load starting at t≈15 s then hit alarm 01520 (V37 not permitted).
    // -0 matches a clean idle after fresh start without the V50N07 dialog.
    {
        const int addr = 01011;  // MODREG
        m_pAGC->Erasable[(addr >> 8) & 07][addr & 0377] = 077777;
    }

    m_dTotalCDUPulseIncrements = Vector3d(0,0,0);

    m_vLastCM_IMU_PIPA_Pulses = Vector3d(0,0,0);
    m_vPipaPulsesInjectedCum = Vector3d(0,0,0);

    {
        const char *dbg = std::getenv("IMU_DEBUG");
        m_bImuDebug = (dbg != nullptr && dbg[0] != '\0' && std::strcmp(dbg, "0") != 0);
        m_pImuDebugFile = nullptr;
        m_nImuDebugLastSec = -1;
        if (m_bImuDebug)
        {
            m_pImuDebugFile = std::fopen("imu_pipa_cdu.csv", "w");
            if (m_pImuDebugFile)
            {
                std::fprintf(m_pImuDebugFile,
                    "t_sec,liftoff,imu_mode,no_att,pipa_inhibit,"
                    "pipa_plant_x,pipa_plant_y,pipa_plant_z,"
                    "pipa_injected_cum_x,pipa_injected_cum_y,pipa_injected_cum_z,"
                    "pipa_residual_x,pipa_residual_y,pipa_residual_z,"
                    "oga_deg,iga_deg,mga_deg,"
                    "cdu_x_cnt,cdu_y_cnt,cdu_z_cnt,"
                    "cdu_x_deg,cdu_y_deg,cdu_z_deg,"
                    "q_eci_sm_0,q_eci_sm_1,q_eci_sm_2,q_eci_sm_3,"
                    "out11_ve_x,out11_ve_y,out11_ve_z,out11_ve_mag,"
                    "out11_h_m,out11_vb_mps,"
                    "out22_ve_x,out22_ve_y,out22_ve_z,"
                    "out22_xe_x,out22_xe_y,out22_xe_z,"
                    "A_z_deg,"
                    "sm_z_dot_down,sm_x_dot_az,sm_x_elev_deg,"
                    "agc_rn_x,agc_rn_y,agc_rn_z,"
                    "agc_vn_x,agc_vn_y,agc_vn_z,"
                    "agc_piptime_sec,"
                    "refsm_xx,refsm_xy,refsm_xz,"
                    "refsm_yx,refsm_yy,refsm_yz,"
                    "refsm_zx,refsm_zy,refsm_zz,"
                    "rn_err_m,vn_err_mps,"
                    "refsm_x_dot_smx,refsm_z_dot_smz\n");
                std::fflush(m_pImuDebugFile);
                std::fprintf(stderr,
                    "  IMU_DEBUG: logging 1 Hz to imu_pipa_cdu.csv (CWD)\n");
            }
            else
            {
                std::fprintf(stderr,
                    "  IMU_DEBUG: failed to open imu_pipa_cdu.csv for write\n");
                m_bImuDebug = false;
            }
        }
    }
    m_dResidualDeltaT_sec = 0.0;
    m_dAGCResidualDeltaT_sec = 0.0;
    m_dIMUResidualDeltaT_sec = 0.0;

    /************************
     * Initialize Obliq plant *
     ************************/
    m_pszErrorStatus = nullptr;
    memset(&m_inputs, 0, sizeof(m_inputs));
    memset(&m_outputs, 0, sizeof(m_outputs));
    m_outputs.veh_q_ECI[0] = 1.0;

    m_pPlant = SaturnIBPlantObliq_create(MODEL_TIME_STEP_SEC);
    if (m_pPlant == nullptr) {
        (void)fprintf(stderr, "SaturnIBPlantObliq_create failed\n");
        exit(EXIT_FAILURE);
    }

    // Hard-wired (for now) launch date/time/location

    ExternalInputs_saturn_ib_stack * pModelInputs = &m_inputs;

    /*
    pModelInputs->LaunchDate[0] = 1968;
    pModelInputs->LaunchDate[1] = 10;
    pModelInputs->LaunchDate[2] = 11;
    //pModelInputs->LaunchDate[3] = 15;
    //pModelInputs->LaunchDate[4] = 02;
    pModelInputs->LaunchDate[3] = 14;
    pModelInputs->LaunchDate[4] = 57;
    //pModelInputs->LaunchDate[3] = 14;
    //pModelInputs->LaunchDate[4] = 02;
    pModelInputs->LaunchDate[5] = 45.0;
    */

    // Simulation start time is set in SaturnStartupHelper

    int  nYear, nMonth, nDay, nHour, nMin;
    double dSec;

    GetSimClock()->GetCurrentSimulationTime().GetDate( &nYear,  &nMonth,  &nDay, &nHour,  &nMin, &dSec );

    pModelInputs->LaunchDate[0] = nYear;
    pModelInputs->LaunchDate[1] = nMonth;
    pModelInputs->LaunchDate[2] = nDay;
    pModelInputs->LaunchDate[3] = nHour;
    pModelInputs->LaunchDate[4] = nMin;
    pModelInputs->LaunchDate[5] = dSec;

    // Launch azimuth (E of N).  TM-X-62831 / AS-205 desired az = 72°.
    // Older sims used ~82.82°; NaN keeps the TM-X-62831 default.
    pModelInputs->A_z_deg = std::isnan(config.A_z_deg) ? 72.0 : config.A_z_deg;

    pModelInputs->CG_LLA_deg_m[0] = std::isnan(config.CG_LLA_deg_m[0])
        ?  28.521963 : config.CG_LLA_deg_m[0];
    pModelInputs->CG_LLA_deg_m[1] = std::isnan(config.CG_LLA_deg_m[1])
        ? -80.561141 : config.CG_LLA_deg_m[1];
    pModelInputs->CG_LLA_deg_m[2] = std::isnan(config.CG_LLA_deg_m[2])
        ?  34.7      : config.CG_LLA_deg_m[2];

    if (std::isnan(config.q_ECItoSM[0])) {
        // Preserve the historical initialisation exactly, including the
        // pModelInputs->q_ECItoSM[0]-in-the-loop indexing quirk from the
        // original 2009 code.  Net effect for Quaternion(0,0,0): index 0
        // ends up 0.0 and indices 1..3 are left at whatever the model
        // allocation set them to.
        sim::Quaternion q( 0.0, 0.0, 0.0 );
        for( int i=0; i<4; ++i )
        {
            pModelInputs->q_ECItoSM[0] = q.GetE( i );
        }
    } else {
        for (int i = 0; i < 4; ++i)
            pModelInputs->q_ECItoSM[i] = config.q_ECItoSM[i];
    }

    // Time interval from start of simulation to liftoff (5 minutes).
    pModelInputs->T_L_prime_sec = (config.T_L_prime_sec < 0)
                                    ? 300.0
                                    : config.T_L_prime_sec;

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

    m_bCoarseAlignFastRate[0] = false;
    m_bCoarseAlignFastRate[1] = false;
    m_bCoarseAlignFastRate[2] = false;

    m_bCoarseAlignEnable = false;

    m_bErrorCounterEnable = false;

    m_eFineAlignAxis = FA_NO_OPERATION;

    m_dFineAlignPulseCount = 0;
    m_nFineAlignPulseSign = 1;

    m_bLiftoff = false;

    m_bFirstPass = true;

    m_bFirstResetReceived = false;

    m_pCurrentUplinkMessage = NULL;
    m_dNextUplinkTime_sec = -1.0;

    strncpy_s( m_szDebugText, sizeof(m_szDebugText), "", 1 );

    strncpy_s( m_szDownlinkText, sizeof(m_szDownlinkText), "", 1 );

    m_eIMUMode = ISS_IMU_TURN_ON;
    m_bNoAttLampOn = true;               // ISS warm-up: NO ATT lit until turn-on complete
    m_bIssTurnOnDelayComplete = false;
    m_dNoAttOffElapsed_sec = -1.0;
    m_vCoarseAlignCommand_deg = Vector3d( 0, 0, 0 );

    m_mLVtoCM.EulerToMatrix4d( M_PI, 0.0, 0.0 );

    // Mark IMU as "on" and "IMU Turn On request"

    unsigned short usValue;

    usValue = 0;
    usValue |= ReadIO ( m_pAGC, 030 ) & ~(BIT14 | BIT9);
    WriteIO ( m_pAGC, 030, usValue );

    // ch031 BIT15 (IMU power): 0 = ON.  We already asserted IMU
    // OPERATE on ch030, so make ch031 agree.  Leave BIT14/BIT13
    // (CMC MODE FREE/HOLD) at the yaAGC default of 1 -- earlier code
    // here cleared BIT14 as "Free" but the DAP treats BIT14=1 as
    // Free (see RCS-CSM_DIGITAL_AUTOPILOT.s), so clearing it was
    // inverted.  Nominal MODE is HOLD at startup, which is the
    // default already (BIT13 = 1 doesn't matter -- both mode bits
    // read via RAND).
    usValue = ReadIO ( m_pAGC, 031 ) & ~BIT15;
    WriteIO ( m_pAGC, 031, usValue );

    // ch032 must stay at yaAGC's default (077777).  A previous version
    // cleared BIT14 (PROCEED) and BIT1..BIT6, but all of these are
    // active-low pushbutton / min-impulse discretes -- clearing them
    // asserts PRO held down and six simultaneous jet commands from
    // t=0, which prevents V37 program-change dialogs from completing
    // (the AGC sees PRO already asserted and auto-proceeds).

    // Leave ch033 at yaAGC default (077777): AGC WARNING / PIPA FAIL /
    // OSC FAIL all inactive (1).  Cleared BIT14 (AGC WARNING asserted) was
    // previously used to force DOFSTART; with phase tables initialized
    // above, cold-start GOJAM→GOPROG3 no longer needs that, and asserting
    // WARNING made 01107 + PROG light more likely to stick after DOFSTART
    // (DOFSTART does not clear FAILREG; only V36/SLAP1 or RSET does).

    UpdateDebugText();

}

SaturnIBStack::~SaturnIBStack(void)
{
    if (m_pImuDebugFile) {
        std::fclose(m_pImuDebugFile);
        m_pImuDebugFile = nullptr;
    }
    if (m_pPlant) {
        SaturnIBPlantObliq_destroy(m_pPlant);
        m_pPlant = nullptr;
    }
}



bool
SaturnIBStack::TimeStep( double dDeltaT_sec )
{
    double dActualDeltaT_sec = m_dResidualDeltaT_sec + dDeltaT_sec;
    int nResult;
    sim::Quaternion qECItoSM;
    ExternalOutputs_saturn_ib_stack * pModelOut = &m_outputs;

    // Debug: 1 Hz snapshot of IMU state (kept, disabled).  Was used to
    // verify the fine-align physics fix that routes gyro-torque pulses
    // through m_qECItoSM instead of directly into gimbal resolvers.
    // Re-enable by removing the #if 0 to debug related IMU issues.
#if 0
    {
        double t = GetSimClock()->GetElapsedTime();
        static int last_sec = -1;
        int now_sec = int(t);
        if (now_sec != last_sec) {
            last_sec = now_sec;
            double sm_phi_rad, sm_theta_rad, sm_psi_rad;
            sim::Matrix4d mECItoSM_now = m_qECItoSM.QuaternionToMatrix4d();
            MIStoEuler(mECItoSM_now, &sm_phi_rad, &sm_theta_rad, &sm_psi_rad);
            static const char *mode_name[] = {
                "TURN_ON", "COARSE_ALIGN", "FINE_ALIGN", "CAGE",
                "INERTIAL_REFERENCE", "ATTITUDE_ERROR_DISPLAY"
            };
            std::fprintf(stderr,
                "IMU_STATE  t=%9.3f  mode=%s  "
                "OGA=%+9.4f  IGA=%+9.4f  MGA=%+9.4f  "
                "SM_phi=%+9.4f  SM_theta=%+9.4f  SM_psi=%+9.4f  "
                "qE2S=(%+.9f,%+.9f,%+.9f,%+.9f)  "
                "qE2CM=(%+.9f,%+.9f,%+.9f,%+.9f)\n",
                t, mode_name[(int)m_eIMUMode],
                m_dIMU_OGAResolver_deg,
                m_dIMU_IGAResolver_deg,
                m_dIMU_MGAResolver_deg,
                RADtoDEG(sm_phi_rad),
                RADtoDEG(sm_theta_rad),
                RADtoDEG(sm_psi_rad),
                m_qECItoSM.GetE(0), m_qECItoSM.GetE(1),
                m_qECItoSM.GetE(2), m_qECItoSM.GetE(3),
                pModelOut->veh_q_ECI[0], pModelOut->veh_q_ECI[1],
                pModelOut->veh_q_ECI[2], pModelOut->veh_q_ECI[3]);
        }
    }
#endif

    // Send simulated pad load as an uplink.  BatchConfig may override
    // the file path, threshold time, or disable entirely ("-").
    if ( bSentFirstUplink == false
         && m_pad_load_file != "-"
         && GetSimClock()->GetElapsedTime() >= m_pad_load_start_sec )
    {
        if ( StartUplink( m_pad_load_file ) )
        {
            std::fprintf(stderr, "  PAD LOAD STARTED at elapsed=%.3f sec (file=%s)\n",
                         GetSimClock()->GetElapsedTime(),
                         m_pad_load_file.c_str());
            LogInfo("PAD LOAD STARTED\n");
        }
        else
        {
            LogInfo("PAD LOAD file missing\n");
        }

        bSentFirstUplink = true;
    }

    // After pad-load finishes: wait until DSKY NO ATT is off, then an extra
    // SV_AFTER_NOATT_OFF_SEC (45 s) so the pilot can select P02 (P27-legal)
    // before the automatic V71 state-vector uplink.  P27 is allowed in P00/P02
    // only — not P01 — so SV must run while still in P00 or after reaching P02.
    //
    // dTime is time from CMC clock zero (elapsed ≈ AGC TIME with TEPHEM=sim start).
    const double elapsed = GetSimClock()->GetElapsedTime();
    const bool noAttGateOk =
        !m_bNoAttLampOn
        && m_dNoAttOffElapsed_sec >= 0.0
        && elapsed >= m_dNoAttOffElapsed_sec + SV_AFTER_NOATT_OFF_SEC;

    if ( bSentStateVector == false
         && bSentFirstUplink == true
         && m_pCurrentUplinkMessage == NULL
         && noAttGateOk )
    {
        bSentStateVector = true;

        m_pCurrentUplinkMessage = new UplinkMessage();

        // Live OUT22 on-pad ECI (same source rt_OneStep uses pre-liftoff).
        Vector3d vECIPos_meters = Vector3d( pModelOut->OUT22[OUT22_Xe_x_m],
                                            pModelOut->OUT22[OUT22_Xe_y_m],
                                            pModelOut->OUT22[OUT22_Xe_z_m] );
        Vector3d vECIVel_mps = Vector3d( pModelOut->OUT22[OUT22_Ve_x_mps],
                                         pModelOut->OUT22[OUT22_Ve_y_mps],
                                         pModelOut->OUT22[OUT22_Ve_z_mps] );

        // Time from CMC clock zero.  With TEPHEM = sim start and fresh-start
        // clock, elapsed ≈ TIME2/TIME1.  P27 stores this as TET.
        double dTime_sec = GetSimClock()->GetElapsedTime();

        const double r_mag = std::sqrt(
            vECIPos_meters.x * vECIPos_meters.x +
            vECIPos_meters.y * vECIPos_meters.y +
            vECIPos_meters.z * vECIPos_meters.z );
        const double v_mag = std::sqrt(
            vECIVel_mps.x * vECIVel_mps.x +
            vECIVel_mps.y * vECIVel_mps.y +
            vECIVel_mps.z * vECIVel_mps.z );

        // Sanity: pad ECI |R| should be ~WGS radius (~6.37e6 m), |V| ~0.4 km/s.
        if ( r_mag < 1.0e6 || r_mag > 1.0e7 )
        {
            std::fprintf(stderr,
                "  STATE VECTOR UPLINK SKIPPED: |R|=%.0f m out of Earth-pad range "
                "(expected ~6.4e6). Check OUT22.\n", r_mag);
            LogInfo("STATE VECTOR UPLINK SKIPPED (bad |R|)\n");
            delete m_pCurrentUplinkMessage;
            m_pCurrentUplinkMessage = NULL;
            // Leave bSentStateVector true so we do not retry-spam.
        }
        else
        {
            std::fprintf(stderr,
                "  STATE VECTOR UPLINK STARTED at elapsed=%.3f sec\n"
                "    R_ECI=[%.1f, %.1f, %.1f] m   |R|=%.1f m\n"
                "    V_ECI=[%.4f, %.4f, %.4f] m/s |V|=%.4f m/s\n"
                "    T_clock=%.3f s (from CMC zero)  id=CSM/Earth  ECADR=01501\n"
                "    preamble: V37E 00E then V71E (need P00 or P02)\n"
                "    gate: NO ATT off at %.3f + %.0f s\n",
                dTime_sec,
                vECIPos_meters.x, vECIPos_meters.y, vECIPos_meters.z, r_mag,
                vECIVel_mps.x, vECIVel_mps.y, vECIVel_mps.z, v_mag,
                dTime_sec,
                m_dNoAttOffElapsed_sec,
                SV_AFTER_NOATT_OFF_SEC);
            LogInfo("STATE VECTOR UPLINK STARTED\n");

            m_pCurrentUplinkMessage->GenerateStateVectorUpdate( vECIPos_meters,
                                                                vECIVel_mps,
                                                                dTime_sec );

            int nKeycode = m_pCurrentUplinkMessage->GetFirstKeycode();

            WriteUplinkKeycode( nKeycode );
        }
    }

    // Recover current IMU Gimbal angles (by computing the CMtoSM transform)
    //
    // (Any difference since the last time step would be the result of body orientation changes)

    Quaternion qECItoLV( pModelOut->veh_q_ECI[0],
                         pModelOut->veh_q_ECI[1],
                         pModelOut->veh_q_ECI[2],
                         pModelOut->veh_q_ECI[3] );

    Matrix4d mECItoCM = m_mLVtoCM * qECItoLV.QuaternionToMatrix4d();
    Matrix4d mCMtoECI = mECItoCM.Transpose();
    Matrix4d mECItoSM = m_qECItoSM.QuaternionToMatrix4d();
    Matrix4d mCMtoSM = mECItoSM * mCMtoECI;

    double dOGA_rad;
    double dIGA_rad;
    double dMGA_rad;

    MIStoEuler( mCMtoSM, &dOGA_rad, &dIGA_rad, &dMGA_rad );

    m_dIMU_OGAResolver_deg = RADtoDEG( dOGA_rad );
    m_dIMU_IGAResolver_deg = RADtoDEG( dIGA_rad );
    m_dIMU_MGAResolver_deg = RADtoDEG( dMGA_rad );

    // Perform one time step of the aggregate IMU/AGC/Vehicle model

    while ( dActualDeltaT_sec >= MODEL_TIME_STEP_SEC )
    {

        // Send next data uplink word to the CMC (AGC) -- if an uplink is active

        UplinkTimeStep();

        // First step the IMU model and the AGC

        double dIMUActualDeltaT_sec = m_dIMUResidualDeltaT_sec + MODEL_TIME_STEP_SEC;

        while ( dIMUActualDeltaT_sec >= STABLE_MEMBER_MODEL_TIME_STEP_SEC )
        {

            Vector3d vGimbalRates_deg_per_sec( 0.0, 0.0, 0.0 );

            // Determine current CDU/IMU operating mode

            UpdateIMUMode();

            switch ( m_eIMUMode )
            {
            default:
                break;

            case ISS_IMU_FINE_ALIGN:
                {
                    // Apollo IRIG scale factor: 0.549 deg/s at 3200 pulses/s
                    // = 0.000172 deg/pulse (matches Apollo hardware
                    // spec of ~0.615 arcsec/pulse).  Value used is from LM SIM.
                    const double dFineRate_deg_per_sec = 0.54931644444444444444444444444444;

                    const double dPulseCoundownRate_pulses_per_step = 3200.0 * STABLE_MEMBER_MODEL_TIME_STEP_SEC;
                    double dGimbalRate_deg_per_sec = 0.0;
                    bool bDone = false;

                    // Debug: fine-align session-begin trace (kept, disabled).
#if 0
                    static ISS_Fine_Align_Axis last_active_axis = FA_NO_OPERATION;
                    if (m_eFineAlignAxis != last_active_axis) {
                        static const char *axis_name[] = {"NONE","X","Y","Z"};
                        std::fprintf(stderr,
                            "FA_BEGIN   t=%9.3f  axis=%s  sign=%+d  pulses_pending=%.1f  "
                            "qE2S=(%+.9f,%+.9f,%+.9f,%+.9f)\n",
                            GetSimClock()->GetElapsedTime(),
                            axis_name[m_eFineAlignAxis], m_nFineAlignPulseSign,
                            m_dFineAlignPulseCount,
                            m_qECItoSM.GetE(0), m_qECItoSM.GetE(1),
                            m_qECItoSM.GetE(2), m_qECItoSM.GetE(3));
                        last_active_axis = m_eFineAlignAxis;
                    }
#endif

                    // How many pulses does this 1 ms tick consume?  Cap at
                    // pending count so we don't over-torque on the last tick.
                    double dPulsesThisStep;
                    if ( m_dFineAlignPulseCount > dPulseCoundownRate_pulses_per_step )
                    {
                        dPulsesThisStep = dPulseCoundownRate_pulses_per_step;
                        dGimbalRate_deg_per_sec = dFineRate_deg_per_sec;
                        m_dFineAlignPulseCount -= dPulseCoundownRate_pulses_per_step;
                    }
                    else
                    {
                        dPulsesThisStep = m_dFineAlignPulseCount;
                        dGimbalRate_deg_per_sec = dFineRate_deg_per_sec *
                                                  m_dFineAlignPulseCount /
                                                  dPulseCoundownRate_pulses_per_step;
                        m_dFineAlignPulseCount = 0.0;
                        bDone = true;
                    }

                    // ---- Physical model (corrected 2026-08-17) --------------
                    // Gyro torque pulses rotate the SM in SM-body frame about
                    // the pulsed axis.  Empirically verified conventions:
                    //   * right-multiply: q_ECI->SM_new = q_ECI->SM_old * q_delta
                    //   * sign=+1 -> +rotation about SM axis; sign=-1 -> -rotation
                    //   * X pulse -> SM X axis; Y -> SM Y; Z -> SM Z (direct)
                    // The old code applied the rate directly to gimbal resolvers,
                    // which (a) routed the torque through the coarse-align motor
                    // path (wrong physics) and (b) lost effect near IGA=+/-90
                    // because the resolver-to-SM Jacobian is singular there.
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

                    // Recompute resolvers from the new SM orientation so the
                    // CDU pulse-delta code (AGCUpdateGimbalCDUCounters below)
                    // reports the physically-correct gimbal change back to
                    // the AGC.  vGimbalRates_deg_per_sec stays zero for this
                    // case so the "direct integration" block below is a no-op.
                    {
                        Matrix4d m_newECItoSM = m_qECItoSM.QuaternionToMatrix4d();
                        Matrix4d m_newCMtoSM  = m_newECItoSM * mCMtoECI;
                        double dNewOGA_rad, dNewIGA_rad, dNewMGA_rad;
                        MIStoEuler(m_newCMtoSM,
                                   &dNewOGA_rad, &dNewIGA_rad, &dNewMGA_rad);
                        m_dIMU_OGAResolver_deg = RADtoDEG(dNewOGA_rad);
                        m_dIMU_IGAResolver_deg = RADtoDEG(dNewIGA_rad);
                        m_dIMU_MGAResolver_deg = RADtoDEG(dNewMGA_rad);
                    }

                    if ( bDone )
                    {
                        // Debug: fine-align session-end trace (kept, disabled).
#if 0
                        static const char *axis_name[] = {"NONE","X","Y","Z"};
                        std::fprintf(stderr,
                            "FA_END     t=%9.3f  axis=%s  sign=%+d  "
                            "gimbal_rate=%+8.4f  step_dt=%.5f\n",
                            GetSimClock()->GetElapsedTime(),
                            axis_name[m_eFineAlignAxis], m_nFineAlignPulseSign,
                            dGimbalRate_deg_per_sec,
                            STABLE_MEMBER_MODEL_TIME_STEP_SEC);
#endif
                        m_eFineAlignAxis = FA_NO_OPERATION;
                    }
                }
                break;

            // In the case of turn-on mode, we're driving the gimbal angles to zero

            case ISS_IMU_TURN_ON:
            case ISS_IMU_COARSE_ALIGN:
                {
                    Vector3d vQuantizedCurrentAngles_deg;
                    const double dOneCoarseStep_deg = 2.8125;
                    const double dMaxFastCoarseError_deg = 1.2;
                    const double dMaxSlowCoarseError_deg = 0.07;
                    const double dCoarseAlignFastRate_deg_per_sec = 35.0;
                    const double dCoarseAlignSlowRate_deg_per_sec = 4.45;
                    double dGimbalRate_deg_per_sec = 0.0;

                    vQuantizedCurrentAngles_deg = Vector3d(
                        FineResolverQuantization(m_dIMU_OGAResolver_deg),
                        FineResolverQuantization(m_dIMU_IGAResolver_deg),
                        FineResolverQuantization(m_dIMU_MGAResolver_deg));

                    m_vCoarseAlignError_deg = m_vCoarseAlignCommand_deg - vQuantizedCurrentAngles_deg;

                    // X-Gimbal

                    static int n = 0;

                    // Log every 1/10th second in CA mode
                    if ( n++ % 100 ) {
                        LogError1( "CA X %.2f deg\n", m_vCoarseAlignError_deg.x );
                    }

                    // Normalize error signal
                    if ( m_vCoarseAlignError_deg.x < - 180.0 )
                    {
                        m_vCoarseAlignError_deg.x += 360.0;
                    }
                    else if ( m_vCoarseAlignError_deg.x > 180.0 )
                    {
                        m_vCoarseAlignError_deg.x -= 360.0;
                    }

                    m_bCoarseAlignFastRate[0] = false;

                    // determine gimbal slew rate
                    if (m_vCoarseAlignError_deg.x <= -dMaxFastCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = -dCoarseAlignFastRate_deg_per_sec;
                        m_bCoarseAlignFastRate[0] = true;
                    }
                    if (m_vCoarseAlignError_deg.x <= -dMaxSlowCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = -dCoarseAlignSlowRate_deg_per_sec;
                    }
                    else if (m_vCoarseAlignError_deg.x >= dMaxFastCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = dCoarseAlignFastRate_deg_per_sec;
                        m_bCoarseAlignFastRate[0] = true;
                    }
                    else if (m_vCoarseAlignError_deg.x >= dMaxSlowCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = dCoarseAlignSlowRate_deg_per_sec;
                    }
                    else
                    {
                        dGimbalRate_deg_per_sec = 0.0;
                    }

                    vGimbalRates_deg_per_sec.x = dGimbalRate_deg_per_sec;

                    // Y-Gimbal

                    // Normalize error signal
                    if ( m_vCoarseAlignError_deg.y < - 180.0 )
                    {
                        m_vCoarseAlignError_deg.y += 360.0;
                    }
                    else if ( m_vCoarseAlignError_deg.y > 180.0 )
                    {
                        m_vCoarseAlignError_deg.y -= 360.0;
                    }

                    m_bCoarseAlignFastRate[1] = false;

                    // determine gimbal slew rate
                    if (m_vCoarseAlignError_deg.y <= -dMaxFastCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = -dCoarseAlignFastRate_deg_per_sec;
                        m_bCoarseAlignFastRate[1] = true;
                    }
                    if (m_vCoarseAlignError_deg.y <= -dMaxSlowCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = -dCoarseAlignSlowRate_deg_per_sec;
                    }
                    else if (m_vCoarseAlignError_deg.y >= dMaxFastCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = dCoarseAlignFastRate_deg_per_sec;
                        m_bCoarseAlignFastRate[1] = true;
                    }
                    else if (m_vCoarseAlignError_deg.y >= dMaxSlowCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = dCoarseAlignSlowRate_deg_per_sec;
                    }
                    else
                    {
                        dGimbalRate_deg_per_sec = 0.0;
                    }

                    vGimbalRates_deg_per_sec.y = dGimbalRate_deg_per_sec;

                    // Z-Gimbal

                    // Normalize error signal
                    if ( m_vCoarseAlignError_deg.z < - 180.0 )
                    {
                        m_vCoarseAlignError_deg.z += 360.0;
                    }
                    else if ( m_vCoarseAlignError_deg.z > 180.0 )
                    {
                        m_vCoarseAlignError_deg.z -= 360.0;
                    }

                    m_bCoarseAlignFastRate[2] = false;

                    // determine gimbal slew rate
                    if (m_vCoarseAlignError_deg.z <= -dMaxFastCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = -dCoarseAlignFastRate_deg_per_sec;
                        m_bCoarseAlignFastRate[2] = true;
                    }
                    if (m_vCoarseAlignError_deg.z <= -dMaxSlowCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = -dCoarseAlignSlowRate_deg_per_sec;
                    }
                    else if (m_vCoarseAlignError_deg.z >= dMaxFastCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = dCoarseAlignFastRate_deg_per_sec;
                        m_bCoarseAlignFastRate[2] = true;
                    }
                    else if (m_vCoarseAlignError_deg.z >= dMaxSlowCoarseError_deg)
                    {
                        dGimbalRate_deg_per_sec = dCoarseAlignSlowRate_deg_per_sec;
                    }
                    else
                    {
                        dGimbalRate_deg_per_sec = 0.0;
                    }

                    vGimbalRates_deg_per_sec.z = dGimbalRate_deg_per_sec;
                }
                break;
            }

            // "Integrate" IMU Gimbal angle changes due to torquers
            m_dIMU_OGAResolver_deg += vGimbalRates_deg_per_sec.x * STABLE_MEMBER_MODEL_TIME_STEP_SEC;
            m_dIMU_IGAResolver_deg += vGimbalRates_deg_per_sec.y * STABLE_MEMBER_MODEL_TIME_STEP_SEC;
            m_dIMU_MGAResolver_deg += vGimbalRates_deg_per_sec.z * STABLE_MEMBER_MODEL_TIME_STEP_SEC;

            // Emit unprogrammed increments to AGC for gimbal angle changes
            AGCUpdateGimbalCDUCounters();

            // Step the AGC simulator an appropriate number of
            // CPU cycles
            double dACGStep_sec = m_dAGCResidualDeltaT_sec + STABLE_MEMBER_MODEL_TIME_STEP_SEC;

            // Run the AGC

            while ( dACGStep_sec >= AGC_CPU_CYCLE_SEC )
            {
                nResult = agc_engine ( m_pAGC );
                dACGStep_sec -= AGC_CPU_CYCLE_SEC;
            }

            m_dAGCResidualDeltaT_sec = dACGStep_sec;

            dIMUActualDeltaT_sec -= STABLE_MEMBER_MODEL_TIME_STEP_SEC;

        }

        m_dIMUResidualDeltaT_sec = dIMUActualDeltaT_sec;

        // Propagate updated Stable Member orientation to Saturn IB model
        // in preparation of running the next vehicle dynamics time step.
        //
        // In FINE_ALIGN mode m_qECItoSM has already been updated by the
        // gyro-torque path above and the resolvers were recomputed from
        // it there.  Rebuilding m_qECItoSM here from those resolvers is
        // a lossy round-trip (Euler <-> matrix <-> quaternion), so we
        // gate it out during FINE_ALIGN.  Other modes still take the
        // round-trip because COARSE_ALIGN drives resolvers via motor
        // commands and needs the SM to follow them.
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
        {
            pModelInputs->q_ECItoSM[i] = m_qECItoSM.GetE( i );
        }

        // Step the Saturn IB vehicle model

        rt_OneStep();

        // Emit unprogrammed increments to AGC to account for measured velocity changes

        AGCUpdatePIPACounters();

        dActualDeltaT_sec -= MODEL_TIME_STEP_SEC;
    }

    // Record residual for next call to TimeStep()

    m_dResidualDeltaT_sec = dActualDeltaT_sec;

    m_bFirstPass = false;

    MaybeLogImuPipaCdu();

    // One-shot: first non-zero FAILREG → identify DSKY PROG alarm cause.
    // FAILREG is unswitched at 00375..00377.  V05N09; RESET clears PROG.
    if (m_pAGC) {
        static bool s_loggedFailreg = false;
        const unsigned fr0 = (unsigned)(m_pAGC->Erasable[0][0375] & 077777);
        const unsigned fr1 = (unsigned)(m_pAGC->Erasable[0][0376] & 077777);
        const unsigned fr2 = (unsigned)(m_pAGC->Erasable[0][0377] & 077777);
        if (!s_loggedFailreg && fr0 != 0) {
            s_loggedFailreg = true;
            std::fprintf(stderr,
                "  AGC PROGRAM ALARM at elapsed=%.3f sec: "
                "FAILREG=%05o %05o %05o  (V05N09; RESET clears PROG light)\n",
                GetSimClock()->GetElapsedTime(), fr0, fr1, fr2);
        }
    }

    return false;
}

void
SaturnIBStack::ISSTurnOnDelayComplete()
{
    // Respond to AGC's 90 second ISS turn "on delay complete" signal

    // Mark IMU as "ON"; turn off delay request

    unsigned short usValue;

    usValue = 0;
    usValue |= ReadIO ( m_pAGC, 030 ) & ~(BIT9);
    WriteIO ( m_pAGC, 030, usValue | BIT14 );

    m_eIMUMode = ISS_IMU_COARSE_ALIGN;
    UpdateIMUMode();

    m_bIssTurnOnDelayComplete = true;
    std::fprintf(stderr,
        "  ISS turn-on delay complete at elapsed=%.3f sec\n",
        GetSimClock()->GetElapsedTime());

    // If DSPTAB already shows NO ATT off (edge happened before ch012 BIT15),
    // arm now — otherwise wait for the next on→off in NotifyNoAttLamp.
    if (!m_bNoAttLampOn)
        ArmStateVectorAfterNoAttOff(GetSimClock()->GetElapsedTime());
}

void
SaturnIBStack::ArmStateVectorAfterNoAttOff(double elapsed_sec)
{
    if (m_dNoAttOffElapsed_sec >= 0.0)
        return;  // already armed — ignore P01/P02 lamp cycles
    if (!m_bIssTurnOnDelayComplete)
        return;  // ignore pre-ISS-complete DSPTAB clears (caused SV at ~90 s)

    m_dNoAttOffElapsed_sec = elapsed_sec;
    std::fprintf(stderr,
        "  NO ATT OFF (SV arm) at elapsed=%.3f sec — state-vector uplink in +%.0f s "
        "(enter P00 before then; then P01→P02 after SV)\n",
        m_dNoAttOffElapsed_sec, SV_AFTER_NOATT_OFF_SEC);
    LogInfo("NO ATT OFF (SV arm)\n");
}

void
SaturnIBStack::NotifyNoAttLamp(bool lampOn)
{
    // Same bit as panel NOATT_LIGHT.  Arm only after ISS turn-on delay complete,
    // and only once (first valid extinguish).
    if (m_bNoAttLampOn && !lampOn)
        ArmStateVectorAfterNoAttOff(GetSimClock()->GetElapsedTime());
    m_bNoAttLampOn = lampOn;
}

/**
 * Format an Apollo-style mission clock from simulation elapsed time.
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

    if (bPrelaunch) {
        snprintf( szBuf, sizeof(szBuf), "- %02ld:%02d:%02d", nHours, nMin, nSec );
    } else {
        snprintf( szBuf, sizeof(szBuf), "+ %ld:%02d:%02d", nHours, nMin, nSec );
    }
    return szBuf;
}

static const char *ImuModeName(ISS_IMU_Mode mode)
{
    switch (mode) {
    case ISS_IMU_TURN_ON: return "TURN_ON";
    case ISS_IMU_COARSE_ALIGN: return "COARSE_ALIGN";
    case ISS_IMU_FINE_ALIGN: return "FINE_ALIGN";
    case ISS_IMU_CAGE: return "CAGE";
    case ISS_IMU_INERTIAL_REFERENCE: return "INERTIAL_REFERENCE";
    case ISS_IMU_ATTITUDE_ERROR_DISPLAY: return "ATT_ERR_DISP";
    default: return "UNKNOWN";
    }
}

static int CduRegToSignedCount(unsigned short raw)
{
    int v = (int)(raw & 077777);
    if (v & 040000)
        v -= 0100000;
    return v;
}

// Colossus249 absolute erasable (from ERASABLE_ASSIGNMENTS.s).
static const int AGC_ADDR_RN       = 01170;  // B(6) pos, Earth B-29 metres
static const int AGC_ADDR_VN       = 01176;  // B(6) vel, Earth B-7 m/csec
static const int AGC_ADDR_PIPTIME  = 01204;  // B(2) csec B-28
static const int AGC_ADDR_REFSMMAT = 01735;  // I(18) 3x3, unit vectors B-1

/** yaAGC Erasable[bank][offset] for a 12-bit absolute erasable address. */
static int16_t AgcErasableWord(const agc_t *agc, int addr12)
{
    const int a = addr12 & 07777;
    return agc->Erasable[(a >> 8) & 07][a & 0377];
}

/**
 * Decode AGC DP (two SP ones'-complement words) to engineering units.
 * Matches UplinkMessage::GenerateStateVectorUpdate encoding:
 *   nScaled = |value| / scale_factor * 2^28, split across two 14-bit halves.
 */
static double AgcDecodeDP(int16_t hi, int16_t lo, double scale_factor)
{
    int h = hi & 077777;
    int l = lo & 077777;
    const bool neg = (h & 040000) != 0;
    if (neg)
    {
        h = (~h) & 077777;
        l = (~l) & 077777;
    }
    // Keep 14-bit magnitudes (bit15 is sign/parity in SP; encode used 037777).
    h &= 037777;
    l &= 037777;
    const long nScaled = ((long)h << 14) | (long)l;
    const double mag = (double)nScaled / 268435456.0 * scale_factor;
    return neg ? -mag : mag;
}

static void AgcDecodeDPVector(const agc_t *agc, int addr0,
                              double scale, double out[3])
{
    for (int i = 0; i < 3; ++i)
    {
        const int16_t hi = AgcErasableWord(agc, addr0 + 2 * i);
        const int16_t lo = AgcErasableWord(agc, addr0 + 2 * i + 1);
        out[i] = AgcDecodeDP(hi, lo, scale);
    }
}

void
SaturnIBStack::MaybeLogImuPipaCdu()
{
    if (!m_bImuDebug || !m_pImuDebugFile || !m_pAGC)
        return;

    const double t = GetSimClock()->GetElapsedTime();
    const int sec = (int)t;
    if (sec == m_nImuDebugLastSec)
        return;
    m_nImuDebugLastSec = sec;

    // Matches AGCUpdatePIPACounters: no inject before liftoff, else mode gate.
    const bool pipaInhibit =
        !m_bLiftoff ||
        !(m_eIMUMode == ISS_IMU_INERTIAL_REFERENCE ||
          m_eIMUMode == ISS_IMU_ATTITUDE_ERROR_DISPLAY ||
          m_eIMUMode == ISS_IMU_FINE_ALIGN);

    const int cdu_x = CduRegToSignedCount(m_pAGC->Erasable[0][RegCDUX]);
    const int cdu_y = CduRegToSignedCount(m_pAGC->Erasable[0][RegCDUY]);
    const int cdu_z = CduRegToSignedCount(m_pAGC->Erasable[0][RegCDUZ]);
    const double cdu_x_deg = cdu_x * 360.0 / 32768.0;
    const double cdu_y_deg = cdu_y * 360.0 / 32768.0;
    const double cdu_z_deg = cdu_z * 360.0 / 32768.0;

    const ExternalOutputs_saturn_ib_stack *out = &m_outputs;
    const double vex = out->OUT11[OUT11_Ve_x_mps];
    const double vey = out->OUT11[OUT11_Ve_y_mps];
    const double vez = out->OUT11[OUT11_Ve_z_mps];
    const double vemag = std::sqrt(vex * vex + vey * vey + vez * vez);

    const double res_x = m_vCM_IMU_PIPA_Pulses.x - m_vLastCM_IMU_PIPA_Pulses.x;
    const double res_y = m_vCM_IMU_PIPA_Pulses.y - m_vLastCM_IMU_PIPA_Pulses.y;
    const double res_z = m_vCM_IMU_PIPA_Pulses.z - m_vLastCM_IMU_PIPA_Pulses.z;

    // GSOP preferred SM check: X along A_z (horizontal), Z down.
    // QuaternionToMatrix4d columns are SM axes expressed in ECI.
    const double xe = out->OUT22[OUT22_Xe_x_m];
    const double ye = out->OUT22[OUT22_Xe_y_m];
    const double ze = out->OUT22[OUT22_Xe_z_m];
    const double rmag = std::sqrt(xe * xe + ye * ye + ze * ze);
    double sm_z_dot_down = 0.0;
    double sm_x_dot_az = 0.0;
    double sm_x_elev_deg = 0.0;
    double xsm_x = 0.0, xsm_y = 0.0, xsm_z = 0.0;
    double zsm_x = 0.0, zsm_y = 0.0, zsm_z = 0.0;
    const double az_deg = m_inputs.A_z_deg;
    if (rmag > 1.0) {
        const double up_x = xe / rmag;
        const double up_y = ye / rmag;
        const double up_z = ze / rmag;
        // east = UNIT(Z_eci × up), north = up × east
        double east_x = -up_y;
        double east_y = up_x;
        double east_z = 0.0;
        const double east_mag = std::sqrt(east_x * east_x + east_y * east_y);
        if (east_mag > 1e-12) {
            east_x /= east_mag;
            east_y /= east_mag;
            const double north_x = up_y * east_z - up_z * east_y;
            const double north_y = up_z * east_x - up_x * east_z;
            const double north_z = up_x * east_y - up_y * east_x;
            const double az_rad = az_deg * (M_PI / 180.0);
            const double caz = std::cos(az_rad);
            const double saz = std::sin(az_rad);
            const double az_x = caz * north_x + saz * east_x;
            const double az_y = caz * north_y + saz * east_y;
            const double az_z = caz * north_z + saz * east_z;

            Matrix4d mECItoSM = m_qECItoSM.QuaternionToMatrix4d();
            // QuaternionToMatrix4d writes Hamilton R into m[row][col].
            // mat(i,j) returns m[j][i] (transposed view) — so read storage
            // columns directly: they are SM axes in ECI at preferred LO.
            xsm_x = mECItoSM.m[0][0];
            xsm_y = mECItoSM.m[1][0];
            xsm_z = mECItoSM.m[2][0];
            zsm_x = mECItoSM.m[0][2];
            zsm_y = mECItoSM.m[1][2];
            zsm_z = mECItoSM.m[2][2];

            sm_z_dot_down = -(zsm_x * up_x + zsm_y * up_y + zsm_z * up_z);
            sm_x_dot_az = xsm_x * az_x + xsm_y * az_y + xsm_z * az_z;
            const double x_up = xsm_x * up_x + xsm_y * up_y + xsm_z * up_z;
            sm_x_elev_deg = std::asin(std::max(-1.0, std::min(1.0, x_up))) * (180.0 / M_PI);
        }
    }

    // AGC SERVICER state (Colossus249): RN B-29 m, VN B-7 m/csec → m/s,
    // REFSMMAT rows = SM axes in REF at B-1 (true = AGC_frac * 1/2).
    const double POS_SCALE = 536870912.0;      // 2^29
    const double VEL_SCALE = 128.0 * 100.0;    // B-7 m/csec ← m/s
    const double TIME_SCALE = 268435456.0;     // 2^28 → centiseconds
    // B-1: true = AGC_frac * 2^(+1). (Using 0.5 made ||row||≈0.25.)
    const double REFSM_SCALE = 2.0;
    double agc_rn[3] = {0, 0, 0};
    double agc_vn[3] = {0, 0, 0};
    double refsm[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    AgcDecodeDPVector(m_pAGC, AGC_ADDR_RN, POS_SCALE, agc_rn);
    AgcDecodeDPVector(m_pAGC, AGC_ADDR_VN, VEL_SCALE, agc_vn);
    for (int i = 0; i < 9; ++i)
    {
        const int16_t hi = AgcErasableWord(m_pAGC, AGC_ADDR_REFSMMAT + 2 * i);
        const int16_t lo = AgcErasableWord(m_pAGC, AGC_ADDR_REFSMMAT + 2 * i + 1);
        refsm[i] = AgcDecodeDP(hi, lo, REFSM_SCALE);
    }
    const double piptime_csec = AgcDecodeDP(
        AgcErasableWord(m_pAGC, AGC_ADDR_PIPTIME),
        AgcErasableWord(m_pAGC, AGC_ADDR_PIPTIME + 1),
        TIME_SCALE);
    const double piptime_sec = piptime_csec * 0.01;

    const double plant_ve_x = out->OUT22[OUT22_Ve_x_mps];
    const double plant_ve_y = out->OUT22[OUT22_Ve_y_mps];
    const double plant_ve_z = out->OUT22[OUT22_Ve_z_mps];
    const double drn_x = agc_rn[0] - xe;
    const double drn_y = agc_rn[1] - ye;
    const double drn_z = agc_rn[2] - ze;
    const double rn_err_m = std::sqrt(drn_x * drn_x + drn_y * drn_y + drn_z * drn_z);
    const double dvn_x = agc_vn[0] - plant_ve_x;
    const double dvn_y = agc_vn[1] - plant_ve_y;
    const double dvn_z = agc_vn[2] - plant_ve_z;
    const double vn_err_mps = std::sqrt(dvn_x * dvn_x + dvn_y * dvn_y + dvn_z * dvn_z);
    // REFSMMAT rows vs host SM axes (columns of q matrix).
    const double refsm_x_dot_smx =
        refsm[0] * xsm_x + refsm[1] * xsm_y + refsm[2] * xsm_z;
    const double refsm_z_dot_smz =
        refsm[6] * zsm_x + refsm[7] * zsm_y + refsm[8] * zsm_z;

    std::fprintf(m_pImuDebugFile,
        "%.3f,%d,%s,%d,%d,"
        "%.6f,%.6f,%.6f,"
        "%.1f,%.1f,%.1f,"
        "%.6f,%.6f,%.6f,"
        "%.4f,%.4f,%.4f,"
        "%d,%d,%d,"
        "%.4f,%.4f,%.4f,"
        "%.9f,%.9f,%.9f,%.9f,"
        "%.4f,%.4f,%.4f,%.4f,"
        "%.3f,%.4f,"
        "%.4f,%.4f,%.4f,"
        "%.3f,%.3f,%.3f,"
        "%.4f,"
        "%.6f,%.6f,%.4f,"
        "%.3f,%.3f,%.3f,"
        "%.4f,%.4f,%.4f,"
        "%.4f,"
        "%.6f,%.6f,%.6f,"
        "%.6f,%.6f,%.6f,"
        "%.6f,%.6f,%.6f,"
        "%.3f,%.4f,"
        "%.6f,%.6f\n",
        t,
        m_bLiftoff ? 1 : 0,
        ImuModeName(m_eIMUMode),
        m_bNoAttLampOn ? 1 : 0,
        pipaInhibit ? 1 : 0,
        m_vCM_IMU_PIPA_Pulses.x, m_vCM_IMU_PIPA_Pulses.y, m_vCM_IMU_PIPA_Pulses.z,
        m_vPipaPulsesInjectedCum.x, m_vPipaPulsesInjectedCum.y, m_vPipaPulsesInjectedCum.z,
        res_x, res_y, res_z,
        m_dIMU_OGAResolver_deg, m_dIMU_IGAResolver_deg, m_dIMU_MGAResolver_deg,
        cdu_x, cdu_y, cdu_z,
        cdu_x_deg, cdu_y_deg, cdu_z_deg,
        m_qECItoSM.GetE(0), m_qECItoSM.GetE(1), m_qECItoSM.GetE(2), m_qECItoSM.GetE(3),
        vex, vey, vez, vemag,
        out->OUT11[OUT11_h_m], out->OUT11[OUT11_Vb_mps],
        plant_ve_x, plant_ve_y, plant_ve_z,
        xe, ye, ze,
        az_deg,
        sm_z_dot_down, sm_x_dot_az, sm_x_elev_deg,
        agc_rn[0], agc_rn[1], agc_rn[2],
        agc_vn[0], agc_vn[1], agc_vn[2],
        piptime_sec,
        refsm[0], refsm[1], refsm[2],
        refsm[3], refsm[4], refsm[5],
        refsm[6], refsm[7], refsm[8],
        rn_err_m, vn_err_mps,
        refsm_x_dot_smx, refsm_z_dot_smz);
    std::fflush(m_pImuDebugFile);
}

ISS_IMU_Mode
SaturnIBStack::UpdateIMUMode()
{
    ISS_IMU_Mode eIMUMode;

    // Determine the current operating state of the IMU/CDU

    if ( m_eIMUMode == ISS_IMU_TURN_ON )
    {
        eIMUMode = ISS_IMU_TURN_ON;
    }
    else if ( GetErrorCounterEnable() )
    {
        if ( GetCoarseAlignEnable() )
        {
            // Coarse Aligment mode
            eIMUMode = ISS_IMU_COARSE_ALIGN;
        }
        else
        {
            if ( m_eFineAlignAxis != FA_NO_OPERATION )
            {
                eIMUMode = ISS_IMU_FINE_ALIGN;
            }
            else
            {
                eIMUMode = ISS_IMU_ATTITUDE_ERROR_DISPLAY;
            }
        }
    }
    else
    {
        if ( m_eFineAlignAxis != FA_NO_OPERATION )
        {
            eIMUMode = ISS_IMU_FINE_ALIGN;
        }
        else
        {
            eIMUMode = ISS_IMU_INERTIAL_REFERENCE;
        }
    }

    m_eIMUMode = eIMUMode;

    return eIMUMode;
}

double
SaturnIBStack::CoarseResolverQuantization( double dAngle_deg )
{
    // Based on the "Apollo Training: Guidance and Control Systems Block II" (dated 1967-02-10),
    // Coarse resolver is quantized through a 7-bit A/D ladder.

    // This equation will work with "canonical" resolver angles (0.0 <= x < 360.0) as well as the
    // (-180 <= x <= 180) used for control signals

    return (double) ((int) ( dAngle_deg / 360.0 * 128.0 ) * 360.0 / 128.0 );
}

double
SaturnIBStack::FineResolverQuantization( double dAngle_deg )
{
    // Based on the "Apollo Training: Guidance and Control Systems Block II" (dated 1967-02-10).

    // This equation will work with "canonical" resolver angles (0.0 <= x < 360.0) as well as the
    // (-180 <= x <= 180) used for control signals

    return (double) ((int) ( dAngle_deg / 360.0 * 32768.0 ) * 360.0 / 32768.0 );
}

/* Obliq plant step + host entity propagation (RTW-free). */
void
SaturnIBStack::rt_OneStep()
{
    SaturnIBPlantObliq_step(m_pPlant, &m_inputs, &m_outputs);

    // Propagate state into the Entity

    ExternalOutputs_saturn_ib_stack * pModelOut = &m_outputs;

    // Take state information from the "on pad" data, S-IB stage or S-IVB stage,
    // depending on the state of the L/V stack.

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
    else {

        m_wcGCLocation.m_latitude_rad = DEGtoRAD( pModelOut->OUT12[OUT12_lat_deg] );
        m_wcGCLocation.m_longitude_rad = DEGtoRAD( pModelOut->OUT12[OUT12_lon_deg] );
        m_wcGCLocation.m_altitude_meters = pModelOut->OUT12[OUT12_h_m];

        m_dP_feet[0] = METERStoFEET (pModelOut->OUT12[OUT12_Xe_x_m]);
        m_dP_feet[1] = METERStoFEET (pModelOut->OUT12[OUT12_Xe_y_m]);
        m_dP_feet[2] = METERStoFEET (pModelOut->OUT12[OUT12_Xe_z_m]);

    }

    // Must swap signs on Y/Z axis rates as +Z CM is mated to -Z L/V (rotated 180 degrees)
    m_dFDAI_RollDot_dps = pModelOut->FDAI_RollDot_dps;
    m_dFDAI_PitchDot_dps = - pModelOut->FDAI_PitchDot_dps;
    m_dFDAI_YawDot_dps = - pModelOut->FDAI_YawDot_dps;

    m_vCM_IMU_PIPA_Pulses = Vector3d(pModelOut->CM_IMU_PIPA_Pulses[0],
                                     pModelOut->CM_IMU_PIPA_Pulses[1],
                                     pModelOut->CM_IMU_PIPA_Pulses[2]);

    m_dFDAI_Yaw_deg = m_dIMU_MGAResolver_deg;
    m_dFDAI_Pitch_deg = m_dIMU_IGAResolver_deg;
    m_dFDAI_Roll_deg = m_dIMU_OGAResolver_deg;

    bool bPrevLiftoff = m_bLiftoff;

    m_bLiftoff = pModelOut->bLiftoff ? true : false;

    // S-IVB signalling liftoff? Notify the AGC

    if ( m_bLiftoff != bPrevLiftoff )
    {
        unsigned short usValue;

        // BIT5 is the liftoff discrete signal from the S-IVB (negative true)

        usValue = 0;
        usValue |= ReadIO ( m_pAGC, 030 ) & ~(BIT5);

        if ( m_bLiftoff == false )
        {
            usValue |= BIT5;
        }

        WriteIO ( m_pAGC, 030, usValue );
    }
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
SaturnIBStack::AGCUpdatePIPACounters()
{
    // --- When does real hardware/software suppress PIPA ΔV to the AGC? ---
    // ISS turn-on (AC Electronics / APM notes): PSA can inhibit PIPA *pulse
    //   torquing* during the 90 s turn-on delay (float centering) — that is
    //   loop torquing, not the same as blanking AGC ΔV counters, but it marks
    //   turn-on as a special PIPA regime.
    // PIPA loop itself: forward/backward counter rejects 3:3 null dither so
    //   zero net accel does not spam the AGC; *net* specific force (incl. 1 g
    //   on the pad once the IMU is in operate) *does* produce ΔV pulses.
    // AGC (IMU_MODE_SWITCHING / SERVICER): PIPUSE zeros PIPAX/Y/Z when a
    //   program starts using PIPAs; PIPASR reads+clears each average cycle.
    //   Mode switching already gates our inject to fine/inertial/att-err.
    //
    // Host guard (sim): plant CM_IMU_PIPA_Pulses currently accumulates large
    // non-physical counts on the pad (see imu_pipa_cdu.csv).  Until that emit
    // path is fixed, do not inject into yaAGC before liftoff — and keep
    // last=plant so the inhibit→enable edge cannot dump a backlog in one step.
    // After liftoff, resume mode-based inhibit as before.

    if ( ! m_bLiftoff )
    {
        m_vLastCM_IMU_PIPA_Pulses = m_vCM_IMU_PIPA_Pulses;
        return;
    }

    Vector3d vPIPAPulseCount = m_vCM_IMU_PIPA_Pulses - m_vLastCM_IMU_PIPA_Pulses;
    int nPulses;
    bool bPIPAInhibit;

    if ( m_eIMUMode == ISS_IMU_INERTIAL_REFERENCE ||
         m_eIMUMode == ISS_IMU_ATTITUDE_ERROR_DISPLAY ||
         m_eIMUMode == ISS_IMU_FINE_ALIGN )
    {
        bPIPAInhibit = false;
    }
    else
    {
        bPIPAInhibit = true;
    }

    // Each PIPA pulse represents a 5.85 cm/sec velocity change

    auto inject_axis = [&]( double *pLast, double *pInjectedCum,
                            int reg, double delta_axis ) {
        int n = (int) delta_axis;
        *pLast += n;
        while ( n != 0 )
        {
            if ( n > 0 )
            {
                if ( ! bPIPAInhibit )
                {
                    UnprogrammedIncrement ( m_pAGC, reg, 0 );
                    *pInjectedCum += 1.0;
                }
                n--;
            }
            else
            {
                if ( ! bPIPAInhibit )
                {
                    UnprogrammedIncrement ( m_pAGC, reg, 2 );
                    *pInjectedCum -= 1.0;
                }
                n++;
            }
        }
    };

    inject_axis( &m_vLastCM_IMU_PIPA_Pulses.x, &m_vPipaPulsesInjectedCum.x,
                 RegPIPAX, vPIPAPulseCount.x );
    inject_axis( &m_vLastCM_IMU_PIPA_Pulses.y, &m_vPipaPulsesInjectedCum.y,
                 RegPIPAY, vPIPAPulseCount.y );
    inject_axis( &m_vLastCM_IMU_PIPA_Pulses.z, &m_vPipaPulsesInjectedCum.z,
                 RegPIPAZ, vPIPAPulseCount.z );
}

void
SaturnIBStack::AGCUpdateGimbalCDUCounters()
{
    const double dOnePulse_deg = READ_COUNTER_PULSES_TO_DEG(1);
    double dGimbalDelta_deg;
    int nSign;

    // No action until we receive our first reset from the AGC
    if ( m_eIMUMode == ISS_IMU_TURN_ON )
    {
        UpdateDebugText();
        return;
    }

    if ( m_bFirstPass )
    {
        m_dLastSentIMU_OGAResolver_deg = m_dIMU_OGAResolver_deg;
        m_dLastSentIMU_IGAResolver_deg = m_dIMU_IGAResolver_deg;
        m_dLastSentIMU_MGAResolver_deg = m_dIMU_MGAResolver_deg;
    }
    else
    {

        ISS_IMU_Mode eIMUMode;

        eIMUMode = UpdateIMUMode();

        /*========================================================================
         *
         *  ISS Outer (X) Gimbal
         *
         *=======================================================================*/

        dGimbalDelta_deg = m_dIMU_OGAResolver_deg - m_dLastSentIMU_OGAResolver_deg;

        // Handle cases where current and last values space zero degrees

        if ( dGimbalDelta_deg > 180.0 )
        {
            dGimbalDelta_deg -= 360.0;
        }
        else if ( dGimbalDelta_deg < -180.0 )
        {
            dGimbalDelta_deg += 360.0;
        }

        if ( dGimbalDelta_deg > 0.0 )
        {
            nSign = 1;
        }
        else
        {
            nSign = -1;
        }

        dGimbalDelta_deg = fabs( dGimbalDelta_deg );

        // Now send PCDU/MCDU commands, as needed

        while ( dGimbalDelta_deg >  dOnePulse_deg )
        {
            if ( nSign == 1 )
            {
                 m_dLastSentIMU_OGAResolver_deg += dOnePulse_deg;
 #if 0
                 // SEND PCDU (if yaw rate greater than "4" in ya LM sim, then this is a "PCDU Fast"
                 if ( eIMUMode == ISS_IMU_FINE_ALIGN || m_bCoarseAlignFastRate[0] == false )
                 {
                     UnprogrammedIncrement ( m_pAGC, RegCDUX, 001 );
                     //printf("UnprogrammedIncrement ( m_pAGC, RegCDUX, 001 );\n");
                 }
                 else {
                     UnprogrammedIncrement ( m_pAGC, RegCDUX, 021 );
                     //printf("UnprogrammedIncrement ( m_pAGC, RegCDUX, 021 );\n");
                 }
#endif
            }
            else
            {
                m_dLastSentIMU_OGAResolver_deg -= dOnePulse_deg;
#if 0
                // SEND MCDU
                if ( eIMUMode == ISS_IMU_FINE_ALIGN || m_bCoarseAlignFastRate[0] == false  )
                {
                    UnprogrammedIncrement ( m_pAGC, RegCDUX, 003 );
                    //printf("UnprogrammedIncrement ( m_pAGC, RegCDUX, 003 );\n");
                }
                else {
                    UnprogrammedIncrement ( m_pAGC, RegCDUX, 023 );
                    //printf("UnprogrammedIncrement ( m_pAGC, RegCDUX, 023 );\n");
                }
#endif
            }

            SetCDURegister( RegCDUX, m_dLastSentIMU_OGAResolver_deg );

            m_dTotalCDUPulseIncrements.x += 1.0;
            dGimbalDelta_deg -= dOnePulse_deg;
        }

        // Normalize last sent value
        if ( m_dLastSentIMU_OGAResolver_deg >= 180.0 )
        {
            m_dLastSentIMU_OGAResolver_deg -= 360.0;
        }
        else if ( m_dLastSentIMU_OGAResolver_deg < -180.0 )
        {
            m_dLastSentIMU_OGAResolver_deg += 360.0;
        }

        /*========================================================================
         *
         *  ISS Inner (Y) Gimbal
         *
         *=======================================================================*/

        dGimbalDelta_deg = m_dIMU_IGAResolver_deg - m_dLastSentIMU_IGAResolver_deg;

        // Handle cases where current and last values space zero degrees

        if ( dGimbalDelta_deg > 180.0 )
        {
            dGimbalDelta_deg -= 360.0;
        }
        else if ( dGimbalDelta_deg < -180.0 )
        {
            dGimbalDelta_deg += 360.0;
        }

        if ( dGimbalDelta_deg > 0.0 )
        {
            nSign = 1;
        }
        else
        {
            nSign = -1;
        }

        dGimbalDelta_deg = fabs( dGimbalDelta_deg );

        // Now send PCDU/MCDU commands, as needed

        while ( dGimbalDelta_deg >  dOnePulse_deg )
        {
            if ( nSign == 1 )
            {
                 m_dLastSentIMU_IGAResolver_deg += dOnePulse_deg;
 #if 0
                 // SEND PCDU (if yaw rate greater than "4" in ya LM sim, then this is a "PCDU Fast"
                 if ( eIMUMode == ISS_IMU_FINE_ALIGN || m_bCoarseAlignFastRate[1] == false )
                 {
                     UnprogrammedIncrement ( m_pAGC, RegCDUY, 001 );
                     //printf("UnprogrammedIncrement ( m_pAGC, RegCDUY, 001 );\n");
                 }
                 else {
                     UnprogrammedIncrement ( m_pAGC, RegCDUY, 021 );
                     //printf("UnprogrammedIncrement ( m_pAGC, RegCDUY, 021 );\n");
                 }
 #endif
            }
            else
            {
                m_dLastSentIMU_IGAResolver_deg -= dOnePulse_deg;
#if 0
                // SEND MCDU
                if ( eIMUMode == ISS_IMU_FINE_ALIGN || m_bCoarseAlignFastRate[1] == false  )
                 {
                     UnprogrammedIncrement ( m_pAGC, RegCDUY, 003 );
                     //printf("UnprogrammedIncrement ( m_pAGC, RegCDUY, 003 );\n");
                 }
                 else {
                     UnprogrammedIncrement ( m_pAGC, RegCDUY, 023 );
                     //printf("UnprogrammedIncrement ( m_pAGC, RegCDUY, 023 );\n");
                 }
 #endif
            }
            SetCDURegister( RegCDUY, m_dLastSentIMU_IGAResolver_deg );
            m_dTotalCDUPulseIncrements.y += 1.0;
            dGimbalDelta_deg -= dOnePulse_deg;
        }

        // Normalize last sent value
        if ( m_dLastSentIMU_IGAResolver_deg >= 180.0 )
        {
            m_dLastSentIMU_IGAResolver_deg -= 360.0;
        }
        else if ( m_dLastSentIMU_IGAResolver_deg < -180.0 )
        {
            m_dLastSentIMU_IGAResolver_deg += 360.0;
        }


        /*========================================================================
         *
         *  ISS Middle (Z) Gimbal
         *
         *=======================================================================*/

        dGimbalDelta_deg = m_dIMU_MGAResolver_deg - m_dLastSentIMU_MGAResolver_deg;

        // Handle cases where current and last values space zero degrees

        if ( dGimbalDelta_deg > 180.0 )
        {
            dGimbalDelta_deg -= 360.0;
        }
        else if ( dGimbalDelta_deg < -180.0 )
        {
            dGimbalDelta_deg += 360.0;
        }

        if ( dGimbalDelta_deg > 0.0 )
        {
            nSign = 1;
        }
        else
        {
            nSign = -1;
        }

        dGimbalDelta_deg = fabs( dGimbalDelta_deg );

        // Now send PCDU/MCDU commands, as needed

        while ( dGimbalDelta_deg >  dOnePulse_deg )
        {
            if ( nSign == 1 )
            {
                 m_dLastSentIMU_MGAResolver_deg += dOnePulse_deg;
#if 0
                 // SEND PCDU (if yaw rate greater than "4" in ya LM sim, then this is a "PCDU Fast"
                 if ( eIMUMode == ISS_IMU_FINE_ALIGN || m_bCoarseAlignFastRate[2] == false )
                 {
                     UnprogrammedIncrement ( m_pAGC, RegCDUZ, 001 );
                     //printf("UnprogrammedIncrement ( m_pAGC, RegCDUZ, 001 );\n");
                 }
                 else {
                     UnprogrammedIncrement ( m_pAGC, RegCDUZ, 021 );
                     //printf("UnprogrammedIncrement ( m_pAGC, RegCDUZ, 021 );\n");
                 }
#endif
            }
            else
            {

                m_dLastSentIMU_MGAResolver_deg -= dOnePulse_deg;
#if 0
                // SEND MCDU
                if ( eIMUMode == ISS_IMU_FINE_ALIGN || m_bCoarseAlignFastRate[2] == false )
                 {
                     UnprogrammedIncrement ( m_pAGC, RegCDUZ, 003 );
                     //printf("UnprogrammedIncrement ( m_pAGC, RegCDUZ, 003 );\n");
                 }
                 else {
                     UnprogrammedIncrement ( m_pAGC, RegCDUZ, 023 );
                     //printf("UnprogrammedIncrement ( m_pAGC, RegCDUZ, 023 );\n");
                 }
#endif
            }

            SetCDURegister( RegCDUZ, m_dLastSentIMU_MGAResolver_deg );

            m_dTotalCDUPulseIncrements.z += 1.0;
            dGimbalDelta_deg -= dOnePulse_deg;
        }

        // Normalize last sent value
        if ( m_dLastSentIMU_MGAResolver_deg >= 180.0 )
        {
            m_dLastSentIMU_MGAResolver_deg -= 360.0;
        }
        else if ( m_dLastSentIMU_MGAResolver_deg < -180.0 )
        {
            m_dLastSentIMU_MGAResolver_deg += 360.0;
        }
    }

    UpdateDebugText();
}

void SaturnIBStack::UpdateDebugText()
{
    ExternalOutputs_saturn_ib_stack * pModelOut = &m_outputs;
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
        " ECIX %8.0f   ECIY %8.0f   ECIZ %8.0f\r\n"
        ,
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
    ISS_IMU_Mode eIMUMode;

    eIMUMode = UpdateIMUMode();

    if ( eIMUMode == ISS_IMU_COARSE_ALIGN )
    {
        m_vCoarseAlignCommand_deg.x += nSign * nPulseCount * READ_COUNTER_PULSES_TO_DEG(4);
    }
    else /* if ( GetErrorCounterEnable() ) */
    {
        // CDU Generates analog FDAI Error Needle offsets
        m_nCDUXErrorCounter_pulses += nSign * nPulseCount;
    }
}

void
SaturnIBStack::UpdateIMUYGimbal( short nSign, unsigned short nPulseCount  )
{
    ISS_IMU_Mode eIMUMode;

    eIMUMode = UpdateIMUMode();

    if ( eIMUMode == ISS_IMU_COARSE_ALIGN )
    {
        m_vCoarseAlignCommand_deg.y += nSign * nPulseCount * READ_COUNTER_PULSES_TO_DEG(4);
    }
    else /* if ( GetErrorCounterEnable() ) */
    {
        // CDU Generates analog FDAI Error Needle offsets
        m_nCDUYErrorCounter_pulses += nSign * nPulseCount;
    }
}
void SaturnIBStack::UpdateIMUZGimbal( short nSign, unsigned short nPulseCount  )
{
    ISS_IMU_Mode eIMUMode;

    eIMUMode = UpdateIMUMode();

    if ( eIMUMode == ISS_IMU_COARSE_ALIGN )
    {
        m_vCoarseAlignCommand_deg.z += nSign * nPulseCount * READ_COUNTER_PULSES_TO_DEG(4);
    }
    else /* if ( GetErrorCounterEnable() ) */
    {
        // CDU Generates analog FDAI Error Needle offsets
        m_nCDUZErrorCounter_pulses += nSign * nPulseCount;
    }

}

void SaturnIBStack::IncrementFineAlignPulseCount( ISS_Fine_Align_Axis eAxis,
                                                  int nSign,
                                                  unsigned short usPulseCount )
{
    // Debug: log every gyro-torque command from the AGC (kept, disabled).
#if 0
    if (eAxis != FA_NO_OPERATION || usPulseCount != 0) {
        static const char *axis_name[] = {"NONE", "X", "Y", "Z"};
        std::fprintf(stderr,
            "GYRO_PULSE  t=%9.3f  axis=%s  sign=%+d  count=%u  "
            "pending_before=(axis=%s,sign=%+d,count=%.1f)\n",
            GetSimClock()->GetElapsedTime(),
            axis_name[eAxis], nSign, usPulseCount,
            axis_name[m_eFineAlignAxis],
            m_nFineAlignPulseSign, m_dFineAlignPulseCount);
    }
#endif

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
                // Handle sign change in pulse counts -- we might count down past zero
                m_dFineAlignPulseCount -= usPulseCount;

                if ( m_dFineAlignPulseCount < 0.0 )
                {
                    m_dFineAlignPulseCount = - m_dFineAlignPulseCount;
                    m_nFineAlignPulseSign = - nSign;
                }
            }
        }

        // Unlikely, but worth the check
        if ( m_dFineAlignPulseCount == 0 )
        {
            m_eFineAlignAxis = FA_NO_OPERATION;
        }
    }
    else if ( nSign == 0 )
    {
        // force shutdown of gyro pulsing
        m_eFineAlignAxis = FA_NO_OPERATION;
        m_dFineAlignPulseCount = 0.0;
        m_nFineAlignPulseSign = 1;
    }

    // The change we've just made most likely put the IMU in Fine Align mode

    UpdateIMUMode();
}

void SaturnIBStack::ResetCDUReadCounters()
{
    ExternalOutputs_saturn_ib_stack * pModelOut = &m_outputs;

    m_dLastSentIMU_OGAResolver_deg = 0.0;
    m_dLastSentIMU_IGAResolver_deg = 0.0;
    m_dLastSentIMU_MGAResolver_deg = 0.0;

    // ECI to Launch vehicle transform
    Quaternion qECItoLV( pModelOut->veh_q_ECI[0],
                         pModelOut->veh_q_ECI[1],
                         pModelOut->veh_q_ECI[2],
                         pModelOut->veh_q_ECI[3] );

    m_qECItoSM = (Quaternion) (m_mLVtoCM * (Matrix4d) qECItoLV);

    m_vCoarseAlignCommand_deg = Vector3d( 0, 0, 0 );

    m_bFirstResetReceived = true;

    // TODO: Figure-out yaAGC CDU registers
    m_pAGC->Erasable[0][RegCDUX] = 0;
    m_pAGC->Erasable[0][RegCDUY] = 0;
    m_pAGC->Erasable[0][RegCDUZ] = 0;

    m_nCDUXErrorCounter_pulses = 0;
    m_nCDUYErrorCounter_pulses = 0;
    m_nCDUZErrorCounter_pulses = 0;
}

void
SaturnIBStack::SetCDURegister( int nRegister, double dAngle_deg )
{
    if ( dAngle_deg < 0.0 )
    {
        dAngle_deg += 360.0;
    }
    int nPulses = (int)( dAngle_deg * 32768.0 / 360.0 );
    m_pAGC->Erasable[0][nRegister] = nPulses & 077777;
}


extern "C" {

    //----------------------------------------------------------------------
    // A function for handling reception of "input channel" data from
    // any connected client.  At most one channel of input is handled per
    // call.  The function returns 0 if there is no input, or non-zero if
    // there is input.  If the input causes an interrupt (as, for example,
    // a keystroke from the DSKY), then the function should set the
    // appropriate request in State->InterruptRequests[].
    //
    // The function also handles unprogrammed counter increments caused
    // by inputs to the CPU.  The function returns 1 if a counter-increment
    // was performed, and returns zero otherwise.

    static int CurrentChannelValues[256] = { 0 };
    static const unsigned char Signatures[4] = { 0x00, 0x40, 0x80, 0xC0 };
    static const unsigned char SignaturesAgs[4] = { 0x00, 0xC0, 0x80, 0x40 };

    int ChannelInput ( agc_t *State )
    {
        int nResult = 0;

        // Process all channel inputs forwarded to us by devices
        // (currently, only the panel instrument panel)
        if ( g_AGCChannelInputQueue.size() > 0 ) {

            int nValue;

            ChannelIOList::iterator p = g_AGCChannelInputQueue.begin();
            ChannelIOOperation op = *p;
            g_AGCChannelInputQueue.pop_front();

            // Preserve state of bits not in the operation's mask
            nValue = op.m_usValue & op.m_usMask;
            nValue |= ReadIO ( State, op.m_usChannel ) & ~op.m_usMask;
            WriteIO ( State, op.m_usChannel, nValue );

            //printf( "WriteIO( %03.3o, %05.5o )\n", op.m_usChannel, nValue );

            // DSKY keyboard interrupts the CPU
            if ( op.m_usChannel == 015 )
            {
                State->InterruptRequests[5] = 1;
            }

            nResult = 1;
        }

        return nResult;
    }

#if 0
        static int SocketInterlace = 0;
        int i, j, k;
        unsigned char c;
        Client_t *Client;
        int Channel, Value;

        //We use SocketInterlace to slow down the number
        // of polls of the sockets.
        if (SocketInterlace > 0)
            SocketInterlace--;
        else
        {
            SocketInterlace = SocketInterlaceReload;
            for (i = 0, Client = Clients; i < MAX_CLIENTS; i++, Client++)
                if (Client->Socket != -1)
                {
                    // We arbitrarily adopt the rule that we'll process at most one
                    // packet per client per instruction cycle.
                    for (j = Client->Size; j < 4; j++)
                    {
                        k = recv (Client->Socket, (char *) &c, 1, 0);
                        if (k == 0 || k == -1)
                            break;
                        // 20090318 RSB.  Added this filter for a little robustness,
                        // but it shouldn't be needed.
                        if (!(
                            (Signatures[j] == (c & 0xC0)) ||
                            (DebugDeda && (SignaturesAgs[j] == (c & 0xC0)))
                            )
                            )
                        {
                            Client->Size = 0;
                            if (0 != (c & 0xC0))
                            {
                                j = -1;
                                continue;
                            }
                            j = 0;
                        }
                        Client->Packet[Client->Size++] = c;
                    }
                    // Process a received packet.
                    if (Client->Size >= 4)
                    {
                        int uBit, Type, Data;
                        //printf ("Received from %d: %02X %02X %02X %02X\n",
                        //	i, Client->Packet[0], Client->Packet[1],
                        //	Client->Packet[2], Client->Packet[3]);
                        if (!ParseIoPacket (Client->Packet, &Channel, &Value, &uBit))
                        {
                            // Convert to AGC format (upper 15 bits).
                            Value &= 077777;
                            if (uBit)
                            {
                                Client->ChannelMasks[Channel] = Value;
                            }
                            else if (Channel & 0x80)
                            {
                                // In this case we're dealing with a counter increment.
                                // So increment the counter.
                                //printf ("Channel=%02o Int=%o\n", Channel, Value);
                                UnprogrammedIncrement (State, Channel, Value);
                                Client->Size = 0;
                                return (1);
                            }
                            else
                            {
                                Value &= Client->ChannelMasks[Channel];
                                Value |=
                                    ReadIO (State,
                                    Channel) & ~Client->ChannelMasks[Channel];
                                WriteIO (State, Channel, Value);
                                // If this is a keystroke from the DSKY, generate an interrupt req.
                                if (Channel == 015)
                                    State->InterruptRequests[5] = 1;
                                // If this is on fictitious input channel 0173, then the data
                                // should be placed in the INLINK counter register, and an
                                // UPRUPT interrupt request should be set.
                                else if (Channel == 0173)
                                {
                                    State->Erasable[0][RegINLINK] = (Value & 077777);
                                    State->InterruptRequests[7] = 1;
                                }
                                // Fictitious registers for rotational hand controller (RHC).
                                // Note that the RHC angles are not immediately used, but
                                // merely squirreled away for later.  They won't actually
                                // go into the counter registers until the RHC counters are
                                // enabled and the data requested (bits 8,9 of channel 13).
                                else if (Channel == 0166)
                                {
                                    LastRhcPitch = Value;
                                    ChannelOutput (State, Channel, Value);	// echo
                                }
                                else if (Channel == 0167)
                                {
                                    LastRhcYaw = Value;
                                    ChannelOutput (State, Channel, Value);	// echo
                                }
                                else if (Channel == 0170)
                                {
                                    LastRhcRoll = Value;
                                    ChannelOutput (State, Channel, Value);	// echo
                                }
                                else if (Channel == 031)
                                {
                                    static int LastInDetent = 040000;
                                    int InDetent;
                                    ChannelOutput (State, Channel, Value);
                                    // If the RHC stick has moved out of detent,
                                    // generate a RUPT10 interrupt.
                                    InDetent = (040000 & Value);
                                    if (LastInDetent && !InDetent)
                                        State->InterruptRequests[10] = 1;
                                    LastInDetent = InDetent;
                                }
                                //---------------------------------------------------------------
                                // For --debug-dsky mode.
                                if (DebugDsky)
                                {
                                    if (Channel == 032)
                                    {
                                        // For DebugDsky purposes only, the PRO key is translated
                                        // to appear as the otherwise-fictitious KeyCode 0.
                                        if (0 != (Value & 020000))
                                        {
                                            Channel = 015;
                                            Value = 0;
                                        }
                                    }
                                    if (Channel == 015)
                                    {
                                        int i, CurrentValue;
                                        Value &= 077777;
                                        for (i = 0; i < NumDebugRules; i++)
                                            if (Value == DebugRules[i].KeyCode)
                                            {
                                                CurrentValue =
                                                    CurrentChannelValues[DebugRules[i].
                                                    Channel];
                                                switch (DebugRules[i].Logic)
                                                {
                                                case '=':
                                                    CurrentValue = DebugRules[i].Value;
                                                    break;
                                                case '|':
                                                    CurrentValue |= DebugRules[i].Value;
                                                    break;
                                                case '&':
                                                    CurrentValue &= DebugRules[i].Value;
                                                    break;
                                                case '^':
                                                    CurrentValue ^= DebugRules[i].Value;
                                                    break;
                                                default:
                                                    break;
                                                }
                                                CurrentChannelValues[DebugRules[i].
                                                    Channel] =
                                                    CurrentValue;
                                                ChannelOutput (State,
                                                    DebugRules[i].Channel,
                                                    CurrentValue);
                                            }
                                    }
                                    //if (Channel != 032 && Channel != 015)
                                    //  WriteIO (State, Channel, Value);
                                }
                                //---------------------------------------------------------------
                            }
                        }
                        else if (DebugDeda && !ParseIoPacketAGS (Client->Packet, &Type, &Data))
                        {
                            // The following code is present only for debugging yaDEDA
                            // communications, and has no interesting purpose yaAGC-wise.
                            static unsigned char Buffer[9];
                            static int NumInBuffer = 0, NumWanted = 0, Collecting = 0;
                            static unsigned char Packet[4];
                            if (Type == 05 && (Data == 0777002 || Data == 0777004 ||
                                Data == 0777010 || Data == 0777020))
                                printf ("DEDA key release.\n");
                            else if (Collecting && Type == 07)
                            {
                                Buffer[NumInBuffer++] = Data >> 13;
                                if (NumInBuffer < NumWanted)
                                    send (Client->Socket, (const char *) Packet, 4, 0);
                                else
                                {
                                    int i;
                                    Collecting = 0;
                                    printf ("Received %d DEDA nibbles:", NumWanted);
                                    for (i = 0; i < NumWanted; i++)
                                        printf (" %1X", Buffer[i]);
                                    printf ("\n");
                                    if (NumWanted == 3)
                                    {
                                        if (!DedaQuiet)
                                            DedaMonitor = 1;
                                        DedaAddress = Buffer[0] * 0100 + Buffer[1] * 010 + Buffer[2];
                                        DedaWhen = State->CycleCounter;
                                    }
                                }
                            }
                            else if (Type == 05 && (Data == 0775002 || Data == 0773004))
                            {
                                NumInBuffer = 0;
                                Collecting = 1;
                                if (Data == 0775002)
                                {
                                    printf ("Received DEDA READOUT.\n");
                                    NumWanted = 3;
                                }
                                else
                                {
                                    printf ("Received DEDA ENTR.\n");
                                    NumWanted = 9;
                                }
                                FormIoPacketAGS (040, ~010, Packet);
                                send (Client->Socket, (const char *) Packet, 4, 0);
                            }
                            else if (Type == 05 && Data == 0767010)
                            {
                                printf ("Received DEDA HOLD.\n");
                                DedaMonitor = 0;
                            }
                            else if (Type == 05 && Data == 0757020)
                            {
                                printf ("Received DEDA CLR.\n");
                                DedaMonitor = 0;
                            }
                            else
                                printf ("Unknown AGS packet %02X %02X %02X %02X\n",
                                Client->Packet[0], Client->Packet[1],
                                Client->Packet[2], Client->Packet[3]);
                        }
                        Client->Size = 0;
                    }
                }
        }
        return (0);
    }
#endif

    void ChannelRoutine (agc_t *State)
    {
        // No action required
    }

    // yaAGC host hook (added upstream 2024): populate RNRAD with radar data.
    // CSM/Colossus249 does not use landing/rendezvous radar; leave empty.
    void RequestRadarData (agc_t *State)
    {
        (void)State;
    }

    void ChannelOutput (agc_t * State, int Channel, int Value)
    {
        //int i, j;
        //int n;
        //Client_t *Client;
        //unsigned char Packet[4];
        extern int DebugMode;
        // Some output channels have purposes within the CPU, so we have to
        // account for those separately.
        if (Channel == 7)
        {
            State->InputChannel[7] = State->OutputChannel7 = (Value & 0160);
            return;
        }
        // Stick data into the RHCCTR registers, if bits 8,9 of channel 013 are set.
        if (Channel == 013 && 0600 == (0600 & Value) && !CmOrLm)
        {
            State->Erasable[0][042] = LastRhcPitch;
            State->Erasable[0][043] = LastRhcYaw;
            State->Erasable[0][044] = LastRhcRoll;
        }

        if (Channel == 012)
        {
            g_pSaturnIBStack->SetCoarseAlignEnable( (Value & BIT4) ? true : false );

            g_pSaturnIBStack->SetErrorCounterEnable( (Value & BIT6) ? true : false );

            // Zero CDU Counter command
            if ( Value & BIT5 )
            {
                g_pSaturnIBStack->ResetCDUReadCounters();
                //LogInfo("ResetCDUReadCounters()\n");
            }

            if ( Value & BIT15 )
            {
                g_pSaturnIBStack->ISSTurnOnDelayComplete();
            }
        }
        /**
         * CDU X drive bits
         *
         * Simulate the processing of 3200 cps +/- delta_Theta_c pulse trains from the AGC
         * to the CMD (really the pulses arrive at the Error Counter.
         *
         * (See APOLLO OPERATIONS HANDBOOK, dated 16 July 1969, pg. 2.2-33 and the yaAGC
         * Developer's page)
         */

        else if ( Channel == 0174 )
        {
            //if ( g_pSaturnIBStack->GetCoarseAlignEnable() )
            //{
                short nSign = (Value & BIT15) ? -1 : 1;
                unsigned short nPulseCount = Value & 037777;

                g_pSaturnIBStack->UpdateIMUXGimbal( nSign, nPulseCount );

                //printf( "UpdateIMUXGimbal( %d, %d )\n", nSign, nPulseCount );
            //}
        }
        // CDU Y drive bits
        else if ( Channel == 0175 )
        {
            //if ( g_pSaturnIBStack->GetCoarseAlignEnable() )
            //{
                short nSign = (Value & BIT15) ? -1 : 1;
                unsigned short nPulseCount = Value & 037777;

                g_pSaturnIBStack->UpdateIMUYGimbal( nSign, nPulseCount );

                //printf( "UpdateIMUYGimbal( %d, %d )\n", nSign, nPulseCount );
            //}
        }
        // CDU Z drive bits
        else if ( Channel == 0176 )
        {
            //if ( g_pSaturnIBStack->GetCoarseAlignEnable() )
            //{
                short nSign = (Value & BIT15) ? -1 : 1;
                unsigned short nPulseCount = Value & 037777;

                g_pSaturnIBStack->UpdateIMUZGimbal( nSign, nPulseCount );

                //printf( "UpdateIMUZGimbal( %d, %d )\n", nSign, nPulseCount );
            //}
        }
        else if ( Channel == 0177 )
        {
            //printf ("FINE ALIGNMENT %o %d\n", Value, Value & 03777 );
            int nSign = (Value & BIT15) ? -1 : 1;
            unsigned short usPulseCount = Value & 03777;
            ISS_Fine_Align_Axis eAxis = FA_NO_OPERATION;

            // "Gyro enable"
            if ( Value & BIT12 )
            {
                // Gyro select "A"
                if ( Value & BIT14 )
                {
                    // Gyro select "B"
                    if ( Value & BIT13 )
                    {
                        eAxis = FA_Z_AXIS;
                    }
                    else
                    {
                        eAxis = FA_Y_AXIS;
                    }
                }
                else
                {
                    // Gyro select "B"
                    if ( Value & BIT13 )
                    {
                        eAxis = FA_X_AXIS;
                    }
                    else
                    {
                        eAxis = FA_NO_OPERATION;
                    }
                }

                if ( eAxis != FA_NO_OPERATION )
                {
                    // This will put us in Fine Align mode until all pulses are completed
                    g_pSaturnIBStack->IncrementFineAlignPulseCount( eAxis, nSign, usPulseCount );
                }
                else
                {
                    g_pSaturnIBStack->IncrementFineAlignPulseCount( FA_NO_OPERATION, 0, 0 );
                }
            }
            else
            {
                g_pSaturnIBStack->IncrementFineAlignPulseCount( FA_NO_OPERATION, 0, 0 );
            }

        }

        // Process downlink data stream (see DecodeDigitalDownlink.c)

        else if ( Channel == 013 || Channel == 034 || Channel == 035 )
        {
            int CmOrLm = 1; /* "CM" */
            DecodeDigitalDownlink ( Channel, Value, CmOrLm );
        }

        // DSKY OUT0 / DSPTAB relays (channel 010): track NO ATT for SV gate.
        // apolloa_main process_010 case 12: NOATT_LIGHT = (w & (1<<3)) != 0.
        else if ( Channel == 010 )
        {
            const int cmd = (Value >> 11) & 037;
            if ( cmd == 12 && g_pSaturnIBStack )
            {
                // BIT4 = 000010 octal = 1<<3 — same mask as the panel lamp.
                g_pSaturnIBStack->NotifyNoAttLamp( (Value & BIT4) != 0 );
            }
            ChannelIOOperation op( (unsigned short) Channel, (unsigned short) Value );
            g_AGCChannelOutputQueue.push_back( op );
        }

        // Push the operation onto the channel output queue -- this will
        // be picked-up by the panel user interface
        // TODO: should only pass channel related to DSKY digits
        else
        {
            ChannelIOOperation op( (unsigned short) Channel, (unsigned short) Value );
            g_AGCChannelOutputQueue.push_back( op );

            //printf( "g_AGCChannelOutputQueue.push_back( %03.3o, %05.5o )\n", op.m_usChannel, op.m_usValue );
        }

#if 0
        if (FormIoPacket (Channel, Value, Packet))
            return;
        for (i = 0, Client = Clients; i < MAX_CLIENTS; i++, Client++)
            if (Clients[i].Socket != -1)
            {
                j = send (Client->Socket, (const char *) Packet, 4, MSG_NOSIGNAL);
                if (j == SOCKET_ERROR && SOCKET_BROKEN)
                {
                    if (!DebugMode)
                        printf ("Removing socket %d\n", Clients[i].Socket);
#ifdef unix
                    close (Clients[i].Socket);
#else
                    closesocket (Clients[i].Socket);
#endif
                    Clients[i].Socket = -1;
                }
            }
#endif
    }

    /**
     * Unused in a CM simulation
     */
    void ShiftToDeda (agc_t *State, int Data)
    {
    }
}

void
SaturnIBStack::SaveState( const std::string & strName )
{
    char szCoreName[1024];

    strcpy_s( szCoreName, sizeof(szCoreName), strName.c_str() );
    strcat_s( szCoreName, sizeof(szCoreName), ".agc_core" );
    MakeCoreDump ( m_pAGC, szCoreName );

    strcpy_s( szCoreName, sizeof(szCoreName), strName.c_str() );
    strcat_s( szCoreName, sizeof(szCoreName), "_state.txt" );

    Quaternion qECItoSM = m_qECItoSM;
    ExternalInputs_saturn_ib_stack * pModelInputs = &m_inputs;

    std::ofstream out( szCoreName );
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
SaturnIBStack::WriteUplinkKeycode( int nKeycode )
{
    double dUplinkInterval_sec = 0.01;

    if ( nKeycode != -1 && nKeycode < 32 )
    {
        // Keycodes are composed of 5-bits of data -- the actual uplink word is 15 bits,
        // repeating the keycode bits three times with the middle instance complemented.

        LogInfo1( "UplinkKeycode: %d\n", nKeycode );

        nKeycode &= 037;
        nKeycode |= ((nKeycode << 10) | ((nKeycode ^ 037) << 5));

        m_pAGC->Erasable[0][RegINLINK] = (nKeycode & 077777);
		m_pAGC->InterruptRequests[7] = 1;

        m_dNextUplinkTime_sec = GetSimClock()->GetElapsedTime() + dUplinkInterval_sec;
    }
    else if ( nKeycode == 254 )
    {
        // two second delay
        m_dNextUplinkTime_sec = GetSimClock()->GetElapsedTime() + 2.0;
    }
    else if ( nKeycode == 255 )
    {
        // one second delay
        m_dNextUplinkTime_sec = GetSimClock()->GetElapsedTime() + 1.0;
    }
    else if ( nKeycode == -1 )
    {
        // Finished with this uplink
        std::fprintf(stderr, "  UPLINK COMPLETE at elapsed=%.3f sec\n",
                     GetSimClock()->GetElapsedTime());
        delete m_pCurrentUplinkMessage;
        m_pCurrentUplinkMessage = NULL;
        m_dNextUplinkTime_sec = -1;
    }
}

/**
 * Open the specified uplink file and send its contents to the CMC as a simulated
 * uplink data stream.
 */
bool SaturnIBStack::StartUplink( const std::string & strFile )
{
    bool bResult;

    if ( m_pCurrentUplinkMessage )
    {
        delete m_pCurrentUplinkMessage;
        m_pCurrentUplinkMessage = NULL;
    }

    m_pCurrentUplinkMessage = new UplinkMessage();

    bResult = m_pCurrentUplinkMessage->LoadFromFile( strFile );

    if ( bResult )
    {
        int nKeycode = m_pCurrentUplinkMessage->GetFirstKeycode();

        WriteUplinkKeycode( nKeycode );
    }
    else
    {
        delete m_pCurrentUplinkMessage;
        m_pCurrentUplinkMessage = NULL;
    }

    return bResult;
}

/**
 * Uses the simulation clock to manage transmitting the current uplink file to the CMC
 * @return returns true if the uplink operation is complete
 */
bool
SaturnIBStack::UplinkTimeStep()
{
    bool bResult = false;

    if ( m_pCurrentUplinkMessage )
    {
        if ( GetSimClock()->GetElapsedTime() > m_dNextUplinkTime_sec )
        {
            int nKeycode = m_pCurrentUplinkMessage->GetNextKeycode();

            WriteUplinkKeycode( nKeycode );

            if ( nKeycode == -1 )
            {
                bResult = true;
            }
        }
    }
    return bResult;
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
    double sinPhi, cosPhi, sinTheta, cosTheta, sinPsi, cosPsi;
    Matrix4d mResult;

    sinPhi = sin (dOGA_rad);
    cosPhi = cos (dOGA_rad);
    sinTheta = sin (dIGA_rad);
    cosTheta = cos (dIGA_rad);
    sinPsi = sin (dMGA_rad);
    cosPsi = cos (dMGA_rad);

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

static void
SwriteDownlinkWriter ()
{
    size_t i;
    size_t n;
    char * pText = g_pSaturnIBStack->GetDownlinkText();

    *pText = '\0';

    for (i = 0; i < (size_t) Sheight; i++)
    {
        n = strlen( Sbuffer[i] );
        strcpy( pText, Sbuffer[i] );
        strcat( pText, "\n" );
        pText += n + 1;
    }

    // One-shot confirmation that TSV-driven decode is feeding the UI buffer.
    static bool s_loggedFirstDownlink = false;
    if (!s_loggedFirstDownlink && Sheight > 0 && Sbuffer[0][0] != '\0')
    {
        s_loggedFirstDownlink = true;
        std::fprintf(stderr, "AGC downlink: first decoded list \"%s\"\n",
                     Sbuffer[0]);
    }
}