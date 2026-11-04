

{0}------------------------------------------------

#### SECTION 15

#### DEFINITIONS

#### 15.1 INTRODUCTION

This section contains two reference listings for use with the text and equations of Parts I and II of this document. The first listing is a summary of all abbreviations; the second listing contains descriptions of all variables in this document.

#### 15.2 ABBREVIATIONS

Abbreviations for terms related to the Saturn Program, vehicle hardware, flight program software, etc., are listed alphabetically in Table 15-1.

#### 15.3 VARIABLE DICTIONARY

The variable dictionary, contained in Table 15-2, defines the variables used in the text and equations of this document. For each variable there is an identifying symbol, a definition, and, where appropriate, the dimensional unit to be used in the flight program. (Variables used in Eq. 5.4.2 through 5.4.7 and in Section 13 are dummy variables and are not defined in this listing. Their meaning is conveyed by the appropriate discussion in the text.)

Symbols with vincula identify mathematically defined vectors. In general, only this symbol is given in the dictionary. The identical symbol, without the vinculum, denotes the magnitude of the vector. Unless otherwise noted, any of the subscripts A, B, E, G, S, V, or 4 may be added to a vector symbol to

{1}------------------------------------------------

indicate the appropriate coordinate system to which the vector is referenced (see Section 2). A component of a vector is denoted by adding the additional subscripts x, y, or z, corresponding to the coordinate system axis.

Note 1: The definition provided by the dictionary consists only of the primary event which occurs at the corresponding time. Refer to the Event Sequence Timeline Table in Part II.

#### 15.4 MAXIMUM VALUES OF SELECTED VARIABLES

Table 15-3 contains the maximum expected values of selected variables. These maxima were derived from a series of simulations designed to force each variable to its maximum value.

The flight program scaling of these variables and the associated quantities must be capable of accommodating the values given in the table.

{2}------------------------------------------------

# TABLE 15-1 ABBREVIATIONS

| AP   | Accelerometer Processing                          |
|------|---------------------------------------------------|
| BML  | Boost Major Loop                                  |
| BR   | Buffer Register                                   |
| BTC  | Beginning of Telemetry Cycle                      |
| CCS  | Command Communications System                     |
| CIU  | Computer Interface Unit                           |
| CM   | Command Module                                    |
| CO   | Cutoff                                            |
| COD  | Crossover Detector                                |
| CRP  | Computer Reset Pulse                              |
| CSM  | Command and Service Modules                       |
| D/A  | Digital to Analog Conversion                      |
| DB   | Disagreement Bit                                  |
| DCS  | Digital Command System                            |
| DI   | Discrete Input (may have numerical suffix)        |
| DIR  | Discrete Input Register                           |
| DIS  | Discrete Input Spares (may have numerical suffix) |
| DO   | Discrete Output (may have numerical suffix)       |
| DOM  | Data Output Multiplexer                           |
| DOR  | Discrete Output Register                          |
| EBW  | Exploding Bridge Wire                             |
| EC   | Expected Change                                   |
| ECS  | Environmental Control System                      |
| EDD  | Equation Defining Document                        |
| EDS  | Emergency Detection System                        |
| EMR  | Error Monitor Register                            |
| EMRC | Engine Mixture Ratio Change                       |
| EOM  | End of Mission                                    |
| ESE  | Electrical Support Equipment                      |
| ETC  | End of Telemetry Cycle                            |
| ETW  | Error Time Word                                   |
| FCC  | Flight Control Computer                           |
| G&C  | Guidance and Control                              |
| GFP  | Generalized Flight Program                        |
|      |                                                   |

{3}------------------------------------------------

#### TABLE 15-1 ABBREVIATIONS (CONTINUED)

Greenwich Mean Time GMT Guidance Reference Failure GRF Guidance Reference Release GRR Guidance Reference Release Alert GRRA HOP Transfer of program flow in LVDC HSL High Speed (Cutoff) Loop Input Axis TA Interface Control Document ICD Internal Control Register ICR Identification ID Iterative Guidance Mode IGM INT Interrupt (may have numerical suffix) Interrupt Register IR Interface Revision Notice IRN Interrupt Storage Register ISR IU Instrument Unit KSC Kennedy Space Center LCC Launch Control Computer Launch Escape System LES LH2 Liquid Hydrogen LM Lunar Module Line of Nodes L.N. LO Liftoff LOX Liquid Oxygen Least Significant Bit LSB Launch Umbilical Tower LUT LV Launch Vehicle Launch Vehicle Data Adapter LVDA LVDC Launch Vehicle Digital Computer MCC Mission Control Center (Houston, Texas) MC24 Mode Code 24 MC25 Mode Code 25

Mode Code 27

MC27

{4}------------------------------------------------

#### TABLE 15-1 ABBREVIATIONS (CONTINUED)

ML Minor Loop Mobile Launch Computer MLC Minor Loop Support MLS Most Significant Bit MSB Marshall Space Flight Center (Huntsville, Alabama) MSFC NM Nautical Mile N.P. North Pole NPV Non-Propulsive Vent OA Output Axis OM/D Orbital Mode-Data Bit in DCS word transmitted from ground OML Orbital Major Loop ON Orbital Navigation PCM Pulse Code Modulation PIO Process Input-Output PTL Prepare to Launch PU Propellant Utilization (qms) Quarter Milliseconds QMS RCA-110A Designation for ground computer used in checkout and prelaunch RTC Real Time Clock (as used in Parts III & IV) RTC Reasonableness Test Constant (as used in Parts I & II) S-IB Designation for Saturn IB vehicle first stage S-IVB Designation for Saturn IB vehicle second stage S/C, SC Spacecraft SLA Spacecraft Lunar Module Adapter Service Module SM SMC Steering Misalignment Correction Spin Reference Axis SRA SS Switch Selector Command ST-124M Designation for the launch vehicle inertial platform

To be determined

TBD

{5}------------------------------------------------

# TABLE 15-1 ABBREVIATIONS (CONTINUED)

| TBO | <br>Time Base 0                                            |
|-----|------------------------------------------------------------|
| TB1 | <br>Time Base 1                                            |
| TB2 | Time Base 2                                                |
| TB3 | Time Base 3                                                |
| TB4 | <br>Time Base 4                                            |
| TLC | Temporary Loss of Control (LVDC simultaneous memory error) |
| TM  | Telemetry                                                  |

{6}------------------------------------------------

## TABLE 15-2 DEFINITIONS

Definition and Comments

Symbol Symbol

| $\overline{A}$ $(\overline{A}_p)$      | Vehicle acceleration vector (predicted) [m/sec <sup>2</sup> ]                                                          |
|----------------------------------------|------------------------------------------------------------------------------------------------------------------------|
| A '                                    | Intermediate parameter used in HSL time-to-go calculations [m/sec <sup>2</sup> ]                                       |
| Α"                                     | Intermediate parameter used in HSL time-to-go calculations [m/sec <sup>3</sup> ]                                       |
| A <sub>c0</sub>                        | Estimated uncertainty in the expected velocity change computation [m/sec]                                              |
| $\overline{A}_{D} (\overline{A}_{Dp})$ | Drag acceleration vector (predicted) [m/sec <sup>2</sup> ]                                                             |
| a <sub>e</sub>                         | Semi-major axis of the earth-model ellipsoid [m]                                                                       |
| Az                                     | Platform azimuth, computed in Preflight routines [pirads]                                                              |
| AzL                                    | Position I azimuth at launch [pirads]                                                                                  |
| A <sub>00</sub>                        | Constant term of variable azimuth polynomial, used by Preflight routines [deg]                                         |
| A <sub>01</sub>                        | Coefficient of inclination i in variable azimuth polynomial, used by Preflight routines                                |
| A <sub>10</sub>                        | Coefficient of descending node angle $\boldsymbol{\lambda}$ in variable azimuth polynomial, used by Preflight routines |

{7}------------------------------------------------

## Symbol Definition and Comments

- A Coefficient of product of inclination i and descending node angle  $\lambda$  in variable azimuth polynomial, used by Preflight routines [deg  $^{-1}$ ]
- A<sub>1</sub>-A<sub>6</sub> Coefficients used to compute pitch, yaw, and roll attitude correction commands

\*

\*

ж

\*

\*

{8}------------------------------------------------

| Symbol .        | Definition and Comments                                                                                                                           |   |
|-----------------|---------------------------------------------------------------------------------------------------------------------------------------------------|---|
| В               |                                                                                                                                                   |   |
| b               | Semi-minor axis of the earth-model ellipsoid [m]                                                                                                  |   |
| B <sub>11</sub> | Coefficient used to calculate delta freeze time $(\Delta T_{\mbox{\scriptsize f}})$ for S-IB engine out during first S-IB engine failure interval | * |
| B <sub>12</sub> | Coefficient to calculate delta freeze time ( $\Delta T_f$ ) for S-IB engine out during first S-IB engine failure interval [sec]                   | * |
| B <sub>21</sub> | Coefficient used to calculate delta freeze time ( $\Delta T_{\mbox{\it f}}$ ) for S-IB engine out during second S-IB engine failure interval      | * |
| B <sub>22</sub> | Coefficient used to calculate delta freeze time ( $\Delta T_f$ ) for S-IB engine out during second S-IB engine failure interval [sec]             | * |
| BN <sub>1</sub> | The time (referenced to T*) to start building velocity history for S-IVB cutoff [sec] (see note 1)                                                |   |
| <sup>BN</sup> 5 | Time in TB4 to begin orbital navigation [sec] . (see note 1)                                                                                      |   |
| BU1             | Time in TB3 to start checking DI22 and release inhibit on INT6 [sec] (see note 1)                                                                 |   |
| BU2             | Time in TBO to begin search for liftoff acceleration [sec] (see note 1)                                                                           |   |
| BU3             | Time in TBO to prevent TB1 initiation if liftoff has not been detected [sec] (see note 1)                                                         |   |

