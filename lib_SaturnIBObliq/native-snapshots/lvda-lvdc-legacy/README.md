# LVDA_LVDC legacy (hand-edited golden snapshot)

**Status:** A/B reference only. Not linked into `saturnib` by default.

This is the hand-edited `LVDA_LVDC.c` that was the sole LVDC implementation
before the modular rewrite under `../lvda-lvdc-rewrite/` was promoted as the
default. It is retained to compare trajectory residuals against the rewrite
and to preserve the EDD / TM citation comments that informed the rewrite.

## Linking the legacy build

```bash
cmake -B build-legacy -DLVDC_USE_REWRITE=OFF
cmake --build build-legacy --target batch_sim
```

Under `LVDC_USE_REWRITE=OFF`, saturnib uses this `LVDA_LVDC.c` + its matching
`LVDA_LVDC.h` instead of the rewrite tree. The parent plant model's
`LVDA_LVDC_inputs_t` (in `../../plant/generated/LVDA_LVDC.h`) must stay
byte-identical to the layout both snapshots expect — see
`docs/LVDA_LVDC_PORT_PROVENANCE.md`.

## Regeneration policy

- `plant/generated/LVDA_LVDC.{c,h}` is refreshed on every
  `./scripts/regen-fullstack.sh`.
- `--initialize` copies the regenerated files on top of THIS directory
  (seeds the legacy snapshot from codegen). Reserved for forensic work on
  the golden path. Never auto-overwrites the rewrite snapshot.

Do not hand-edit this file expecting regen to preserve changes; use the
rewrite tree instead.
