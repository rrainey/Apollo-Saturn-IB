#pragma once

#include "SimSupport.h"
#include "SaturnIBExternalIO.h"

namespace dskymatic { class BinaryWriter; class BinaryReader; }

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
 * Roughly 40 arc-seconds per CDU Read counter pulse
 */
#define RAD_TO_READ_COUNTER_PULSES(x)  (RADtoDEG(x) * 32768.0 / 360.0)

#define READ_COUNTER_PULSES_TO_DEG(x) ((x) * 360.0 / 32768.0)

/**
 * Roughly 160 arc-seconds per CDU error counter step
 */
#define RAD_TO_ERROR_COUNTER_STEPS(x)  (RADtoDEG(x) * 512.0 / 22.5)

#define ERROR_COUNTER_STEPS_TO_DEG(x)  ((x) * 22.5 / 512.0)

class IAgcBridge;

class SaturnIBStack : public sim::SimModel
{
public:
    // -------------------------------------------------------------------
    // Batch / headless configuration.  Every field has a sentinel that
    // means "use the historical hardcoded default": empty string for
    // paths, NaN for doubles, negative for pad_load_start_sec.  A
    // default-constructed BatchConfig reproduces the GUI's behaviour.
    //
    // pad_load_file == "-" disables the automatic uplink entirely.
    //
    // rom_path / pad_load_file / pad_load_start_sec are AGC-bridge
    // concerns.  SaturnIBStack ignores them; the saturnib_cmc bridge
    // reads them when it is attached.
    //
    // LaunchDate is pulled from sim::SimClock::GetCurrentSimulationTime().
    // Set the sim time BEFORE constructing SaturnIBStack.
    // -------------------------------------------------------------------
    struct BatchConfig {
        std::string rom_path;              // "" = "Colossus249.bin"  (bridge)
        std::string pad_load_file;         // "" = "TM-X-62831_PAD_LOAD.txt", "-" disables (bridge)
        double      pad_load_start_sec;    // <0 = 15.0  (bridge)
        double      A_z_deg;               // NaN = 72.0 (TM-X-62831 launch azimuth)
        double      CG_LLA_deg_m[3];       // {NaN,NaN,NaN} = KSC LC-34
        double      T_L_prime_sec;         // <0 = 300.0
        double      GRR_offset_sec;        // NaN = -5.0 (AS-205 per TM-X-62831)
        double      q_ECItoSM[4];          // {NaN,...} = historical default

        BatchConfig();
    };

    SaturnIBStack(const BatchConfig & config = BatchConfig());
    virtual ~SaturnIBStack(void);

    /** Install (or detach, with nullptr) the CMC bridge.  Called by the
     *  SaturnIBAgcBridge ctor/dtor.  Default is an internal null bridge;
     *  AGC hook points then no-op. */
    void SetAgcBridge(IAgcBridge *bridge);
    IAgcBridge * GetAgcBridge() const { return m_pAgcBridge; }

    // Raw plant I/O accessors (SaturnIBExternalIO.h).  Note that
    // q_ECItoSM is overwritten on every TimeStep from m_qECItoSM —
    // set initial IMU alignment via BatchConfig::q_ECItoSM.
    ExternalInputs_saturn_ib_stack        * GetModelInputs();
    const ExternalOutputs_saturn_ib_stack * GetModelOutputs() const;

    // Non-null when the model has reported an error status.
    const char * GetErrorMessage() const;

    /** Fill IGM Layer-1 allowlist sample (igm_trace.h). */
    bool FillIgmTrace(struct IgmTraceSample_tag *out, double t_sec) const;

    /** Fill LVDC I/O trace sample — inputs the body actually reads
     *  plus R_S_bar_m as the golden nav output. For the lvdc-rewrite
     *  shadow-run test. */
    bool FillLvdcIoTrace(struct LvdcIoTraceSample_tag *out, double t_sec) const;

