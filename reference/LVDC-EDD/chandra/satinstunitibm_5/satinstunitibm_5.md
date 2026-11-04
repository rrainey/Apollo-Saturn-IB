## SECTION 12

### PREFLIGHT TESTS

#### 12.1 INTRODUCTION

To make the LVDC flight program usable during preflight vehicle tests, special flight program test options must be provided. These options must provide simulation of flight-like conditions which exercise the interfaces between the LVDC/LVDA and the remainder of the vehicle, and must allow preflight linkage tests, which exercise the control system and allow ground communication via the Digital Command System (DCS).

#### 12.2 FLIGHT SIMULATION

A flight simulation option must provide the capability to demonstrate integrity of the LVDC memory, issue switch selector commands, issue special LVDA ladder profiles, and accept certain discretizes and interrupts. Flight simulation must have two modes: repeatable and non-repeatable.

##### 12.2.1 Repeatable

The repeatable flight simulation mode will provide the capability to execute a portion of the LVDC memory logic with an exact predictability. This capability must be furnished to provide a check on the integrity of the memory. This test mode will use the actual flight equations; however, to insure complete predictability, the following modifications must be made to nominal flight input processing.

- • Substitution of stored acceleration profile for accelerometer input
- • Substitution of internal timing for external sequencing events

1------------------------------------------------

- • Substitution of computed guidance angles for attitude angles
- • Substitution of preset typical targeting load data for the indeterminate DCS targeting load parameters. \*  
  \* \*

The use of the stored acceleration or  $(F/M)_c$  profile will assure that the program will bypass the accelerometer error processing; thus, providing predictable input to the boost navigation equations. The stored acceleration or  $(F/M)_c$  profile is generated as a function of prestored vehicle mass, mass flow rate, and thrust values based on vehicle performance predictions (Eq. 4.2.12, 4.2.13, and 12.2.1). The resulting acceleration is then used in Eq. 12.2.2 to produce velocity changes which replace the velocity changes normally read from external accelerometers. The velocity changes computed from  $(F/M)_c$  will drive the navigation equations and produce a boost trajectory.

The use of internal timing will allow the program to insure that event dependent processing occurs at fixed times. The nominal time base start times, as specified in the Event Sequence Timeline Table in Part II, will be used to internally force time base initiation at the nominal time. \*  
\* \*

The use of guidance angles will allow predictable resolution of the  $(F/M)_c$  profile and will also assure bypassing the flight program gimbal angle error processing. Minor loop processing will essentially be reduced to processing the ladder profiles, as discussed in Section 12.2.2.5.

The use of preset typical targeting load data in place of DCS targeting data is necessary to assure predictable input to the guidance equations. \*  
\* \* \*

In order to allow simultaneous LVDC and stage testing, the repeatable test mode must bypass switch selector output processing. This feature will also allow repeatable test execution

2------------------------------------------------

during the terminal countdown without causing erroneous stage sequencing on the pad.

The repeatable mode must have a means of initializing all hardware and software functions that would influence the predictability of flight simulation execution. The areas requiring processing include:

- • The status of the real-time clock and the two internal interrupt counters
- • The variable data words in LVDC memory
- • The activation of logic to bypass external input sampling.

The termination of a repeatable test run will be a function of internal processing of T4+145 seconds. Upon termination of each repeatable test run, LVDC memory occupied by the flight program must have a bit-by-bit configuration identical to the configuration at the end of any other repeatable run made with that program. This will provide the capability to verify that memory has not been altered from the verified state, and that the LVDC is functioning normally. The technique that will be used to verify memory will be under the control of the RCA-110A. Each memory sector will be summed and the sector sums will be compared against a predetermined set of memory sums. Any deviation from the predetermined sums will imply that either the memory has been altered or that the LVDC/LVDA has malfunctioned.

#### 12.2.2 Non-Repeatable

The non-repeatable flight simulation mode must provide the capability to selectively satisfy the following objectives:

3------------------------------------------------

- • Verify correct accelerometer interface
- • Verify proper gimbal angle interface
- • Verify proper acceptance of vehicle inputs
- • Verify proper use of switch selector interface
- • Issue ladder commands to exercise the flight control system.

The non-repeatable mode differs from the repeatable in that acceptance of external inputs causes timing differences which affect the LVDC operation and, hence, result in non-repeatable (non-compare) memory checksums. The following external inputs must be accepted depending on test definition:

- • Accelerometer readings
- • Gimbal readings
- • Vehicle interrupts and discretes
- • Switch selector processing for designated stages.

#### 12.2.2.1 Accelerometer Interface

The verification of the interface between the LVDC/LVDA and the accelerometers can only be accomplished in tests that include a powered-up platform which becomes inertial at GRR in the test countdown. This interface will be verified in the following manner for powered-up platform runs. Since the platform is inertial, earth's rotational effects will be measured by the accelerometers. This expected earth rotational effect will be computed by Eq. 12.2.3.

4------------------------------------------------

The LVDC/LVDA, through the flight program, will read and process accelerometer readings and subtract the expected readings from the actual readings, Eq. 12.2.4. Any discrepancy between the expected and the actual reading will result in abnormal navigation and guidance results. These abnormalities will in turn cause the S-IVB cutoff time to vary outside nominal limits, thereby providing an indication of an error.

#### 12.2.2.2 Gimbal Interface

The verification of the interface between the LVDC/LVDA and the gimbals is accomplished in much the same manner as the accelerometer verification. Again, the inertial platform must be powered up for gimbal interface verification. For repeatable simulations, this gimbal interface will be accomplished by the following method. The expected gimbal readings due to the earth's rotation are calculated using Eq. 12.2.5 through 12.2.7. The attitude errors are calculated using Eq. 12.2.8 and 12.2.9. \*

These attitude errors, if any, are combined with the increments from the ladder profiles as defined in Section 12.2.2.5 to generate the ladder commands. Since all ladder commands are recorded on strip charts during preflight tests, any unexpected ladder signal will be immediately noticeable and will indicate either platform alignment error or erroneous gimbal operation. Although erroneous gimbal readings will result in abnormal ladder outputs, the trajectory flown will not be affected since the commanded attitude angles, and not the gimbal angles, are used to resolve the accelerometer vector.

5------------------------------------------------

### 12.2.2.3 Vehicle Interrupt and Discrete Interface

Since in the non-repeatable simulation mode, external interrupts must be enabled and the vehicle discrete input register must be sampled, indications of time base initiation events can be set through the ground computer to force the flight program to initiate the desired time bases. Thus, proper recognition of vehicle inputs by the LVDC/LVDA can be verified.

### 12.2.2.4 Switch Selector Interface

Non-repeatable flight simulation mode must provide the capability to issue switch selector commands for selected vehicle stages. By monitoring LVDC issuance of switch selector commands, it will be possible to verify that all interfaces between LVDC/LVDA and the vehicle via the switch selector are correct. It will also be possible to simulate failures in the switch selector and, thereby, force the flight program to take corrective action as defined in Section 9.

### 12.2.2.5 Flight Control Computer Interface

Another function of flight simulation will be the issuance of a series of prestored ladder profiles. Execution of these profiles will exercise the flight control computer by issuing a series of ramps at specified times. Proper functioning of the control computer can be determined by monitoring its output in response to these inputs.

The required profiles are defined in the Flight Simulation Ladder Profiles Figure in Part II. The slope of the ramps shown in the figure must be one degree per second. The square waves must be generated at the maximum allowable ladder rate,

6------------------------------------------------

maintained at the specified level for at least two seconds, then returned to zero at the maximum allowable ladder rate.

The levels shown must be commanded with a tolerance of  $\pm 1$  ladder bit. The profiles must be issued at the times specified with a tolerance of + 200 milliseconds.

### 12.3 FLIGHT SIMULATION OPTIONS

In order to allow flexibility in operating the flight program in the flight simulation mode, the capability to select combinations of operating conditions must be available. A flight simulation indicator word will be loaded into the LVDC via the ground computer. The word will supply the following information:

- • Flight simulation selected
- • Switch selector status
- • Repeatable/non-repeatable option
- • Data compression status
- • Platform status
- • Test site indication.

An explanation of these options is given in the following paragraphs.

#### 12.3.1 Flight Simulation Selected

Since the indicator word is examined in flight as well as flight simulation, the indicator must be set to flight simulation status prior to each preflight test run. All flight

7------------------------------------------------

simulation runs, then, upon completion, must reset the indicator to flight status.

#### 12.3.2 Switch Selector Status

Since all stages of the launch vehicle may not be powered during a flight simulation test, this option must be provided to issue switch selector commands to selected stages only.

#### 12.3.3 Repeatable/Non-Repeatable

The selection of the repeatable or non-repeatable option will be controlled through this indication.

#### 12.3.4 Data Compression Status

This option must be provided to bypass the storing of compressed data over the preflight LVDC programs, thus eliminating the need to reload the preflight programs after every flight simulation run.

#### 12.3.5 Platform Status

As defined earlier, the LVDC programs must have the capability to test LVDC/LVDA platform interfaces. Under some conditions, it will be necessary to make flight simulation runs without powering the platform. When the platform interface is not exercised, Eq. 12.2.3 and 12.2.5 through 12.2.9 are not solved by the LVDC programs.

#### 12.3.6 Test Site Indicator

As is shown in Eq. 12.2.3 and 12.2.5, the earth's rotational effects on the platform are functions of the test site latitude

8------------------------------------------------

and total gravity acceleration at the test site. This indicator, therefore, determines whether Huntsville or KSC values should be used.

#### 12.4 PREFLIGHT LINKAGE TESTS

The LVDC program must provide the access linkage to allow the preflight logic to exercise certain portions of the flight logic during vehicle tests. The two tests presently defined are the Guidance and Control (G & C) Steering Test and the Preflight Command Receiver Test.

