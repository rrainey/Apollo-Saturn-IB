#pragma once

#include "SimSupport.h"
#include "SaturnIBExternalIO.h"

struct SaturnIBPlantObliq_tag;
typedef struct SaturnIBPlantObliq_tag SaturnIBPlantObliq;

typedef enum _ISS_IMU_Mode {
    ISS_IMU_TURN_ON,
    ISS_IMU_COARSE_ALIGN,
    ISS_IMU_FINE_ALIGN,
    ISS_IMU_CAGE,
    ISS_IMU_INERTIAL_REFERENCE,
    ISS_IMU_ATTITUDE_ERROR_DISPLAY
} ISS_IMU_Mode;

typedef enum _ISS_Fine_Align_Axis {
    FA_NO_OPERATION,
    FA_X_AXIS,
    FA_Y_AXIS,
    FA_Z_AXIS
} ISS_Fine_Align_Axis;

/**
 * Saturn-IB stack model runs at 200 Hz
 */
#define MODEL_TIME_STEP_SEC               0.005

/**
 * Stable member simulation model runs at 1,000 Hz
 */
#define STABLE_MEMBER_MODEL_TIME_STEP_SEC 0.001

/**
 * One AGC CPU cycle is 11.7 microseconds
 */
#define AGC_CPU_CYCLE_SEC   (11.71875 / 1000000.0)


#define CDU_READ_COUNTER_STEPS 65536

/**
 * Roughly 20 arc-seconds per CDU Read counter step
 */
#define RAD_TO_READ_COUNTER_STEPS(x)  (RADtoDEG(x) * 65536.0 / 360.0)

/**
 * Roughly 160 arc-seconds per CDU error counter step
 */
#define RAD_TO_ERROR_COUNTER_STEPS(x)  (RADtoDEG(x) * 512.0 / 22.5)

#define ERROR_COUNTER_STEPS_TO_DEG(x)  ((x) * 22.5 / 512.0)

/**
 * Roughly 40 arc-seconds per CDU Read counter pulse
 */
#define RAD_TO_READ_COUNTER_PULSES(x)  (RADtoDEG(x) * 32768.0 / 360.0)

#define READ_COUNTER_PULSES_TO_DEG(x) ((x) * 360.0 / 32768.0)

extern "C" {
// Forward declaration from yaAGC (full definition in agc_engine.h)
typedef struct agc_t agc_t;
};

/* ExternalInputs/Outputs: see SaturnIBExternalIO.h (RTW-layout compatible). */

class UplinkMessage;


class SaturnIBStack : public sim::SimModel
{
public:
    // -------------------------------------------------------------------
    // Batch / headless configuration.
    //
    // Every field has a sentinel that means "use the historical
    // hardcoded default": empty string for paths, NaN for doubles,
    // negative for pad_load_start_sec.  A default-constructed
    // BatchConfig reproduces the GUI's behaviour exactly.
    //
    // pad_load_file == "-" disables the automatic uplink entirely.
    //
    // LaunchDate is NOT in this struct -- it is pulled from
    // sim::SimClock::GetCurrentSimulationTime().  Set the sim time
    // BEFORE constructing SaturnIBStack.
    // -------------------------------------------------------------------
    struct BatchConfig {
        std::string rom_path;              // "" = "Colossus249.bin"
        std::string pad_load_file;         // "" = "TM-X-62831_PAD_LOAD.txt", "-" disables
        double      pad_load_start_sec;    // <0 = 15.0
        double      A_z_deg;               // NaN = 72.0 (TM-X-62831 launch azimuth)
        double      CG_LLA_deg_m[3];       // {NaN,NaN,NaN} = KSC LC-34
        double      T_L_prime_sec;         // <0 = 300.0
        double      q_ECItoSM[4];          // {NaN,...} = historical default

        BatchConfig();
    };

    SaturnIBStack(const BatchConfig & config = BatchConfig());
    virtual ~SaturnIBStack(void);

    // Raw plant I/O accessors (SaturnIBExternalIO.h).  Note that
    // q_ECItoSM is overwritten on every TimeStep from m_qECItoSM —
    // set initial IMU alignment via BatchConfig::q_ECItoSM.
    ExternalInputs_saturn_ib_stack        * GetModelInputs();
    const ExternalOutputs_saturn_ib_stack * GetModelOutputs() const;