    /** IC (<S5>) trace — stub on Obliq until plant exposes BlockIO equivalents. */
    bool FillIcTrace(struct IcTraceSample_tag *out, double t_sec) const;

    /** On Pad (<S8>) trace — stub on Obliq until plant exposes OUT22/BlockIO. */
    bool FillOnPadTrace(struct OnPadTraceSample_tag *out, double t_sec) const;

    /** S-IB Stage OUT11 trace — stub on Obliq until plant packs OUT11. */
    bool FillSibTrace(struct SibTraceSample_tag *out, double t_sec) const;

    /** Advance the model in time (plant + IMU physics; AGC via virtual hooks). */
    virtual bool TimeStep( double dDeltaT );

    /** Return values used to set the visual state of the FDAI. */
    void GetFDAIState( sim::Vector3d * pvAngles_deg,
                       sim::Vector3d * pvRates_dps,
                       sim::Vector3d * pvErrorNeedles_deg );

    bool GetCoarseAlignEnable() const { return m_bCoarseAlignEnable; }
    void SetCoarseAlignEnable( bool bState ) { m_bCoarseAlignEnable = bState; }

    bool GetErrorCounterEnable() const { return m_bErrorCounterEnable; }
    void SetErrorCounterEnable( bool bState ) { m_bErrorCounterEnable = bState; }

    /** Zero the stack-side CDU accumulators and reseed SM orientation from
     *  the current vehicle attitude.  The AGC-side erasable zeroing lives
     *  in the bridge (SaturnIBAgcBridge::ResetCDUReadCounters). */
    void ResetCDUInternalState();

    /** Called from the AGC-bridge channel handlers when the CMC drives the
     *  IMU gimbal CDUs (coarse-align mode) or writes error-needle counters. */
    void UpdateIMUXGimbal( short nSign, unsigned short nPulseCount );
    void UpdateIMUYGimbal( short nSign, unsigned short nPulseCount );
    void UpdateIMUZGimbal( short nSign, unsigned short nPulseCount );

    /** Recalculate (and return) the current IMU operating mode. */
    ISS_IMU_Mode UpdateIMUMode();

    /** @return string suitable for insertion into a GL TextGrid for debugging. */
    char * GetDebugText() { return m_szDebugText; }

    /** Quantization helpers for the IMU resolvers. */
    double CoarseResolverQuantization(double dAngle_deg);
    double FineResolverQuantization( double dAngle_deg );

    /** Record the AGC-driven fine-align pulse command. */
    void IncrementFineAlignPulseCount( ISS_Fine_Align_Axis eAxis,
                                       int nSign,
                                       unsigned short usPulseCount );

    /** Record state of the simulation for a restart.  SaturnIBStack writes
     *  the plant-side dump; SaturnIBAgcBridge overrides to also write the
     *  AGC core dump. */
    virtual void SaveState( const std::string & strName );

    /** Binary save-state hooks (used by dskymatic-studio's --save-state-time
     *  / --resume).  SaturnIBStack writes its own fields and the plant's
     *  persistent partitions (states, data_stores, enable_states, time
     *  scalars); SaturnIBAgcBridge overrides to also write the bridge
     *  state + agc_t core.  Caller must have the matching sim objects
     *  already constructed before DeserializeState is invoked. */
    virtual void SerializeState  (dskymatic::BinaryWriter &w) const;
    virtual bool DeserializeState(dskymatic::BinaryReader &r);

