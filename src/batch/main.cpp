// Batch driver for the Saturn-IB stack.  Reads inputs from a JSON file,
// runs the sim for a configurable duration at a configurable step,
// writes final state + 1 Hz time-series.
//
// Usage: batch_sim <inputs.json> [--run-time SEC] [--final PATH] [--csv PATH]
//                  [--igm-trace PATH] [--igm-trace-dt SEC]
//                  [--ic-trace PATH] [--onpad-trace PATH] [--sib-trace PATH]
//
// See examples/AS-205-nominal.json for the schema.
// IGM trace: dual-harness Layer-1 CSV (igm_trace.h); compare with
// scripts/compare_igm_trace.py.
// IC trace: <S5> Initial Conditions JSON (ic_trace.h) for matched compare.
// On Pad trace: <S8>/OUT22 + nested <S74> JSON (onpad_trace.h).
// S-IB trace: OUT11 + bLiftoff JSON (sib_trace.h); pad gate vs On Pad.

#include <SaturnIBStack.h>
#include <SaturnIBExternalIO.h>   // ExternalInputs/Outputs field defs
#include <SimSupport.h>

#include "igm_trace.h"
#include "lvdc_io_trace.h"
#include "ic_trace.h"
#include "onpad_trace.h"
#include "sib_trace.h"

#include <nlohmann/json.hpp>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

using json = nlohmann::json;

// -----------------------------------------------------------------------
// Named-field layouts for OUT11, OUT12, OUT22.
//
// OUT11/OUT12 index -> name mapping mirrors the OUT11_* / OUT12_*
// #defines in SaturnIBStack.cpp (which are private to that TU).  OUT12
// is populated only post-staging; pre-staging its fields are all zero.
//
// Semantic prefixes for CSV / final.json:
//   pad_*  = OUT22, S-IB pad reference frame (largely static)
//   s1_*   = OUT11, joined stack pre-staging -> dropped S-IB post-staging
//   s2_*   = OUT12, zero pre-staging -> S-IVB + CSM post-staging
// -----------------------------------------------------------------------
struct NamedField {
    int         index;
    const char *suffix;   // appended after prefix (e.g. "lat_deg")
};

// OUT11: 26 elements.  0..23 have documented names in SaturnIBStack.cpp;
// [24] is the model block "Compare_c3" -- a discrete flag we surface
// under an intentionally generic name pending a semantic identification.
// [25] is the adapter-computed Fischer-1960 geocentric-radial altitude
// (see docs/ALTITUDE_CONVENTIONS.md); the native [2] altitude is WGS-84.
static const NamedField OUT11_FIELDS[] = {
    { 0, "lat_deg"},         { 1, "lon_deg"},         { 2, "h_m"},
    { 3, "Xe_x_m"},          { 4, "Xe_y_m"},          { 5, "Xe_z_m"},
    { 6, "Ve_x_mps"},        { 7, "Ve_y_mps"},        { 8, "Ve_z_mps"},
    { 9, "IMU_theta_x_deg"}, {10, "IMU_theta_y_deg"}, {11, "IMU_theta_z_deg"},
    {12, "alpha_deg"},       {13, "Vb_mps"},
    {14, "q_ECI_q0"},        {15, "q_ECI_q1"},        {16, "q_ECI_q2"}, {17, "q_ECI_q3"},
    {18, "omega_b_x_rps"},   {19, "omega_b_y_rps"},   {20, "omega_b_z_rps"},
    {21, "Vb_x_mps"},        {22, "Vb_y_mps"},        {23, "Vb_z_mps"},
    {24, "compare_c3"},
    {25, "h_fischer_m"},
};

// OUT12: 25 elements, [0..23] layout matches OUT11, [24] = Fischer altitude.
static const NamedField OUT12_FIELDS[] = {
    { 0, "lat_deg"},         { 1, "lon_deg"},         { 2, "h_m"},
    { 3, "Xe_x_m"},          { 4, "Xe_y_m"},          { 5, "Xe_z_m"},
    { 6, "Ve_x_mps"},        { 7, "Ve_y_mps"},        { 8, "Ve_z_mps"},
    { 9, "IMU_theta_x_deg"}, {10, "IMU_theta_y_deg"}, {11, "IMU_theta_z_deg"},
    {12, "alpha_deg"},       {13, "Vb_mps"},
    {14, "q_ECI_q0"},        {15, "q_ECI_q1"},        {16, "q_ECI_q2"}, {17, "q_ECI_q3"},
    {18, "omega_b_x_rps"},   {19, "omega_b_y_rps"},   {20, "omega_b_z_rps"},
    {21, "Vb_x_mps"},        {22, "Vb_y_mps"},        {23, "Vb_z_mps"},
    {24, "h_fischer_m"},
};