    // Non-null when the model has reported an error status
    // (via rtmGetErrorStatus).  Otherwise nullptr.
    const char * GetErrorMessage() const;

    /**
     * Fill IGM Layer-1 allowlist sample (igm_trace.h).
     * Returns true if plant signals were available.
     */
    bool FillIgmTrace(struct IgmTraceSample_tag *out, double t_sec) const;

    /** IC (<S5>) trace — stub on Obliq until plant exposes BlockIO equivalents. */
    bool FillIcTrace(struct IcTraceSample_tag *out, double t_sec) const;

    /** On Pad (<S8>) trace — stub on Obliq until plant exposes OUT22/BlockIO. */
    bool FillOnPadTrace(struct OnPadTraceSample_tag *out, double t_sec) const;

    /** S-IB Stage OUT11 trace — stub on Obliq until plant packs OUT11. */
    bool FillSibTrace(struct SibTraceSample_tag *out, double t_sec) const;

    /**
     * Advance the model in time
     * overrride this function if you have extra flight state values to integrate
     */
    virtual bool TimeStep( double dDeltaT );

    /**
     * Return values used to set the visual state of the FDAI
     * Error needle outputs from -1 to +1 (1.0 representing full scale deflection)
     */
    void GetFDAIState( sim::Vector3d * pvAngles_deg,
                       sim::Vector3d * pvRates_dps,
                       sim::Vector3d * pvErrorNeedles_deg );

    bool GetCoarseAlignEnable() const { return m_bCoarseAlignEnable; }
    void SetCoarseAlignEnable( bool bState ) { m_bCoarseAlignEnable = bState; }

    bool GetErrorCounterEnable() const { return m_bErrorCounterEnable; }
    void SetErrorCounterEnable( bool bState ) { m_bErrorCounterEnable = bState; }

    /**
     * Simulate reception of the "reset CDU" signal from the AGC
     */
    void ResetCDUReadCounters();

    /**
     * Called when the AGC signals ISS Turn on delay is complete
     */
    void ISSTurnOnDelayComplete();

    /** DSKY NO ATT lamp edge from ch010 DSPTAB lights relay (panel NOATT_LIGHT). */
    void NotifyNoAttLamp(bool lampOn);

    /**
     * Simulate P/MINC increment of a CDU register.  Functions by updating the value of a register
     * with the given angle.
     * @param nRegister register to be incremented (e.g. RegCDUX)
     * @param dAngle_deg angle 0..360.0
     */
    void SetCDURegister( int nRegister, double dAngle_deg );

    /**
     * Update Gimbal angles and CDU gimbal counters to reflect changes in gimbal angles.
     */
    void UpdateIMUXGimbal( short nSign, unsigned short nPulseCount );
    void UpdateIMUYGimbal( short nSign, unsigned short nPulseCount );
    void UpdateIMUZGimbal( short nSign, unsigned short nPulseCount );

    /**
     * Recalculate (and return) the current IMU operating mode
     */
    ISS_IMU_Mode UpdateIMUMode();

    /**
     * @return returns a string suitable for insertion into a GL TextGrid for debugging
     */
    char * GetDebugText() { return m_szDebugText; }

     /**
     * @return returns the yaAGC decoded downlink string
     * suitable for insertion into a GL TextGrid for debugging
     */
    char * GetDownlinkText() { return m_szDownlinkText; }

    /**
     * Compute the quantized equivalent of the given input angle.  The coarse resolver is
     * quantized through a 7-bit A/D ladder, so it moves in 2.8125 degree steps.
     */
    double CoarseResolverQuantization(double dAngle_deg);

    double FineResolverQuantization( double dAngle_deg );

    /**
     * Update the count Fine Align pulses remaining to be executed.
     */
    void IncrementFineAlignPulseCount( ISS_Fine_Align_Axis eAxis,
                                       int nSign,
                                       unsigned short usPulseCount );

    /**
     * Record the state of the simulation for a restart. This includes saving the current AGC state
     * as well as the state of the IMU and vehicle (this is inly partially implemented at this point).
     */
    void SaveState( const std::string & strName );

    /**
     * Diagnostic accessor: returns the underlying yaAGC state pointer so a
     * host application can inspect input/output channels for tracing.
     * Not part of the intended public API -- do not use for control.
     */
    agc_t * DebugGetAGC() { return m_pAGC; }

protected:
    // BatchConfig-derived state consulted at runtime.
    std::string m_pad_load_file;         // "" or "-" disables auto pad-load
    double      m_pad_load_start_sec;    // elapsed-time threshold