{9}------------------------------------------------

| Symbol .                           | Definition and Comments                                                                            |
|------------------------------------|----------------------------------------------------------------------------------------------------|
| C                                  |                                                                                                    |
| C                                  | Telemetry station radius vector [m]                                                                |
|                                    |                                                                                                    |
| <sup>C</sup> arl                   | Constant time bias used to modify $T_{ar}$ for an S-IB * engine failure during Time Base 1 [sec] * |
| CD                                 | Computed vehicle ballistic coefficient                                                             |
| C <sub>D1</sub> -C <sub>D5</sub>   | Coefficients for the polynomial used to compute $^{\rm C}{}_{\rm D}$                               |
| c <sub>2</sub>                     | IGM intermediate parameter used in calculation of position correction terms                        |
| c <sub>4</sub>                     | IGM intermediate parameter used in calculation of position correction terms                        |
| CP <sub>M1</sub> -CP <sub>M5</sub> | Orbital attitude (pitch) variables for maneuvers<br>1 thru 5                                       |
| CPITCH                             | In-plane attitude parameter used in orbital guidance                                               |
| CY <sub>M1</sub> -                 | Orbital attitude (yaw) variables for maneuvers<br>1 thru 5                                         |
| CYAW                               | Out-of-plane attitude parameter used in orbital guidance                                           |

{10}------------------------------------------------

| Symbol Symbol  | Definition and Comments                                                              |
|----------------|--------------------------------------------------------------------------------------|
|                |                                                                                      |
| <u>D</u>       |                                                                                      |
| D              | Coefficient of fourth zonal harmonic in expansion of earth's gravitational potential |
| d <sub>A</sub> | Altitude of the vehicle above the horizon of a telemetry station [m]                 |
| D <sub>p</sub> | IGM parameter used in calculation of pitch attitude correction term                  |
| D <sub>y</sub> | IGM parameter used in calculation of yaw attitude correction term                    |
| DCRTC          | Real time clock reading associated with $\Delta\chi$ updating [qms]                  |

{11}------------------------------------------------

| Symbol_                          | Definition and Comments                                                                                                       |
|----------------------------------|-------------------------------------------------------------------------------------------------------------------------------|
| <u>F</u>                         |                                                                                                                               |
| F                                | Thrust Vector                                                                                                                 |
| F'                               | Unit thrust vector                                                                                                            |
| Fk                               | Current predicted average thrust; used to compute $(F/M)_{c}[kg]$                                                             |
| F <sub>1</sub>                   | Predicted average S-IB thrust; used to compute * (F/M) c [kg]                                                                 |
| F <sub>2</sub>                   | Predicted average S-IVB thrust before EMRC; used to compute $(F/M)_{c}[kg]$                                                   |
| F <sub>2I</sub>                  | Predicted average S-IVB buildup thrust; used to compute $(F/M)_{c}[kg]$                                                       |
| F 3                              | Predicted average S-IVB thrust after EMRC; used to compute $(F/M)_{c}[kg]$                                                    |
| F <sub>10</sub> -F <sub>13</sub> | Coefficients of first segment time tilt polynomial [pirads, pirads/sec, pirads/sec <sup>2</sup> , pirads/sec <sup>3</sup> ]   |
| F <sub>20</sub> -F <sub>23</sub> | Coefficients of second segment time tilt polynomial. [pirads, pirads/sec, pirads/sec <sup>2</sup> , pirads/sec <sup>3</sup> ] |
| F30 <sup>-F</sup> 33             | Coefficients of third segment time tilt polynomial [pirads, pirads/sec, pirads/sec <sup>2</sup> , pirads/sec <sup>3</sup> ]   |

{12}------------------------------------------------

| Symbo1             | Definition and Comments                                                                        |
|--------------------|------------------------------------------------------------------------------------------------|
| 2                  |                                                                                                |
|                    |                                                                                                |
|                    |                                                                                                |
| (F/M)              | Total measured vehicle acceleration computed from accelerometer readings [m/sec <sup>2</sup> ] |
|                    | accelerometer readings [m/sec ]                                                                |
| (F/M) <sub>c</sub> | Backup acceleration computed from prestored values [m/sec <sup>2</sup> ]                       |
|                    |                                                                                                |
| FMO                | Initial (F/M) for S-IB burn [m/sec <sup>2</sup> ]                                              |
| FM2                | Constant (F/M) during TB2 [m/sec <sup>2</sup> ]                                                |
| FM4                | Initial (F/M) for S-IVB burn [m/sec <sup>2</sup> ]                                             |

{13}------------------------------------------------

| Symbol                            | Definition and Comments                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |   |
|-----------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---|
| G                                 |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |   |
| $\overline{G}$ $(\overline{G_p})$ | Gravitational acceleration vector (predicted) [m/sec <sup>2</sup> ]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |   |
| G*                                | Average $\overline{G}$ for remaining boost flight path [m/sec <sup>2</sup> ]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |   |
| <sup>G</sup> H                    | Huntsville gravity value used in flight simulation [m/sec <sup>2</sup> ]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |   |
| $\overline{G_L}$                  | Gravitational acceleration vector at the launch site [m/sec <sup>2</sup> ]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |   |
| G <sub>T</sub>                    | Terminal gravitational acceleration vector [m/sec <sup>2</sup> ]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         |   |
| GANTRY                            | Height of the launch umbilical tower [m]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |   |
| GOMTYP                            | Parameter used by execute generalized maneuver, execute special maneuver, and return to nominal timeline DCS commands, indicating orbital maneuver type | * |
|                                   |  | * |
|                                   |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |   |

{14}------------------------------------------------

| Symbol .       | Definition and Comments                              |
|----------------|------------------------------------------------------|
|                |                                                      |
| <u>H</u>       |                                                      |
|                |                                                      |
| h              | Altitude of the vehicle above the oblate spheroid    |
|                | of the earth [m]                                     |
| Н              | Coefficient of third zonal harmonic in the expansion |
|                | of earth's gravitational potential                   |
| h 1            | Lower limit of h for atmospheric density polynomial  |
|                | [m]                                                  |
| h <sub>2</sub> | Upper limit of h for atmospheric density polynomial  |
|                | [m]                                                  |

{15}------------------------------------------------

#

{16}------------------------------------------------

| Symbol .        | Definition and Comments                                                                                                                    |
|-----------------|--------------------------------------------------------------------------------------------------------------------------------------------|
| <u> </u>        |                                                                                                                                            |
| J               | Coefficient of second zonal harmonic in the expan-                                                                                         |
|                 | sion of earth's gravitational potential                                                                                                    |
| J <sub>p</sub>  | IGM parameter used in calculation of pitch attitude correction term                                                                        |
| j <sub>S</sub>  | Unit vector along Y <sub>S</sub> axis [m]                                                                                                  |
| Jy              | IGM parameter used in calculation of yaw attitude correction term                                                                          |
| J <sub>1</sub>  | Estimated position-to-be-gained in first IGM phase used in calculation of total IGM position-to-be-gained [m]                              |
| J <sub>3</sub>  | Corrected position-to-be-gained in second IGM phase used in calculation of total IGM position-to-be-gained [m]                             |
| J' <sub>3</sub> | Estimated position-to-be-gained in second IGM phase used in the calculations of J $_3$ and the predicted terminal range angle $\phi_T$ [m] |

{17}------------------------------------------------

| Symbol Symbol  | Definition and Comments                                                                                    |
|----------------|------------------------------------------------------------------------------------------------------------|
| K              |                                                                                                            |
| KD             | Orbital drag model constant, dependent on the mass and average presentation area of the vehicle $[m^2/kg]$ |
| K p            | IGM parameter used in calculation of pitch attitude correction term                                        |
| k <sub>S</sub> | Unit vector along $Z_{S}$ axis                                                                             |
| Ку             | IGM parameter used in calculation of yaw attitude correction term                                          |
| к <sub>1</sub> | Altitude constrant for determining pitch steering angle [radians]                                          |
| к <sub>2</sub> | Altitude rate constraint for determining pitch steering angle [radians/sec]                                |
| К3             | Altitude constraint for determining yaw steering angle [radians]                                           |
| K <sub>4</sub> | Altitude rate constraint for determining yaw steering angle [radians/sec]                                  |
|                |                                                                                                            |

{18}------------------------------------------------

(THIS PAGE HAS BEEN LEFT BLANK INTENTIONALLY)

{19}------------------------------------------------

