#include "lvdc_io_trace.h"

const char *lvdc_io_trace_csv_header(void) {
    return "t_sec,sample_tick,"
           "V_m_bar_x_mps,V_m_bar_y_mps,V_m_bar_z_mps,"
           "Theta_x_deg,Theta_y_deg,Theta_z_deg,"
           "bLiftoff,bSIBPropellantSensorDry,"
           "R_S_x_m,R_S_y_m,R_S_z_m";
}

void lvdc_io_trace_write_row(FILE *fp, const LvdcIoTraceSample *s) {
    if (!fp || !s) return;
    fprintf(fp,
            "%.12g,%llu,"
            "%.17g,%.17g,%.17g,"
            "%.17g,%.17g,%.17g,"
            "%d,%d,"
            "%.17g,%.17g,%.17g\n",
            s->t_sec, s->sample_tick,
            s->V_m_bar_mps[0], s->V_m_bar_mps[1], s->V_m_bar_mps[2],
            s->Theta_deg[0],   s->Theta_deg[1],   s->Theta_deg[2],
            s->bLiftoff ? 1 : 0,
            s->bSIBPropellantSensorDry ? 1 : 0,
            s->R_S_bar_m[0], s->R_S_bar_m[1], s->R_S_bar_m[2]);
}