// OUT22: 10 elements, S-IB pad reference frame; [9] = Fischer altitude.
static const NamedField OUT22_FIELDS[] = {
    { 0, "lat_deg"},   { 1, "lon_deg"},   { 2, "h_m"},
    { 3, "Xe_x_m"},    { 4, "Xe_y_m"},    { 5, "Xe_z_m"},
    { 6, "Ve_x_mps"},  { 7, "Ve_y_mps"},  { 8, "Ve_z_mps"},
    { 9, "h_fischer_m"},
};
constexpr size_t OUT11_N = sizeof(OUT11_FIELDS)/sizeof(*OUT11_FIELDS);
constexpr size_t OUT12_N = sizeof(OUT12_FIELDS)/sizeof(*OUT12_FIELDS);
constexpr size_t OUT22_N = sizeof(OUT22_FIELDS)/sizeof(*OUT22_FIELDS);

// -----------------------------------------------------------------------
// Config surface (populated from JSON + CLI overrides)
// -----------------------------------------------------------------------
struct RunConfig {
    // sim.*
    double run_time_sec  = 800.0;   // enough to cover launch + first-stage burn
    double time_step_sec = 0.01;
    // launch date -- passed to SimClock::SetCurrentSimulationTime
    // (which SaturnIBStack::ctor reads to populate pModelInputs->LaunchDate)
    int    year = 1968, month = 10, day = 11;
    int    hour = 14,   minute = 57;
    double second = 45.0;
    // Output paths
    std::string final_path = "final.json";
    std::string csv_path   = "results.csv";
    // Plant + AGC config
    SaturnIBStack::BatchConfig sat;
};