| Symbol [ ]     | Definition and Comments                                   |
|----------------|-----------------------------------------------------------|
|                |                                                           |
| L              |                                                           |
| L <sub>p</sub> | Component in the pitch plane of the total corrected       |
| Γ.             | velocity-to-be-gained by IGM, used in the calcula-        |
|                | tion of position correction terms $K_1$ and $K_2$ [m/sec] |
| Ly             | Total corrected velocity-to-be-gained by IGM, used        |
| ,              | in calculation of position correction term [m/sec]        |
| L'y            | Total estimated velocity-to-be-gained by IGM, used        |
| y              | in calculation of the predicted terminal range angle      |
|                | φ <sub>T</sub> [m/sec]                                    |
|                |                                                           |
| L <sub>1</sub> | Velocity-to-be-gained in first phase IGM, used in         |
|                | calculation of the predicted terminal range angle         |
|                | φ <sub>T</sub> [m/sec]                                    |
| L <sub>3</sub> | Corrected velocity-to-be-gained in second phase IGM,      |
|                | used in calculation of the pitch guidance command         |
|                | Xy [m/sec]                                                |
|                |                                                           |
| L'3            | Estimated velocity-to-be-gained in second phase IGM,      |
|                | used in calculation of the predicted terminal range       |
|                | angle $\phi_{T}$ [m/sec]                                  |
|                |                                                           |

{20}------------------------------------------------

| Symbol              | <u>Definition</u> and <u>Comments</u>                                                                             |
|---------------------|-------------------------------------------------------------------------------------------------------------------|
| M                   |                                                                                                                   |
| М                   | Current mass of the vehicle [kg-sec <sup>2</sup> /m]                                                              |
| м                   | Current mass flow rate used in computing $(F/M)_{c}$ [kg-sec/m]                                                   |
| M <sub>cb</sub>     | Current total backup mass used in computing $(F/M)_c$ [kg-sec <sup>2</sup> /m]                                    |
| $^{\mathrm{M}}$ G R | Estimated mass of S-IVB at IGM initiation [ $kg-sec^2/m$ ]                                                        |
| <sup>M</sup> 1      | Mass of the vehicle at liftoff; used to compute $(F/M)_c$ [kg-sec <sup>2</sup> /m]                                |
| M <sub>1</sub>      | S-IB backup mass flow rate used to compute $(F/M)_{c}$ [kg-sec/m]                                                 |
| <sup>M</sup> 2      | Mass of the vehicle (not including LES) at T3+T3 $_{\rm FMC}$ ; used to compute (F/M) $_{\rm c}$ [kg-sec $^2$ /m] |
| М <sub>2</sub>      | S-IVB backup mass flow rate prior to EMRC; used to compute $(F/M)_c$ [kg-sec/m]                                   |
| <sup>М</sup> 3      | S-IVB backup mass flow rate after EMRC; used to compute $(F/M)_c$ [kg-sec/m]                                      |
| (M/F)               | Reciprocal of the total measured acceleration (1/(F/M)) [ $\sec^2/m$ ]                                            |
| (M/F) <sub>S</sub>  | Current smoothed (filtered) value of $(M/F)$ [sec <sup>2</sup> /m]                                                |

{21}------------------------------------------------

# Symbol Definition and Comments

| (M/F) <sub>S1</sub> -(M/F) <sub>S4</sub> | First through fourth consecutive past values of $(M/F)_S$ [sec <sup>2</sup> /m]                          |
|------------------------------------------|----------------------------------------------------------------------------------------------------------|
| (M/F) <sub>1</sub> -(M/F) <sub>4</sub>   | Present through third past consecutive values of (M/F) $[\sec^2/m]$                                      |
| (M/F) <sub>0</sub>                       | Predicted M/F at T3+T3 $_{\rm FM}$ : used to initialize the smoothing filter at T0+0 [sec $^2/{\rm m}$ ] |
| [MBS]                                    | Transformation matrix from B-system to S-system                                                          |
| [MEG]                                    | Transformation matrix from E-system to G-system                                                          |
| [MES]                                    | Transformation matrix from E-system to S-system                                                          |
| MFK1-MFK8                                | (M/F) <sub>S</sub> filter coefficients                                                                   |
| [MGA]                                    | Transformation matrix from G-system to A-system                                                          |
| [MG4]                                    | Transformation matrix from G-system to 4-system                                                          |
| MLKRC1                                   | Gimbal angle RTC used on first pass on the backup gimbal only (all channels) [deg]                       |
| MLKRC3                                   | Gimbal angle RTC used after first pass on the backup gimbal (pitch and yaw channels) [deg/ML]            |
| MLKRC5                                   | Gimbal angle RTC used after first pass on the backup gimbal (roll channel) [deg/ML]                      |
| MLK2                                     | Fine gimbal angle RTC (pitch and yaw channels) [deg/ML]                                                  |

{22}------------------------------------------------

| Symbol | <u>Definition</u> and <u>Comments</u>                                                                        |
|--------|--------------------------------------------------------------------------------------------------------------|
|        |                                                                                                              |
| MLK12  | Fine gimbal angle RTC (roll channel) [deg/ML]                                                                |
| MLR    | Minor Loop rate [sec <sup>-1</sup> ]                                                                         |
| ML6HUN | Zero gimbal angle test constant (all channels) [deg]                                                         |
| [MSA]  | Transformation matrix from S-system to A-system                                                              |
| [MSG]  | Transformation matrix from S-system to G-system                                                              |
| MSK5   | Ladder rate limit (all channels) [deg/ML]                                                                    |
| MSK6   | Ladder magnitude limit (pitch and yaw channels) [deg]                                                        |
| MSK16  | Ladder magnitude limit (roll channel) [deg]                                                                  |
| MSLIM1 | Attitude command ( $\chi$ ) rate limit (roll channel) [deg/ML]                                               |
| MSLIM2 | Attitude command ( $\chi$ ) rate limit (pitch and yaw channels) [deg/ML]                                     |
| [MSV]  | Transformation matrix from S-system to V-system                                                              |
| MS04DT | The reciprocal of MS25DT                                                                                     |
| MS15DT | Number of permissible unreasonable backup resolver readings per computation cycle before setting DO4 and DO6 |

{23}------------------------------------------------

| Symbol . |            | Definitio   | n and Con            | ments         |         |
|----------|------------|-------------|----------------------|---------------|---------|
|          |            |             |                      |               |         |
|          |            | 1           |                      |               |         |
| MS 25DT  |            | minor loop  | s per $\Delta T_{N}$ | through min   | or loop |
|          | support    |             |                      |               |         |
| [MS4]    | Transforma | ation matri | x from S-            | -system to 4- | system  |
| [M4V]    | Transform  | ation matri | x from 4-            | -system to V- | system  |

{24}------------------------------------------------

| Symbol .       | Definition and Comments                                                              | ja ja i |   |
|----------------|--------------------------------------------------------------------------------------|---------|---|
|                |                                                                                      |         |   |
| <u>N</u>       |                                                                                      |         |   |
| $\overline{N}$ | Unit vector perpendicular to the orbital plane                                       |         |   |
| NTSTB2         | Nominal time in Time Base 1 to start Time Base in repeatable flight simulation [sec] | 2       | * |
| NTSTB3         | Nominal time in Time Base 2 to start Time Base in repeatable flight simulation [sec] | 3       | * |
| NUPTIM         | Time from GRR at which a DCS Navigation Update to be implemented [sec]               | is      |   |

{25}------------------------------------------------

| Symbol         | <u>Definition</u> and <u>Comments</u>                                     |
|----------------|---------------------------------------------------------------------------|
| <u>P</u>       |                                                                           |
| P              | Pointing vector along the longitudinal axis of the vehicle                |
| P              | Parameter used in calculation of gravitational acceleration vector        |
| P <sub>C</sub> | Elapsed time in artificial $\tau$ mode at EMRC [sec]                      |
| PCMR           | Preset length of artificial $\tau$ mode after EMRC [sec]                  |
| P <sub>1</sub> | Unit vector perpendicular to plane of $\overline{N}$ and $\overline{R_1}$ |
| P 1            | IGM parameter used in calculation of range angle                          |
| P 3            | IGM parameter used in calculation of pitch attitude correction term       |
| P 34           | Parameter in harmonic expansion of earth's gravitational potential        |
| PITCH          | Pitch attitude error correction command [ladder bits]                     |

{26}------------------------------------------------

| Symbol         | Definition and Comments                         |
|----------------|-------------------------------------------------|
| Q              |                                                 |
|                |                                                 |
| Q <sub>p</sub> | IGM parameter used in calculation of position * |
| Ρ .            | correction term                                 |
|                |                                                 |
| Qy             | IGM parameter used in calculation of position   |
|                | correction term                                 |
|                |                                                 |
| Q <sub>1</sub> | IGM parameter used in calculation of position   |
|                | correction term                                 |
|                |                                                 |
| Q <sub>3</sub> | IGM parameter used in calculation of position   |
|                | correction term                                 |

{27}------------------------------------------------

