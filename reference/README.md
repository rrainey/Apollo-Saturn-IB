# Reference documents

Source documents used to build and check the model, with OCR text of each. The OCR is machine output and contains errors, especially in tables, so check values against the PDF before relying on them.

## NASA TM-X-62831

*AS-205/CSM-101 Launch Vehicle Operational Trajectory, Revision I* (10 Sep 1968), the primary reference for the AS-205 launch profile.

| Path | Contents |
|------|----------|
| `NASA-TM-X-62831/NASA-TM-X-62831.pdf` | The document |
| `NASA-TM-X-62831/chandra/` | Chandra OCR, as Markdown and HTML. The tables in `model/as205-reference/tm_x_62831_*.csv` were digitized from this one. |
| `NASA-TM-X-62831/marker/` | Marker OCR |
| `NASA-TM-X-62831/ocr-comparison/` | Five sample pages (`sample.pdf`) run through Chandra, DeepSeek-OCR-2 and olmOCR, to compare the tools |

## Project Apollo Coordinate System Standards

*Project Apollo Coordinate System Standards* (NASA MSC MA-1461, Apollo
Program Director, 1 Jun 1965). The authoritative statement of the
Apollo-program reference ellipsoid (1960 Fischer) and the nineteen
Project Apollo Standard Coordinate Systems (PASCs). Cited from
`docs/ALTITUDE_CONVENTIONS.md` and the frame-anchoring section of
`README.md`.

| Path | Contents |
|------|----------|
| `project-apollo-coordinate-system-standards.pdf` | The document |

## LVDC Equation Defining Document

*Saturn Instrument Unit: LVDC Equation Defining Document (EDD) for the Saturn IB Flight Program, Parts I and II, Revision A* (IBM No. 70-207-0001, MSFC-DRL-008A, 11 April 1972). The scan is split across seven PDFs.

| Path | Contents |
|------|----------|
| `LVDC-EDD/satinstunitibm_1.pdf` | Part I: front matter through Section 3, including Section 2, reference systems and transformations |
| `LVDC-EDD/satinstunitibm_2.pdf` | Part I, from Section 4: boost navigation and guidance |
| `LVDC-EDD/satinstunitibm_3.pdf` | Part I, from Section 7: time bases, discretes and interrupts |
| `LVDC-EDD/satinstunitibm_4.pdf` | Part I, from Section 10: digital command system |
| `LVDC-EDD/satinstunitibm_5.pdf` | Part I, from Section 12: preflight tests |
| `LVDC-EDD/satinstunitibm_6.pdf` | Part I, from Section 15: definitions |
| `LVDC-EDD/satinstunitibm_7.pdf` | Part II |
| `LVDC-EDD/chandra/`, `LVDC-EDD/marker/` | OCR of each PDF |

Each OCR folder also has the tool's metadata and the images it extracted.
