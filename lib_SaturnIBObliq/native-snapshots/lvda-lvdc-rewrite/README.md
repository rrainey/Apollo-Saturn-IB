# LVDA_LVDC rewrite (Phase B workspace)

**Branch:** `lvdc-rewrite`
**Status:** work in progress. Not linked into `saturnib` yet. Built
only when the top-level `BUILD_LVDC_REWRITE` option is on.

An EDD-anchored re-implementation of the Saturn IB Instrument Unit
LVDA/LVDC flight software. Replaces the hand-edited obliq-2 native
atomic snapshot at `../lvda-lvdc/`.

See `docs/LVDC_REWRITE_PLAN.md` for scope, module layout and
validation strategy.

## Module layout

| File | EDD anchor | Content |
|------|------------|---------|
| `LVDA_LVDC.h` | n/a (ABI) | Public obliq-2 native atomic ABI. Field-identical to `../lvda-lvdc/LVDA_LVDC.h` so the parent model compile does not change. |
| `LVDA_LVDC.c` | Dispatcher | obliq-2 entry points. Thin coordinator — real work lives in the modules below. |
| `lvdc_params.h` | Part II + TM-X-62831 | Preset parameter catalogue. Every value cited. |
| `lvdc_internal.h` | n/a | Internal types for the rewrite. Not exposed on the parent ABI. |
| `lvdc_frames.{c,h}` | §2.3 | Reference-frame transforms: `[MSG]`, `[MES]`, `[MBS]`, `[MSV]`, `[MSA]`. |
| `lvdc_quant.{c,h}` | §4.2.1, §8.2.1, §8.2.3 | PIPA, resolver and D/A quantization helpers. |
| `lvdc_nav.{c,h}` | §4.2, §4.3 | Boost navigation: accel data processing, F/M, M/F smoothing, nav integration (Eqs. 4.3.1-5), oblate gravity (Eqs. 4.3.6-12). |

**Future (Phase C+):**

| File | EDD anchor | Content |
|------|------------|---------|
| `lvdc_preigm.{c,h}` | §4.4.1 | First-stage time-tilt chi. |
| `lvdc_igm.{c,h}` | §4.4.2, §4.4.4, §4.4.5 | IGM intermediate parameters, Chi assembly, Chi steering, SMC, HSL. |
| `lvdc_grf.{c,h}` | §4.4.3, §8.2.2 | Guidance reference failure detection. |
| `lvdc_scheduler.{c,h}` | §7.2 | Time bases T0..T4 and minor-loop / BML scheduler. |
| `lvdc_discretes.{c,h}` | §7.3, §7.4 | Switch-selector output bits and discrete input latches. |

## Non-goals (v1)

- A/B disagreement, zero and reasonableness tests (EDD §4.2.2).
- Backup resolver, ladder channel switchover (EDD §8.2).
- 26-bit two's-complement internal word scaling (doubles throughout).
- BML pass-to-pass timing jitter.
- EDD §5 (orbital nav), §10 (DCS), §12 (preflight), S-IVB restart.

## Build

```bash
cmake -B build -DBUILD_LVDC_REWRITE=ON
cmake --build build --target lvdc_rewrite
```

Not linked into `saturnib`. The main simulation still runs on
`../lvda-lvdc/LVDA_LVDC.c` until Phase D swap-in.