    // ------------------------------------------------------------------
    // Accessors for the bridge (read current stack state inside AGC hooks).
    // ------------------------------------------------------------------
    double   GetIMUOGAResolverDeg() const { return m_dIMU_OGAResolver_deg; }
    double   GetIMUIGAResolverDeg() const { return m_dIMU_IGAResolver_deg; }
    double   GetIMUMGAResolverDeg() const { return m_dIMU_MGAResolver_deg; }
    const sim::Vector3d & GetPipaPulses() const { return m_vCM_IMU_PIPA_Pulses; }
    bool     GetLiftoff() const { return m_bLiftoff; }
    bool     IsFirstPass() const { return m_bFirstPass; }
    ISS_IMU_Mode GetIMUMode() const { return m_eIMUMode; }

protected:
    /* Obliq plant + ExtIn/Out I/O structs (no Simulink model pointer). */
    SaturnIBPlantObliq * m_pPlant = nullptr;
    ExternalInputs_saturn_ib_stack m_inputs{};
    ExternalOutputs_saturn_ib_stack m_outputs{};
    const char * m_pszErrorStatus = nullptr;

    double m_dFDAI_Roll_deg;
    double m_dFDAI_Pitch_deg;
    double m_dFDAI_Yaw_deg;
    double m_dFDAI_RollDot_dps;
    double m_dFDAI_PitchDot_dps;
    double m_dFDAI_YawDot_dps;
    sim::GeodeticPosition m_wcGCLocation;
    double m_dP_feet[3];

    /* Plant CM IMU PIPA output (bridge reads via GetPipaPulses). */
    sim::Vector3d m_vCM_IMU_PIPA_Pulses;

    /* Stable member orientation in ECI frame. */
    sim::Quaternion m_qECItoSM;

    /* ISS SM gimbal angles (physics state; bridge reads via accessors). */
    double m_dIMU_OGAResolver_deg;
    double m_dIMU_IGAResolver_deg;
    double m_dIMU_MGAResolver_deg;

    /* Last gimbal angle sent to the CMC (bridge writes, stack reads from
     * UpdateDebugText).  Kept on stack so debug text has no AGC dependency. */
    double m_dLastSentIMU_OGAResolver_deg;
    double m_dLastSentIMU_IGAResolver_deg;
    double m_dLastSentIMU_MGAResolver_deg;

    sim::Vector3d m_dTotalCDUPulseIncrements;

    /** First-pass flag used by plant and bridge to seed last-sent values. */
    bool m_bFirstPass;

    /** Current IMU operating mode (updated at least once per TimeStep). */
    ISS_IMU_Mode m_eIMUMode;

    /* Coarse-align commanded angles (CMC CDU drive pulses accumulate here). */
    sim::Vector3d m_vCoarseAlignCommand_deg;
    sim::Vector3d m_vCoarseAlignError_deg;
    bool    m_bCoarseAlignFastRate[3];

    /* Fine-align pulse accumulator (one axis at a time). */
    ISS_Fine_Align_Axis m_eFineAlignAxis;
    double  m_dFineAlignPulseCount;
    int     m_nFineAlignPulseSign;

    /* CDU Error Counter pulses → FDAI needle displacements. */
    short m_nCDUXErrorCounter_pulses;
    short m_nCDUYErrorCounter_pulses;
    short m_nCDUZErrorCounter_pulses;

    bool m_bCoarseAlignEnable;    // ch012 BIT4
    bool m_bErrorCounterEnable;   // ch012 BIT6

    /* Liftoff discrete from S-IVB to CM (plant output, latched here). */
    bool m_bLiftoff;

    double m_dResidualDeltaT_sec;    // outer-loop time remainder
    double m_dIMUResidualDeltaT_sec; // IMU sub-loop remainder

    char   m_szDebugText[17*80];

    /* Body→CM mount (CM +Z opposes L/V -Z; 180° about X). */
    sim::Matrix4d m_mLVtoCM;

    /** Execute one plant time step (Obliq backend). */
    void StepPlant();

    void UpdateDebugText();

    void MIStoEuler( const sim::Matrix4d & mMIS,
                     double *pdOGA_rad, double *pdIGA_rad, double *pdMGA_rad);
    sim::Matrix4d EulerToMIS( double dOGA_rad, double dIGA_rad, double dMGA_rad );

private:
    IAgcBridge * m_pAgcBridge;    // null-bridge default; set by SetAgcBridge()
};

extern SaturnIBStack * g_pSaturnIBStack;