##### 12.4.1 G & C Steering Linkage

The G & C Steering Test requires that the preflight program use the flight program's Minor Loop logic to monitor and respond to testing of the ST-124M inertial platform. The flight program must provide the access linkage from the preflight program to allow the following processing:

- • The initialization of linkage parameters between the preflight and the flight routines
- • The initialization of required transformation matrices
- • The initialization of gimbal input parameters
- • The initialization of gimbal reading processing parameters
- • The initialization of timekeeping and scheduling parameters.

9------------------------------------------------

This initialization must insure the complete activation of all logic required for subsequent execution of the Minor Loop by the preflight program as required by test procedures.

#### 12.4.2 Preflight Command Receiver Linkage

The Preflight Command Receiver test, also known as the Houston Interface test, requires that the preflight program use the flight program's Digital Command System (DCS) logic to respond to and process DCS commands from Mission Control Center in Houston for checkout of the vehicle Command Receiver system. The flight logic must provide the access linkage from the preflight logic to allow the following processing:

- • The initialization of linkage parameters between the preflight and flight programs
- • The initialization of all logic switches and processing parameters
- • The initialization of timekeeping and scheduling parameters.

This initialization must insure that all mission dependent commands are enabled and that DCS processing is activated as defined for actual flight. In order to prevent undesirable program action such as issuance of Generalized Switch Selector commands, the flight program must insure that all command response remains internal to the DCS logic.

10------------------------------------------------

## SECTION 13

### ALGORITHMS

#### 13.1 INTRODUCTION

Elementary functions and mathematical procedures must be computed by the algorithms and techniques described below. Logic necessary to make the algorithms flexible enough to accept inputs in the ranges expected is also defined.

The angular unit to be used in the flight program and in these algorithms is the pirad. One pirad is equal to  $\pi$  radians, or 180 degrees. For example,

$$45^\circ = 45^\circ \left( \frac{1 \text{ pirad}}{180^\circ} \right) = 0.25 \text{ pirad}$$
$$.628 \text{ radians} = .628 \text{ radians} \left( \frac{1 \text{ pirad}}{\pi \text{ radians}} \right) = 0.20 \text{ pirads}$$

All principal angles are in the range of -1.0 to +1.0 when expressed in pirads.

The polynomials cited in this section for the calculation of sines, arctangents, and natural logarithms are Chebyshev approximations.<sup>1</sup> The coefficients are converted to be compatible with flight program units.

#### 13.2 SINE-COSINE ALGORITHM

The sine-cosine algorithm uses a polynomial approximation of the sine function. The sine polynomial is

---

<sup>1</sup>Approximations for Digital Computers, by Cecil Hastings, Jr. (Princeton University Press, 1955).

11------------------------------------------------

$$\sin \alpha = 3.1415897\alpha - 5.1673678\alpha^3 + 2.5436052\alpha^5 - 0.55839693\alpha^7 \quad (13.2.1)^2$$

for  $-0.5 \leq \alpha \leq 0.5$ .

Given the angle  $\alpha'$  in pirads, the quadrant in which  $\alpha'$  lies must be determined and the two associated quantities,  $\bar{\alpha}$  and  $\tilde{\alpha}$  must be computed according to Table 13-1.

Table 13-1 SINE/COSINE ARGUMENT SELECTION

<table border="1">
<thead>
<tr>
<th><math>\alpha'</math><br/>Quadrant</th>
<th><math>\bar{\alpha}</math></th>
<th><math>\tilde{\alpha}</math></th>
</tr>
</thead>
<tbody>
<tr>
<td>I</td>
<td><math>\alpha'</math></td>
<td><math>0.5 - \alpha'</math></td>
</tr>
<tr>
<td>II</td>
<td><math>1.0 - \alpha'</math></td>
<td><math>0.5 - \alpha'</math></td>
</tr>
<tr>
<td>III</td>
<td><math>1.0 - \alpha'</math></td>
<td><math>0.5 + \alpha'</math></td>
</tr>
<tr>
<td>IV</td>
<td><math>\alpha'</math></td>
<td><math>0.5 + \alpha'</math></td>
</tr>
</tbody>
</table>

The quantities  $\bar{\alpha}$  and  $\tilde{\alpha}$  are now such that both lie between  $-0.5$  and  $0.5$ , satisfying the following identities:

$$\sin \alpha' = \sin \bar{\alpha}$$

$$\cos \alpha' = \cos \tilde{\alpha}$$

The numbers  $\bar{\alpha}$  and  $\tilde{\alpha}$  are substituted into the polynomial to obtain  $\sin \alpha'$  and  $\cos \alpha'$ , respectively.

The error characteristics of the sine-cosine algorithm are shown in Fig. 13-1.

---

<sup>2</sup> Parenthetical numbers in right field of page identify equations; a comprehensive list of equations for all sections of this document is presented in Section 14.

12------------------------------------------------

### 13.3 ARCTANGENT ALGORITHM

The arctangent algorithm uses a polynomial approximation to the arctangent function. This algorithm requires both the numerator Y and the denominator Z of the arctangent argument. An associated number V is computed from the following.

$$V = \frac{|Z| - |Y|}{|Z| + |Y|}$$

The principal arctangent of V, for  $-1.0 \leq V \leq 1.0$ , is calculated from the polynomial

$$\begin{aligned} \text{Arctan } V = & 0.31830264V - 0.10587734V^3 \\ & + 0.06160678V^5 - 0.037061733V^7 \\ & + 0.016760072V^9 - 0.0037309738V^{11} \end{aligned} \quad (13.3.1)$$

Since  $-1.0 \leq V \leq 1.0$ , the principal arctangent satisfies the inequality  $-0.25 \leq \text{Arctan } V \leq 0.25$ .

The algorithm thus determines  $\text{Arctan} \left( \frac{Y}{Z} \right)$ , from Table 13-2, such that the inequality

$$-1.0 \leq \text{Arctan} \left( \frac{Y}{Z} \right) \leq 1.0$$

is satisfied.

TABLE 13-2 ARCTANGENT POLYNOMIAL ARGUMENT

<table border="1">
<thead>
<tr>
<th>Y</th>
<th>Z</th>
<th>Arctan (Y/Z)</th>
</tr>
</thead>
<tbody>
<tr>
<td>+,0</td>
<td>+,0</td>
<td>0.25-Arctan V</td>
</tr>
<tr>
<td>+,0</td>
<td>-</td>
<td>0.75+Arctan V</td>
</tr>
<tr>
<td>-</td>
<td>+,0</td>
<td>-0.25+Arctan V</td>
</tr>
<tr>
<td>-</td>
<td>-</td>
<td>-0.75-Arctan V</td>
</tr>
</tbody>
</table>

13------------------------------------------------

The error characteristics for this algorithm are shown in Figure 13-2.

![Graph of Absolute Error vs Angle (Pirads) for the Sine-Cosine Algorithm.](447390796004f81ec3c3335b4b327176_2_img.webp)

A line graph showing the absolute error of the Sine-Cosine Algorithm. The vertical axis is labeled 'ABSOLUTE ERROR' and ranges from  $-1 \times 10^{-6}$  to  $1 \times 10^{-6}$  with major grid lines every  $1 \times 10^{-6}$ . The horizontal axis is labeled 'ANGLE (PIRADS)' and ranges from 0 to 1.0 with major grid lines every 0.1. The curve starts at (0,0), dips to a minimum of approximately  $-0.5 \times 10^{-6}$  at  $x \approx 0.05$ , rises to a peak of approximately  $0.8 \times 10^{-6}$  at  $x \approx 0.2$ , dips to a minimum of approximately  $-1.5 \times 10^{-6}$  at  $x \approx 0.4$ , rises to a sharp peak of approximately  $0.9 \times 10^{-6}$  at  $x \approx 0.45$ , dips to a sharp minimum of approximately  $-1.5 \times 10^{-6}$  at  $x \approx 0.5$ , rises to a sharp peak of approximately  $0.8 \times 10^{-6}$  at  $x \approx 0.55$ , dips to a minimum of approximately  $-1.5 \times 10^{-6}$  at  $x \approx 0.65$ , rises to a peak of approximately  $0.8 \times 10^{-6}$  at  $x \approx 0.8$ , dips to a minimum of approximately  $-0.5 \times 10^{-6}$  at  $x \approx 0.95$ , and ends at (1.0, 0).

Figure 13-1 Error Characteristics of the Sine-Cosine Algorithm

![Graph of Absolute Error (Pirads) vs Value of 'V' for the Arctangent Algorithm.](447390796004f81ec3c3335b4b327176_4_img.webp)

A line graph showing the absolute error of the Arctangent Algorithm. The vertical axis is labeled 'ABSOLUTE ERROR (PIRADS)' and ranges from  $-1 \times 10^{-6}$  to  $1 \times 10^{-6}$  with major grid lines every  $1 \times 10^{-6}$ . The horizontal axis is labeled 'VALUE OF "V"' and ranges from 0 to 1.0 with major grid lines every 0.1. The curve starts at (0,0), dips to a minimum of approximately  $-0.5 \times 10^{-6}$  at  $x \approx 0.05$ , rises to a peak of approximately  $0.8 \times 10^{-6}$  at  $x \approx 0.1$ , dips to a minimum of approximately  $-0.5 \times 10^{-6}$  at  $x \approx 0.2$ , rises to a peak of approximately  $0.8 \times 10^{-6}$  at  $x \approx 0.3$ , dips to a minimum of approximately  $-0.5 \times 10^{-6}$  at  $x \approx 0.5$ , rises to a peak of approximately  $0.8 \times 10^{-6}$  at  $x \approx 0.8$ , and ends at (1.0, 0).

Figure 13-2 Error Characteristics for Arctangent Algorithm

