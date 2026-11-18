/*
 * lvdc_nav.c — boost-phase navigation (EDD §4.2 and §4.3).
 *
 * Phase B scaffolding. The function bodies land in the next commit;
 * the stubs here keep the dispatcher compilable.
 */

#include "lvdc_nav.h"

#include <string.h>

void lvdc_nav_reset(lvdc_state_t *s) {
    memset(s, 0, sizeof(*s));
    s->last_compute_tick = (uint64_t)-1;
    s->last_memory_tick  = (uint64_t)-1;
}

void lvdc_nav_grr_latch(lvdc_state_t *s,
                        const double V_m_bar_mps_now[3],
                        const double X_0_eq343[3],
                        const double V_0_eq344[3]) {
    /* TODO (Phase B commit 2): EDD §3.4 Eq. 3.4.3/3.4.4 seed and
     * §4.2.3 zero reset. */
    (void)s;
    (void)V_m_bar_mps_now;
    (void)X_0_eq343;
    (void)V_0_eq344;
}

void lvdc_nav_bml_step(lvdc_state_t *s,
                       const double V_m_bar_mps_now[3],
                       double t_now_sec) {
    /* TODO (Phase B commit 2): EDD §4.2 accel processing + §4.3 nav. */
    (void)s;
    (void)V_m_bar_mps_now;
    (void)t_now_sec;
}