    /* Obliq plant + RTW-shaped I/O (no Simulink model pointer). */
    SaturnIBPlantObliq * m_pPlant = nullptr;
    ExternalInputs_saturn_ib_stack m_inputs{};
    ExternalOutputs_saturn_ib_stack m_outputs{};
    const char * m_pszErrorStatus = nullptr;

    double m_dFDAI_Roll_deg;                 /* '<Root>/FDAI_Roll_deg' */
    double m_dFDAI_Pitch_deg;                /* '<Root>/FDAI_Pitch_deg' */
    double m_dFDAI_Yaw_deg;                  /* '<Root>/FDAI_Yaw_deg' */
    double m_dFDAI_RollDot_dps;              /* '<Root>/FDAI_RollDot_dps' */
    double m_dFDAI_PitchDot_dps;             /* '<Root>/FDAI_PitchDot_dps' */
    double m_dFDAI_YawDot_dps;               /* '<Root>/FDAI_YawDot_dps' */
    sim::GeodeticPosition m_wcGCLocation;    // current stage position (debug display)
    double m_dP_feet[3];                     // current stage ECI position, feet (debug display)

    sim::Vector3d m_vCM_IMU_PIPA_Pulses;     /* '<Root>/CM_IMU_PIPA_Pulses' */

    /**
     * Stable member orientation in ECI frame
     */
    sim::Quaternion m_qECItoSM;

    /**
     * ISS SM outer (X) gimbal angle, expressed in degrees
     */
    double m_dIMU_OGAResolver_deg;

    /**
     * ISS SM inner (Y) gimbal angle, expressed in degrees
     */
    double m_dIMU_IGAResolver_deg;

    /**
     * ISS SM middle (Z) gimbal angle, expressed in degrees
     */
    double m_dIMU_MGAResolver_deg;

    double m_dLastSentIMU_OGAResolver_deg;
    double m_dLastSentIMU_IGAResolver_deg;
    double m_dLastSentIMU_MGAResolver_deg;

    sim::Vector3d m_dTotalCDUPulseIncrements;
    
    /**
     * Boolean flag indicating first pass through TimeStep -- used to correctly
     * drive CDU and PIPA pulse tracking.
     */
    bool m_bFirstPass;

    /**
     * We will not send any CDU pulses until we are reset by the AGC
     */
    bool m_bFirstResetReceived;

    /**
     * Reflects current IMU operating mode.  Updated as needed (at least once every time step).
     */
    ISS_IMU_Mode m_eIMUMode;

    /**
     * DSKY NO ATT lamp (DSPTAB lights relay, ch010 cmd 12 bit4 / 1<<3).
     * Latched from AGC channel output — same signal apolloa uses for NOATT_LIGHT.
     */
    bool m_bNoAttLampOn;

    /**
     * Set when AGC asserts ch012 BIT15 (ISS 90 s turn-on delay complete).
     * NO ATT offs *before* this are ignored for SV arming (DSPTAB can clear
     * the bit early and falsely arm ~45 s into the run).
     */
    bool m_bIssTurnOnDelayComplete;

    /**
     * Sim elapsed time of the first *valid* NO ATT on→off after ISS turn-on
     * delay complete; <0 if not yet armed.  Never updated on later P01/P02
     * lamp cycles.  SV waits until this + SV_AFTER_NOATT_OFF_SEC (lamp off).
     */
    double m_dNoAttOffElapsed_sec;

    /** Delay after valid NO ATT extinguish before auto P27 state-vector uplink. */
    static constexpr double SV_AFTER_NOATT_OFF_SEC = 45.0;

    /** Latch first valid NO ATT-off arm time (idempotent). */
    void ArmStateVectorAfterNoAttOff(double elapsed_sec);

    /**
     * PIPA pulse counts at the last sample taken by the AGC
     */
    sim::Vector3d m_vLastCM_IMU_PIPA_Pulses;

    /** Cumulative signed PIPA pulses actually injected into yaAGC (host path). */
    sim::Vector3d m_vPipaPulsesInjectedCum;

    /** IMU_DEBUG logger state */
    bool   m_bImuDebug = false;
    FILE * m_pImuDebugFile = nullptr;
    int    m_nImuDebugLastSec = -1;

    /**
     * Gimbal angles commanded by CMC. Only valid in Coarse Alignment mode.
     */
    sim::Vector3d m_vCoarseAlignCommand_deg;