// -----------------------------------------------------------------------
// JSON helpers
// -----------------------------------------------------------------------
namespace {

double get_dbl(const json &obj, const char *key, double dflt) {
    return obj.contains(key) ? obj.at(key).get<double>() : dflt;
}
int get_int(const json &obj, const char *key, int dflt) {
    return obj.contains(key) ? obj.at(key).get<int>() : dflt;
}
std::string get_str(const json &obj, const char *key, const std::string &dflt) {
    return obj.contains(key) ? obj.at(key).get<std::string>() : dflt;
}

void parse_array_3(const json &obj, const char *key, double out[3]) {
    if (!obj.contains(key)) return;
    const auto &a = obj.at(key);
    if (!a.is_array() || a.size() != 3)
        throw std::runtime_error(std::string(key) + " must be a 3-element array");
    for (int i = 0; i < 3; ++i) out[i] = a[i].get<double>();
}
void parse_array_4(const json &obj, const char *key, double out[4]) {
    if (!obj.contains(key)) return;
    const auto &a = obj.at(key);
    if (!a.is_array() || a.size() != 4)
        throw std::runtime_error(std::string(key) + " must be a 4-element array");
    for (int i = 0; i < 4; ++i) out[i] = a[i].get<double>();
}

RunConfig load_config(const std::string &path) {
    RunConfig cfg;
    std::ifstream in(path);
    if (!in)
        throw std::runtime_error("cannot open " + path);
    json j;
    in >> j;

    if (j.contains("sim")) {
        const auto &s = j["sim"];
        cfg.run_time_sec  = get_dbl(s, "run_time_sec",  cfg.run_time_sec);
        cfg.time_step_sec = get_dbl(s, "timeStep",      cfg.time_step_sec);
        cfg.final_path    = get_str(s, "final_output",  cfg.final_path);
        cfg.csv_path      = get_str(s, "csv_output",    cfg.csv_path);
    }
    if (j.contains("launch_date")) {
        const auto &d = j["launch_date"];
        cfg.year   = get_int(d, "year",   cfg.year);
        cfg.month  = get_int(d, "month",  cfg.month);
        cfg.day    = get_int(d, "day",    cfg.day);
        cfg.hour   = get_int(d, "hour",   cfg.hour);
        cfg.minute = get_int(d, "minute", cfg.minute);
        cfg.second = get_dbl(d, "second", cfg.second);
    }
    if (j.contains("model")) {
        const auto &m = j["model"];
        cfg.sat.A_z_deg        = get_dbl(m, "A_z_deg",       cfg.sat.A_z_deg);
        cfg.sat.T_L_prime_sec  = get_dbl(m, "T_L_prime_sec", cfg.sat.T_L_prime_sec);
        parse_array_3(m, "CG_LLA_deg_m", cfg.sat.CG_LLA_deg_m);
        parse_array_4(m, "q_ECItoSM",    cfg.sat.q_ECItoSM);
    }
    if (j.contains("agc")) {
        const auto &a = j["agc"];
        cfg.sat.rom_path             = get_str(a, "rom_path",          "");
        cfg.sat.pad_load_file        = get_str(a, "pad_load_file",     "");
        cfg.sat.pad_load_start_sec   = get_dbl(a, "pad_load_start_sec", cfg.sat.pad_load_start_sec);
    }
    return cfg;
}

// -----------------------------------------------------------------------
// Snapshot ExternalOutputs -> json
// -----------------------------------------------------------------------
json snapshot_outputs(const ExternalOutputs_saturn_ib_stack *o,
                      double elapsed_sim_sec) {
    json j;
    j["elapsed_sim_sec"]      = elapsed_sim_sec;
    j["FDAI_Roll_deg"]        = o->FDAI_Roll_deg;
    j["FDAI_Pitch_deg"]       = o->FDAI_Pitch_deg;
    j["FDAI_Yaw_deg"]         = o->FDAI_Yaw_deg;
    j["FDAI_RollDot_dps"]     = o->FDAI_RollDot_dps;
    j["FDAI_PitchDot_dps"]    = o->FDAI_PitchDot_dps;
    j["FDAI_YawDot_dps"]      = o->FDAI_YawDot_dps;
    j["CM_IMU_PIPA_x"]        = o->CM_IMU_PIPA_Pulses[0];
    j["CM_IMU_PIPA_y"]        = o->CM_IMU_PIPA_Pulses[1];
    j["CM_IMU_PIPA_z"]        = o->CM_IMU_PIPA_Pulses[2];
    j["CM_IMU_IGA_rad"]       = o->CM_IMU_IGA_rad;
    j["CM_IMU_MGA_Rad"]       = o->CM_IMU_MGA_Rad;
    j["CM_IMU_OGA_rad"]       = o->CM_IMU_OGA_rad;
    j["bLiftoff"]             = static_cast<bool>(o->bLiftoff);
    j["bStageSep"]            = static_cast<bool>(o->bStageSep);
    j["bIECO"]                = static_cast<bool>(o->bIECO);
    j["bOECO"]                = static_cast<bool>(o->bOECO);
    j["bS_IVB_EngineStart"]   = static_cast<bool>(o->bS_IVB_EngineStart);
    j["veh_q_ECI_q0"]         = o->veh_q_ECI[0];
    j["veh_q_ECI_q1"]         = o->veh_q_ECI[1];
    j["veh_q_ECI_q2"]         = o->veh_q_ECI[2];
    j["veh_q_ECI_q3"]         = o->veh_q_ECI[3];
    j["BodyToSM_Phi_deg"]     = o->BodyToSM_Phi_deg;
    j["BodyToSM_Theta_deg"]   = o->BodyToSM_Theta_deg;
    j["BodyToSM_Psi_deg"]     = o->BodyToSM_Psi_deg;
    // OUT22 -> pad_*  (S-IB pad reference frame)
    for (const auto &f : OUT22_FIELDS)
        j[std::string("pad_") + f.suffix] = o->OUT22[f.index];
    // OUT11 -> s1_*   (joined stack pre-staging, dropped S-IB post-staging)
    for (const auto &f : OUT11_FIELDS)
        j[std::string("s1_") + f.suffix] = o->OUT11[f.index];
    // OUT12 -> s2_*   (zeros pre-staging, S-IVB + CSM post-staging)
    for (const auto &f : OUT12_FIELDS)
        j[std::string("s2_") + f.suffix] = o->OUT12[f.index];
    return j;
}

// -----------------------------------------------------------------------
// CSV writer.  One row per second of sim time; column order matches
// snapshot_outputs (scalars only; arrays flattened to indexed columns).
// -----------------------------------------------------------------------
// Table-driven CSV writer.  Header emission and row emission share the
// same column-descriptor arrays, so schema stays in sync automatically.
class CsvWriter {
public:
    CsvWriter(const std::string &path) : f_(path) {
        if (!f_) throw std::runtime_error("cannot open " + path);
        // main() calls _Exit(0) to bypass a destructor-ordering crash; that
        // skips ofstream destructors so no automatic flush happens.
        // Disable buffering entirely.
        f_.rdbuf()->pubsetbuf(nullptr, 0);
        write_header();
    }
    void flush() { f_.flush(); }
    void row(double t, const ExternalOutputs_saturn_ib_stack *o) {
        f_ << t;
        // Scalars first (order must match write_header below).
        emit(o->FDAI_Roll_deg);
        emit(o->FDAI_Pitch_deg);
        emit(o->FDAI_Yaw_deg);
        emit(o->FDAI_RollDot_dps);
        emit(o->FDAI_PitchDot_dps);
        emit(o->FDAI_YawDot_dps);
        emit(o->CM_IMU_PIPA_Pulses[0]);
        emit(o->CM_IMU_PIPA_Pulses[1]);
        emit(o->CM_IMU_PIPA_Pulses[2]);
        emit(o->CM_IMU_OGA_rad);
        emit(o->CM_IMU_IGA_rad);
        emit(o->CM_IMU_MGA_Rad);
        emit(int(o->bLiftoff));
        emit(int(o->bStageSep));
        emit(int(o->bIECO));
        emit(int(o->bOECO));
        emit(int(o->bS_IVB_EngineStart));
        emit(o->veh_q_ECI[0]);
        emit(o->veh_q_ECI[1]);
        emit(o->veh_q_ECI[2]);
        emit(o->veh_q_ECI[3]);
        emit(o->BodyToSM_Phi_deg);
        emit(o->BodyToSM_Theta_deg);
        emit(o->BodyToSM_Psi_deg);
        // pad_* (OUT22)
        for (const auto &fld : OUT22_FIELDS) emit(o->OUT22[fld.index]);
        // s1_* (OUT11)
        for (const auto &fld : OUT11_FIELDS) emit(o->OUT11[fld.index]);
        // s2_* (OUT12)
        for (const auto &fld : OUT12_FIELDS) emit(o->OUT12[fld.index]);
        f_ << '\n';
    }
private:
    template <class T> void emit(T v) { f_ << ',' << v; }