14------------------------------------------------

### 13.4 NATURAL LOGARITHM ALGORITHM

The natural logarithm algorithm uses a limited range polynomial combined with appropriate algebraic identities to extend the range for greater usefulness.

To find the logarithm of a number  $\alpha$ , the flight program must first determine a number  $\alpha'$  and an integer  $N$  such that  $1.0 \leq \alpha' \leq 2.0$  and  $\alpha' = \alpha 2.0^{-N}$ . The natural logarithm of  $\alpha'$  will then be determined by evaluating the following polynomial:

$$\begin{aligned} \ln \alpha' = & 0.99990167(\alpha' - 1.0) - 0.49787544(\alpha' - 1.0)^2 \\ & + 0.31765005(\alpha' - 1.0)^3 - 0.19376149(\alpha' - 1.0)^4 \\ & + 0.08556927(\alpha' - 1.0)^5 - 0.01833831(\alpha' - 1.0)^6, \quad 1.0 \leq \alpha' \leq 2.0 \end{aligned} \quad (13.4.1)$$

Finally, the relation

$$\ln \alpha = \ln(2.0^N \alpha') = N \ln 2.0 + \ln \alpha' = 0.6931471806 N + \ln \alpha' \quad (13.4.2)$$

will be used to calculate the natural logarithm of  $\alpha$ . The algorithm must accept  $\alpha$  in the range  $0.125 \leq \alpha \leq 8.0$ .

The error characteristics for this algorithm are given in Figure 13-3.

### 13.5 SQUARE ROOT ALGORITHM

The square root algorithm is derived from Newton's method of successive approximations. Given some positive number  $X (=x^2)$ , the square root of  $X$  will be obtained by iterating the expression

$$x_{i+1} = 0.5(x_i + \frac{X}{x_i}), \quad i = 0, 1, 2, 3, \dots, \quad (13.5.1)$$

15------------------------------------------------

![Graph showing Absolute Error in Log versus Polynomial Argument for the Natural Logarithm Algorithm. The curve oscillates between approximately 1.0 and 2.0 on the x-axis, with peaks and troughs reaching values between 10^-6 and -10^-6 on the y-axis.](ee0a20a88ba41de30b3fb3a7cd6f5474_1_img.webp)

The graph shows a periodic oscillation of the absolute error in the logarithm of the polynomial argument. The x-axis is labeled 'POLYNOMINAL ARGUMENT' and ranges from 1.0 to 2.0 with major grid lines every 0.5 units. The y-axis is labeled 'ABSOLUTE ERROR IN LOG' and ranges from  $-1 \times 10^{-6}$  to  $1 \times 10^{-6}$  with major grid lines every  $1 \times 10^{-6}$  units. The curve starts at (1.0, 0), rises to a peak of approximately  $1.2 \times 10^{-6}$  at x ≈ 1.1, crosses the x-axis at x ≈ 1.25, reaches a trough of approximately  $-1.2 \times 10^{-6}$  at x ≈ 1.4, crosses the x-axis again at x ≈ 1.55, reaches a second peak of approximately  $1.2 \times 10^{-6}$  at x ≈ 1.7, crosses the x-axis at x ≈ 1.85, reaches a second trough of approximately  $-1.2 \times 10^{-6}$  at x ≈ 1.95, and ends at x ≈ 2.0.

Figure 13-3 Error Characteristics for Natural Logarithm Algorithm

where  $x_0$  is an initial guess for the square root of  $X$ . Following this procedure,  $x_i \rightarrow \sqrt{X}$  as  $i \rightarrow \infty$ .

Four iterations are required to calculate square roots to an accuracy of six decimal places for  $\Delta x_0 = |x_0 - \sqrt{X}| < 0.50 \sqrt{X}$ . Three iterations are required for  $\Delta x_0 < 0.10 \sqrt{X}$ ; two iterations are required for  $\Delta x_0 < 0.01 \sqrt{X}$ ; and a single iteration is sufficient for  $\Delta x_0 < 0.001 \sqrt{X}$ .

In order to insure sufficient accuracy, the square root algorithm must be iterated three times each time the routine is called.

### 13.6 INVERSE SQUARE ROOT ALGORITHM

The inverse square root algorithm is a variation of Newton's method. The approximation for  $1.0/\sqrt{X}$  will be obtained by iterating the expression

16------------------------------------------------

$$x_{i+1} = 0.5 x_i (3.0 - x_i^2 X), \quad i = 0, 1, 2, 3, \dots \quad (13.6.1)$$

where  $x_0$  is an initial guess for the inverse square root of  $X$ . These iterations produce  $x_i \rightarrow 1.0/\sqrt{X}$  as  $i \rightarrow \infty$ .

Six iterations are required to calculate inverse square roots to an accuracy of six decimal places for  $\Delta x_0 = |x_0 - 1.0/\sqrt{X}| < 0.50(1.0/\sqrt{X})$ . Three iterations are required for  $\Delta x_0 < 0.10(1.0/\sqrt{X})$ ; two iterations are required for  $\Delta x_0 < 0.01(1.0/\sqrt{X})$ ; and a single iteration is sufficient for  $\Delta x_0 < 0.001(1.0/\sqrt{X})$ .

### 13.7 VECTOR DOT PRODUCT

The method to be used in calculating the dot product of two vectors  $\bar{A}$  and  $\bar{B}$  is

$$\bar{A} \cdot \bar{B} = A_x B_x + A_y B_y + A_z B_z, \quad (13.7.1)$$

where  $A_x, A_y, A_z$  and  $B_x, B_y, B_z$  are the components of  $\bar{A}$  and  $\bar{B}$ , respectively.

### 13.8 VECTOR CROSS PRODUCT

The method to be used to obtain the cross product vector  $\bar{C}$  of two vectors  $\bar{A}$  and  $\bar{B}$  is given by

$$\bar{C} = \begin{bmatrix} C_x \\ C_y \\ C_z \end{bmatrix} = \begin{bmatrix} (A_y B_z - A_z B_y) \\ (A_z B_x - A_x B_z) \\ (A_x B_y - A_y B_x) \end{bmatrix} = \bar{A} \times \bar{B} \quad (13.8.1)$$

where  $A_x, A_y$ , and  $A_z$  and  $B_x, B_y, B_z$  are the components of  $\bar{A}$  and  $\bar{B}$ , respectively, and  $C_x, C_y, C_z$  are the components of the cross product vector  $\bar{C}$ .

17------------------------------------------------

(THIS PAGE HAS BEEN LEFT BLANK INTENTIONALLY.)

18------------------------------------------------

## SECTION 14

### EQUATIONS

#### 14.1 INTRODUCTION

This section lists the equations defined by this document. The equations are appropriately numbered by sections for easy reference to the sections of the text.

In these equations, the liberty has been taken to define the equality sign according to the commonly accepted computer language meaning; that is, the symbol "=" shall mean "is replaced by" or "is set equal to."

19------------------------------------------------

(THIS PAGE HAS BEEN LEFT BLANK INTENTIONALLY.)

20------------------------------------------------

## 14.2 EQUATIONS

### [MSG] Matrix

$$[MSG] = \begin{bmatrix} \cos \phi_L & \sin \phi_L \sin A_z & -\sin \phi_L \cos A_z \\ -\sin \phi_L & \cos \phi_L \sin A_z & -\cos \phi_L \cos A_z \\ 0 & \cos A_z & \sin A_z \end{bmatrix} \quad (2.3.1)$$

### [MG4] Matrix

$$[MG4] = \begin{bmatrix} \cos \lambda & 0 & \sin \lambda \\ \sin i \sin \lambda & \cos i & -\sin i \cos \lambda \\ -\cos i \sin \lambda & \sin i & \cos i \cos \lambda \end{bmatrix} \quad (2.3.2)$$

### [MBS] Matrix

$$MBS11 = \cos \theta_y \cos \theta_z$$

$$MBS12 = -\cos \theta_x \cos \theta_y \sin \theta_z + \sin \theta_x \sin \theta_y$$

$$MBS13 = \sin \theta_x \cos \theta_y \sin \theta_z + \cos \theta_x \sin \theta_y$$

$$MBS21 = \sin \theta_z$$

$$MBS22 = \cos \theta_x \cos \theta_z$$

$$MBS23 = -\sin \theta_x \cos \theta_z$$

$$MBS31 = -\sin \theta_y \cos \theta_z$$

$$MBS32 = \cos \theta_x \sin \theta_y \sin \theta_z + \sin \theta_x \cos \theta_y$$

$$MBS33 = -\sin \theta_x \sin \theta_y \sin \theta_z + \cos \theta_x \cos \theta_y \quad (2.3.3)$$

21------------------------------------------------

[M4V] Matrix

$$[M4V] = \begin{bmatrix} \cos \phi_T & 0 & \sin \phi_T \\ 0 & 1 & 0 \\ -\sin \phi_T & 0 & \cos \phi_T \end{bmatrix} \quad (2.3.4)$$

[MS4] Matrix

$$\begin{aligned} MS411 &= \cos \lambda \cos \phi_L \\ MS412 &= \cos \lambda \sin \phi_L \sin A_z + \sin \lambda \cos A_z \\ MS413 &= -\cos \lambda \sin \phi_L \cos A_z + \sin \lambda \sin A_z \\ MS421 &= \sin i \sin \lambda \cos \phi_L - \cos i \sin \phi_L \\ MS422 &= \sin i \sin \lambda \sin \phi_L \sin A_z + \cos i \cos \phi_L \sin A_z \\ &\quad - \sin i \cos \lambda \cos A_z \\ MS423 &= -\sin i \sin \lambda \sin \phi_L \cos A_z - \cos i \cos \phi_L \cos A_z \\ &\quad - \sin i \cos \lambda \sin A_z \\ MS431 &= -\cos i \sin \lambda \cos \phi_L - \sin i \sin \phi_L \\ MS432 &= -\cos i \sin \lambda \sin \phi_L \sin A_z + \sin i \cos \phi_L \sin A_z \\ &\quad + \cos i \cos \lambda \cos A_z \\ MS433 &= \cos i \sin \lambda \sin \phi_L \cos A_z - \sin i \cos \phi_L \cos A_z \\ &\quad + \cos i \cos \lambda \sin A_z \end{aligned} \quad (2.3.5)$$

22------------------------------------------------

[MGA] Matrix

$$[MGA] = \begin{bmatrix} \cos \theta_R & 0 & \sin \theta_R \\ 0 & 1 & 0 \\ -\sin \theta_R & 0 & \cos \theta_R \end{bmatrix} \quad (2.3.6)$$

[MSA] Matrix

$$MSA11 = \cos \theta_R \cos \phi_L$$

$$MSA12 = \cos \theta_R \sin \phi_L \sin A_z + \sin \theta_R \cos A_z$$

$$MSA13 = -\cos \theta_R \sin \phi_L \cos A_z + \sin \theta_R \sin A_z$$

$$MSA21 = -\sin \phi_L$$

$$MSA22 = \cos \phi_L \sin A_z$$

$$MSA23 = -\cos \phi_L \cos A_z$$

$$MSA31 = -\sin \theta_R \cos \phi_L$$

$$MSA32 = -\sin \theta_R \sin \phi_L \sin A_z + \cos \theta_R \cos A_z$$

$$MSA33 = \sin \theta_R \sin \phi_L \cos A_z + \cos \theta_R \sin A_z \quad (2.3.7)$$

[MSV] Matrix

$$MSV11 = \cos \phi_T \cos \lambda \cos \phi_L - \sin \phi_T \cos i \sin \lambda \cos \phi_L \\ - \sin \phi_T \sin i \sin \phi_L$$

$$MSV12 = \cos \phi_T \cos \lambda \sin \phi_L \sin A_z + \cos \phi_T \sin \lambda \cos A_z \\ - \sin \phi_T \cos i \sin \lambda \sin \phi_L \sin A_z \\ + \sin \phi_T \sin i \cos \phi_L \sin A_z \\ + \sin \phi_T \cos i \cos \lambda \cos A_z$$

23------------------------------------------------

[MSV] Matrix (continued)

$$\begin{aligned}
 \text{MSV13} = & -\cos \phi_T \cos \lambda \sin \phi_L \cos A_z + \cos \phi_T \sin \lambda \sin A_z \\
 & + \sin \phi_T \cos i \sin \lambda \sin \phi_L \cos A_z \\
 & - \sin \phi_T \sin i \cos \phi_L \cos A_z \\
 & + \sin \phi_T \cos i \cos \lambda \sin A_z
 \end{aligned}$$

$$\text{MSV21} = \sin i \sin \lambda \cos \phi_L - \cos i \sin \phi_L$$

$$\begin{aligned}
 \text{MSV22} = & \sin i \sin \lambda \sin \phi_L \sin A_z + \cos i \cos \phi_L \sin A_z \\
 & - \sin i \cos \lambda \cos A_z
 \end{aligned}$$

$$\begin{aligned}
 \text{MSV23} = & -\sin i \sin \lambda \sin \phi_L \cos A_z - \cos i \cos \phi_L \cos A_z \\
 & - \sin i \cos \lambda \sin A_z
 \end{aligned}$$

$$\begin{aligned}
 \text{MSV31} = & -\sin \phi_T \cos \lambda \cos \phi_L - \cos \phi_T \cos i \sin \lambda \cos \phi_L \\
 & - \cos \phi_T \sin i \sin \phi_L
 \end{aligned}$$

$$\begin{aligned}
 \text{MSV32} = & -\sin \phi_T \cos \lambda \sin \phi_L \sin A_z - \sin \phi_T \sin \lambda \cos A_z \\
 & - \cos \phi_T \cos i \sin \lambda \sin \phi_L \sin A_z \\
 & + \cos \phi_T \sin i \cos \phi_L \sin A_z \\
 & + \cos \phi_T \cos i \cos \lambda \cos A_z
 \end{aligned}$$

$$\begin{aligned}
 \text{MSV33} = & \sin \phi_T \cos \lambda \sin \phi_L \cos A_z - \sin \phi_T \sin \lambda \sin A_z \\
 & + \cos \phi_T \cos i \sin \lambda \sin \phi_L \cos A_z \\
 & - \cos \phi_T \sin i \cos \phi_L \cos A_z \\
 & + \cos \phi_T \cos i \cos \lambda \sin A_z
 \end{aligned} \tag{2.3.8}$$

[MEG] Matrix

$$\begin{aligned}
 [\text{MEG}] = & \begin{bmatrix} \cos \theta_E & \sin \theta_E & 0 \\ 0 & 0 & -1 \\ -\sin \theta_E & \cos \theta_E & 0 \end{bmatrix}
 \end{aligned} \tag{2.3.9}$$

24------------------------------------------------

[MES] Matrix

$$\text{MES11} = \cos \phi_L \cos \theta_E$$
$$\text{MES12} = \cos \phi_L \sin \theta_E$$
$$\text{MES13} = \sin \phi_L$$
$$\text{MES21} = \sin \phi_L \sin A_z \cos \theta_E - \cos A_z \sin \theta_E$$
$$\text{MES22} = \sin \phi_L \sin A_z \sin \theta_E + \cos A_z \cos \theta_E$$
$$\text{MES23} = -\cos \phi_L \sin A_z$$
$$\text{MES31} = -\sin \phi_L \cos A_z \cos \theta_E - \sin A_z \sin \theta_E$$
$$\text{MES32} = -\sin \phi_L \cos A_z \sin \theta_E + \sin A_z \cos \theta_E$$
$$\text{MES33} = \cos \phi_L \cos A_z$$

(2.3.10)

25------------------------------------------------

(THIS PAGE HAS BEEN LEFT BLANK INTENTIONALLY.)

26------------------------------------------------

### Platform Azimuth

$$T_D = T_L' - 5 - T_{GRR0} \quad (3.2.1)$$
$$\lambda = \lambda_0 - \dot{\lambda}(T_D) \quad (3.2.2)$$
$$A_z = A_{00} + A_{01}\dot{i} + A_{10}\lambda + A_{11}i\lambda \quad (3.2.3)$$

### Flight Descending Node

$$T_D = T_{GMT} - T_{GRR0} \quad (3.4.1)$$
$$\lambda = \lambda_0 - \dot{\lambda}(T_D) \quad (3.4.2)$$

### Initial Position, Velocity, and Gravitational Acceleration Vectors Calculations

$$\overline{R_S} = R_L \begin{bmatrix} \cos(\phi_L - \phi_L') \\ \sin(\phi_L - \phi_L') \sin A_z \\ -\sin(\phi_L - \phi_L') \cos A_z \end{bmatrix} \quad (3.4.3)$$
$$\overline{V_S} = R_L \omega_E \pi \begin{bmatrix} 0 \\ \cos \phi_L' \cos A_z \\ \cos \phi_L' \sin A_z \end{bmatrix} \quad (3.4.4)$$
$$Y_G = -X_S \sin \phi_L + Y_S \cos \phi_L \sin A_z - Z_S \cos \phi_L \cos A_z \quad (3.4.5)$$
$$S = -\frac{\mu}{R^3} \left\{ 1 + J \left( \frac{a_e}{R} \right)^2 \left[ 1 - 5 \left( \frac{Y_G}{R} \right)^2 \right] \right\} \quad (3.4.6)$$
$$P = -\frac{\mu}{R^2} \left( \frac{a_e}{R} \right)^2 \left( \frac{2JY_G}{R} \right) \quad (3.4.7)$$

27------------------------------------------------

Initial Position, Velocity, and Gravitational Acceleration  
Vectors Calculations (Continued)

$$\overline{G}_S = S \overline{R}_S + P \begin{bmatrix} -\sin\phi_L \\ \cos\phi_L \sin A_z \\ -\cos\phi_L \cos A_z \end{bmatrix} \quad (3.4.8)$$

28------------------------------------------------

### Expected Velocity Changes Using Past Acceleration

$$\overline{\Delta V_f} = \begin{bmatrix} \Delta V_{fx} \\ \Delta V_{fy} \\ \Delta V_{fz} \end{bmatrix} = 20(F/M)\Delta T \begin{bmatrix} \cos \theta_y & \cos \theta_z \\ \sin \theta_z & \\ -\sin \theta_y & \cos \theta_z \end{bmatrix} \quad (4.2.1)$$

### Disagreement Test

$$|\Delta A_x - \Delta B_x| \leq 2 \text{ bits} \quad (4.2.1.1) \quad *$$
$$\Delta V_{Mx} = \begin{cases} \Delta A_x & \text{for A channel used} \\ \Delta B_x & \text{for B channel used} \end{cases} \quad (4.2.2)$$

Same for Y and Z channel

### Zero Test Values

$$\begin{cases} A_{c0} = 20(F/M)\Delta T \sin (2^\circ) \\ A_{c0} = 20(F/M)\Delta T \sin (6^\circ) \end{cases} \quad (4.2.3)$$
$$|\overline{\Delta V_f}| \leq A_{c0} \quad (4.2.3.1) \quad *$$

### Reasonableness Test

$$(0.5\overline{\Delta V_f} - 20RTC \Delta T) \leq \overline{\Delta V_M} \leq (1.5\overline{\Delta V_f} + 20RTC \Delta T) \quad \text{for } \overline{\Delta V_f} \geq 0 \quad (4.2.4)$$
$$(1.5\overline{\Delta V_f} - 20RTC \Delta T) \leq \overline{\Delta V_M} \leq (0.5\overline{\Delta V_f} + 20RTC \Delta T) \quad \text{for } \overline{\Delta V_f} < 0 \quad (4.2.5)$$

29------------------------------------------------

### Velocity Accumulation

$$\overline{\Delta V} = \begin{cases} \overline{\Delta V_M} & \text{for reasonable accelerometer readings} \\ \overline{\Delta V_B} & \text{for unreasonable accelerometer readings} \end{cases} \quad (4.2.6)$$

$$\overline{V_M} = \begin{bmatrix} \dot{X}_M \\ \dot{Y}_M \\ \dot{Z}_M \end{bmatrix} = \overline{V_M} + \overline{\Delta V} \quad (4.2.7)$$

### F/M Calculations

$$\overline{V_m} = \begin{bmatrix} \dot{X}_m \\ \dot{Y}_m \\ \dot{Z}_m \end{bmatrix} = \frac{\overline{V_M}}{20} \quad (4.2.8)$$

$$\overline{A_m} = \begin{bmatrix} \ddot{X}_m \\ \ddot{Y}_m \\ \ddot{Z}_m \end{bmatrix} = \frac{\overline{V_m} - \left( \overline{V_m} \right)_{\text{past}}}{\Delta T} \quad (4.2.9)$$

$$F/M = \left( \ddot{X}_m^2 + \ddot{Y}_m^2 + \ddot{Z}_m^2 \right)^{1/2} \quad (4.2.10)$$

### Reciprocal Acceleration Filter

$$\begin{aligned} (M/F)_S = & MFK1(M/F)_1 + MFK2(M/F)_2 + MFK3(M/F)_3 \\ & + MFK4(M/F)_4 + MFK5(M/F)_{S1} + MFK6(M/F)_{S2} \\ & + MFK7(M/F)_{S3} + MFK8(M/F)_{S4} \end{aligned} \quad (4.2.11)$$

30------------------------------------------------

### Backup Accelerations

$$M_{cb} = M_j - \dot{M}_k \Delta T \quad (j,k) = (1,1), (2,2), (2,3) \quad (4.2.12)$$

$$(F/M)_c = F_k / M_{cb} \quad k = 1, 2, 3 \quad (4.2.13)$$

### Calculated Velocity Changes Using Backup Acceleration

$$\overline{\Delta V_B} = \begin{bmatrix} \Delta V_{Bx} \\ \Delta V_{By} \\ \Delta V_{Bz} \end{bmatrix} = 20(F/M)_c \Delta T \begin{bmatrix} \cos \theta_y & \cos \theta_z \\ \sin \theta_z & \\ -\sin \theta_y & \cos \theta_z \end{bmatrix} \quad (4.2.14)$$

### Position and Velocity Calculations

$$\left\{ \begin{array}{l} \Delta X_{Sg} = (\dot{X}_{sg} + \frac{1}{2} \ddot{X}_{sg} \Delta T) \Delta T \\ \Delta Y_{Sg} = (\dot{Y}_{sg} + \frac{1}{2} \ddot{Y}_{sg} \Delta T) \Delta T \\ \Delta Z_{Sg} = (\dot{Z}_{sg} + \frac{1}{2} \ddot{Z}_{sg} \Delta T) \Delta T \end{array} \right. \quad (4.3.1)$$

$$\left\{ \begin{array}{l} \Delta X_{mf} = \frac{1}{2} (\dot{X}_m + [\dot{X}_m]_{past}) \Delta T \\ \Delta Y_{mf} = \frac{1}{2} (\dot{Y}_m + [\dot{Y}_m]_{past}) \Delta T \\ \Delta Z_{mf} = \frac{1}{2} (\dot{Z}_m + [\dot{Z}_m]_{past}) \Delta T \end{array} \right. \quad (4.3.2)$$

$$\left\{ \begin{array}{l} X_S = X_S + \Delta X_{Sg} + \Delta X_{mf} + \dot{X}_0 \Delta T \\ Y_S = Y_S + \Delta Y_{Sg} + \Delta Y_{mf} + \dot{Y}_0 \Delta T \\ Z_S = Z_S + \Delta Z_{Sg} + \Delta Z_{mf} + \dot{Z}_0 \Delta T \end{array} \right. \quad (4.3.3)$$

31------------------------------------------------

Position and Velocity Calculations (Continued)

$$\left\{ \begin{array}{l} \dot{x}_{Sg} = \dot{x}_{Sg} + \frac{1}{2} (\ddot{x}_{Sg} + [\ddot{x}_{Sg}]_{past}) \Delta T \\ \dot{y}_{Sg} = \dot{y}_{Sg} + \frac{1}{2} (\ddot{y}_{Sg} + [\ddot{y}_{Sg}]_{past}) \Delta T \\ \dot{z}_{Sg} = \dot{z}_{Sg} + \frac{1}{2} (\ddot{z}_{Sg} + [\ddot{z}_{Sg}]_{past}) \Delta T \end{array} \right. \quad (4.3.4)$$

$$\left\{ \begin{array}{l} \dot{x}_S = \dot{x}_m + \dot{x}_{Sg} + \dot{x}_0 \\ \dot{y}_S = \dot{y}_m + \dot{y}_{Sg} + \dot{y}_0 \\ \dot{z}_S = \dot{z}_m + \dot{z}_{Sg} + \dot{z}_0 \end{array} \right. \quad (4.3.5)$$

$$V = (\dot{x}_S^2 + \dot{y}_S^2 + \dot{z}_S^2)^{1/2} \quad (4.3.5.1) \quad *$$

Gravitational Acceleration \*

$$Y_G = -X_S \sin \phi_L + Y_S \cos \phi_L \sin A_z - Z_S \cos \phi_L \cos A_z \quad (4.3.6)$$

$$R = (X_S^2 + Y_S^2 + Z_S^2)^{1/2} \quad (4.3.7)$$

$$S_{34} = H \left( \frac{a_e}{R} \right) \left( \frac{Y_G}{R} \right) \left[ 3 - 7 \left( \frac{Y_G}{R} \right)^2 \right] + \frac{D}{7} \left( \frac{a_e}{R} \right)^4 \left[ 3 - 42 \left( \frac{Y_G}{R} \right)^2 + 63 \left( \frac{Y_G}{R} \right)^4 \right] \quad (4.3.8)$$

$$P_{34} = \frac{H}{5} \left( \frac{a_e}{R} \right) \left[ 15 \left( \frac{Y_G}{R} \right)^2 - 3 \right] + \frac{D}{7} \left( \frac{a_e}{R} \right)^2 \left( \frac{Y_G}{R} \right) \left[ 12 - 28 \left( \frac{Y_G}{R} \right)^2 \right] \quad (4.3.9)$$

$$S = - \frac{\mu}{R^3} \left\{ 1 + J \left( \frac{a_e}{R} \right)^2 \left[ 1 - 5 \left( \frac{Y_G}{R} \right)^2 \right] + S_{34} \right\} \quad (4.3.10)$$

32------------------------------------------------

Gravitational Acceleration (Continued)

$$P = - \frac{\mu}{R^2} \left( \frac{a_e}{R} \right)^2 \left\{ 2J \left( \frac{Y_G}{R} \right) + P_{34} \right\} \quad (4.3.11)$$

$$\overline{G}_S = S \overline{R}_S + P \begin{bmatrix} -\sin \phi_L \\ \cos \phi_L \sin A_z \\ -\cos \phi_L \cos A_z \end{bmatrix} \quad (4.3.12)$$

Time-tilt Computations

$$T_c = T_{as} - T_l + T_d \quad (4.4.1)$$

$$\chi_y = \begin{cases} \sum_{j=0}^3 F_{1j} (T_c - \Delta T_f)^j & \text{for } T_{S0} \leq T_c - \Delta T_f < T_{S1} \\ \sum_{j=0}^3 F_{2j} (T_c - \Delta T_f)^j & \text{for } T_{S1} \leq T_c - \Delta T_f < T_{S2} \\ \sum_{j=0}^3 F_{3j} (T_c - \Delta T_f)^j & \text{for } T_{S2} \leq T_c - \Delta T_f < T_{ar} \end{cases} \quad (4.4.2)$$

$$\chi_z = \left[ \frac{YAWC(K) - YAWC(K-1)}{TYAW(K) - TYAW(K-1)} \right] (T_c - TYAW(K)) + YAWC(K) \quad (4.4.2.1)$$

for  $\chi_z$  computations  $K = 2, 3, 4 \dots 25$  and  
 $TYAW(K-1) \leq T_c < TYAW(K)$

S-IB Engine Out Freeze Time

$$T_{EO} = T_{as} - T_l \quad (4.4.3)$$

$$\Delta T_f = \begin{cases} B_{11} T_{EO} + B_{12} & T_{EO} < T_{f1} \\ B_{21} T_{EO} + B_{22} & T_{f1} \leq T_{EO} < T_{f2} \\ 0 & T_{f2} \leq T_{EO} \end{cases} \quad (4.4.4)$$

33------------------------------------------------

### S-IB Engine Out Freeze Time (Continued)

$$T_{MEFRZ} = T_c + \Delta T_f \quad (4.4.5)$$
$$T_{ar} = T_{ar} + \Delta T_f - C_{ar1} \quad \text{if } T_{EO} < T_{far} \quad (4.4.6) \quad *$$
$$T_{ar} = T_{ar} + \Delta T_f \quad \text{if } T_{far} \leq T_{EO} \quad *$$

### First Phase Time-to-go Calculation

$$\tau_1 = V_{ex1} \left( \frac{M}{F} \right)_S \quad (4.4.7)$$
$$T_{1i} = T_{1i} - \Delta T \quad (4.4.8)$$

### Second Phase Time-to-go Initialization

$$T_{1i} = 0.0 \quad (4.4.9)$$
$$\alpha_f = \frac{\tau_{10}}{T_{10} + \tau_1} \quad (4.4.10)$$
$$\tau_{3N} = \frac{M_{GR} - \alpha_f \dot{M}_2 T_{10}}{\dot{M}_3} \quad (4.4.11)$$

### Second Phase Artificial $\tau$ Calculations

$$\tau_{3M} = V_{ex3} \left( \frac{M}{F} \right)_S \quad (4.4.12)$$
$$\tau_{3K} = \tau_{3N} - P_C \quad (4.4.13)$$
$$\tau_3 = \tau_{3K} + \left( \tau_{3M} - \tau_{3K} \right) \left( \frac{P_C}{P_{CMR}} \right)^4 \quad (4.4.14) \quad *$$
$$P_C = P_C + \Delta T \quad (4.4.15)$$

34------------------------------------------------

### Second Phase Time-to-go Calculations

$$\tau_3 = V_{ex3} \left( \frac{M}{F} \right)_S \quad (4.4.16)$$
$$T_{3i} = T_{3i} - \Delta T \quad (4.4.17)$$

### Transformation of Position and Velocity to 4-System

$$\overline{R}_4 = [MS4] \overline{R}_S \quad (4.4.18)$$
$$\overline{V}_4 = [MS4] \overline{V}_S \quad (4.4.19)$$

### IGM Intermediate Parameters

$$L_1 = V_{ex1} \ln \left( \frac{\tau_1}{\tau_1 - T_{1i}} \right) \quad (4.4.20)$$
$$J_1 = L_1 \tau_1 - V_{ex1} T_{1i} \quad (4.4.21)$$
$$S_1 = L_1 T_{1i} - J_1 \quad (4.4.22)$$
$$Q_1 = S_1 \tau_1 - \frac{1}{2} V_{ex1} T_{1i}^2 \quad (4.4.23)$$
$$P_1 = J_1 \tau_1 - \frac{1}{2} V_{ex1} T_{1i}^2 \quad (4.4.24)$$
$$U_1 = Q_1 \tau_1 - \frac{1}{6} V_{ex1} T_{1i}^3 \quad (4.4.25)$$
$$L'_3 = V_{ex3} \ln \left( \frac{\tau_3}{\tau_3 - T_{3i}} \right) \quad (4.4.26)$$

35------------------------------------------------

### IGM Intermediate Parameters (Continued)

$$L'_y = L_1 + L'_3 \quad (4.4.27)$$
$$J'_3 = L'_3 \tau_3 - V_{ex3} T_{3i} \quad (4.4.28)$$

### Total Time-to-go Prediction

$$T^* = T_{1i} + T_{3i} \quad (4.4.29)$$

### Range Angle

$$\phi_i = \text{Arctan} \left( \frac{Z_4}{X_4} \right) \quad (4.4.30)$$

### Predicted Terminal Range Angle

$$\delta_2 = V T^* - J'_3 + L'_y T_{3i} - \left( \frac{ROV}{V_{ex3}} \right) \left[ \left( \tau_1 - T_{1i} \right) L_1 + \left( \tau_3 - T_{3i} \right) L'_3 \right] \left[ L'_y + V - V_T \right] \quad (4.4.31)$$
$$\phi_{iT} = \left( \frac{1}{R_T} \right) \left( S_1 + \delta_2 \right) \frac{\cos \Theta_T}{\pi} \quad (4.4.32)$$
$$\phi_T = \phi_i + \phi_{iT} \quad (4.4.33)$$

### Terminal Position, Velocity, and Gravity

$$X_{VT} = R_T \quad (4.4.34)$$
$$\dot{X}_{VT} = V_T \sin \Theta_T \quad (4.4.35)$$

36------------------------------------------------

Terminal Position, Velocity, and Gravity (Continued)

$$\dot{Y}_{VT} = 0 \quad (4.4.36)$$
$$\dot{Z}_{VT} = V_T \cos \theta_T \quad (4.4.37)$$
$$\ddot{X}_{VgT} = G_T \quad (4.4.38)$$
$$\ddot{Y}_{VgT} = 0 \quad (4.4.39)$$
$$\ddot{Z}_{VgT} = 0 \quad (4.4.40)$$
$$\phi_T = \phi_T \quad (4.4.41)$$
$$[MSV] = [M4V][MS4] \quad (4.4.42)$$
$$\overline{R_V} = [M4V] \overline{R_4} \quad (4.4.43)$$
$$\overline{V_V} = [M4V] \overline{V_4} \quad (4.4.44)$$
$$\overline{G_V} = [MSV] \overline{G_S} \quad (4.4.45)$$

Estimated Velocity-to-be-gained

$$\overline{G_V^*} = \frac{1}{2} \left( \overline{G_{TV}} + \overline{G_V} \right) \quad (4.4.46)$$
$$\overline{\Delta V_V'} = \overline{V_{TV}} - \overline{V_V} - T^* \overline{G_V^*} \quad (4.4.47)$$

37------------------------------------------------

Improved Estimate of Total Time-to-go

$$\Delta L_3 = \frac{1}{2} \left[ \frac{(\Delta V'_V)^2}{L'_y} - L'_y \right] \quad (4.4.48)$$
$$L_3 = L'_3 + \Delta L_3 \quad (4.4.49)$$
$$\Delta T'_3 = \Delta L_3 \left( \frac{\tau_3 - T_{3i}}{V_{ex3}} \right) \quad (4.4.50)$$

\*

$$T_{3i} = T_{3i} + \Delta T'_3 \quad (4.4.52)$$
$$T^* = T_{1i} + T_{3i} \quad (4.4.53)$$

Improved Estimate of Velocity-to-be-gained

$$\overline{\Delta V}_V = \overline{\Delta V}'_V - \Delta T'_3 \overline{G}^*_V \quad (4.4.54)$$

Preliminary Guidance Commands Satisfying Velocity Constraint

$$\tilde{\chi}_y = \text{Arctan} \left( \frac{\Delta \dot{\chi}_V}{\Delta \dot{z}_V} \right) \quad (4.4.55)$$
$$\tilde{\chi}_z = \text{Arctan} \left[ \frac{\Delta \dot{\chi}_V}{\left( \Delta \dot{\chi}_V^2 + \Delta \dot{z}_V^2 \right)^{1/2}} \right] \quad (4.4.56)$$

38------------------------------------------------

Position Correction Terms

$$J_3 = J'_3 + T_{3i} \Delta L_3 \quad (4.4.51) \quad *$$

$$S_3 = L_3 T_{3i} - J_3 \quad (4.4.57)$$

$$Q_3 = S_3 \tau_3 - \frac{1}{2} V_{ex3} T_{3i}^2 \quad (4.4.58)$$

$$L_y = L_1 + L_3 \quad (4.4.59)$$

$$J_y = J_1 + J_3 + L_3 T_{1i} \quad (4.4.60)$$

$$S_y = S_1 - J_3 + L_y T_{3i} \quad (4.4.61)$$

$$Q_y = Q_1 + Q_3 + S_3 T_{1i} + J_1 T_{3i} \quad (4.4.62) \quad *$$

$$K_y = \frac{L_y}{J_y} \quad (4.4.63)$$

$$D_y = S_y - K_y Q_y \quad (4.4.64)$$

$$\Delta Y_V = Y_V + \dot{Y}_V T^* + \frac{1}{2} \ddot{Y}_V^{*2} T^{*2} + S_y \sin \tilde{\chi}_z \quad (4.4.65)$$

$$K_3 = \frac{\Delta Y_V}{D_y \cos \tilde{\chi}_z} \quad (4.4.66)$$

$$K_4 = K_y K_3 \quad (4.4.67)$$

$$P_3 = J_3 \left( \tau_3 + 2T_{1i} \right) - \frac{1}{2} V_{ex3} T_{3i}^2 \quad (4.4.68)$$

$$U_3 = Q_3 \left( \tau_3 + 2T_{1i} \right) - \frac{1}{6} V_{ex3} T_{3i}^3 \quad (4.4.69)$$

$$L_p = L_y \cos \tilde{\chi}_z \quad (4.4.70)$$

39------------------------------------------------

Position Correction Terms (Continued)

$$C_2 = \cos \tilde{\chi}_z + K_3 \sin \tilde{\chi}_z \quad (4.4.71)$$
$$C_4 = K_4 \sin \tilde{\chi}_z \quad (4.4.72)$$
$$J_p = J_y C_2 - C_4 \left( P_1 + P_3 + L_3 T_{1i}^2 \right) \quad (4.4.73)$$
$$S_p = S_y C_2 - Q_y C_4 \quad (4.4.74)$$
$$Q_p = Q_y C_2 - C_4 \left( U_1 + U_3 + S_3 T_{1i}^2 + T_{3i} P_1 \right) \quad (4.4.75)$$
$$K_p = \frac{L_p}{J_p} \quad (4.4.76)$$
$$D_p = S_p - K_p Q_p \quad (4.4.77)$$
$$\Delta X_V = X_V - X_{VT} + \dot{X}_V T^* + \frac{1}{2} \ddot{X}_{Vg}^* T^{*2} + S_p \sin \tilde{\chi}_y \quad (4.4.78)$$
$$K_1 = \frac{\Delta X_V}{D_p \cos \tilde{\chi}_y} \quad (4.4.79)$$
$$K_2 = K_p K_1 \quad (4.4.80)$$

Corrected Guidance Commands in 4-System Satisfying  
Position Constraint

$$X_{y4} = \tilde{X}_y - \frac{1}{\pi} \left( K_1 - K_2 \Delta T_N \right) - \phi_T - \frac{1}{2} \quad (4.4.81)$$
$$X_{z4} = \tilde{X}_z - \frac{1}{\pi} \left( K_3 - K_4 \Delta T_N \right) \quad (4.4.82)$$

40------------------------------------------------

Steering Misalignment Correction Terms

$$SMCY = SMCY + SMCG \left\{ \frac{\ddot{z}_m + \ddot{x}_m \tan \left[ \frac{1}{2} \left( \chi_y + [\chi_y]_{\text{past}} \right) - SMCY \right]}{\ddot{x}_m - \ddot{z}_m \tan \left[ \frac{1}{2} \left( \chi_y + [\chi_y]_{\text{past}} \right) - SMCY \right]} \right\} \frac{\Delta T}{\pi} \quad (4.4.83)$$

$$SMCZ = SMCZ + SMCG \left\{ \frac{\sin \left[ \frac{1}{2} \left( \chi_z + [\chi_z]_{\text{past}} \right) - SMCZ \right] - \left( \frac{M}{F} \right) \ddot{y}_m}{\cos \left[ \frac{1}{2} \left( \chi_z + [\chi_z]_{\text{past}} \right) - SMCZ \right]} \right\} \frac{\Delta T}{\pi} \quad (4.4.84)$$

Thrust Vector Transformation

$$\overline{F}'_S = [MS4]^T \begin{bmatrix} \cos \chi_{y_4} & \cos \chi_{z_4} \\ & \sin \chi_{z_4} \\ -\sin \chi_{y_4} & \cos \chi_{z_4} \end{bmatrix} \quad (4.4.85)$$

Final Guidance Commands in S-System Corrected for Thrust Misalignment

$$\chi_y = \text{Arctan} \left( \frac{-F'_S z}{F'_S x} \right) + SMCY \quad (4.4.86)$$

$$\chi_z = \text{Arctan} \left[ \frac{F'_S y}{(1 - F'^2_{Sy})^{1/2}} \right] + SMCZ \quad (4.4.87)$$

High Speed Loop for S-IVB Cutoff

$$\Delta T_b = \Delta V_b \Delta T \left( \frac{1}{V-V'} \right) \quad (4.4.88)$$

$$T_{3i} = T_{3i} - \Delta T_b \quad (4.4.89)$$

41------------------------------------------------

High Speed Loop for S-IVB Cutoff (Continued)

$$T_{3i} = T_{3i} - \Delta T \quad (4.4.90)$$
$$A'' = \frac{[(V-V') \frac{\Delta T'}{\Delta T} - (V'-V'') \frac{\Delta T}{\Delta T}]}{(\Delta T' + \Delta T)} \quad (4.4.91)$$
$$A' = A'' \Delta T + \left( \frac{V-V'}{\Delta T} \right) \quad (4.4.92)$$
$$T_{3i} = \frac{V_T - \Delta V_b - V}{A' + A'' T_{3i}} \quad (4.4.93) \quad *$$
$$T_{CO} = T_{3i} + T_{as} \quad (4.4.94)$$

42------------------------------------------------

### Position for Orbital Guidance

$$\overline{R}_{OG} = (\overline{R})_t + T_{SON}(\overline{V})_t \quad (5.4.1)$$

### Integration Scheme

$$\left\{ \begin{array}{l} (\overline{R})_{t-4} = (\overline{R})_{t-8} + 4(\overline{V})_{t-8} + 8(\overline{A})_{t-8} \\ (\overline{V})_{t-4} = (\overline{V})_{t-8} + 4(\overline{A})_{t-8} \end{array} \right. \quad (5.4.2)$$

$$(\overline{A})_{t-4} = (\overline{G})_{t-4} + (\overline{A}_D)_{t-4} \quad (5.4.3)$$

$$\left\{ \begin{array}{l} (\overline{R}_p)_t = (\overline{R})_{t-8} + 8(\overline{V})_{t-8} + 32(\overline{A})_{t-4} \\ (\overline{V}_p)_t = (\overline{V})_{t-4} + 8(\overline{A})_{t-4} \end{array} \right. \quad (5.4.4)$$

$$(\overline{A}_p)_t = (\overline{G}_p)_t + (\overline{A}_{Dp})_t \quad (5.4.5)$$

$$\left\{ \begin{array}{l} (\overline{\Delta R})_t = 8(\overline{V})_{t-8} + \frac{32}{3} [(\overline{A})_{t-8} + 2(\overline{A})_{t-4}] \\ \quad + (\overline{\Delta R}_{SAV})_{t-8} + 8(\overline{\Delta V}_{SAV})_{t-8} \\ (\overline{R})_t = (\overline{R})_{t-8} + (\overline{\Delta R})_t \\ (\overline{\Delta V})_t = \frac{4}{3} [(\overline{A})_{t-8} + 4(\overline{A})_{t-4} + (\overline{A}_p)_t] + (\overline{\Delta V}_{SAV})_{t-8} \\ (\overline{V})_t = (\overline{V})_{t-8} + (\overline{\Delta V})_t \end{array} \right. \quad (5.4.6)$$

$$(\overline{A})_t = (\overline{G})_t + (\overline{A}_D)_t \quad (5.4.7)$$

### Drag Acceleration

$$h = R - a_e + (a_e - b) \left( \frac{Y_G}{R} \right)^2 \quad (5.4.8)$$

43------------------------------------------------

Drag Acceleration (Continued)

$$\rho = \begin{cases} 0 & \text{for } h > h_2 \\ \rho_0 + \rho_1 h + \rho_2 h^2 + \rho_3 h^3 + \rho_4 h^4 + \rho_5 h^5 & \text{for } h_2 \geq h > h_1 \\ \rho_c & \text{for } h_1 > h \end{cases} \quad (5.4.9)$$

$$\begin{cases} \overline{V}_r = \overline{V}_S - \overline{\omega}_{ES} \times \overline{R}_S \\ V_r = (V_{rx}^2 + V_{ry}^2 + V_{rz}^2)^{1/2} \end{cases} \quad (5.4.10)$$

$$\cos \alpha = \frac{1}{V_r} [V_{rx} \cos \theta_y \cos \theta_z + V_{ry} \sin \theta_z - V_{rz} \sin \theta_y \cos \theta_z] \quad (5.4.11)$$

$$C_D = \sum_{j=1}^5 C_{Dj} (\cos \alpha)^{j-1} \quad (5.4.12)$$

$$\overline{A}_{DS} = - \rho C_D K_D V_r \overline{V}_r \quad (5.4.13)$$

Inertial Guidance Commands

$$\begin{cases} \chi_x = \text{ROLLA} \\ \chi_y = \text{SPITCH} \\ \chi_z = \text{SYAW} \end{cases} \quad (5.5.1)$$

Track Local Reference Guidance Commands

$$\overline{R}_4 = [\text{MS4}] \overline{R}_S \quad (5.5.2)$$

$$R_4 = (X_4^2 + Z_4^2)^{1/2} \quad (5.5.3)$$

44------------------------------------------------

Track Local Reference Guidance Commands (Continued)

$$\begin{cases} \sin \chi_{y_4} = \text{SPITCH} \begin{pmatrix} -Z_4 \\ R_4 \end{pmatrix} + \text{CPITCH} \begin{pmatrix} -X_4 \\ R_4 \end{pmatrix} \\ \cos \chi_{y_4} = \text{CPITCH} \begin{pmatrix} -Z_4 \\ R_4 \end{pmatrix} - \text{SPITCH} \begin{pmatrix} -X_4 \\ R_4 \end{pmatrix} \end{cases} \quad (5.5.4)$$

$$\begin{cases} \sin \chi_{z_4} = \text{SYAW} \\ \cos \chi_{z_4} = \text{CYAW} \end{cases} \quad (5.5.5)$$

$$\chi_x = \text{ROLLA} \quad (5.5.6)$$

Inertial Hold Guidance Commands (upon return of control from S/C to IU)

$$\begin{cases} \chi_x = \theta'_x \\ \chi'_x = \theta'_x \\ \chi_y = \theta'_y \\ \chi'_y = \theta'_y \\ \chi_z = \theta'_z \\ \chi'_z = \theta'_z \end{cases} \quad (5.5.7)$$

Track Local Horizontal Guidance Commands (upon return of control from S/C to IU)

$$\chi_x = \theta'_x \quad (5.5.8)$$

$$\overline{P}_S = \begin{bmatrix} \cos \theta'_y \cos \theta'_z \\ \sin \theta'_z \\ -\sin \theta'_y \cos \theta'_z \end{bmatrix} \quad (5.5.9)$$

45------------------------------------------------

Track Local Horizontal Guidance Commands (upon return of control from S/C to IU)

$$\overline{P}_4 = \begin{bmatrix} P_{4x} \\ P_{4y} \\ P_{4z} \end{bmatrix} = [MS4] \overline{P}_S \quad (5.5.10)$$

$$\begin{cases} \sin \chi_{z_4} = P_{4y} \\ \cos \chi_{z_4} = (1 - \sin^2 \chi_{z_4})^{1/2} \end{cases} \quad (5.5.11)$$

$$\begin{cases} \phi_{SO} = \text{Arctan} \left( \frac{Z_4}{X_4} \right) \\ \phi_P = \text{Arctan} \left( \frac{-P_{4z}}{P_{4x}} \right) \\ \alpha_{LH} = \phi_{SO} + \phi_P + 0.5 \end{cases} \quad (5.5.12)$$

\*

$$\begin{cases} \text{SPITCH} = \sin \alpha_{LH} \\ \text{CPITCH} = \cos \alpha_{LH} \\ \text{SYAW} = \sin \chi_{z_4} \\ \text{CYAW} = \cos \chi_{z_4} \\ \text{ROLLA} = \Theta'_x \end{cases} \quad (5.5.13)$$

Telemetry Station Acquisition Test

$$\Theta_R = \omega_E T_{as} \quad (5.6.1)$$

46------------------------------------------------

Telemetry Station Acquisition Test (Continued)

$$\begin{bmatrix} X_A \\ Y_A \\ Z_A \end{bmatrix} = [MSA] \begin{bmatrix} X_S \\ Y_S \\ Z_S \end{bmatrix} \quad (5.6.2)$$
$$\overline{R}_A = \begin{bmatrix} X_A \\ Y_A \\ Z_A \end{bmatrix} \quad (5.6.3)$$
$$\overline{C}_A(i) = \begin{bmatrix} C_{Ax}(i) \\ C_{Ay}(i) \\ C_{Az}(i) \end{bmatrix} \quad (5.6.4)$$
$$d_A(i) = \overline{R}_A \cdot \overline{C}_A(i) - R_{STA} \quad (5.6.5)$$

47------------------------------------------------

(THIS PAGE HAS BEEN LEFT BLANK INTENTIONALLY.)

48------------------------------------------------

### Attitude Error Commands

$$\begin{cases} \chi'_x &= \chi_x + \Delta\chi'_x \\ \chi'_y &= \chi_y + \Delta\chi'_y \\ \chi'_z &= \chi_z + \Delta\chi'_z \end{cases} \quad (8.2.1)$$
$$\begin{cases} \Delta Z &= \theta'_z - \chi'_z \\ \text{YAW} &= A_5 \Delta Z - A_4 \Delta Y \\ \Delta Y &= \theta'_y - \chi'_y \\ \text{PITCH} &= A_1 \Delta Y + A_2 \Delta Z \\ \Delta X &= \theta'_x - \chi'_x \\ \text{ROLL} &= A_6 \Delta X + A_3 \Delta Y \end{cases} \quad (8.2.2)$$

### Attitude Increments

$$\begin{cases} \Delta\chi'_x &= (\chi_x - \chi'_x) / [(MLR)(\Delta T_N)] \\ \Delta\chi'_y &= (\chi_y - \chi'_y) / [(MLR)(\Delta T_N)] \\ \Delta\chi'_z &= (\chi_z - \chi'_z) / [(MLR)(\Delta T_N)] \end{cases} \quad (8.3.1)$$

### Gimbal-to-Body Transformation

$$\begin{cases} \chi'_{xp} &= \chi'_x + \Delta\chi'_x [(MLR)(\Delta T_N)] \\ \chi'_{zp} &= \chi'_z + \Delta\chi'_z [(MLR)(\Delta T_N)] \end{cases} \quad (8.3.2)$$
$$\theta_{xa} = \frac{1}{2} (\chi'_{xp} + \theta'_x) \quad (8.3.3)$$
$$\theta_{za} = \frac{1}{2} (\chi'_{zp} + \theta'_z) \quad (8.3.4)$$

49------------------------------------------------

$$A_1 = SF \cos(\theta_{xa}) \cos(\theta_{za}) \quad (8.3.5)$$
$$A_2 = SF \sin(\theta_{xa}) \quad (8.3.6)$$
$$A_3 = SF \sin(\theta_{za}) \quad (8.3.7)$$
$$A_4 = SF \sin(\theta_{xa}) \cos(\theta_{za}) \quad (8.3.8)$$
$$A_5 = SF \cos(\theta_{xa}) \quad (8.3.9)$$
$$A_6 = SF \quad (8.3.10)$$

50------------------------------------------------

### Computed Acceleration Profile

$$\begin{bmatrix} \ddot{X}_S \\ \ddot{Y}_S \\ \ddot{Z}_S \end{bmatrix} = [MBS] \begin{bmatrix} \left(\frac{F}{M}\right)_c \\ 0 \\ 0 \end{bmatrix} \quad (12.2.1)$$

### Computed Velocity Changes

$$\left\{ \begin{array}{l} \dot{X}_S = \frac{1}{2} (\ddot{X}_S + [\ddot{X}_S]_{\text{past}}) \Delta T \\ \dot{Y}_S = \frac{1}{2} (\ddot{Y}_S + [\ddot{Y}_S]_{\text{past}}) \Delta T \\ \dot{Z}_S = \frac{1}{2} (\ddot{Z}_S + [\ddot{Z}_S]_{\text{past}}) \Delta T \end{array} \right. \quad (12.2.2)$$

### Velocity Increments

$$\left\{ \begin{array}{l} \Delta V_{gx} = 20G_L [\sin^2 \phi_L + \cos \omega_{E\text{as}} T_{\text{as}} \cos^2 \phi_L] \Delta T \\ \Delta V_{gy} = 20G_L \cos \phi_L [\cos A_z \sin \omega_{E\text{as}} T_{\text{as}} \\ \quad - (1 - \cos \omega_{E\text{as}} T_{\text{as}}) \sin A_z \sin \phi_L] \Delta T \\ \Delta V_{gz} = 20G_L \cos \phi_L [\sin A_z \sin \omega_{E\text{as}} T_{\text{as}} \\ \quad + (1 - \cos \omega_{E\text{as}} T_{\text{as}}) \cos A_z \sin \phi_L] \Delta T \end{array} \right. \quad (12.2.3)$$

$$\left\{ \begin{array}{l} \Delta V_x = \Delta V_{cx} + \Delta V_{fx} - \Delta V_{gx} \\ \Delta V_y = \Delta V_{cy} + \Delta V_{fy} - \Delta V_{gy} \\ \Delta V_z = \Delta V_{cz} + \Delta V_{fz} - \Delta V_{gz} \end{array} \right. \quad (12.2.4)$$

51------------------------------------------------

### Expected Gimbal Angles

$$\left\{ \begin{aligned} \dot{\Theta}_{xe} &= \omega_E [\sin \phi_L + \cos \phi_L \tan \Theta_{ze} \sin(\Theta_{xe} + A_z)] \\ \dot{\Theta}_{ye} &= -\omega_E \cos \phi_L \sin(\Theta_{xe} + A_z) \left( \frac{1}{\cos \Theta_{ze}} \right) \\ \dot{\Theta}_{ze} &= \omega_E \cos \phi_L \cos(\Theta_{xe} + A_z) \end{aligned} \right. \quad (12.2.5) \quad *$$

$$\left\{ \begin{aligned} \Delta \Theta_{xe} &= \frac{1}{2} (\dot{\Theta}_{xe} + [\dot{\Theta}_{xe}]_{\text{past}}) \Delta T \\ \Delta \Theta_{ye} &= \frac{1}{2} (\dot{\Theta}_{ye} + [\dot{\Theta}_{ye}]_{\text{past}}) \Delta T \\ \Delta \Theta_{ze} &= \frac{1}{2} (\dot{\Theta}_{ze} + [\dot{\Theta}_{ze}]_{\text{past}}) \Delta T \end{aligned} \right. \quad (12.2.6)$$

$$\left\{ \begin{aligned} \Theta_{xe} &= \Theta_{xe} + \Delta \Theta_{xe} \\ \Theta_{ye} &= \Theta_{ye} + \Delta \Theta_{ye} \\ \Theta_{ze} &= \Theta_{ze} + \Delta \Theta_{ze} \end{aligned} \right. \quad (12.2.7)$$

### Attitude Errors

$$\left\{ \begin{aligned} X''_x &= X''_x + \Delta \Theta_{xe} \\ X''_y &= X''_y + \Delta \Theta_{ye} \\ X''_z &= X''_z + \Delta \Theta_{ze} \end{aligned} \right. \quad (12.2.8)$$

$$\left\{ \begin{aligned} \Delta X &= \Theta_x - X''_x \\ \Delta Y &= \Theta_y - X''_y \\ \Delta Z &= \Theta_z - X''_z \end{aligned} \right. \quad (12.2.9)$$

52------------------------------------------------

### Sine-Cosine Algorithm

$$\sin \alpha = 3.1415897\alpha - 5.1673678\alpha^3 + 2.5436052\alpha^5 - 0.55839693\alpha^7 \quad (13.2.1)$$

### Arctangent Algorithm

$$\begin{aligned} \text{Arctan } V = & 0.31830264V - 0.10587734V^3 \\ & + 0.06160678V^5 - 0.037061733V^7 \\ & + 0.016760072V^9 - 0.00373097V^{11} \end{aligned} \quad (13.3.1)$$

### Natural Logarithm Algorithm

$$\begin{aligned} \ln \alpha' = & 0.99990167(\alpha' - 1.0) - 0.49787544(\alpha' - 1.0)^2 \\ & + 0.31765005(\alpha' - 1.0)^3 - 0.19376149(\alpha' - 1.0)^4 \\ & + 0.08556927(\alpha' - 1.0)^5 - 0.01833831(\alpha' - 1.0)^6 \end{aligned} \quad (13.4.1)$$
$$\begin{aligned} \ln \alpha = & \ln(2.0^N \alpha') = N \ln 2.0 + \ln \alpha' = \\ & 0.6931471806 N + \ln \alpha' \end{aligned} \quad (13.4.2)$$

### Square Root Algorithm

$$x_{i+1} = 0.5(x_i + \frac{X}{x_i}), \quad i = 0, 1, 2, 3, \dots \quad (13.5.1)$$

### Inverse Square Root Algorithm

$$x_{i+1} = 0.5x_i(3.0 - x_i^2 X), \quad i = 0, 1, 2, 3, \dots \quad (13.6.1)$$

### Vector Dot Product

$$\bar{A} \cdot \bar{B} = A_x B_x + A_y B_y + A_z B_z \quad (13.7.1)$$

53------------------------------------------------

Vector Cross Product

$$\bar{C} = \begin{bmatrix} C_x \\ C_y \\ C_z \end{bmatrix} = \begin{bmatrix} A_y B_z - A_z B_y \\ A_z B_x - A_x B_z \\ A_x B_y - A_y B_x \end{bmatrix} = \bar{A} \times \bar{B}$$

(13.8.1)