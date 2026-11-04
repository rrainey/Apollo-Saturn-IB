# Third-party components

| Component | Where | License | Notes |
|-----------|-------|---------|-------|
| Virtual AGC `yaAGC` (Ronald S. Burkey and contributors) | Cloned at build time into `third_party/virtualagc/`; not stored in this repo | GPL version 2 or later, used here under GPL version 3 | Pinned to `9a60b41bad3156caeb191c7718652d57dba639b3`. Local changes are in `third_party/virtualagc-patches/`. |
| Colossus 249 flight software (`data/Colossus249.bin`) | `data/` | Believed public domain, per Virtual AGC's `LICENSE.txt` | Apollo 8 CMC software, written by MIT Instrumentation Laboratory for NASA. Word-for-word identical to `Colossus249/Colossus249.binsource` at the pinned commit. |
| Downlink tables (`data/ddd-*.tsv`) | `data/` | Virtual AGC (GPL version 2 or later) | Byte-identical copies of `yaAGC/ddd-*` at the pinned commit. |
| NASA TM-X-62831 PDF, OCR text and images | `reference/NASA-TM-X-62831/` | U.S. government work | NASA technical memorandum. |
| LVDC Equation Defining Document PDFs, OCR text and images | `reference/LVDC-EDD/` | No copyright notice found in the OCR text | Prepared by IBM for NASA Marshall Space Flight Center under contract NAS8-14000. |
| nlohmann/json 3.11.3 | `third_party/nlohmann/` | MIT | |
| stb_image | `third_party/stb/` | MIT or public domain | |