| Symbol                            | Definition and Comments                                                       |   |
|-----------------------------------|-------------------------------------------------------------------------------|---|
|                                   |                                                                               |   |
| R                                 |                                                                               |   |
| $\overline{R}$ $(\overline{R}_p)$ | Vehicle geocentric radius vector (predicted) [m]                              |   |
| R <sub>I</sub>                    | Geocentric radius to the center of the IU on the                              |   |
| L                                 | launch pad. Used to calculate initial conditions                              |   |
|                                   | [m]                                                                           |   |
| ROG                               | Radius vector for use in orbital guidance [m]                                 |   |
| R <sub>STA</sub>                  | Mean radius of telemetry stations [m]                                         |   |
| $\overline{R}_{\overline{T}}$     | Terminal radius vector [m]                                                    |   |
| $\overline{R_1}$                  | Unit vector along $\overline{R}$                                              |   |
| . D.A                             |                                                                               |   |
| RA <sub>M1</sub> -                | Poll and a fan ambital and days a second 1.5                                  |   |
| RA <sub>M5</sub>                  | Roll angle for orbital guidance maneuvers 1-5 [pirads]                        |   |
| ROLL                              | Roll attitude error correction command [ladder bits]                          | * |
| ROLLA                             | Presetting for $\chi_{_{\mbox{\scriptsize X}}}$ for orbital guidance [pirads] |   |
| ROV                               | Constant used to bias terminal range angle pre-                               |   |
|                                   | diction                                                                       |   |
| RTCX                              | Reasonableness test constant (RTC) for X accelerometer channel $[m/sec^2]$    | * |

{28}------------------------------------------------

| Symbol |     |     |   | Definition an | n d | Commer | nts                   |
|--------|-----|-----|---|---------------|-----|--------|-----------------------|
|        |     |     |   |               |     |        |                       |
| RTCY   | RTC | for | Y | accelerometer | ch  | annel  | $[m/sec^2]$           |
| RTCZ   | RTC | for | Z | accelerometer | ch  | annel  | [m/sec <sup>2</sup> ] |

{29}------------------------------------------------

| Symbol          | <u>Definition and Comments</u>                                                                                  |   |
|-----------------|-----------------------------------------------------------------------------------------------------------------|---|
| S               |                                                                                                                 |   |
| S               | Parameter used in calculation of gravitational acceleration vector                                              |   |
| Sp              | IGM parameter used in calculation of position correction term                                                   |   |
| Sy              | IGM parameter used in calculation of position correction term                                                   |   |
| <sup>S</sup> 1  | IGM parameter used in intermediate calculation of position correction term                                      |   |
| <sup>S</sup> 3  | IGM parameter used in calculation of position correction terms                                                  |   |
| s <sub>34</sub> | Parameter representing the third and fourth zonal harmonics in the expansion of earth's gravitational potential |   |
| SEQNUM          | Four bit indicator selecting one of fifteen different preprogrammed DCS alternate sequences                     | * |
| SF              | Scale factor (which includes conversion from pirads to ladder bits and multiplication by attitude error gain)   |   |
| SMCG            | Steering misalignment correction (SMC) gain [rad/sec]                                                           | * |
| SMCY            | Pitch SMC term [pirads]                                                                                         |   |
| SMCZ            | Yaw SMC term [pirads]                                                                                           |   |

{30}------------------------------------------------

| Symbol . | Definition | and | Comments |
|----------|------------|-----|----------|
|          |            |     |          |