    /**
     * Current Coarse alignment error signal (used to determine M/PINC pulse rates.
     */
    sim::Vector3d m_vCoarseAlignError_deg;

    bool    m_bCoarseAlignFastRate[3];

    /**
     * Fine alignment is only performed one axis at a time
     */
    ISS_Fine_Align_Axis m_eFineAlignAxis;

    /**
     * Currently remaining fine alignment pulse count (sign indicates the direction
     * of the pulse operations).  Fine align operation ends when the pulse count reaches zero.
     */
    double  m_dFineAlignPulseCount;

    int     m_nFineAlignPulseSign;

    /**
     * CDU Error Counter tracking (for FDAI needle outputs)
     */
    short m_nCDUXErrorCounter_pulses;
    short m_nCDUYErrorCounter_pulses;
    short m_nCDUZErrorCounter_pulses; 

    /**
     * Channel 012 signal, bit 4
     */
    bool m_bCoarseAlignEnable;

    /**
     * Channel 012 signal, bit 6
     */
    bool m_bErrorCounterEnable;

    /**
     * Liftoff discrete from S-IVB to CM
     */
    bool m_bLiftoff;

    /**
     *  The yaAGC simulator engine (thanks to Ron Burkey and team!)
     */
    agc_t   * m_pAGC;

    /**
     * Used to hold uneven integration time chunks between time steps
     */
    double m_dResidualDeltaT_sec;

    /**
     * Used to hold fractional AGC CPU cycles between time steps
     */
    double m_dAGCResidualDeltaT_sec;

    /**
     * Used to hold fractional IMU cycles between time steps
     */
    double m_dIMUResidualDeltaT_sec;

    char   m_szDebugText[17*80];

    char   m_szDownlinkText[16*1024];

    UplinkMessage * m_pCurrentUplinkMessage;

    /**
     * Simulated elapsed time to send the next uplink word to the CMC.
     * Uplink is 1,600 bps with 22-bit words (I have not found a deatiled enough reference to
     * include framing information or other system delays, so we'll add some padding onto that 
     * and send one command word every 0.02 seconds.
     */
    double  m_dNextUplinkTime_sec;

    /**
     * A constant matrix transforming the Launch Vehicle frame to the Command Module frame.
     * CSM is mated to the L/V such that the CM +Z axis is aligned with the LV -Z axis.
     */
    sim::Matrix4d m_mLVtoCM;

    /**
     * Open the specified uplink file and send it's contents to the CMC as a simulated
     * uplink data stream.
     */
    bool StartUplink( const std::string & strFile );

    /**
     * Uses the simulation clock to manage transmitting the current uplink file to the CMC
     * @return returns true if the uplink operation is complete
     */
    bool UplinkTimeStep();

    void WriteUplinkKeycode( int nKeycode );

    /**
     * Execute one plant time step (Obliq backend)
     */
    void rt_OneStep();

    /**
     * Update PIPA Counters to reflect velocity changes sensed by the CM IMU
     * since the last call to AGCUpdatePIPACounters();
     */
    void AGCUpdatePIPACounters();

    /**
     * Send any required unprogrammed increment(s) to AGC to reflect
     * changes in ISS Gimbal angles
     */
    void AGCUpdateGimbalCDUCounters();

    void UpdateDebugText();

    /**
     * 1 Hz CSV logger for P11 / IMU triage (PIPAs, CDUs, resolvers, plant Ve/h,
     * SM triad quality, and AGC RN/VN/REFSMMAT vs plant).
     * Enabled when env IMU_DEBUG is set (any value except "0"). Writes imu_pipa_cdu.csv
     * in the process CWD (typically ApolloA/).
     */
    void MaybeLogImuPipaCdu();

    /**
     * @param mMIS Body to Stable Member transform
     * @param pdOGA_rad X-Axis Gimbal angle
     * @param pdIGA_rad Y-Axis Gimbal angle
     * @param pdMGA_rad Z-Axis Gimbal angle
     */
    void MIStoEuler( const sim::Matrix4d & mMIS, double *pdOGA_rad, double *pdIGA_rad, double *pdMGA_rad);

    /**
     *
     */
    sim::Matrix4d EulerToMIS( double dOGA_rad, double dIGA_rad, double dMGA_rad );
};

extern SaturnIBStack * g_pSaturnIBStack;