    void write_header() {
        f_ << "elapsed_sim_sec,"
              "FDAI_Roll_deg,FDAI_Pitch_deg,FDAI_Yaw_deg,"
              "FDAI_RollDot_dps,FDAI_PitchDot_dps,FDAI_YawDot_dps,"
              "CM_IMU_PIPA_x,CM_IMU_PIPA_y,CM_IMU_PIPA_z,"
              "CM_IMU_OGA_rad,CM_IMU_IGA_rad,CM_IMU_MGA_rad,"
              "bLiftoff,bStageSep,bIECO,bOECO,bS_IVB_EngineStart,"
              "veh_q_ECI_q0,veh_q_ECI_q1,veh_q_ECI_q2,veh_q_ECI_q3,"
              "BodyToSM_Phi_deg,BodyToSM_Theta_deg,BodyToSM_Psi_deg";
        for (const auto &fld : OUT22_FIELDS) f_ << ",pad_" << fld.suffix;
        for (const auto &fld : OUT11_FIELDS) f_ << ",s1_"  << fld.suffix;
        for (const auto &fld : OUT12_FIELDS) f_ << ",s2_"  << fld.suffix;
        f_ << '\n';
    }

    std::ofstream f_;
};

// -----------------------------------------------------------------------
// CLI
// -----------------------------------------------------------------------
struct CliArgs {
    std::string inputs_path;
    // Overrides for JSON values (empty / NaN means "no override")
    double run_time_override = std::nan("");
    std::string final_override;
    std::string csv_override;
    std::string igm_trace_path;          // empty = disabled
    double igm_trace_dt = IGM_TRACE_DT_SEC;
    std::string ic_trace_path;           // empty = disabled (<S5> JSON once)
    std::string onpad_trace_path;        // empty = disabled (<S8> JSON once)
    std::string sib_trace_path;          // empty = disabled (OUT11 JSON once)
    std::string lvdc_io_trace_path;      // empty = disabled (lvdc-rewrite shadow run)
};

CliArgs parse_cli(int argc, char **argv) {
    CliArgs a;
    for (int i = 1; i < argc; ++i) {
        std::string s = argv[i];
        if (s == "--run-time" && i + 1 < argc) {
            a.run_time_override = std::atof(argv[++i]);
        } else if (s == "--final" && i + 1 < argc) {
            a.final_override = argv[++i];
        } else if (s == "--csv" && i + 1 < argc) {
            a.csv_override = argv[++i];
        } else if (s == "--igm-trace" && i + 1 < argc) {
            a.igm_trace_path = argv[++i];
        } else if (s == "--igm-trace-dt" && i + 1 < argc) {
            a.igm_trace_dt = std::atof(argv[++i]);
        } else if (s == "--ic-trace" && i + 1 < argc) {
            a.ic_trace_path = argv[++i];
        } else if (s == "--onpad-trace" && i + 1 < argc) {
            a.onpad_trace_path = argv[++i];
        } else if (s == "--sib-trace" && i + 1 < argc) {
            a.sib_trace_path = argv[++i];
        } else if (s == "--lvdc-io-trace" && i + 1 < argc) {
            a.lvdc_io_trace_path = argv[++i];
        } else if (s == "--help" || s == "-h") {
            std::fprintf(stderr,
                "batch_sim <inputs.json> [--run-time SEC] [--final PATH] [--csv PATH]\n"
                "          [--igm-trace PATH] [--igm-trace-dt SEC]\n"
                "          [--ic-trace PATH] [--onpad-trace PATH] [--sib-trace PATH]\n"
                "          [--lvdc-io-trace PATH]\n"
                "  --igm-trace      write Layer-1 IGM interface CSV \n"
                "  --igm-trace-dt   sample period (default %.1f s)\n"
                "  --ic-trace       write <S5> Initial Conditions JSON (once when valid)\n"
                "  --onpad-trace    write <S8> On Pad / OUT22 JSON (once when valid)\n"
                "  --sib-trace      write S-IB OUT11 JSON (once when valid; pad gate)\n"
                "  --lvdc-io-trace  write LVDA_LVDC I/O trace CSV (every step)\n",
                IGM_TRACE_DT_SEC);
            std::exit(0);
        } else if (a.inputs_path.empty() && s[0] != '-') {
            a.inputs_path = s;
        } else {
            std::fprintf(stderr, "batch_sim: unknown arg '%s'\n", s.c_str());
            std::exit(2);
        }
    }
    if (a.inputs_path.empty()) {
        std::fprintf(stderr, "batch_sim: usage: batch_sim <inputs.json> [--run-time SEC]\n");
        std::exit(2);
    }
    return a;
}

}  // namespace