| SP <sub>M1</sub> - | In-plane attitude parameters of orbital guidance                   |
|--------------------|--------------------------------------------------------------------|
| SP <sub>M5</sub>   | maneuvers 1-5                                                      |
| SPITCH             | Pitch angle presetting for orbital guidance                        |
|                    |                                                                    |
| SQ1                | First guess at sqrt $(\Delta Z_V^2 + \Delta X_V^2)$ for S-IVB burn |
|                    | [m/sec]                                                            |
|                    |                                                                    |
| SQ2                | First guess at sqrt $(1 - (F'_{Sy})^2)$                            |
|                    |                                                                    |

 $SY_{M1}^-$  Out-of-plane attitude parameters for orbital guidance maneuvers 1-5  $SY_{M5}$  Current maneuver yaw angle presetting used in orbital guidance S4IGTM Time in TB3 to look for S-IVB cutoff conditions

(see note 1) [sec]

{31}------------------------------------------------

| Symbol                           | Definition and Comments                                                                                                 |     |
|----------------------------------|-------------------------------------------------------------------------------------------------------------------------|-----|
| T                                |                                                                                                                         |     |
| T*                               | Predicted IGM total time-to-go to reach insertion conditions [sec]                                                      |     |
| Tar                              | Tilt arrest time (based on $T_c$ ) [sec] (see note 1)                                                                   |     |
| Tas                              | Time (from T0) of last accelerometer sampling in boost mode or of initiation of orbit navigation pass in orbit [sec]    |     |
| T <sub>C</sub>                   | Time (from T1) biased by T <sub>d</sub> [sec]                                                                           |     |
| Tco                              | Time (from T1) for S-IVB cutoff [sec]                                                                                   |     |
| T <sub>d</sub>                   | Time bias to compensate for delays between computing and commanding attitude (used to compute $T_c$ ) [sec]             |     |
| T <sub>D</sub>                   | Difference between $T_{\mbox{\footnotesize{GRR}}}$ and $T_{\mbox{\footnotesize{GRR0}}}$ used to compute $\lambda$ [sec] |     |
| TEO                              | Time (from T1) of engine failure [sec]                                                                                  |     |
| T <sub>far</sub>                 | Partition time constant used to determine a time bias for T in case of an S-IB engine failure [sec]                     | * * |
| <sup>T</sup> f1 <sup>-T</sup> f2 | Partition times for time tilt freeze time polynomial [sec]                                                              |     |
| T <sub>GMT</sub>                 | Universal time or Greenwich mean time (GMT) [sec]                                                                       |     |
| TGRR                             | Time of GRR in universal time $(T_{GMT})$ [sec]                                                                         |     |
| T <sub>GRR0</sub>                | Nominal value of T <sub>GRR</sub> [sec]                                                                                 |     |
|                                  | Rev                                                                                                                     | Δ   |

{32}------------------------------------------------

| Symbol            | Definition and Comments                                                                                                                                                  |
|-------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| HSL               | Time (referenced to T*) to begin HSL calculations [sec]                                                                                                                  |
| T'L               | Predicted time of liftoff (in GMT) as transmitted * to the LVDC from the RCA-110A [sec]                                                                                  |
| TRNTL             | Time in current time base to implement the DCS return to nominal timeline [sec] (see note 1)                                                                             |
| TRTC              | Time in TB1 to change accelerometer RTC's [sec]                                                                                                                          |
| T <sub>SEQ</sub>  | Time in current time base to start a DCS alternate * sequence; referenced to TBB, the current time base time biased by any time base updates received [sec] (see note 1) |
| T <sub>SMC</sub>  | Time in TB3 to enable SMC calculations [sec] (see note 1)                                                                                                                |
| TSOM              | Time in current time base to start the DCS generalized maneuver or DCS special maneuver [sec] (see note 1) *                                                             |
| TSON              | Time since last orbital navigation pass [sec]                                                                                                                            |
| T <sub>SO</sub>   | Backup time (based on $T_{\rm c}$ ) to start roll maneuver and begin using first segment of time tilt coefficients [sec] (see note 1)                                    |
| T <sub>S1</sub>   | Time (based on $T_c$ ) to begin using second segment of time tilt coefficients [sec] (see note 1)                                                                        |
| T <sub>S1EO</sub> | Time in TB1 to enable DI14 and DI15 check for S-IB engine out [sec] (see note 1)                                                                                         |
| T <sub>S2</sub>   | Time (based on $T_c$ ) to begin using third segement of time tilt coefficients [sec] (see note 1)                                                                        |

{33}------------------------------------------------

Symbol Symbol

Definition and Comments

| T <sub>T2</sub>    | Earliest time (based on $T_c$ ) for $\chi$ freeze initiation                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
|--------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|                    | if S-IB engine failure is detected [sec] (see note 1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
|                    |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| T <sub>li</sub>    | Time-to-go in first phase of IGM [sec]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         |
| 11                 |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| T                  | Nominal duration of first phase IGM [sec]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
| <sup>T</sup> 10    | Nominal delation of first phase 16M [sec]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
| _                  |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| T <sub>3i</sub>    | Time-to-go in second phase of IGM [sec]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |
|                    |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| T <sub>4M2</sub> - | Preset times in TB4 to execute maneuver 2-5 [sec]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
| T <sub>4M5</sub>   | (see note 1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
|                    |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| TB                 | Elapsed time in prevailing time base [sec]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
|                    |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| ТВА                | Time of station acquisition in TB4 [sec]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
|                    |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| ТВВ                | Value of TP biseed by any time been made to                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |
| 1 0 0              | Value of TB biased by any time base updates                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |
|                    | received [sec]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
|                    |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| TBL                | Time of station loss in TB4 [sec]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
|                    |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| TI                 | Time (from $T_{GRR}$ ) of initiation of the current                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
|                    | time base [sec]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
|                    |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| TLCK               | Target load checksum factor set to the two's                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
|                    | complement of the sum of the other seven target                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
|                    | load variables                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
|                    | 2044 141140165                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| TLPAST             | Enternal total manufacture and the second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second second se |
| ILFASI             | Extrapolated real time clock value during orbit                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
|                    | for telemetry [qms]                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |

{34}------------------------------------------------

| Symbol .                              | Definition and Comments                                                                                                                                            |
|---------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| TLTSEC                                | Time of the once per second position extrapolation during orbit mode, measured from GRR [sec]                                                                      |
| TM                                    | Time (from TO) to activate ECS water valve logic [sec] (see note 1)                                                                                                |
| TMEFRZ                                | Time (based on $T_c$ ) to end $X_y$ freeze following an S-IB engine failure [sec] (see note 1)                                                                     |
| TYAW01-<br>TYAW25                     | Table of times associated with the S-IB stage yaw guidance commands [sec]                                                                                          |
| TYP <sub>M1</sub> - TYP <sub>M5</sub> | The type of orbital maneuver to be executed (for maneuvers 1-5) (+1: inertial reference, 0: chi freeze, -1: local reference, -2: inertial hold of local reference) |
| TO                                    | Time (from $T_{GRR}$ ) of initiation of TBO [sec]                                                                                                                  |
| T1                                    | Time (from $T_{GRR}$ ) of initiation of TB1 [sec]                                                                                                                  |
| TIBIAS                                | Bias by which Tl is adjusted if TBl was initiated by the backup criteria [sec]                                                                                     |
| Т2 .                                  | Time (from $T_{GRR}$ ) of initiation of TB2 [sec]                                                                                                                  |
| Т3                                    | Time (from $T_{GRR}$ ) of initiation of TB3 [sec]                                                                                                                  |
| T3 <sub>FM</sub>                      | Time in TB3 to begin (F/M) computations [sec] (see note 1)                                                                                                         |
| T3 <sub>FMC</sub>                     | Time in TB3 to begin $(F/M)_c$ computations [sec] (see note 1)                                                                                                     |

{35}------------------------------------------------

# $\frac{\text{Definition and Comments}}{\text{T3}_{\text{IGM}}}$ Time in TB3 to initiate IGM guidance [sec] (see note 1) T4 Time (from $\text{T}_{\text{GRR}}$ ) of initiation of TB4 [sec]

{36}------------------------------------------------

| Symbol .       | Definition and Comments                       |
|----------------|-----------------------------------------------|
|                |                                               |
| U              |                                               |
|                |                                               |
| U <sub>1</sub> | IGM parameter used in calculation of position |
| *              | correction term                               |
|                |                                               |
| U <sub>3</sub> | IGM parameter used in calculation of position |
| 41             | correction term                               |

{37}------------------------------------------------

| Symbol .                                   | Definition and Comments                                                                       |
|--------------------------------------------|-----------------------------------------------------------------------------------------------|
| V                                          |                                                                                               |
| V                                          | Total space-fixed velocity [m/sec]                                                            |
| $\overline{V}$ $(\overline{V_p})$          | Vehicle velocity vector (predicted) [m/sec]                                                   |
| V *                                        | Velocity at end of past computation cycle, used in HSL time-to-go calculations [m/sec]        |
| V"                                         | Velocity at end of second past computation cycle, used in HSL time-to-go calculations [m/sec] |
| V <sub>ex1</sub>                           | <pre>Engine exhaust velocity during first IGM phase [m/sec]</pre>                             |
| V <sub>ex3</sub>                           | <pre>Engine exhaust velocity during second IGM phase [m/sec]</pre>                            |
| VGRD                                       | Velocity guard for HSL [m/sec]                                                                |
| V <sub>r</sub>                             | Velocity vector relative to the earth's atmosphere [m/sec]                                    |
| $\overline{v}_{T}$                         | Terminal velocity vector [m/sec]                                                              |
| VENT <sub>1A</sub> -<br>VENT <sub>8A</sub> | First through eighth vent accelerations [m/sec <sup>2</sup> ]                                 |
| VENTNO                                     | Number of vent accelerations                                                                  |
| VTIM2-<br>VTIM8                            | Time (from T4) to start using second through eighth vent acceleration [sec]                   |

{38}------------------------------------------------

| Symbol Symbol        | <u>Definition</u> and <u>Comments</u>                                                                                        |
|----------------------|------------------------------------------------------------------------------------------------------------------------------|
| <u>x</u>             |                                                                                                                              |
| XA                   | Component of $\overline{R}$ along $X_{\overline{A}}$ axis $[m]$                                                              |
| X <sub>B</sub>       | Component of $\overline{R}$ along $X_{\overline{B}}$ axis [m]                                                                |
| x <sub>DS</sub>      | Component of $\overline{A_D}$ along $X_S$ axis $[m/sec^2]$                                                                   |
| X <sub>G</sub>       | Component of $\overline{R}$ along $X_{\overline{G}}$ axis [m]                                                                |
| <br>Х <sub>G g</sub> | Component of $\overline{G}$ along $X_{\overline{G}}$ axis $[m/sec^2]$                                                        |
| X <sub>LS</sub>      | Component of $\overline{R_L}$ along $X_S$ axis [m]                                                                           |
| X <sub>m</sub>       | Measured platform velocity along $X_S$ axis [m/sec]                                                                          |
| x <sub>m</sub>       | Average platform acceleration along $X_S$ axis $[m/sec^2]$                                                                   |
| x <sub>0</sub>       | Initial value of $\dot{X}_{S}$ [m/sec]                                                                                       |
| x <sub>p</sub>       | Component of the non-orthogonal inertial platform $*$ gimbal system, identical to the $\mathbf{X}_{\mathbf{B}}$ axis [m] $*$ |
| Xref                 | X reference angle of execute generalized maneuver commanded attitude [pirads]                                                |
| x <sub>s</sub>       | Component of $\overline{R}$ along $X_S$ axis [m]                                                                             |
| x <sub>s</sub>       | Component of $\overline{V}$ along $X_S$ axis [m/sec]                                                                         |
| x <sub>S</sub>       | Component of $\overline{A}$ along $X_S$ axis $[m/sec^2]$                                                                     |

{39}------------------------------------------------

| Symbol Symbol        |            | <u>Definition</u> and <u>Comments</u>                   |
|----------------------|------------|---------------------------------------------------------|
|                      |            |                                                         |
|                      |            |                                                         |
| X <sub>Sg</sub>      | Component  | of $\overline{V}$ along $X_S$ axis due to gravitational |
| 38                   | accelerati | 5                                                       |
| <br>X <sub>Sg</sub>  | Component  | of $\overline{G}$ along $X_S$ axis $[m/sec^2]$          |
| x v                  | Component  | of $\overline{R}$ along $X_{\overline{V}}$ axis [m]     |
| x <sub>v</sub>       | Component  | of $\overline{V}$ along $X_{\overline{V}}$ axis [m/sec] |
| <br>X* <sub>Vg</sub> | Component  | of $\overline{G*}$ along $X_{V}$ axis $[m/sec^{2}]$     |
| x <sub>VgT</sub>     | Component  | of $\overline{G_T}$ along $X_V$ axis $[m/sec^2]$        |
| X <sub>VT</sub>      | Component  | of $\overline{R_T}$ along $X_V$ axis [m]                |
| x <sub>vT</sub>      | Component  | of $\overline{V_T}$ along $X_V$ axis [m/sec]            |
| x 4                  | Component  | of R along X <sub>4</sub> axis [m]                      |
| x <sub>4</sub>       | Component  | of $\overline{V}$ along $X_4$ axis $[m/sec]$            |

{40}------------------------------------------------

| Symbol Symbol       | Definition and Comments                                                                                         |
|---------------------|-----------------------------------------------------------------------------------------------------------------|
| <u>Y</u>            |                                                                                                                 |
| YA                  | Component of $\overline{R}$ along $Y_A$ axis [m]                                                                |
| YB                  | Component of $\overline{R}$ along $Y_B$ axis [m]                                                                |
| Y <sub>DS</sub>     | Component of $\overline{A}_D$ along $Y_S$ axis $[m/sec^2]$                                                      |
| Y <sub>G</sub>      | Component of $\overline{R}$ along $Y_{\overline{G}}$ axis $[m]$                                                 |
| <br>Y <sub>Gg</sub> | Component of $\overline{G}$ along $Y_{\overline{G}}$ axis $[m/sec^2]$                                           |
| YLS                 | Component of $\overline{R_L}$ along $Y_S$ axis [m]                                                              |
| Y <sub>m</sub>      | Measured platform velocity along $Y_S$ axis [m/sec]                                                             |
| Y m                 | Average platform acceleration along $Y_S$ axis $[m/sec^2]$                                                      |
| Ý <sub>0</sub>      | Initial value of Y <sub>S</sub> [m/sec]                                                                         |
| YP                  | Component of the non-orthogonal inertial platform $*$ gimbal system, identical to the Y $_{\rm S}$ axis [m] $*$ |
| Yref                | Y reference angle of execute generalized maneuver commanded attitude [pirads]                                   |
| YS                  | Component of $\overline{R}$ along $Y_{\overline{S}}$ axis [m]                                                   |
| Ý <sub>S</sub>      | Component of $\overline{V}$ along $Y_S$ axis $[m/sec]$                                                          |
| Ÿ <sub>S</sub>      | Component of $\overline{A}$ along $Y_S$ axis $[m/sec^2]$                                                        |

{41}------------------------------------------------

| Symbol Symbol   | Definition and Comments                                                                                                |   |
|-----------------|------------------------------------------------------------------------------------------------------------------------|---|
| Y<br>Sg         | Component of $\overline{V}$ along $Y_S$ axis due to gravitational acceleration [m/sec]                                 | * |
| Ÿ <sub>Sg</sub> | Component of $\overline{G}$ along $Y_S$ axis $[m/sec^2]$                                                               |   |
| YV              | Component of $\overline{R}$ along $X_{\overline{V}}$ axis. This is identical to $Y_4$ [m]                              |   |
| Ý <sub>V</sub>  | Component of $\overline{V}$ along $Y_{\overline{V}}$ axis $[m/sec]$                                                    |   |
| Y*<br>Vg        | Component of $\overline{G*}$ along $Y_V$ axis $[m/sec^2]$                                                              |   |
| Y<br>VgT        | Component of $\overline{G_T}$ along $Y_V$ axis $[m/sec^2]$                                                             |   |
| YVT             | Component of $\overline{R_T}$ along $Y_V$ axis [m]                                                                     |   |
| ·<br>VT         | Component of $\overline{V_T}$ along $Y_V$ axis [m/sec]                                                                 |   |
| Y <sub>4</sub>  | Component of $\overline{R}$ along $Y_{\mbox{\scriptsize 4}}$ axis. This is identical to $Y_{\mbox{\scriptsize V}}$ [m] |   |
| Ŷ <sub>4</sub>  | Component of $\overline{V}$ along $Y_4$ axis [m]                                                                       | * |
| YAW             | Yaw attitude error command [ladder bits]                                                                               | * |
| YAWC01-         | Table of yaw commands for S-IB stage guidance                                                                          | * |
| YAWC25          | associated with times in table TYAW [deg]                                                                              | * |
| YAWLIM          | Yaw guidance command limit [pirads]                                                                                    |   |

{42}------------------------------------------------

| Symbol Symbol    | Definition and Comments                                                                |   |
|------------------|----------------------------------------------------------------------------------------|---|
| Z                |                                                                                        |   |
| z <sub>A</sub>   | Component of $\overline{R}$ along the $Z_{\overline{A}}$ axis [m]                      |   |
| z <sub>B</sub>   | Component of $\overline{R}$ along the $Z_B$ axis [m]                                   |   |
| z <sub>DS</sub>  | Component of $\overline{A_D}$ along $Z_S$ axis [m/sec <sup>2</sup> ]                   |   |
| $z_{g}$          | Component of $\overline{R}$ along $Z_{\overline{G}}$ axis [m]                          |   |
| Ϊ <sub>Gg</sub>  | Component of $\overline{G}$ along $Z_{\overline{G}}$ axis $[m/sec^2]$                  |   |
| Z <sub>LS</sub>  | Component of $\overline{R_L}$ along $Z_S$ axis [m]                                     |   |
| ż <sub>m</sub>   | Measured platform velocity along the $Z_S$ axis [m/sec]                                |   |
| z <sub>m</sub>   | Average platform acceleration along the $\mathbf{Z}_{S}$ axis $[\mathrm{m/sec}^{2}]$   |   |
| ż                | Initial value of Z <sub>S</sub> [m/sec]                                                |   |
| z <sub>P</sub>   | Component of the non-orthogonal inertial platform gimbal system [m]                    | * |
| z <sub>ref</sub> | Z reference angle of execute generalized maneuver commanded attitude [pirads]          |   |
| z <sub>s</sub>   | Component of $\overline{R}$ along $Z_S$ axis [m]                                       |   |
| ż <sub>S</sub>   | Component of $\overline{V}$ along $Z_S$ axis [m/sec]                                   |   |
| z <sub>S</sub>   | Component of $\overline{A}$ along $Z_S$ axis $[m/sec^2]$                               |   |
| ż <sub>Sg</sub>  | Component of $\overline{V}$ along $Z_S$ axis due to gravitational acceleration [m/sec] |   |

{43}------------------------------------------------

Definition and Comments

Symbo1

| <br>Z <sub>Sg</sub> | Component | of $\overline{G}$ along $Z_S$ axis $[m/sec^2]$             |
|---------------------|-----------|------------------------------------------------------------|
| $z_v$               | Component | of $\overline{R}$ along $Z_{\overline{V}}$ axis [m]        |
| ż <sub>v</sub>      | Component | of $\overline{V}$ along $Z_{\overline{V}}$ axis [m/sec]    |
| ä*<br>Vg            | Component | of $\overline{G*}$ along the $Z_V$ axis $[m/sec^2]$        |
| $\ddot{z}_{VgT}$    | Component | of $\overline{G}_{T}$ along the $Z_{V}$ axis $[m/sec^{2}]$ |
| ZVI                 | Component | of $\overline{R_T}$ along the $Z_V$ axis [m]               |
| ż <sub>VT</sub>     | Component | of $\overline{v_T}$ along the $z_V$ axis [m/sec]           |
| z <sub>4</sub>      | Component | of $\overline{R}$ along the Z <sub>4</sub> axis [m]        |
| ż                   | Component | of $\overline{V}$ along the $Z_4$ axis [m/sec]             |

{44}------------------------------------------------

| Symbol                | Definition and Comments                                                                                                                                |
|-----------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------|
| ALPHA                 |                                                                                                                                                        |
| α                     | Angle of attack [pirads]                                                                                                                               |
| $\alpha_{\mathbf{f}}$ | Weighting factor based on first phase IGM performance used in adjusting $\boldsymbol{\tau}_3$ computations                                             |
| α <sub>LH</sub>       | Angle between desired $\overline{P}$ and the local horizontal [pirads]                                                                                 |
| DELTA                 |                                                                                                                                                        |
| δ <sub>2</sub>        | Intermediate parameter used in terminal range angle prediction                                                                                         |
| ΔΑ                    | Measured velocity change from accelerometer optisyn                                                                                                    |
| ΔΒ                    | Measured velocity change from accelerometer optisyn                                                                                                    |
| $\Delta L_3$          | Correction to velocity-to-be-gained L <sub>3</sub> [m/sec]                                                                                             |
| $\overline{\Delta R}$ | Radius-to-be-gained vector [m]                                                                                                                         |
| ΔR <sub>SAV</sub>     | Vector symbol for the low order bits of the computed position change during the past orbital navigation pass, saved to reduce truncation error [m/sec] |
| ΔΤ                    | Execution time of the last BML [sec]                                                                                                                   |

{45}------------------------------------------------

| Symbol           | <u>Definition</u> and Comments                                              |
|------------------|-----------------------------------------------------------------------------|
|                  |                                                                             |
|                  |                                                                             |
| ΔΤ'              | Previous value of $\Delta T$ [sec]                                          |
| ΔT' <sub>a</sub> | Adjusted value of $\Delta T$ for artificial tau calculations [sec]          |
| ΔТь              | Time bias to compensate for S-IVB engine thrust decay                       |
| Ь                | after cutoff [sec]                                                          |
| ΔT <sub>f</sub>  | Duration of S-IB guidance freeze [sec]                                      |
| $\Delta T_{N}$   | Nominal value of $\Delta T$ [sec]                                           |
| ΔT <sub>N1</sub> | Nominal $\Delta T$ from T0 + 0.0 until T1 + 0.0 [sec]                       |
| ΔT <sub>N2</sub> | Nominal $\Delta T$ from T1 + 0.0 until T3 + T3 <sub>IGM</sub> [sec]         |
| ΔT <sub>N3</sub> | Nominal $\Delta T$ from T3 + T3 <sub>IGM</sub> until T4- $\epsilon_2$ [sec] |
| ΔT <sub>N4</sub> | Nominal $\Delta T$ from T4- $\epsilon_2$ to T4-T <sub>HSL</sub> [sec]       |
| ΔT <sub>N5</sub> | Nominal $\Delta T$ from T4 + 0.0 until T4 + BN <sub>5</sub> [sec]           |
| ΔT <sub>N6</sub> | Nominal $\Delta T$ from T4 + BN <sub>5</sub> to EOM [sec]                   |
| ΔΤ'3             | Time-to-go correction term based on vehicle per-                            |
|                  | formance during the past BML [sec]                                          |
| ΔV               | Actual change in velocity in S-system [pulses]                              |

{46}------------------------------------------------

Symbol Symbol

Definition and Comments

| $\overline{\Delta V}$ | Velocity-to-be-gained vector [m/sec]                                      |
|-----------------------|---------------------------------------------------------------------------|
| ΔV'                   | Approximation for $\overline{\Delta V}$ [m/sec]                           |
| $\Delta v_b$          | Velocity bias to allow for engine thrust decay after cutoff [m/sec]       |
| ΔV <sub>cx</sub>      | Change in velocity along $X_S$ from velocity counter [m/sec]              |
| ΔV <sub>cy</sub>      | Change in velocity along $Y_S$ from velocity counter [m/sec]              |
| ΔV <sub>cz</sub>      | Change in velocity along $\mathbf{Z}_{S}$ from velocity counter [m/sec]   |
| ΔV <sub>f</sub>       | Expected change in velocity in the S-coordinate system [pulses]           |
| ΔV <sub>fx</sub>      | Expected change in velocity along $\mathbf{X}_{S}$ due to thrust [pulses] |
| $\Delta v_{fy}$       | Expected change in velocity along $Y_S$ due to thrust [pulses]            |
| ΔV <sub>fz</sub>      | Expected change in velocity along $\mathbf{Z}_{S}$ due to thrust [pulses] |
| ΔV <sub>gx</sub>      | Change in velocity along $X_S$ due to gravity [m/sec]                     |

{47}------------------------------------------------

| Symbol Symbol           | Definition and Comments                                                                                                                                |   |
|-------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------|---|
|                         |                                                                                                                                                        |   |
| ΔV <sub>gy</sub>        | Change in velocity along $Y_S$ due to gravity [m/sec]                                                                                                  |   |
| ΔV <sub>gz</sub>        | Change in velocity along $Z_S$ due to gravity [m/sec]                                                                                                  |   |
| ΔV <sub>SAV</sub>       | Vector symbol for the low order bits of the computed velocity change during the past orbital navigation pass, saved to reduce truncation error [m/sec] |   |
| $\Delta V_{\mathbf{x}}$ | Change in velocity along X <sub>S</sub> [m/sec]                                                                                                        |   |
| $\Delta v_{\mathbf{y}}$ | Change in velocity along Y <sub>S</sub> [m/sec]                                                                                                        |   |
| $\Delta V_{\mathbf{z}}$ | Change in velocity along Z <sub>S</sub> [m/sec]                                                                                                        |   |
| ΔΧ                      | Attitude error about roll gimbal axis [pirads]                                                                                                         | * |
| $\Delta X_{mf}$         | Change in $X_S$ in $\Delta T$ seconds due to thrust [m]                                                                                                |   |
| ΔX <sub>Sg</sub>        | Change in radius vector along $\mathbf{X}_{S}$ axis due to gravitational acceleration [m]                                                              |   |
| $\Delta x_{V}$          | Component of $\overline{\Delta R}$ along $X_V$ axis [m]                                                                                                |   |
| Δx <sub>v</sub>         | Component of $\overline{\Delta V}$ along $X_{\overline{V}}$ axis [m/sec]                                                                               |   |
| ΔXV                     | Component of $\overline{\Delta V}^{\dagger}$ along $X_{\overline{V}}$ axis [m/sec]                                                                     |   |
| ΔΥ                      | Attitude error about pitch gimbal axis [pirads]                                                                                                        | * |

{48}------------------------------------------------

| Symbol Symbol     | Definition and Comments                                                                         |   |
|-------------------|-------------------------------------------------------------------------------------------------|---|
|                   |                                                                                                 |   |
| $\Delta Y_{m  f}$ | Change in $Y_S$ in $\Delta T$ seconds due to thrust [m]                                         |   |
| <sup>ΔΥ</sup> Sg  | Change in radius vector along $\mathbf{Y}_{\hat{S}}$ axis due to gravitational acceleration [m] |   |
| $\Delta Y_{V}$    | Component of $\overline{\Delta R}$ along $Y_V$ axis [m]                                         |   |
| ΔΫ́ν              | Component of $\overline{\Delta V}$ along $Y_{\overline{V}}$ axis [m/sec]                        |   |
| ΔΫ́V              | Component of $\overline{\Delta V}$ along $Y_V$ axis [m/sec]                                     |   |
| ΔΖ                | Attitude error about yaw gimbal axis [pirads]                                                   | * |
| ΔZ <sub>mf</sub>  | Change in $Z_S$ in $\Delta T$ seconds due to thrust [m]                                         |   |
| ΔZ <sub>Sg</sub>  | Change in radius vector along $\mathbf{Z}_{S}$ axis due to gravitational acceleration [m]       |   |
| $\Delta z_{v}$    | Component of $\overline{\Delta R}$ along $Z_{V}$ axis [m]                                       |   |
| Δż <sub>v</sub>   | Component of $\overline{\Delta V}$ along $Z_{\overline{V}}$ axis [m/sec]                        |   |
| ΔZV               | Component of $\overline{\triangle V}$ along $Z_{V}$ axis [m/sec]                                | * |
| ΔΘ <sub>xe</sub>  | Expected change in $\Theta_{\mathbf{x}}$ [pirads]                                               |   |
| ΔΘ<br>ye          | Expected change in $\Theta_{\mathbf{y}}$ [pirads]                                               |   |
| ΔΘ <sub>ze</sub>  | Expected change in $\Theta_{\mathbf{z}}$ [pirads]                                               |   |

{49}------------------------------------------------

| Symbol                                  | Definition and Comments                                                                      |
|-----------------------------------------|----------------------------------------------------------------------------------------------|
|                                         |                                                                                              |
| $\Delta\chi_{\mathbf{x}}$               | Computed change in $\chi_{\mathbf{x}}^{ \bullet}$ for present major loop [pirads]            |
| $\Delta \chi_{\mathbf{x}}^{\mathbf{x}}$ | Computed $\chi_{_{\mbox{\bf X}}}$ guidance command increment for minor loop [pirads]         |
| $\Delta \chi_{\mathbf{y}}$              | Computed change in $\chi_y^{\prime}$ for present major loop [pirads]                         |
| $\Delta \chi_y^{\dagger}$               | Computed $\chi_{\boldsymbol{y}}$ guidance command increment for minor loop [pirads]          |
| $\Delta \chi_{\mathbf{z}}^{-1}$         | Computed change in $\chi_{\mathbf{Z}}^{\boldsymbol{\prime}}$ for present major loop [pirads] |
| $\Delta \chi_{\mathbf{z}}^{\prime}$     | Computed $\chi_{_{\mathbf{Z}}}$ guidance command increment for minor loop [pirads]           |
| EPSILON                                 |                                                                                              |
| <sup>ε</sup> <b>2</b>                   | Time prior to S-IVB cutoff to begin $\tilde{\chi}$ -steering [sec] (see note 1)              |
| THETA                                   |                                                                                              |
| $\Theta_{\mathbf{R}}$                   | Total angular rotation of earth since GRR [pirads]                                           |
| $\Theta_{\mathbf{T}}$                   | Desired terminal flight path angle [pirads]                                                  |

{50}------------------------------------------------

Symbol

 $\Theta_{\mathbf{z}}$

Θ'z

Θ<sub>za</sub>

[pirads]

Definition and Comments

| $\Theta_{\mathbf{x}}$ | Platform gimbal angle about $X_{p}$ axis [pirads]                                            |
|-----------------------|----------------------------------------------------------------------------------------------|
| Θ <b>'</b>            | Value of $\Theta_{\mathbf{x}}$ used in minor loop [pirads]                                   |
| <sup>Θ</sup> ха       | Average value of $\Theta_{_{\mbox{\scriptsize X}}}$ during next major loop pass [pirads]     |
| Эхе                   | Expected value of $\theta_{_{\mathbf{X}}}$ [pirads]                                          |
| o <sub>xe</sub>       | Expected rate of change of $\theta_{_{\mathbf{X}}}$ [pirads/sec]                             |
| Θy                    | Platform gimbal angle about $Y_p$ axis [pirads]                                              |
| Θ <b>'</b>            | Value of $\Theta_{\mathbf{y}}$ used in minor loop [pirads]                                   |
| θуа                   | Average value of $\boldsymbol{\theta}_{\boldsymbol{y}}$ during next major loop pass [pirads] |
| <sup>Ө</sup> уе       | Expected value of $\theta_y$ [pirads]                                                        |
| •                     |                                                                                              |

Average value of  $\boldsymbol{\theta}_{_{\mathbf{Z}}}$  during next major loop pass

Expected rate of change of  $\theta_y$  [pirads/sec]

Value of  $\theta_z$  used in minor loop [pirads]

Platform gimbal angle about  $\mathbf{Z}_{\mathbf{p}}$  axis [pirads]

{51}------------------------------------------------

# Symbol Definition and Comments

- $\Theta_{ze}$  Expected value of  $\Theta_{z}$  [pirads]
- $\dot{\Theta}_{ze}$  Expected rate of change of  $\Theta_{z}$  [pirads/sec]

#### LAMBDA

- Angle from launch meridian to the descending node of the desired orbit computed as a function of  $\lambda_0$  ,  $\dot{\lambda}$  , and  $T_D$  .
- $\lambda_{0}$  Value of  $\lambda$  valid for a GRR time of  $T_{GRRO}$  [pirads]
- $\lambda$  Time rate of change of  $\lambda_0$  [pirads/sec]

#### MU

 $\mu$  Product of the universal gravitational constant and the mass of the earth  $[\text{m}^3/\text{sec}^2]$

#### PI

π 3.14159265

{52}------------------------------------------------

| Symbol                         | Definition and Comments                                                                                                  |
|--------------------------------|--------------------------------------------------------------------------------------------------------------------------|
| RHO                            |                                                                                                                          |
| ρ                              | Atmospheric density [kg/m <sup>3</sup> ]                                                                                 |
| <sup>ρ</sup> c                 | Constant $\rho$ for use when altitude is less than $^h1$ [kg/m $^3$ ]                                                    |
| P <sub>0</sub> -P <sub>5</sub> | Coefficients for atmospheric density polynomial $[kg/m^3, kg/m^4, kg/m^5, kg/m^6, kg/m^7, kg/m^8]$                       |
| TAU                            |                                                                                                                          |
| τ                              | Ratio of mass to mass flow rate [sec]                                                                                    |
| τ1                             | Value of $\tau$ during first IGM phase [sec]                                                                             |
| τ 10                           | Initial value of $\tau_1$ used in artificial $\tau$ calculations [sec]                                                   |
| т 3                            | Value of T during second IGM phase [sec]                                                                                 |
| т 3К                           | Intermediate $\tau_3$ based on performance of the first phase burn; used in computing weighted artificial $\tau_3$ [sec] |
| <sup>τ</sup> 3М                | IGM $\tau_3$ based on (M/F) $_S$ and $v_{ex3}$ used in computing weighted artificial $\tau_3$ [sec]                      |
| <sup>τ</sup> 3N                | Nominal value of $\tau_3^{}$ used in computing artificial $\tau_3^{}$ [sec]                                              |

{53}------------------------------------------------

| Symbol_                    | Definition and Comments                                                                 |
|----------------------------|-----------------------------------------------------------------------------------------|
| PHI                        |                                                                                         |
| $\phi_{H}$                 | Geodetic latitude of Huntsville used in flight simulation [pirads]                      |
| φ <sub>i</sub>             | Range angle traveled since liftoff [pirads]                                             |
| $\dot{\phi}_{\mathtt{i}}$  | Rate of change of $\phi_i$ [pirads/sec]                                                 |
| $^{\varphi}\mathtt{_{iT}}$ | Predicted range angle-to-go [pirads]                                                    |
| $\phi_{\mathbf{L}}$        | Geodetic latitude of launch site [pirads]                                               |
| φ <b>'</b> L               | Geocentric latitude of launch site [pirads]                                             |
| φ <sub>P</sub>             | Angle between $\overline{P}$ and $X_4$ axis [pirads]                                    |
| φso                        | Range angle at control return from SC to IU [pirads]                                    |
| $\Phi_{\mathbf{T}}$        | Predicted terminal range angle [pirads]                                                 |
| ф <sub>Т</sub>             | Rate of change of predicted terminal range angle [pirads/sec]                           |
| CHI                        |                                                                                         |
| Χ                          | Desired vehicle attitude (steering angle) [pirads]                                      |
| x                          | Desired vehicle attitude (steering angle) neglecting position correction terms [pirads] |

{54}------------------------------------------------

# Symbol Definition and Comments

| $x_{\mathbf{x}}$    | Desired platform gimbal angle about $\mathbf{X}_{\mathbf{p}}$ axis [pirads]                  |
|---------------------|----------------------------------------------------------------------------------------------|
| Χ <b>'</b>          | Rate limited value of $\chi_{_{\mathbf{X}}}$ in minor loop [pirads]                          |
| χ <b>"</b>          | Value of $\chi_{x}^{\prime}$ used for flight simulation [pirads]                             |
| Xxp                 | Predicted value of $\chi_{_{\mathbf{X}}}^{^{\bullet}}$ at end of present major loop [pirads] |
| Xy                  | Desired platform gimbal angle about $Y_p$ axis [pirads]                                      |
| $\tilde{x}_y$       | Desired pitch attitude neglecting position correction terms [pirads]                         |
| Χ'n                 | Rate limited value of $\chi_y$ used in minor loop [pirads]                                   |
| X"y                 | Value of $\chi_y^{\prime}$ used for flight simulation [pirads]                               |
| X'yp                | Predicted value of $\chi_{y}^{\prime}$ at end of present major loop [pirads]                 |
| X <sub>y4</sub>     | Desired pitch attitude in 4-system calculated by IGM [pirads]                                |
| $\chi_{\mathbf{z}}$ | Desired platform gimbal angle about Z <sub>p</sub> axis [pirads]                             |
| $\tilde{\chi}_z$    | Desired yaw attitude neglecting position correction terms [pirads]                           |

{55}------------------------------------------------

# Symbol Definition and Comments

| Χż              | Rate limited value of $\chi_{\mathbf{z}}$ used in minor loop [pirads]        |
|-----------------|------------------------------------------------------------------------------|
| χ <b>''</b>     | Value of $\chi_z^{\prime}$ used for flight simulation [pirads]               |
| X'zp            | Predicted value of $\chi_{z}^{\prime}$ at end of present major loop [pirads] |
| X <sub>z4</sub> | Desired yaw attitude in 4-system calculated by IGM [pirads]                  |

# OMEGA

 $\omega_{E}$  Rotation rate of the earth [pirads/sec]

{56}------------------------------------------------

| Variable           | Expected<br>Maximum |
|--------------------|---------------------|
| (M/F)              | 0.40 E 00           |
| (M/F) <sub>S</sub> | 0.40 E 00           |
| ΔẌ̈́V              | 0.40 E 04           |
| ΔΫ́V               | 0.20 E 04           |
| ΔŻ̈́V              | 0.80 E 04           |
| 1/R                | 0.20 E-06           |
| G <sub>T</sub>     | 0.10 E 02           |
| GSX                | 0.10 E 02           |
| G <sub>Sy</sub>    | 0.10 E 02           |
| G <sub>Sz</sub>    | 0.10 E 02           |
| c <sub>2</sub>     | 0.19 E 01           |
| C <sub>4</sub>     | 0.19 E-02           |
| Δ <sub>L3</sub>    | 0.20 E 04           |
| D <sub>p</sub>     | 0.10 E 07           |
| ΔΤ'3               | 0.12 E 03           |
| Δx <sub>v</sub>    | 0.40 E 04           |
| ΔX <sub>V</sub>    | 0.50 E 06           |
| Dy                 | 0.10 E 07           |
| ΔΫ́V               | 0.20 E 04           |
| ΔY <sub>V</sub>    | 0.25 E 06           |
| Δż <sub>V</sub>    | 0.80 E 04           |
| J <sub>p</sub>     | 0.20 E 07           |

| Variable       | Expected<br>Maximum |
|----------------|---------------------|
| Jy             | 0.20 E 07           |
| J <sub>1</sub> | 0.10 E 07           |
| J <sub>3</sub> | 0.10 E 07           |
| J'3            | 0.10 E 07           |
| K p            | 0.10 E 01           |
| K <sub>y</sub> | 0.10 E 01           |
| к <sub>1</sub> | 0.10 E 01           |
| K <sub>2</sub> | 0.10 E 01           |
| К <sub>3</sub> | 0.10 E 01           |
| К4             | 0.10 E 01           |
| L p            | 0.80 E 04           |
| Ly             | 0.80 E 04           |
| L'y            | 0.80 E 04           |
| L <sub>1</sub> | 0.80 E 04           |
| L <sub>3</sub> | 0.80 E 04           |
| L'3            | 0.80 E 04           |
| P 1            | 0.25 E 09           |
| P 3            | 0.10 E 10           |
| Q <sub>p</sub> | 0.50 E 09           |
| Qy             | 0.50 E 09           |
| Q <sub>1</sub> | 0.13 E 09           |
| Q <sub>3</sub> | 0.64 E 08           |

{57}------------------------------------------------

TABLE 15-3 MAXIMUM VALUES OF SELECTED VARIABLES (CONTINUED)

| Variable         | Expected<br>Maximum |
|------------------|---------------------|
| SMCY             | 0.10 E 01           |
| SMCZ             | 0.10 E 01           |
| Sp               | 0.40 E 07           |
| Sy               | 0.40 E 07           |
| s <sub>1</sub>   | 0.10 E 07           |
| s <sub>3</sub>   | 0.10 E 07           |
| τ <sub>1</sub>   | 0.10 E 04           |
| т 3              | 0.10 E 04           |
| T*               | 0.10 E 04           |
| T <sub>li</sub>  | 0.50 E 03           |
| T <sub>3i</sub>  | 0.10 E 04           |
| U <sub>1</sub>   | 0.80 E 11           |
| U <sub>3</sub>   | 0.68 E 11           |
| v <sub>T</sub>   | 0.16 E 05           |
| X <sub>VgT</sub> | 0.10 E 02           |
| x <sub>v</sub>   | 0.80 E 04           |
| х <sub>vт</sub>  | 0.40 E 04           |
| x <sub>4</sub>   | 0.16 E 05           |
| F'Sx             | 0.10 E 01           |
| x <sub>V</sub>   | 0.80 E 07           |
| х <sub>у</sub>   | 0.10 E 02           |
| X <sub>VT</sub>  | 0.80 E 07           |

| Variable Ma          | apect<br>aximu<br>30 E | 1 M |
|----------------------|------------------------|-----|
| x <sub>4</sub> 0.8   |                        | 07  |
| •                    | LO E                   |     |
| Y <sub>VgT</sub> 0.1 |                        | 02  |
| Y <sub>V</sub> 0.4   | 40 E                   | 0 4 |
| F'Sy 0.1             | LO E                   | 01  |
| Y v 0.4              | 40 E                   | 0 7 |
| VØ                   | LO E                   | 01  |
| Z <sub>VoT</sub> 0.3 | 10 E                   | 02  |
| $z_{v}$              | 16 E                   | 05  |
| ż <sub>VT</sub> 0.1  | L6 E                   | 0.5 |
|                      | 16 E                   | 05  |
|                      | 10 E                   | 01  |
| V                    | 40 E                   | 07  |
| ". * vg 0.1          | 10 E                   | 02  |
|                      | 30 E                   | 07  |
| 5                    | 16 E                   | 05  |
|                      | 30 E                   | 07  |
| •                    | 40 E                   | 0 4 |
| 5                    | 30 E                   | 07  |
| ż <sub>s</sub> 0.1   | 16 E                   | 0.5 |
|                      | 80 E                   | 07  |
| R 0.8                | 30 E                   | 07  |
|                      |                        |     |