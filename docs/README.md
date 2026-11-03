# Documentation

**Using the simulation**

- [batch_sim.md](batch_sim.md): the headless driver, input schema and output files.
- [apolloa_gui.md](apolloa_gui.md): the interactive panel and the launch procedure.
- [../lib_SaturnIBObliq/README.md](../lib_SaturnIBObliq/README.md): the plant, and which sources are generated and which are hand-edited.

**Engineering notes**

The other files here are working notes from porting the original Simulink stack to Obliq-2, and they describe the state of the work when they were written.

- They use the configuration names *Green* (the Simulink/RTW stack), *Bronze* (Obliq with `LVDA_LVDC` flattened) and *Silver* (Obliq with `LVDA_LVDC` as a separate module). This repository contains Silver only.
- Some paths and commands refer to the earlier layout, including `lib_SaturnIB` and the RTW build, which are not in this repository.
- [saturn-ib-slices.md](saturn-ib-slices.md) and [sample-models-README.md](sample-models-README.md) describe test models in the `obliq-2` project.
