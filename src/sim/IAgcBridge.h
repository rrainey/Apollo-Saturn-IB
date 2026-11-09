#pragma once

#include "SimSupport.h"

/*
 * SaturnIBStack ↔ CMC bridge contract.
 *
 * SaturnIBStack is pure launch-vehicle physics plus IMU-gimbal state; it
 * knows nothing about yaAGC. Where its TimeStep inner loop needs to talk
 * to the CMC it calls through an IAgcBridge. apolloa installs a real
 * SaturnIBAgcBridge (lives in saturnib_cmc, links yaAGC); batch_sim leaves
 * the stack's default NullAgcBridge in place and the AGC hook points
 * degrade to no-ops.
 *
 * All methods are per-tick hooks called from inside SaturnIBStack::TimeStep.
 */
class IAgcBridge {
public:
    virtual ~IAgcBridge() = default;

    /* Called once at the top of TimeStep with the current elapsed sim time
     * and the pad-frame ECI pos/vel (OUT22) so the bridge can decide when
     * to kick off the pad-load uplink and the state-vector uplink. */
    virtual void OnTimeStepBegin(double elapsed_sec,
                                 const sim::Vector3d &pad_pos_eci_m,
                                 const sim::Vector3d &pad_vel_eci_mps) = 0;

    /* Inside TimeStep's per-model-step inner loop (200 Hz) — pump one
     * uplink keycode into the CMC if an uplink is active. */
    virtual void UplinkTimeStep() = 0;

    /* Inside the per-stable-member inner loop (1 kHz) — push gimbal-angle
     * deltas (since last call) to the CMC's CDU read counters. */
    virtual void InjectGimbalCDU() = 0;

    /* Inside the per-stable-member inner loop — advance the AGC core by
     * one stable-member step worth of CPU cycles. */
    virtual void StepAGC(double dt_sec) = 0;

    /* After StepPlant each model step — push PIPA pulse deltas (since last
     * call) to the CMC's PIPA counters. */
    virtual void InjectPIPA() = 0;

    /* End-of-TimeStep: diagnostic logging and any one-shot AGC state checks. */
    virtual void OnTimeStepEnd() = 0;
};

/* Default no-op bridge. SaturnIBStack holds a pointer to this when no real
 * bridge is attached (batch_sim path). */
class NullAgcBridge : public IAgcBridge {
public:
    void OnTimeStepBegin(double, const sim::Vector3d &,
                         const sim::Vector3d &) override {}
    void UplinkTimeStep() override {}
    void InjectGimbalCDU() override {}
    void StepAGC(double) override {}
    void InjectPIPA() override {}
    void OnTimeStepEnd() override {}
};
