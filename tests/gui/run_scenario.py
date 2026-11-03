#!/usr/bin/env python3
"""Drive apolloa through a launch under Xvfb at wall-clock pace.

usage: run_scenario.py --app build/apolloa --cwd data --xclick <path> \
                       --out <dir> [--display 91] [--duration 480]

Presses the DSKY keys on the procedure's schedule, captures a screenshot every
30 s and keeps stderr and the 1 Hz IMU_DEBUG CSV in --out.  Exits non-zero if
the app dies or an expected event does not appear.

V37 is entered as V-3-7-ENTR-d-d-ENTR with no NOUN key.
"""
import argparse, json, os, pathlib, shutil, subprocess, sys, time

KEYS_JSON = pathlib.Path(__file__).resolve().parents[2] / "data" / "panel" / "panel.json"
EXPECTED = ["PAD LOAD STARTED", "UPLINK COMPLETE", "STATE VECTOR"]


def key_positions():
    out = {}
    def walk(nodes):
        for n in nodes:
            if n["name"].startswith("Key") and "vertices" in n:
                xs = [v[0] for v in n["vertices"]]
                ys = [v[1] for v in n["vertices"]]
                lx, ly = n.get("location", [0, 0, 0])[:2]
                out[n["name"]] = (round(lx + sum(xs) / len(xs)), round(800 - (ly + sum(ys) / len(ys))))
            walk(n.get("children", []))
    walk(json.loads(KEYS_JSON.read_text())["nodes"])
    return out


def v37(prog):
    return ["KeyVerb", "Key3", "Key7", "KeyENTR", "Key" + prog[0], "Key" + prog[1], "KeyENTR"]


# (time after launch of the app, keys)
SCHEDULE = [(100, v37("00")), (215, v37("01"))]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--app", required=True)
    ap.add_argument("--cwd", required=True)
    ap.add_argument("--xclick", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--display", default="91")
    ap.add_argument("--duration", type=int, default=480)
    a = ap.parse_args()

    out = pathlib.Path(a.out).resolve()
    out.mkdir(parents=True, exist_ok=True)
    app = str(pathlib.Path(a.app).resolve())
    keys = key_positions()
    env = dict(os.environ, DISPLAY=":" + a.display, IMU_DEBUG="1", LIBGL_ALWAYS_SOFTWARE="1")

    xvfb = subprocess.Popen(["Xvfb", ":" + a.display, "-screen", "0", "1200x800x24", "+extension", "GLX"],
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(1)
    err = open(out / "stderr.log", "w")
    proc = subprocess.Popen([app], cwd=a.cwd, env=env, stdout=subprocess.DEVNULL, stderr=err)
    start = time.time()
    pending = list(SCHEDULE)
    next_shot = 6
    alive = True
    try:
        while time.time() - start < a.duration:
            t = time.time() - start
            if proc.poll() is not None:
                alive = False
                print(f"app exited early at t={t:.1f}: rc={proc.returncode}")
                break
            if t >= next_shot:
                subprocess.run(["import", "-window", "root", str(out / f"t{next_shot:03d}.png")], env=env)
                next_shot = next_shot + 24 if next_shot == 6 else next_shot + 30
            if pending and t >= pending[0][0]:
                for k in pending.pop(0)[1]:
                    subprocess.run([a.xclick, *map(str, keys[k])], env=env)
                    time.sleep(1.0)
            time.sleep(0.2)
    finally:
        if proc.poll() is None:
            proc.terminate()
        proc.wait()
        xvfb.terminate()
        xvfb.wait()
        err.close()

    csv = pathlib.Path(a.cwd) / "imu_pipa_cdu.csv"
    if csv.exists():
        shutil.copy(csv, out / "imu_pipa_cdu.csv")
    log = (out / "stderr.log").read_text()
    missing = [e for e in EXPECTED if e not in log]
    for e in EXPECTED:
        print(("ok      " if e in log else "MISSING ") + e)
    return 0 if alive and not missing else 1


if __name__ == "__main__":
    sys.exit(main())
