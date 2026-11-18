/*
 * LVDA_LVDC.c — obliq-2 native atomic entry points for the LVDC
 * rewrite.
 *
 * Thin dispatcher. The real EDD-anchored work lives in the per-
 * section modules (lvdc_nav, lvdc_frames, lvdc_quant, and the Phase
 * C+ additions). This file owns only:
 *   - the public ABI contract (LVDA_LVDC_* entry points)
 *   - the mapping from obliq-2 inputs/outputs to the internal
 *     rewrite state
 *   - the per-step scheduling (per-tick guard + minor loop + BML
 *     + major cycle gates)
 *
 * The obliq-2 parent compiles against LVDA_LVDC.h which is field-
 * identical to the current hand-edited snapshot. The large
 * signals_t / states_t / data_stores_t / enable_states_t declared
 * there are kept for ABI compatibility; most fields are untouched
 * by this body and remain at their init values.
 *
 * See docs/LVDC_REWRITE_PLAN.md for scope and milestones.
 * See docs/LVDC_RK4_REENTRY_REPORT.md for the obliq-2 native
 * atomic scheduling contract. In short: compute_outputs runs 4
 * times per major step with a stable sample_tick; update_memory_states
 * runs once per step after the RK4 combine.
 */

#include "LVDA_LVDC.h"

#include "lvdc_internal.h"
#include "lvdc_nav.h"

#include <string.h>

/* The rewrite keeps its own state in a module-local singleton. The
 * obliq-2 native atomic ABI stores sub-system state in the
 * LVDA_LVDC_t struct, but the rewrite does not use its signals_t /
 * states_t layout (those are codegen-shaped names). A file-scope
 * static works here because the obliq-2 host does not instantiate
 * LVDA_LVDC more than once in the Saturn IB plant.
 *
 * Init resets this cleanly, so there is no cross-run state leak.
 * Per the RK4 report, statics survive across RK4 stages (good) and
 * across LVDC restarts (hence the explicit reset). */
static lvdc_state_t g_lvdc;

/* ---------------------------------------------------------------------
 * ABI entry points
 * ------------------------------------------------------------------ */

void LVDA_LVDC_init(LVDA_LVDC_t* model) {
    memset(model, 0, sizeof(*model));
    model->enabled = 1;
    lvdc_nav_reset(&g_lvdc);
}

void LVDA_LVDC_compute_outputs(LVDA_LVDC_t* model) {
    if (!model->enabled) {
        return;
    }

    /* Per-tick guard. See docs/LVDC_RK4_REENTRY_REPORT.md.
     * sample_tick is stable across the four RK4 stages of one major
     * step; one-shot work fires on the first call of each new tick. */
    const bool first_call_this_step =
        (model->sample_tick != g_lvdc.last_compute_tick);

    if (first_call_this_step) {
        g_lvdc.last_compute_tick = model->sample_tick;
    }

    /* Phase B scaffolding: no outputs produced yet. The real
     * dispatcher lands in subsequent commits. */
    (void)first_call_this_step;
}

void LVDA_LVDC_update_memory_states(LVDA_LVDC_t* model) {
    if (!model->enabled) {
        return;
    }

    /* Called exactly once per major step, after the RK4 combine.
     * All discrete state transitions (timebase advances, IGM mode
     * changes, HSL latches) belong here, not in compute_outputs. */
    g_lvdc.last_memory_tick = model->sample_tick;
}

void LVDA_LVDC_evaluate_enable_states(LVDA_LVDC_t* model) {
    if (!model->enabled) {
        return;
    }

    /* The old snapshot writes dozens of nested enable flags here for
     * its Simulink-shaped enable sub-system scopes. The rewrite does
     * not use those flags; it schedules by time and mode directly.
     * No-op. */
}