// -----------------------------------------------------------------------
// main
// -----------------------------------------------------------------------
int main(int argc, char **argv) {
    CliArgs cli = parse_cli(argc, argv);

    RunConfig cfg;
    try {
        cfg = load_config(cli.inputs_path);
    } catch (const std::exception &e) {
        std::fprintf(stderr, "batch_sim: config error: %s\n", e.what());
        return 2;
    }
    if (!std::isnan(cli.run_time_override)) cfg.run_time_sec = cli.run_time_override;
    if (!cli.final_override.empty())        cfg.final_path   = cli.final_override;
    if (!cli.csv_override.empty())          cfg.csv_path     = cli.csv_override;

    // ---- Sim init ------------------------------------------------------
    // LogManager -> /dev/null so batch runs don't create saturn_a_log.txt.
    new sim::LogManager("/dev/null");

    sim::Date dtStart_utc;
    dtStart_utc.SetDate(cfg.year, cfg.month, cfg.day, cfg.hour, cfg.minute, cfg.second);

    auto *simClock = new sim::SimClock();
    simClock->SetCurrentSimulationTime(dtStart_utc);
    simClock->Thaw();

    auto *lv = new SaturnIBStack(cfg.sat);
    simClock->Register(lv);

    // ---- Main loop -----------------------------------------------------
    std::fprintf(stderr,
        "batch_sim: run_time=%.3f sec, step=%.4f sec (%.0f iterations)\n",
        cfg.run_time_sec, cfg.time_step_sec, cfg.run_time_sec / cfg.time_step_sec);
    std::fprintf(stderr, "  launch date: %04d-%02d-%02d %02d:%02d:%06.3f\n",
        cfg.year, cfg.month, cfg.day, cfg.hour, cfg.minute, cfg.second);

    CsvWriter csv(cfg.csv_path);
    // Emit row 0 (initial state).
    csv.row(0.0, lv->GetModelOutputs());

    FILE *igm_fp = nullptr;
    double igm_next_t = 0.0;
    const double igm_dt =
        (cli.igm_trace_dt > 0.0) ? cli.igm_trace_dt : IGM_TRACE_DT_SEC;
    if (!cli.igm_trace_path.empty()) {
        igm_fp = std::fopen(cli.igm_trace_path.c_str(), "w");
        if (!igm_fp) {
            std::fprintf(stderr, "batch_sim: cannot write igm-trace %s\n",
                         cli.igm_trace_path.c_str());
            std::_Exit(3);
        }
        std::fprintf(igm_fp, "%s\n", igm_trace_csv_header());
        IgmTraceSample samp;
        if (lv->FillIgmTrace(&samp, 0.0))
            igm_trace_write_row(igm_fp, &samp);
        igm_next_t = igm_dt;
        std::fprintf(stderr, "  igm-trace: %s (dt=%.3f s)\n",
                     cli.igm_trace_path.c_str(), igm_dt);
    }

    FILE *lvdc_io_fp = nullptr;
    if (!cli.lvdc_io_trace_path.empty()) {
        lvdc_io_fp = std::fopen(cli.lvdc_io_trace_path.c_str(), "w");
        if (!lvdc_io_fp) {
            std::fprintf(stderr, "batch_sim: cannot write lvdc-io-trace %s\n",
                         cli.lvdc_io_trace_path.c_str());
            std::_Exit(3);
        }
        std::fprintf(lvdc_io_fp, "%s\n", lvdc_io_trace_csv_header());
        LvdcIoTraceSample samp;
        if (lv->FillLvdcIoTrace(&samp, 0.0))
            lvdc_io_trace_write_row(lvdc_io_fp, &samp);
        std::fprintf(stderr, "  lvdc-io-trace: %s (every step)\n",
                     cli.lvdc_io_trace_path.c_str());
    }

    bool ic_written = false;
    auto try_write_ic = [&](double t) {
        if (ic_written || cli.ic_trace_path.empty())
            return;
        IcTraceSample ic;
        if (!lv->FillIcTrace(&ic, t))
            return;
        if (ic.valid < 0.5)
            return;
        FILE *fp = std::fopen(cli.ic_trace_path.c_str(), "w");
        if (!fp) {
            std::fprintf(stderr, "batch_sim: cannot write ic-trace %s\n",
                         cli.ic_trace_path.c_str());
            return;
        }
        ic_trace_write_json(fp, &ic);
        std::fclose(fp);
        ic_written = true;
        std::fprintf(stderr,
                     "  ic-trace: %s (t=%.6f |Xe|=%.3f)\n",
                     cli.ic_trace_path.c_str(), t,
                     std::sqrt(ic.Xe_0_m[0] * ic.Xe_0_m[0] +
                               ic.Xe_0_m[1] * ic.Xe_0_m[1] +
                               ic.Xe_0_m[2] * ic.Xe_0_m[2]));
    };
    try_write_ic(0.0);

    bool onpad_written = false;
    auto try_write_onpad = [&](double t) {
        if (onpad_written || cli.onpad_trace_path.empty())
            return;
        OnPadTraceSample op;
        if (!lv->FillOnPadTrace(&op, t))
            return;
        if (op.valid < 0.5)
            return;
        FILE *fp = std::fopen(cli.onpad_trace_path.c_str(), "w");
        if (!fp) {
            std::fprintf(stderr, "batch_sim: cannot write onpad-trace %s\n",
                         cli.onpad_trace_path.c_str());
            return;
        }
        onpad_trace_write_json(fp, &op);
        std::fclose(fp);
        onpad_written = true;
        std::fprintf(stderr,
                     "  onpad-trace: %s (t=%.6f lat=%.6f lon=%.6f h=%.3f)\n",
                     cli.onpad_trace_path.c_str(), t, op.lat_deg, op.lon_deg,
                     op.h_m);
    };
    try_write_onpad(0.0);

    bool sib_written = false;
    auto try_write_sib = [&](double t) {
        if (sib_written || cli.sib_trace_path.empty())
            return;
        SibTraceSample sib;
        if (!lv->FillSibTrace(&sib, t))
            return;
        if (sib.valid < 0.5)
            return;
        FILE *fp = std::fopen(cli.sib_trace_path.c_str(), "w");
        if (!fp) {
            std::fprintf(stderr, "batch_sim: cannot write sib-trace %s\n",
                         cli.sib_trace_path.c_str());
            return;
        }
        sib_trace_write_json(fp, &sib);
        std::fclose(fp);
        sib_written = true;
        std::fprintf(stderr,
                     "  sib-trace: %s (t=%.6f bLiftoff=%.0f |Xe|=%.3f)\n",
                     cli.sib_trace_path.c_str(), t, sib.bLiftoff,
                     std::sqrt(sib.Xe_m[0] * sib.Xe_m[0] +
                               sib.Xe_m[1] * sib.Xe_m[1] +
                               sib.Xe_m[2] * sib.Xe_m[2]));
    };
    try_write_sib(0.0);

    const int iters_per_second = std::max(1, int(std::round(1.0 / cfg.time_step_sec)));
    const int total_iters = int(cfg.run_time_sec / cfg.time_step_sec);

    auto wall_start = std::chrono::steady_clock::now();
    for (int i = 1; i <= total_iters; ++i) {
        simClock->FixedTimeUpdate(cfg.time_step_sec);
        const double t = simClock->GetElapsedTime();
        if (i % iters_per_second == 0) {
            csv.row(t, lv->GetModelOutputs());
        }
        try_write_ic(t);
        try_write_onpad(t);
        try_write_sib(t);
        if (igm_fp && t + 1e-9 >= igm_next_t) {
            IgmTraceSample samp;
            if (lv->FillIgmTrace(&samp, t))
                igm_trace_write_row(igm_fp, &samp);
            igm_next_t += igm_dt;
        }
        if (lvdc_io_fp) {
            LvdcIoTraceSample samp;
            if (lv->FillLvdcIoTrace(&samp, t))
                lvdc_io_trace_write_row(lvdc_io_fp, &samp);
        }
    }
    auto wall_end = std::chrono::steady_clock::now();
    double wall_sec = std::chrono::duration<double>(wall_end - wall_start).count();

    // Sanity check: any model error status is a sim failure -- surface
    // it as exit code 3 so batch jobs can distinguish sim faults from
    // config errors (exit 2) or success (exit 0).
    if (lv->GetErrorMessage() != nullptr) {
        std::fprintf(stderr,
            "batch_sim: sim error: %s\n", lv->GetErrorMessage());
        csv.flush();
        if (igm_fp) std::fclose(igm_fp);
        if (lvdc_io_fp) std::fclose(lvdc_io_fp);
        std::_Exit(3);
    }

    // ---- Final snapshot -----------------------------------------------
    json final_j = snapshot_outputs(lv->GetModelOutputs(), simClock->GetElapsedTime());
    final_j["wall_sec"]       = wall_sec;
    final_j["realtime_ratio"] = cfg.run_time_sec / wall_sec;

    {
        std::ofstream f(cfg.final_path);
        if (!f) {
            std::fprintf(stderr, "batch_sim: cannot write %s\n", cfg.final_path.c_str());
            std::_Exit(3);
        }
        f << final_j.dump(2) << "\n";
    }

    csv.flush();
    if (igm_fp) {
        std::fflush(igm_fp);
        std::fclose(igm_fp);
    }
    if (lvdc_io_fp) {
        std::fflush(lvdc_io_fp);
        std::fclose(lvdc_io_fp);
    }
    std::fflush(nullptr);

    std::fprintf(stderr,
        "batch_sim: done  elapsed_sim=%.3f  wall=%.3f  ratio=%.1fx\n"
        "  wrote %s\n"
        "  wrote %s\n",
        simClock->GetElapsedTime(), wall_sec, cfg.run_time_sec / wall_sec,
        cfg.final_path.c_str(), cfg.csv_path.c_str());
    if (!cli.igm_trace_path.empty())
        std::fprintf(stderr, "  wrote %s\n", cli.igm_trace_path.c_str());

    // Bypass destructors -- teardown crashes.  Files flushed above.
    std::_Exit(0);
}
