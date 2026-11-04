## SECTION 10

### DIGITAL COMMAND SYSTEM

#### 10.1 INTRODUCTION

The digital command system (DCS) provides a communication link between the Goddard Network ground stations and the LVDC flight program. The DCS provides a limited real time means of controlling specific flight program timing, navigation, guidance, targeting, sequencing, switch selector functions and obtaining specific program information. \*

#### 10.2 DCS WORD FORMAT

The format of the 35-bit DCS word transmitted from the Goddard Network stations to the command decoder is shown in Table 10-1. Of these 35 input bits only the 2 interrupt, 2 orbital mode/data (OM/D), and 14 information bits will be used by the LVDA/LVDC. The 2 interrupt bits are logically ANDed in the LVDA and the result is presented to the LVDC as the command decoder interrupt (INT8). The 2 OM/D bits are also ANDed and the result is presented to the LVDC as DI2. The 14 information bits make up the flight program DCS command. These bits are stored in the command decoder register and will be presented to the LVDC in the high order 14 bits (S-13) of the accumulator upon request from the flight program. \*

#### 10.3 DCS COMMAND VERIFICATION

Upon detection of the command decoder interrupt (INT8), the flight program must determine the status of the OM/D bit. If this bit is 1, a DCS mode command has been received and must be verified. If this bit is 0, a DCS data command has been received and must be verified.

1------------------------------------------------

TABLE 10-1 DCS WORD FORMAT

<table border="1">
<thead>
<tr>
<th>Command Decoder Bits</th>
<th>LVDC Accumulator Bits</th>
<th>Bit Significance</th>
</tr>
</thead>
<tbody>
<tr>
<td>1-3</td>
<td></td>
<td>Vehicle address</td>
</tr>
<tr>
<td>4-6</td>
<td></td>
<td>Decoder address</td>
</tr>
<tr>
<td>7*</td>
<td>Sign</td>
<td>Information bit 14</td>
</tr>
<tr>
<td>8*</td>
<td></td>
<td>Command decoder interrupt A</td>
</tr>
<tr>
<td>9*</td>
<td></td>
<td>Command decoder interrupt B</td>
</tr>
<tr>
<td>10*</td>
<td></td>
<td>OM/D bit A</td>
</tr>
<tr>
<td>11</td>
<td></td>
<td>Decoder address</td>
</tr>
<tr>
<td>12*</td>
<td></td>
<td>OM/D bit B</td>
</tr>
<tr>
<td>13-14*</td>
<td>1-2</td>
<td>Information bits 13-12</td>
</tr>
<tr>
<td>15-18</td>
<td></td>
<td>Decoder address</td>
</tr>
<tr>
<td>19-24*</td>
<td>3-8</td>
<td>Information bits 11-6</td>
</tr>
<tr>
<td>25</td>
<td></td>
<td>Decoder address</td>
</tr>
<tr>
<td>26-30*</td>
<td>9-13</td>
<td>Information bit 5-1</td>
</tr>
<tr>
<td>31-35</td>
<td></td>
<td>Decoder address</td>
</tr>
</tbody>
</table>

\*These are the only bits of interest to the LVDA/LVDC

---

The flight program must read the contents of the command decoder register and make several tests on the DCS command (mode or data) before it will be accepted for use. Tests are required in order to determine that the orbital mode/data (OM/D) bit (DI2) is correct, the DCS command sequence bit is correct, the true and complement bits are consistent, and that the command is executable.

To insure that no commands are erroneously rejected, the command decoder must be reset and the DCS error counter must be zeroed at T4+0.

2------------------------------------------------

### 10.3.1 DCS Mode Command Verification

When a mode command is received, the following tests must be conducted, in the order listed, before the command will be accepted.

1. 1. True-complement test: The 14 information bits transferred from the command decoder to the LVDC accumulator contain redundant information. LVDC bits S through 6 represent the command in true form. LVDC bits 7 through 13 must be the 1's complement of bits S-6. If they are not, the command must not be accepted and DCS error code 10 must be issued.
2. 2. Sequence bit test: LVDC bit 6 is designated as the sequence bit. This bit must be 0 for DCS mode commands. If this bit is 1, the command must not be accepted and DCS error code 24 must be issued.
3. 3. Terminate command test: If the DCS mode command is the terminate command, then the mode expected test, DCS in progress test, mission acceptance test, and time acceptance test are omitted. The terminate command must be accepted whenever any other DCS command is acceptable.
4. 4. Mode expected test: If a DCS mode command is received when a data command is required to complete the requirements of a previous mode command, the mode command must be rejected and DCS error code 20 must be telemetered.

3------------------------------------------------

1. 5. DCS in progress test: If another DCS routine is presently being processed (i.e., performing a memory dump), then the present mode command must not be accepted and DCS error code 64 must be issued. \*
2. 6. Mission acceptance test: If the DCS mode command is not defined for this mission, it must not be accepted and DCS error code 14 must be issued. \*
3. 7. Time acceptance test: If the DCS mode command is not acceptable at the time it is received, it must be rejected and DCS error code 74 must be issued.

If all these conditions are satisfied, the flight program must: issue Discrete Output 1, which generates a Computer Reset Pulse (CRP); telemeter the proper mode status word twice; and either prepare to receive DCS data commands or perform the commanded function, if no data is required. The format for the mode status word is defined in Table 10-2.

TABLE 10-2 MODE STATUS WORD FORMAT

<table border="1">
<thead>
<tr>
<th>LVDC Bit Position</th>
<th>S-5</th>
<th>6-25 (Octal)</th>
</tr>
</thead>
<tbody>
<tr>
<td>Data</td>
<td>DCS mode command information bits 14-9</td>
<td>0000000</td>
</tr>
</tbody>
</table>

If any test fails, the program must telemeter the appropriate DCS error message twice, monitor the number of consecutive verification failures, and ignore the mode command. No CRP will be issued. The absence of the CRP indicates to the ground station that the command is not accepted by the program. If seven consecutive verification failures occur, an automatic program initiated terminate must be performed, as defined in Section 10.4.5. \*

4------------------------------------------------

### 10.3.2 DCS Data Command Verification

The program will accept DCS data commands only after acceptance of a DCS mode command which requires data. When a data command is received, the following tests must be made, in the order listed, before the command is accepted.

1. 1. Data legal test: If a data command is not expected (the data requirements for the previous mode command have been met), the data command must not be accepted and DCS error code 04 must be issued.
2. 2. True-complement test: As with DCS mode commands, LVDC bits 7-13 must be the 1's complement of bits S-6. If this condition does not exist, the command must not be accepted and DCS error code 44 must be issued.
3. 3. Sequence bit test: For DCS data commands, the sequence bit (LVDC bit 6) must be 1 for the odd-numbered data commands of a set and 0 for the even-numbered data commands. (The sequence bit must alternate for successive data commands in a set required by a DCS mode command.) If the sequence bit is incorrect, the DCS data command must not be accepted and DCS error code 60 must be issued.

If all these conditions are satisfied, the program must telemeter the data status word twice, issue Discrete Output 1, and either prepare to receive another data command or if no more data is required, perform the commanded function. The format for the data status word is defined in Table 10-3.

5------------------------------------------------

TABLE 10-3 DATA STATUS WORD FORMAT

<table border="1">
<tr>
<td>LVDC Bit Position</td>
<td>S-13</td>
<td>14-25 (Octal)</td>
</tr>
<tr>
<td>Data</td>
<td>DCS data command information bits 14-1</td>
<td>17776</td>
</tr>
</table>

If any test fails, the program must telemeter the appropriate DCS error message twice, monitor the number of consecutive verification failures, and ignore the data command. No CRP will be issued, indicating that the program does not accept the command. If seven consecutive verification failures occur, an automatic program initiated terminate must be performed, as defined in Section 10.4.5.

\*  
\*

10.3.3 DCS Data Validation

The data for some mode commands require further testing. This testing is done after all the data have been received and formatted.

- • Illegal memory dump test: The start address and end address requested by a memory dump command must be located in an existing memory module and the start module number must be less than or equal to the end module number. If the module numbers are equal, the start sector number must be less than or equal to the end sector number. If the sector numbers are equal, the start address must be less than or equal to the end address. If any of these conditions are not met, the DCS command must not be accepted and DCS error code 50 must be issued.
- • Valid time test: The implementation time of a navigation update command must be at least 10 seconds in the future after all the data has been received and formatted. If this condition is not met, the DCS command must not be accepted and DCS error code 54 must be issued.

\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*  
\*<

6------------------------------------------------

If either of the above tests is failed, the program must telemeter the appropriate DCS error message twice, and perform an automatic program initiated terminate, as defined in Section 10.4.5.\*

#### 10.3.4 DCS Error Message

Each time the program rejects a DCS command, an error message must be telemetered. The DCS error message must contain the DCS error code corresponding to the failure, the number of consecutive verification failures, and the 14 information bits. The general format for the DCS error message is defined in Table 10-4. If a DCS error is of the nature that all the data must be received and formatted before the error can be detected, the telemetered error message must not contain a failure count or command information bits. The DCS error codes are defined in Table 10-5. DCS error messages must be telemetered twice at the maximum LVDC telemetry rate.

TABLE 10-4 DCS ERROR MESSAGE FORMAT

<table border="1">
<thead>
<tr>
<th>LVDC Bit Position</th>
<th>S-5</th>
<th>6-8</th>
<th>9-11</th>
<th>12-25</th>
</tr>
</thead>
<tbody>
<tr>
<td>Data</td>
<td>DCS error code</td>
<td>Spare</td>
<td>Number of failures</td>
<td>DCS command 14 info bits</td>
</tr>
</tbody>
</table>

#### 10.4 DCS COMMANDS

The flight program must accept and process the following DCS commands:

- • Time base update
- • Navigation update
- • Generalized switch selector
- • Memory Dump

7------------------------------------------------

TABLE 10-5 DCS ERROR CODES

<table border="1">
<thead>
<tr>
<th>Error Code No. (Octal)</th>
<th>DCS Error Message Bits S-5</th>
<th>Description</th>
</tr>
</thead>
<tbody>
<tr>
<td>04</td>
<td>000100</td>
<td>Orbital Mode/Data bit is invalid; data command was received when a mode command was expected</td>
</tr>
<tr>
<td>10</td>
<td>001000</td>
<td>True complement test failed for mode command; information bits 7-1 are not the complement of bits 14-8</td>
</tr>
<tr>
<td>14</td>
<td>001100</td>
<td>Mode command invalid; the mode command received is not defined for this mission</td>
</tr>
<tr>
<td>20</td>
<td>010000</td>
<td>Orbital Mode/Data bit is invalid; mode command was received when expecting a data command</td>
</tr>
<tr>
<td>24</td>
<td>010100</td>
<td>Mode command sequence bit incorrect; the sequence bit received was 1 instead of 0</td>
</tr>
<tr>
<td>34</td>
<td>011100</td>
<td>Unable to issue generalized switch selector function at this time, the last requested generalized switch selector function has not been issued</td>
</tr>
<tr>
<td>44</td>
<td>100100</td>
<td>True complement test failed for data command; information bits 7-1 are not the complement of bits 14-8</td>
</tr>
<tr>
<td>50</td>
<td>101000</td>
<td>The start module, sector, and address requested by the memory dump command is greater than the end module, sector, and address; or one or more locations requested by the memory dump command are in a non-existing module</td>
</tr>
<tr>
<td>54</td>
<td>101100</td>
<td>The time of implementation of a navigation update, execute generalized maneuver, execute maneuver, or return to nominal timeline, is less than 10 sec in the future</td>
</tr>
<tr>
<td>60</td>
<td>110000</td>
<td>Data command sequence bit incorrect; the sequence bit must begin with 1 and alternate from 1 to 0 in each sequential data command of a set</td>
</tr>
<tr>
<td>64</td>
<td>110100</td>
<td>A DCS program is in progress at this time; however, no more data is required; only a terminate mode command can be processed at this time</td>
</tr>
<tr>
<td>74</td>
<td>111100</td>
<td>The mode command received is defined for this mission but is not acceptable in the present time frame</td>
</tr>
</tbody>
</table>

\*  
\*  
\*  
\*  
\*

8------------------------------------------------

- • Terminate
- • ECS water control valve logic inhibit
- • Execute generalized maneuver
- • Return to nominal timeline
- • Execute alternate sequence \*
- • Execute special maneuvers \*
- • Targeting load.

Table 10-6 lists the current DCS mode command assignments, the LVDC 14-bit position format, and the DCS data command requirements. Some of these DCS mode commands may not be required for every mission. The required DCS mode commands and their acceptance times for a particular mission, and any additional commands required for that mission only, will be defined in the Digital Command System commands table in the individual mission requirements, Part II.

#### 10.4.1 Time Base Update

Upon acceptance of this DCS mode command, the flight program must increment or decrement the time in the current nominal switch selector sequence reference by an amount specified by an accompanying DCS data command.

The format for the DCS data command information bits is shown in Table 10-7. A 1 in the LVDC sign position signifies a decrement to the time and a 0 signifies an increment. The magnitude of the update will be contained in LVDC bits 1-5.

9------------------------------------------------

TABLE 10-6 DCS MODE COMMANDS

<table border="1">
<thead>
<tr>
<th rowspan="2">Octal Rep. of LVDC Bit Position S-5</th>
<th colspan="2">Binary Rep. of LVDC Bit Position</th>
<th rowspan="2">Definition (General)</th>
<th rowspan="2">Data Words Req'd.</th>
</tr>
<tr>
<th>S-6</th>
<th>7-13</th>
</tr>
</thead>
<tbody>
<tr>
<td>10</td>
<td>0010000</td>
<td rowspan="12" style="writing-mode: vertical-rl; transform: rotate(180deg);">1's complement of LVDC bits S-6</td>
<td>Time base update</td>
<td>1</td>
</tr>
<tr>
<td>11</td>
<td>0010010</td>
<td>Navigation update</td>
<td>35</td>
</tr>
<tr>
<td>12</td>
<td>0010100</td>
<td>Generalized switch selector</td>
<td>2</td>
</tr>
<tr>
<td>13</td>
<td>0010110</td>
<td>Memory dump</td>
<td>6</td>
</tr>
<tr>
<td>20</td>
<td>0100000</td>
<td>Terminate</td>
<td>0</td>
</tr>
<tr>
<td>21</td>
<td>0100010</td>
<td>Execute alternate sequence</td>
<td>5</td>
</tr>
<tr>
<td>32</td>
<td>0110100</td>
<td>Targeting load</td>
<td>40</td>
</tr>
<tr>
<td>33</td>
<td>0110110</td>
<td>Execute maneuver A</td>
<td>5</td>
</tr>
<tr>
<td>34</td>
<td>0111000</td>
<td>Execute maneuver B</td>
<td>5</td>
</tr>
<tr>
<td>35</td>
<td>0111010</td>
<td>Execute generalized maneuver</td>
<td>20</td>
</tr>
<tr>
<td>36</td>
<td>0111100</td>
<td>Return to nominal timeline</td>
<td>5</td>
</tr>
<tr>
<td>45</td>
<td>1001010</td>
<td>Water control valve logic inhibit</td>
<td>0</td>
</tr>
</tbody>
</table>

TABLE 10-7 TIME BASE UPDATE DCS DATA COMMAND FORMAT

<table border="1">
<thead>
<tr>
<th>LVDC Bit Position</th>
<th>S</th>
<th>1</th>
<th>2</th>
<th>3</th>
<th>4</th>
<th>5</th>
<th>6</th>
<th>7 - 13</th>
<th>14-25</th>
</tr>
</thead>
<tbody>
<tr>
<td>14 Information Bits</td>
<td>14</td>
<td>13</td>
<td>12</td>
<td>11</td>
<td>10</td>
<td>9</td>
<td>8</td>
<td>7 - 1</td>
<td></td>
</tr>
<tr>
<td>Bit Significance</td>
<td>Sign</td>
<td>MSB* data</td>
<td>data</td>
<td>data</td>
<td>data</td>
<td>LSB* data</td>
<td>1</td>
<td>1' Complement of bits S-6</td>
<td>Unused Bits</td>
</tr>
</tbody>
</table>

\*MSB (most significant bit); LSB (least significant bit).

10------------------------------------------------

The value of the least significant bit (LSB) is 4 seconds and the maximum update possible with a single command is 124 seconds.

The time base update must be implemented as soon as the DCS data command is verified and the data is formatted. Unissued switch selectors, with times earlier than the new time, must be issued at maximum rate. No switch selectors will be omitted or reissued as a result of a time base update.

Time base updates received before previous updates are implemented must be accumulated with the unimplemented update and the entire update must be implemented. Time base updates will not change the times of orbital guidance maneuvers or other time base functions except switch selectors and the telemetered time of station acquisition and loss. Time base updates received in a time base after the last switch selector in the nominal time base sequence has been issued must not be implemented.

Bit 9 of Mode Code 27, which will be initially zero, must change state each time update data is accepted for implementation. A terminate command must not prevent an update from being implemented after a valid DCS data command is received.

#### 10.4.2 Navigation Update

This DCS mode command will cause the orbital navigation state vectors (position and velocity components) to be replaced with state vectors supplied from the ground. The new state vectors and the time at which they must be implemented will be contained in 35 DCS data commands. Since only six true information bits are transmitted with each DCS data command, five transmissions are required to form each 26-bit LVDC word. The most significant bit (MSB) of each LVDC word must be transmitted first, and negative quantities must be in two's complement form. The information bit format for the five DCS data commands required for an update quantity is shown in Table 10-8.

11------------------------------------------------

TABLE 10-8 NAVIGATION UPDATE, TARGETING LOAD, EXECUTE GENERALIZED MANEUVER, EXECUTE SPECIAL MANEUVER, OR RETURN TO NOMINAL TIMELINE DCS DATA COMMAND FORMAT\*

<table border="1">
<thead>
<tr>
<th>LVDC Bit Position</th>
<th>S</th>
<th>1</th>
<th>2</th>
<th>3</th>
<th>4</th>
<th>5</th>
<th>6</th>
<th>7 - 13</th>
<th>14 - 25</th>
</tr>
</thead>
<tbody>
<tr>
<td>Information Bits</td>
<td>14</td>
<td>13</td>
<td>12</td>
<td>11</td>
<td>10</td>
<td>9</td>
<td>8</td>
<td>7 - 1</td>
<td></td>
</tr>
<tr>
<td>Data Command 1</td>
<td>LVDC S</td>
<td>LVDC 1</td>
<td>LVDC 2</td>
<td>LVDC 3</td>
<td>LVDC 4</td>
<td>LVDC 5</td>
<td rowspan="5">Sequence Bit**</td>
<td rowspan="5">1's complement of decoder bits 14-8</td>
<td rowspan="5">Unused Bits</td>
</tr>
<tr>
<td>Data Command 2</td>
<td>LVDC 6</td>
<td>LVDC 7</td>
<td>LVDC 8</td>
<td>LVDC 9</td>
<td>LVDC 10</td>
<td>LVDC 11</td>
</tr>
<tr>
<td>Data Command 3</td>
<td>LVDC 12</td>
<td>LVDC 13</td>
<td>LVDC 14</td>
<td>LVDC 15</td>
<td>LVDC 16</td>
<td>LVDC 17</td>
</tr>
<tr>
<td>Data Command 4</td>
<td>LVDC 18</td>
<td>LVDC 19</td>
<td>LVDC 20</td>
<td>LVDC 21</td>
<td>LVDC 22</td>
<td>LVDC 23</td>
</tr>
<tr>
<td>Data Command 5</td>
<td>LVDC 24</td>
<td>LVDC 25</td>
<td>X</td>
<td>X</td>
<td>X</td>
<td>X</td>
</tr>
</tbody>
</table>

\*The format presented in Table 10-8 is for only one 26-bit LVDC word and must be repeated for each LVDC word.

\*\*The sequence bit must alternate 1 and 0 for each data command starting with a 1 for the first data command of the first LVDC word. For Execute Generalized Maneuver DCS commands, this format will only be used for the quantities  $Y_{ref}$ ,  $Z_{ref}$ , and  $X_{ref}$  (see Section 10.4.6).

12------------------------------------------------

The seven update quantities must be transmitted in the order  $\dot{z}_S$ ,  $\dot{x}_S$ ,  $\dot{y}_S$ ,  $z_S$ ,  $x_S$ ,  $y_S$ , and implementation time (NUPTIM).

The velocity components must be in meters per second, scaled 14.

The position components must be in meters, scaled 23.

The implementation time must be in seconds, referenced from guidance reference release (GRR) and scaled 15. After all the update data is accepted and formatted, the time of implementation must be 10 seconds or more in the future. If not, the update must be rejected, an automatic program initiated terminate must be performed as defined in Section 10.4.5, and DCS error code 54 must be issued. Any update awaiting implementation must not be affected. \*

If the update is accepted, the seven quantities must be stored in memory module 4, sector 15, locations 371 through 377, in the order received. Bit 8 of MC27 must be set to 1 when an update is accepted, and reset to zero when the update is implemented.

A navigation update accepted before a previous update is implemented will replace the previous update. A terminate command must not prevent an update after the thirty-fifth valid data word is received.

The navigation update discussion in Section 5 describes the requirements for implementing the new state vectors into the orbital navigation computations. \*

#### 10.4.3 Generalized Switch Selector

This DCS mode command will provide the capability to issue any IU or S-IVB switch selector event specified by two accompanying DCS data commands. If this DCS mode command is received while a previous generalized switch selector event is waiting to be issued, the new DCS mode command must be

13------------------------------------------------

rejected and DCS error code 34 must be issued. The ground station may repeat the request until the previous event is serviced at which time the DCS mode command will be accepted. The information bit format for the two DCS data commands is shown in Table 10-9 (address bits must be in true form). The switch selector event specified is a class 3 alternate sequence.

TABLE 10-9 GENERALIZED SWITCH SELECTOR  
DCS DATA COMMAND FORMAT

<table border="1">
<thead>
<tr>
<th>LVDC Bit Position</th>
<th>S</th>
<th>1</th>
<th>2</th>
<th>3</th>
<th>4</th>
<th>5</th>
<th>6</th>
<th>7 - 13</th>
<th>14 - 25</th>
</tr>
</thead>
<tbody>
<tr>
<td>Information Bits</td>
<td>14</td>
<td>13</td>
<td>12</td>
<td>11</td>
<td>10</td>
<td>9</td>
<td>8</td>
<td>7 - 1</td>
<td rowspan="3">Unused Bits</td>
</tr>
<tr>
<td>Data Command 1</td>
<td>IU Stage</td>
<td>S-IVB Stage</td>
<td>X</td>
<td>Addr 8</td>
<td>Addr 7</td>
<td>Addr 6</td>
<td>1</td>
<td rowspan="2">1's Complement of bits S-6</td>
</tr>
<tr>
<td>Data Command 2</td>
<td>Addr 5</td>
<td>Addr 4</td>
<td>Addr 3</td>
<td>Addr 2</td>
<td>Addr 1</td>
<td>X</td>
<td>0</td>
</tr>
</tbody>
</table>

Issuance of the generalized switch selector event must be attempted as soon as the data is verified and formatted. If the program is already servicing a preprogrammed switch selector command, the generalized switch selector command must be preserved until there is adequate free time (500 ms) for its issuance. A terminate command must not prevent the issuance of a generalized switch selector event after the DCS data commands are accepted.

#### 10.4.4 Memory Dump

\*

This DCS mode command causes the flight program to telemeter the contents of a specified portion (one or more memory words) of the LVDC memory. The complete start and end addresses of the portion of memory to be telemetered will be specified by six accompanying DCS data commands. The start module, sector,

14------------------------------------------------

and address requested must be less than or equal to the end module, sector, and address requested. The information bit format for the six DCS data commands is shown in Table 10-10.

TABLE 10-10 MEMORY DUMP DCS DATA COMMAND FORMAT

\*

<table border="1">
<thead>
<tr>
<th>LVDC Bit Position</th>
<th>S</th>
<th>1</th>
<th>2</th>
<th>3</th>
<th>4</th>
<th>5</th>
<th>6</th>
<th>7-13</th>
<th>14-25</th>
</tr>
</thead>
<tbody>
<tr>
<td>Information Bits</td>
<td>14</td>
<td>13</td>
<td>12</td>
<td>11</td>
<td>10</td>
<td>9</td>
<td>8</td>
<td>7-1</td>
<td rowspan="7">Unused bits</td>
</tr>
<tr>
<td>Data Command 1</td>
<td>Start Addr 8</td>
<td>Start Addr 7</td>
<td>Start Addr 6</td>
<td>Start Addr 5</td>
<td>Start Addr 4</td>
<td>Start Addr 3</td>
<td>1</td>
<td rowspan="6">1's complement of Bits 5-6</td>
</tr>
<tr>
<td>Data Command 2</td>
<td>Start Addr 2</td>
<td>Start Addr 1</td>
<td>X</td>
<td>Start Module MSB</td>
<td>Start Module</td>
<td>Start Module LSB</td>
<td>0</td>
</tr>
<tr>
<td>Data Command 3</td>
<td>Start Sector MSB</td>
<td>Start Sector</td>
<td>Start Sector</td>
<td>Start Sector LSB</td>
<td>X</td>
<td>X</td>
<td>1</td>
</tr>
<tr>
<td>Data Command 4</td>
<td>End Addr 8</td>
<td>End Addr 7</td>
<td>End Addr 6</td>
<td>End Addr 5</td>
<td>End Addr 4</td>
<td>End Addr 3</td>
<td>0</td>
</tr>
<tr>
<td>Data Command 5</td>
<td>End Addr 2</td>
<td>End Addr 1</td>
<td>X</td>
<td>End Module MSB</td>
<td>End Module</td>
<td>End Module LSB</td>
<td>1</td>
</tr>
<tr>
<td>Data Command 6</td>
<td>End Sector MSB</td>
<td>End Sector</td>
<td>End Sector</td>
<td>End Sector LSB</td>
<td>X</td>
<td>X</td>
<td>0</td>
</tr>
</tbody>
</table>

As soon as the sixth DCS data command is verified, the program must verify that none of the portion of memory requested is from a non-existing module. If the addresses are valid, the program must begin dumping data starting at the specified start address. Blocks of 16 LVDC words must be intermixed with other LVDC telemetry until all requested memory locations have been telemetered.

\*  
\*  
\*  
\*  
\*  
\*

15------------------------------------------------

If the number of memory locations requested is not a multiple of 16, the last block telemetered must include less than 16 words, ending with the last word requested. The telemetry tags for each word of a block are defined in Telemetry Tags Table 11-8. The first word of a block must identify the information in the block by indicating the module, sector, and address of the telemetered data. One block of data will be telemetered approximately every 400 ms.

If the start address and end address are the same, the program must telemeter only one data word. If a memory dump is requested from an odd-numbered (duplex) module, the corresponding addresses from the duplex even-numbered (primary) module must be telemetered. The output format of the memory dump is shown in Table 10-11. The header word must include the module, sector, and address of the first data location in the block of 16 consecutive data locations.

TABLE 10-11 MEMORY DUMP FORMAT

<table border="1">
<tbody>
<tr>
<td>LVDC Accumulator</td>
<td>S</td>
<td>1</td>
<td>2</td>
<td>3</td>
<td>4</td>
<td>5</td>
<td>6</td>
<td>7</td>
<td>8</td>
<td>9</td>
<td>10</td>
<td>11</td>
<td>12</td>
<td>13</td>
<td>14</td>
<td>15-25</td>
</tr>
<tr>
<td>Header Word</td>
<td>MSB<br/>M</td>
<td>LSB<br/>M</td>
<td>MSB<br/>S</td>
<td>S</td>
<td>S</td>
<td>LSB<br/>S</td>
<td>MSB<br/>A</td>
<td>A</td>
<td>A</td>
<td>A</td>
<td>A</td>
<td>A</td>
<td>A</td>
<td>LSB<br/>A</td>
<td>0...0</td>
</tr>
<tr>
<td>Data Words</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D</td>
<td>D...D</td>
</tr>
</tbody>
</table>

NOTE: M, S, A = module, sector, and address, respectively of the first data location in the present block

D = data bits

16------------------------------------------------

Once a dump has been started, only a terminate command will be accepted by the program. Other mode commands received during a dump must be rejected and DCS error code 64 must be issued. Receipt of a terminate command any time before the last block is completed must stop the dump.

#### 10.4.5 Terminate

This DCS command will cause the flight program to terminate the receipt of multiple word commands or terminate a memory dump in progress. Upon receipt of this command, the flight program must do the following:

- • Terminate any multiple-word DCS commands prior to the receipt of the last valid data word
- • Terminate any commands which have failed to be validated by the LVDC
- • Terminate a memory dump prior to the telemetry of the last block of data
- • Return to the configuration required to accept a new mode command.

The terminate command will have no effect on the execution of a DCS routine whose data requirements have been met and validated, except for the memory dump routine. The terminate command must be enabled whenever the command decoder interrupt is enabled.

Under certain specified conditions, an automatic program initiated terminate must be performed. These specific conditions are detailed within each applicable DCS command description section. A program initiated terminate must provide all the functions of the DCS terminate command except early memory dump termination.

17------------------------------------------------

#### 10.4.6 Execute Generalized Maneuver

This DCS mode command will provide the capability to initiate either an inertial attitude hold or a track local reference orbital maneuver with any attitude within the limitations defined in Section 8.3.2. The five data quantities required by this mode command, their units and scaling, are described in Table 10-12. \*

TABLE 10-12 EXECUTE GENERALIZED MANEUVER PARAMETERS \*

<table border="1">
<thead>
<tr>
<th>Parameter</th>
<th>Definition</th>
<th>Units</th>
<th>Scaling</th>
</tr>
</thead>
<tbody>
<tr>
<td><math>T_{SOM}</math></td>
<td>Time in current time base to start the generalized orbital maneuver</td>
<td>sec</td>
<td>15</td>
</tr>
<tr>
<td>GOMTYP</td>
<td>Type of generalized orbital maneuver (Inertial hold or track local reference)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td><math>Y_{ref}</math></td>
<td>Y reference angle</td>
<td>pirads</td>
<td>0</td>
</tr>
<tr>
<td><math>Z_{ref}</math></td>
<td>Z reference angle</td>
<td>pirads</td>
<td>0</td>
</tr>
<tr>
<td><math>X_{ref}</math></td>
<td>X reference angle</td>
<td>pirads</td>
<td>0</td>
</tr>
</tbody>
</table>

These parameters will be contained in 20 DCS data commands. Since only six true information bits are transmitted with each DCS data command, five transmissions are required to form each 26-bit LVDC word. Since the GOMTYP parameter only requires two bit locations, it is transmitted in the spare location of the fifth data word of  $T_{SOM}$ . The most significant bit of each LVDC word must be transmitted first, and negative quantities must be in two's complement form. The five parameters must be transmitted in the order  $T_{SOM}$ , GOMTYP,  $Y_{ref}$ ,  $Z_{ref}$ ,  $X_{ref}$ . The information bit format for the first 5 data commands containing \*

18------------------------------------------------

$T_{SOM}$  and GOMTYP is shown in Table 10-13, and the information bit format for the remaining 15 data commands containing  $Y_{ref}$ ,  $Z_{ref}$ , and  $X_{ref}$  is shown in Table 10-8. The GOMTYP parameter must also be set by the program, as defined in Table 10-14, if an execute special maneuver or return to nominal timeline command is accepted.

\*  
\*  
\*  
\*  
\*

TABLE 10-13 EXECUTE GENERALIZED MANEUVER  
TIME AND TYPE DCS DATA  
COMMAND FORMAT\*

<table border="1">
<thead>
<tr>
<th>LVDC Bit Position</th>
<th>S</th>
<th>1</th>
<th>2</th>
<th>3</th>
<th>4</th>
<th>5</th>
<th>6</th>
<th>7 - 13</th>
<th>14-25</th>
</tr>
</thead>
<tbody>
<tr>
<td>Information Bits</td>
<td>14</td>
<td>13</td>
<td>12</td>
<td>11</td>
<td>10</td>
<td>9</td>
<td>8</td>
<td>7 - 1</td>
<td></td>
</tr>
<tr>
<td>Data Command 1</td>
<td><math>T_{SOM}</math><br/>MSB</td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td rowspan="5">Sequence Bit**</td>
<td rowspan="5">1's complement of decoder bits 14-8</td>
<td rowspan="5">Unused Bits</td>
</tr>
<tr>
<td>Data Command 2</td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
</tr>
<tr>
<td>Data Command 3</td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
</tr>
<tr>
<td>Data Command 4</td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math></td>
</tr>
<tr>
<td>Data Command 5</td>
<td><math>T_{SOM}</math></td>
<td><math>T_{SOM}</math><br/>LSB</td>
<td>GOM-TYP<br/>LOCAL<br/>REF.</td>
<td>GOM-TYP<br/>INRTL<br/>HOLD</td>
<td>X</td>
<td>X</td>
</tr>
</tbody>
</table>

\*The format for  $Y_{ref}$ ,  $Z_{ref}$ ,  $X_{ref}$  is contained in Table 10-8.

\*\*The sequence bit must alternate 1 and 0 for each data command, starting with a 1 for data command 1 above, and will continue to alternate until the last data command of  $X_{ref}$  is received.

19------------------------------------------------

TABLE 10-14 GOMTYP (GENERALIZED ORBITAL MANEUVER TYPE STATUS WORD) FORMAT

<table border="1">
<thead>
<tr>
<th>LVDC Bit</th>
<th>Bit Significance</th>
<th>Bit Set Method</th>
</tr>
</thead>
<tbody>
<tr>
<td>S</td>
<td>Local Reference Maneuver Commanded</td>
<td>DCS Command</td>
</tr>
<tr>
<td>1</td>
<td>Inertial Reference Maneuver Commanded</td>
<td>DCS Command</td>
</tr>
<tr>
<td>2</td>
<td>Return to Nominal Timeline Commanded</td>
<td>Set by Program</td>
</tr>
<tr>
<td>3</td>
<td>Special Maneuver A Commanded</td>
<td>Set by Program</td>
</tr>
<tr>
<td>4</td>
<td>Special Maneuver B Commanded</td>
<td>Set by Program</td>
</tr>
<tr>
<td colspan="3">
<p>Note: Whenever any bit of GOMTYP is set, all other GOMTYP bits must be reset.</p>
</td>
</tr>
</tbody>
</table>

After all the data is accepted and formatted,  $T_{SOM}$  must be tested to determine if it is in the future. If  $T_{SOM}$  is zero or some time in the past, the execute generalized maneuver command must be implemented within one computation cycle. If  $T_{SOM}$  is in the future, the execute generalized maneuver command must be scheduled at that time. When the command is accepted, the five parameters must be stored in memory module 2, sector 07, locations 373 through 377 in the order  $Y_{ref}$ ,  $Z_{ref}$ ,  $X_{ref}$ , GOMTYP,  $T_{SOM}$ .

An execute generalized maneuver command accepted before a pending execute generalized maneuver, execute special maneuver or return to nominal timeline command is implemented, will replace the pending command. A terminate command must not prevent this command from being executed after the 20th valid data command is received. If a command is pending when a new orbital primary time base is started, the command will not be executed and the contents of memory module 2, sector 07, locations 373 through 377 must be zeroed.

20------------------------------------------------

When the time (TB) in the current time base becomes greater than or equal to the initiation time ( $T_{SOM}$ ), the program must initiate either an inertial hold or track local reference maneuver according to the value of GOMTYP.

If the maneuver is an inertial hold,  $Y_{ref}$ ,  $Z_{ref}$ , and  $X_{ref}$  will be used as the predefined angles specified in the inertial platform gimbal system and will be implemented as described in the inertial attitude hold discussion in Section 5.5. Bit 11 of MC27 will be set for the duration of this maneuver.

If the maneuver is a track local reference, the sines and cosines of  $Y_{ref}$  and  $Z_{ref}$  must be computed. The sine and cosine of  $Y_{ref}$  will be used as the sine and cosine of the desired in-plane attitude with respect to the local reference, the sine and cosine of  $Z_{ref}$  will be used as the sine and cosine of the desired rotation out of the  $X_4-Z_4$  plane and  $X_{ref}$  will be used as the desired roll attitude. Using these values, the track local reference maneuver as described in Section 5.5 must be computed. Bit 10 of MC27 must be set for the duration of this maneuver.

The program will continue to execute this maneuver until further DCS action initiates another execute generalized maneuver, execute special maneuver, or return to nominal timeline command. \*

Bit 19 of MC27 must be set upon the implementation of a valid execute generalized maneuver. This bit will be reset upon implementation of a return to nominal timeline or execute special maneuver command. \*  
\*  
\*

21------------------------------------------------

#### 10.4.7 Return to Nominal Timeline

This DCS mode command will provide the capability to return to the preprogrammed orbital attitude timeline after other DCS action has been initiated to override the preprogrammed timeline. The mode command requires that the time to return to nominal timeline ( $T_{RNTL}$ ) be sent in five DCS data commands. Since only six true information bits are transmitted with each DCS data command, five transmissions are required to form  $T_{RNTL}$ . The most significant bit of each LVDC word must be transmitted first. The variable  $T_{RNTL}$  must be in seconds, scaled 15. \*  
The information bit format for the five data commands is contained in Table 10-8.

After all the data is accepted and formatted,  $T_{RNTL}$  must be \*  
tested to determine if it is in the future. If  $T_{RNTL}$  is zero \*  
or some time in the past, the return to nominal timeline \*  
command must be implemented within one computation cycle. If \*  
 $T_{RNTL}$  is in the future, the return to nominal timeline command \*  
must be scheduled at that time. When the command is accepted, \*  
 $T_{RNTL}$  must be stored in the same memory location in which  $T_{SOM}$  \*  
is stored (module 2, sector 07, location 377). A 1 must be \*  
stored in bit 2 and all other bits zeroed in the location in which \*  
GOMTYP is stored (module 2, sector 07, location 376). The  
memory locations containing  $Y_{ref}$ ,  $Z_{ref}$ , and  $X_{ref}$  for a pending  
execute generalized maneuver must be zeroed (module 2, sector  
07, locations 373 through 375).

A return to nominal timeline command accepted before a pending  
return to nominal timeline, execute generalized maneuver, or  
execute special maneuver command is implemented, will replace \*  
the pending command. A terminate command must not prevent this  
command from being executed after the fifth valid data command  
is received. If a command is pending when a new orbital primary \*  
time base is started, the command must not be executed and the \*  
contents of memory module 2, sector 07, locations 373 through 377 \*  
must be zeroed. \*

22------------------------------------------------

When time (TB) in the current time base becomes greater than or equal to  $T_{RNTL}$ , the program must terminate any alternate maneuvers being executed, return to the preprogrammed orbital timeline, and reset bit 19 of MC27.

The program will continue to execute the preprogrammed time-line until further DCS action initiates another alternate maneuver.

#### 10.4.8 ECS Water Control Valve Logic Inhibit \*

This DCS mode command will inhibit the programmed ECS water control valve logic. Once this inhibit command is received, the ECS water control valve logic must be bypassed for the remainder of the mission. There is no provision to remove this inhibit once it is applied. Any further changes to the water control valve position must be commanded via the generalized switch selector command. No data commands are required for this function and the terminate command will have no effect on the inhibit. The switch selector issued is a class 3 alternate sequence.

If the inhibit command is received, Bit 18 of Mode Code 27 (which must be initialized to 1 at GRR to indicate that the water control valve logic is active) must be reset to zero.

#### 10.4.9 Execute Special Maneuvers \*

When required, either one or two DCS commands must provide the capability to initiate special preprogrammed orbital maneuvers

23------------------------------------------------

A or B. In general, a special maneuver will not be an inertial hold or track local reference as defined in Section 5.5.

Each execute maneuver command will provide the capability to initiate a special maneuver command at a transmitted time or at the time of command acceptance, as dictated by mission requirements. The time,  $T_{SOM}$ , at which the special maneuver is to be executed must be contained in 5 DCS data commands. Since only six true information bits are transmitted with each DCS data command, five transmissions are required to form the 26-bit LVDC word. The most significant bit of the LVDC word must be transmitted first. The variable  $T_{SOM}$  must be in seconds, scaled 15.

After the data is accepted and formatted,  $T_{SOM}$  must be tested to determine if it is in the future. If  $T_{SOM}$  is zero or some time in the past, the execute special maneuver command must be implemented within one computation cycle. If  $T_{SOM}$  is in the future, the execute generalized maneuver command must be scheduled at that time. When the command is accepted,  $T_{SOM}$  must be stored in memory module 2, sector 07, location 377. If the execute maneuver A or execute maneuver B command is accepted, a 1 must be stored in bit 3 (maneuver A) or bit 4 (maneuver B), respectively, and all other bits zeroed in the location in which GOMTYP is stored (module 2, sector 07, location 376).

An execute special maneuver command accepted before a pending execute generalized maneuver, execute special maneuver or return to a nominal timeline command is implemented will replace the pending command. If a command is pending when a new orbital time base is started, the command will not be executed and memory module 2, sector 07, location 377 will be zeroed.

24------------------------------------------------

When the time in the current time base becomes greater than or equal to the initiation time ( $T_{SOM}$ ), the program must initiate the corresponding special maneuver. The program will continue to execute this maneuver until further DCS action initiates another execute special maneuver, execute generalized maneuver, or return to nominal timeline command.

A specific mode code bit must be set at implementation of each execute special maneuver command. This bit will be reset upon implementation of an execute generalized maneuver or return to nominal timeline command. Specific mode code bit assignments will be made when the corresponding execute special maneuvers are defined.

At present, Execute Maneuver A and Execute Maneuver B are not defined.

#### 10.4.10 Execute Alternate Sequence

\*

When required, this DCS mode command will provide the capability to initiate up to fifteen different preprogrammed alternate sequences. The two data quantities required by this mode command, their units and scaling, are defined in Table 10-15.

These parameters will be contained in five DCS data commands. Since only six true information bits are transmitted with each DCS data command, five transmissions are required to form the 26-bit LVDC word containing  $T_{SEQ}$ . Since the SEQNUM indicator only requires four bit locations, it is transmitted in the spare location of the fifth data word of  $T_{SEQ}$ . The most significant bit of each word must be transmitted first. The information bit format for the five data commands is shown in Table 10-16.

25------------------------------------------------

TABLE 10-15 EXECUTE ALTERNATE SEQUENCE PARAMETERS

<table border="1">
<thead>
<tr>
<th>Parameter</th>
<th>Definition</th>
<th>Units</th>
<th>Scaling</th>
</tr>
</thead>
<tbody>
<tr>
<td>T<sub>SEQ</sub></td>
<td>Time in current time base to start the alternate sequence; referenced to TBB, the current time base time biased by any time base updates received</td>
<td>sec</td>
<td>15</td>
</tr>
<tr>
<td>SEQNUM</td>
<td>Four bit indicator selecting one of fifteen preprogrammed alternate sequences</td>
<td>N/A</td>
<td>N/A</td>
</tr>
</tbody>
</table>

TABLE 10-16 EXECUTE ALTERNATE SEQUENCE TIME AND NUMBER DATA COMMAND FORMAT

<table border="1">
<thead>
<tr>
<th>LVDC Bit Position</th>
<th>S</th>
<th>1</th>
<th>2</th>
<th>3</th>
<th>4</th>
<th>5</th>
<th>6</th>
<th>7-13</th>
<th>14-25</th>
</tr>
</thead>
<tbody>
<tr>
<td>Information Bits</td>
<td>14</td>
<td>13</td>
<td>12</td>
<td>11</td>
<td>10</td>
<td>9</td>
<td>8</td>
<td>7-1</td>
<td></td>
</tr>
<tr>
<td>Data Command 1</td>
<td>T<sub>SEQ</sub> MSB</td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>1</td>
<td rowspan="5">1's complement of decoder bit 14-8</td>
<td rowspan="5">Unused bits</td>
</tr>
<tr>
<td>Data Command 2</td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>0</td>
</tr>
<tr>
<td>Data Command 3</td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>1</td>
</tr>
<tr>
<td>Data Command 4</td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub></td>
<td>0</td>
</tr>
<tr>
<td>Data Command 5</td>
<td>T<sub>SEQ</sub></td>
<td>T<sub>SEQ</sub> LSB</td>
<td>SEQNUM MSB</td>
<td>SEQNUM</td>
<td>SEQNUM</td>
<td>SEQNUM LSB</td>
<td>1</td>
</tr>
</tbody>
</table>

NOTE: SEQNUM will be used to select up to 15 different alternate sequences by transmitting the 15 bit configurations 01<sub>8</sub> through 17<sub>8</sub>.

26------------------------------------------------

After the data is accepted and formatted, SEQNUM must be tested to determine if it is legal. If the value of SEQNUM is zero or is undefined for the present mission, the command must be rejected, DCS error message 70 must be telemetered as defined in Section 10.3.4, a terminate must be issued by the LVDC, and any other DCS alternate sequence previously accepted and pending must remain unaffected. If the value of SEQNUM is defined for the present mission,  $T_{SEQ}$  must be tested to determine if it is in the future. If  $T_{SEQ}$  is zero or some time in the past, the selected alternate sequence must be requested immediately and issued within normal switch selector priorities applicable to the class of the specific alternate sequence. If  $T_{SEQ}$  is in the future, the selected alternate sequence must be scheduled at that time. When the command is accepted,  $T_{SEQ}$  and SEQNUM must be stored in memory module TBD, sector TBD, locations TBD and TBD, respectively.

An execute alternate sequence command accepted before a pending execute alternate sequence command is implemented will replace the pending command. If a command is pending when a new orbital primary time base is started, the command will not be executed and memory module TBD, sector TBD, locations TBD and TBD will be zeroed.

When an alternate sequence command is implemented, a bit in MC27 must be set to 1. When the alternate sequence command is implemented, this bit must be reset to zero. This bit indicates that an alternate sequence is pending but does not identify which alternate sequence, since that information is available in memory module TBD, sector TBD, locations TBD and TBD.

At present, no preprogrammed DCS alternate sequences are defined. Switch selector alternate sequence classes, telemetry mode code bits, and other specific details of implementation will be defined when requirements for individual DCS alternate sequences are defined.

27------------------------------------------------

10.4.11 Targeting Load

\*

This DCS command will provide the capability to load eight targeting parameters into the LVDC. These eight parameters, their units and scaling are described in Table 10-17.

TABLE 10-17 TARGETING LOAD PARAMETERS

<table border="1">
<thead>
<tr>
<th>Parameter</th>
<th>Definition</th>
<th>Units</th>
<th>Scaling</th>
</tr>
</thead>
<tbody>
<tr>
<td><math>V_T</math></td>
<td>Desired insertion velocity</td>
<td>m/sec</td>
<td>14</td>
</tr>
<tr>
<td><math>R_T</math></td>
<td>Desired insertion radius</td>
<td>meters</td>
<td>23</td>
</tr>
<tr>
<td><math>\theta_T</math></td>
<td>Desired insertion flight path angle</td>
<td>pirads</td>
<td>0</td>
</tr>
<tr>
<td><math>i</math></td>
<td>Desired orbital inclination</td>
<td>pirads</td>
<td>0</td>
</tr>
<tr>
<td><math>\lambda_0</math></td>
<td>Longitude of the desired descending node</td>
<td>pirads</td>
<td>0</td>
</tr>
<tr>
<td><math>\dot{\lambda}</math></td>
<td>Rate of change of <math>\lambda_0</math></td>
<td>pirads/sec</td>
<td>-15</td>
</tr>
<tr>
<td><math>T_{GRR0}</math></td>
<td>Nominal GRR time in terms of <math>T_{GMT}</math></td>
<td>sec</td>
<td>17</td>
</tr>
<tr>
<td>TLCK</td>
<td>Checksum Factor</td>
<td>N/A</td>
<td>N/A</td>
</tr>
</tbody>
</table>

\*

\*

This targeting will be contained in 40 DCS data commands. Since only six true information bits are transmitted with each DCS data command, five transmissions are required to form each 26-bit LVDC word. The MSB of each LVDC word must be transmitted first, and negative quantities must be in two's complement form. The eight targeting quantities must be transmitted in the order  $V_T$ ,  $R_T$ ,  $\theta_T$ ,  $i$ ,  $\lambda_0$ ,  $\dot{\lambda}$ ,  $T_{GRR0}$ , TLCK. The information bit format for the five DCS data commands required for an

28------------------------------------------------

update quantity is shown in Table 10-8. The variable TLCK is a checksum factor which is equal to the two's complement of the sum of the other seven variables. This factor is added to prevent the targeting load command from changing the checksums in the LVDC.

After all the targeting data is accepted and formatted the eight quantities must be stored in memory module 0, Sector 16, locations 360 through 367, in the order received. Targeting currently stored in the above locations must not be affected until all the targeting data is accepted and formatted. A terminate command must not prevent an update after the fortieth valid data word is received. \*

Acceptance times of this DCS command are discussed in Section 3.2.1.

29------------------------------------------------

(THIS PAGE HAS BEEN LEFT BLANK INTENTIONALLY)

30------------------------------------------------

## SECTION 11

### REAL TIME TELEMETRY AND DATA COMPRESSION

#### 11.1 INTRODUCTION

The flight program must provide telemetry for both real time analysis and post flight evaluation. Continuous real time telemetry must be generated throughout the mission. During orbital mode, certain quantities must be sampled, tested, and stored according to specified norms. This compressed data must be stored in LVDC memory and transmitted during acquisition of designated telemetry dump stations, after the real time telemetry has been processed.

#### 11.2 TELEMETRY SYSTEM INTERFACE

The acceptance rate and the priority of the telemetry, originating from LVDC or LVDA, is determined by the telemetry system. These hardware limitations must be considered in fulfilling the requirements outlined in Section 11.4.

Telemetry data is accepted by the telemetry system at a rate of 240 words per second or one word every 4.167 milliseconds.

Telemetry originating in the LVDC has priority over all LVDA telemetry. If the LVDC were to issue telemetry at the maximum rate, only LVDC data would be transmitted.

All LVDA telemetry is issued through the data output multiplexer (DOM) to the telemetry system. The DOM contains a temporary storage area for up to four data words. Once this storage area is filled, further LVDA data will be ignored until

31------------------------------------------------

a location is cleared by transmitting the data via the telemetry system. The exception is the priority processing of error monitor register (EMR) data. When a bit is set in the EMR, the LVDA generates an error word which is forced into the DOM. If the DOM has an empty location, the error word is loaded into it. If all locations are filled, the error word is forced into one of two locations destroying the data. In either case, the DOM pointers are set to issue the error word telemetry at the first opportunity for DOM telemetry.

### 11.3 IDENTIFICATION TAGS

Telemetry data is issued using specific codes called process input-output (PIO) tags. The PIO code, the contents of the 26 bit data word, a validity bit, a bit assigning odd parity, and the mode register (with LVDC data) or real time clock bits 3 - 6 (with LVDA data) compose the 40 bit telemetry word. This word is transformed by the telemetry system into a pulse code modulation (PCM) telemetry format, which is transmitted to the ground. The correspondence between the PIO tags and PCM tags is described in the notes following Table 11-8.

The PIO codes, identification codes for the telemetered data, must meet the following specifications. For purposes of discussion, the nine bits of the PIO tag are referred to as bits A9 - A1 (the complement is indicated by an attached "c"), where A9 is the most significant bit.

- • Each quantity must have a unique tag for the entire mission, from prelaunch to end of mission (EOM).
- • The same tag must be retained for the same quantity from mission to mission.

32------------------------------------------------

- • For LVDC telemetry, A2 must be equal to 0. For LVDA telemetry, the following is determined by the hardware: A2 will be equal to 1 and the expression, A2 AND A6 AND A1c AND A3c AND A4c AND A5c AND A7c, will be equal to 0.
- • For LVDC telemetry, the origin of the data must be indicated by selecting proper values for A9 and A8 as shown in Table 11-1.
- • For LVDC telemetry, the mode register must be equal to octal 4 for all command receiver data and octal 3 for all compressed data. The other possible mode register settings (octal 0, 1, 2, 5, 6, and 7) are available for other types of data.

TABLE 11-1 LVDC DATA ORIGIN

<table border="1">
<thead>
<tr>
<th>Data Origin</th>
<th>State of A9 and A8</th>
</tr>
</thead>
<tbody>
<tr>
<td>LVDC accumulator</td>
<td>A8 = 0, A9 = 0 or 1</td>
</tr>
<tr>
<td>LVDC main memory</td>
<td>A8 = 1, A9 = 0</td>
</tr>
<tr>
<td>LVDC residual memory</td>
<td>A8 = 1, A9 = 1</td>
</tr>
</tbody>
</table>

The limitations on LVDC telemetry PIOs discussed in the latter two items above, limit the unique codes for data from the LVDC accumulator to 128 unique identification codes. Data originating from LVDC main memory and residual memory must retain the memory address as the unique identification code. This limits the PIOs for LVDC data from memory to 128 unique codes, 64 from main memory and 64 from residual memory.

The number of LVDA codes is determined by hardware limits; a complete list, including spares, is included in Table 11-8.

33------------------------------------------------

## 11.4 GENERAL REQUIREMENTS FOR LVDC AND LVDA TELEMETRY

Telemetry must adhere to following ground rules:

- • Data must be telemetered in the computation cycle in which it is generated, whenever possible.
- • Data must be telemetered as specified, at regular intervals, when computed, or on occurrence of an event.

The Telemetry Tags Table 11-8 specifies the unique PIO and PCM tag, the frequency and time interval for telemetry (when applicable), and the scaling and units (when applicable) for each quantity that must be telemetered.

### 11.4.1 LVDC Telemetry

For all LVDC telemetry, 4.35 ms (rather than the telemetry buffer sampling interval of 4.167 ms) must elapse between the issuance of LVDC PIOs to assure valid telemetry (i.e., at least one transmission of each word).

#### 11.4.1.1 LVDC Regularly Scheduled Telemetry

Regularly scheduled telemetry must be issued at regular time intervals. For most data (in particular, the navigation quantities), it is adequate to specify that telemetry be issued when the quantities are computed, since they are computed at regular intervals. Other data, for example, the start of a new time base, does not change at regular intervals, yet the requirement is for telemetry at regular intervals.

34------------------------------------------------

#### 11.4.1.1.1 Navigation Telemetry

Navigation computations are made once per boost major loop (BML) during the boost mode and once per 8 seconds during orbital mode. All specified navigation quantities must be telemetered in the computation cycle in which they are computed. These quantities include the position, velocity, and acceleration vectors due to thrust, gravitation, and drag (orbital mode only).

#### 11.4.1.1.2 Guidance Quantities

The guidance commands, the changes in the guidance commands and the time that these updates are valid, and the attitude of the vehicle must be telemetered once per BML during boost mode and once per second during orbital mode.

#### 11.4.1.1.3 Accelerometer Data

The accelerometers must be read in both boost and orbital modes. In boost, the accelerometer readings are used in navigation computations; in orbit, they are read for telemetry purposes only.

In boost, the accelerometers must be read twice during each BML. The first set of readings must be those used in the navigation computations. The time,  $T_{as}$ , associated with these readings must mark the beginning of the boost major loop. The other set of readings must match those forced onto the DOM for telemetry. The same real time clock reading, taken after the extra set has been read and stored, must be associated with each set of readings.

35------------------------------------------------

In orbital mode, the accelerometer readings made for DOM telemetry must be transmitted once per second via LVDC telemetry. In addition, the accelerometers must be sampled and compressed as discussed in Section 11.5.2.1.

#### 11.4.1.1.4 IGM Guidance Parameters

All Iterative Guidance Mode (IGM) quantities specified in the Telemetry Tags Table 11-8 must be telemetered when computed. This means that when a quantity is no longer computed (which is the case for  $K_1$ ,  $K_2$ ,  $K_3$ ,  $K_4$ , at the initiation of  $\tilde{\chi}$ -steering guidance), it is no longer necessary to telemeter it.

#### 11.4.1.1.5 Mode Code Words

Mode code words 24, 25, and 27 are special status words. The criteria for setting the bits of each word are discussed in the applicable sections of the EDD. A summary of the significance of each bit is given in the Mode Code Description Table 11-13.

Mode code words 24, 25, and 27 must be telemetered every BML during the boost mode and once per second during orbital mode. The telemetry of these words must be done in such a way as to ensure that the telemetered status of these words reflects the events in the computation cycle from which they are telemetered.

Since the real time telemetry of the mode code words is transmitted from the Goddard Network to MCC in eight-bit segments, bits 24 and 25 of MC24 and MC27 must not be assigned for a particular mission unless absolutely necessary. \*

36------------------------------------------------

#### 11.4.1.1.6 Special Telemetry

The time at which the current time base started must be telemetered to allow an accurate determination of switch selector command times. This information must be telemetered once every BML during boost mode and once per second during orbital mode, but will change only when a new time base begins.

The special codes shown in Table 11-2 are necessary to adequately mark the end of the telemetry during each computation cycle and to mark the beginning of the telemetry for the next cycle.

TABLE 11-2 ETC/BTC TELEMETRY

<table border="1"><thead><tr><th>Symbol</th><th>Data Word</th></tr></thead><tbody><tr><td>ETC (end of telemetry cycle)</td><td>7 0 7 0 7 0 7 0 6</td></tr><tr><td>BTC (begin telemetry cycle)</td><td>Computation cycle counter</td></tr></tbody></table>

The ETC word must be telemetered immediately after all LVDC telemetry is completed in each BML in boost mode or in each one second interval in orbital mode; the BTC word must be telemetered just prior to the beginning of the telemetry cycle. The redundancy is necessary since interrupts can destroy these LVDC words.

The discrete input register (DIR), the internal control register (ICR), and the discrete output register (DOR), must be telemetered once during each BML in boost mode and once per second during orbital mode.

The error monitor register (EMR) will be read into the LVDC each time it is PIOed. All readings of the EMR within each BML in boost mode must be ORed together into a single word

37------------------------------------------------

and telemetered in the same BML, at which time the ORed word must be reset. In orbital mode, a one second accumulation and an eight second accumulation of ORed EMR readings must be telemetered at a regular once per second frequency and once per eight second frequency, respectively. The one second accumulated ORed word must be reset after the telemetry is issued. The eight second accumulation must continue to be ORed with subsequent EMR readings. When a dump station loss is processed it must be reset. \*

#### 11.4.1.2 LVDC On Occurrence Telemetry

Special telemetry is required when events significant to the flight sequence occur. The occurrence of these events is indicated by the setting of particular bits in discrete hardware registers as outputs from the LVDC (discrete output register or switch selector register) and as inputs to LVDC (discrete input register, interrupt storage register or switch selector feedback register).

In addition, the start time of each time base in conjunction with a special indication of whether the time base was initiated nominally or on the backup test must be telemetered.

##### 11.4.1.2.1 Discrete Register Inputs and Outputs

An indication of the discrete inputs (DIs) given in Table 11-3 must be telemetered when detected in the BML during boost mode.

38------------------------------------------------

TABLE 11-3 DISCRETE INPUTS

<table border="1">
<thead>
<tr>
<th>Discrete Input</th>
<th>Function</th>
</tr>
</thead>
<tbody>
<tr>
<td>DI5</td>
<td>S-IVB Engine Out "A"</td>
</tr>
<tr>
<td>DI14</td>
<td>S-IB Outboard Engine Out</td>
</tr>
<tr>
<td>DI15</td>
<td>S-IB Inboard Engine Out</td>
</tr>
<tr>
<td>DI21</td>
<td>S-IB Low Level Sensors Dry "B"</td>
</tr>
<tr>
<td>DI22</td>
<td>Manual Initiation of S-IVB Engine Cutoff "B"</td>
</tr>
<tr>
<td>DI23</td>
<td>S-IB Outboard Engines Cutoff "B"</td>
</tr>
<tr>
<td>DI24</td>
<td>Liftoff</td>
</tr>
</tbody>
</table>

\*  
\*  
\*

An indication of the discrete outputs (DOs) given in Table 11-4 must be telemetered when set or reset by the flight program.

TABLE 11-4 DISCRETE OUTPUTS

<table border="1">
<thead>
<tr>
<th>Discrete Output</th>
<th>Function</th>
</tr>
</thead>
<tbody>
<tr>
<td>DO4 and DO6</td>
<td>Guidance Reference Failure</td>
</tr>
<tr>
<td>DO12</td>
<td>LVDA/LVDC Firing Commit Enable</td>
</tr>
<tr>
<td>DO13</td>
<td>LVDA/LVDC Firing Commit Inhibit</td>
</tr>
</tbody>
</table>

An indication of the interrupts (INTs) given in Table 11-5 must be telemetered when recognized by the flight program.

TABLE 11-5 INTERRUPTS

<table border="1">
<thead>
<tr>
<th>Interrupt</th>
<th>Function</th>
</tr>
</thead>
<tbody>
<tr>
<td>INT2</td>
<td>S-IB Low Level Sensors Dry "A"</td>
</tr>
<tr>
<td>INT4</td>
<td>S-IVB Engine Out "B"</td>
</tr>
<tr>
<td>INT5</td>
<td>S-IB Outboard Engines Cutoff "A"</td>
</tr>
<tr>
<td>INT6</td>
<td>Manual Initiation of S-IVB Engine Cutoff "A"</td>
</tr>
<tr>
<td>INT7</td>
<td>Guidance Reference Release</td>
</tr>
<tr>
<td>INT9</td>
<td>Simultaneous Memory Error (TLC)</td>
</tr>
</tbody>
</table>

\*  
\*  
\*  
\*  
\*  
\*

39------------------------------------------------

The telemetry of the DIs and INTs requires unique ID codes which are specified in the Telemetry Tags Table 11-8. The telemetry of the DOs requires the contents of the DOR as the ID code. These ID codes, left justified, must be contained in the 13 high order bits of the telemetry word. The 13 low order bits must contain the real time clock reading at the time that the flight program detected the event. The required format is defined in Figure 11-1.

<table border="1">
<thead>
<tr>
<th colspan="2">Accumulator Bit Positions</th>
</tr>
</thead>
<tbody>
<tr>
<td>S1 ← 12</td>
<td>13 ← 25</td>
</tr>
<tr>
<td>ID Code (left justified)</td>
<td>Real Time Clock Reading</td>
</tr>
</tbody>
</table>

Figure 11-1

In the event of a switch selector verification failure, the contents of the feedback register must be telemetered.

The stage, address, and real time clock reading at the time the read command is issued must be telemetered for each switch selector that is issued. The required format is defined in Figure 11-2.

<table border="1">
<thead>
<tr>
<th colspan="2">Accumulator Bit Positions</th>
</tr>
</thead>
<tbody>
<tr>
<td>S1 ← 12</td>
<td>13 ← 25</td>
</tr>
<tr>
<td>Stage and Address</td>
<td>Time That<br/>Read Command is Issued</td>
</tr>
</tbody>
</table>

Figure 11-2

40------------------------------------------------

#### 11.4.1.2.2 Special Time Base Start Telemetry

The real time clock reading associated with the initiation of TB0, TB1, TB2, and TB3 must be telemetered. If TB1 is initiated on the backup test, a special indication must be telemetered with an associated real time clock reading. The required format is defined in Figure 11-1 and the specific ID codes are given in the Telemetry Tags Table 11-8.

An indication of which two of the four possible conditions initiated TB4 must be telemetered at T4+0. The specific ID codes for each of the four possible conditions are defined in the Telemetry Tags Table 11-8. The codes of the two conditions that initiated TB4 must be ORed together and used as the ID code for telemetry. The required format is defined in Figure 11-1.

#### 11.4.2 LVDA Telemetry

LVDA telemetry data includes nominal minor loop data and information on LVDA registers and hardware interfaces. All LVDA telemetry is issued through the DOM to the telemetry system. The PIOs for all available LVDA telemetry are defined in the Telemetry Tags Table 11-8. The hardware-determined data formats are described in the LVDC/LVDA Programmers manual.<sup>1</sup>

Telemetry of certain LVDA data must be guaranteed at regular intervals or on occurrence. An interval, of at least 8.334 ms since the last LVDC telemetry PIO or the last LVDA PIO, must be provided to ensure that an empty DOM word is available.

---

<sup>1</sup>IBM: Programmer's Operating Manual, Saturn V LVDC, LVDA, Programmable Test Controller, NAS8-11561, August 1, 1965

41------------------------------------------------

#### 11.4.2.1 LVDA Regularly Scheduled Telemetry

From every minor loop, each of the three crossover detector (COD) counter readings (fine or backup, whichever is in use) combined with the respective ladder output must be telemetered.

The following hardware data must be telemetered at least once per BML during boost mode (except during the high speed cutoff loop) and once per second during orbital mode:

- • Internal control register (ICR)
- • Crossover detector and power supply word
- • Error monitor register (EMR)
- • Discrete input register (DIR)
- • X, Y, and Z accelerometer readings
- • X, Y, and Z backup gimbals.

#### 11.4.2.2 LVDA On-Occurrence Telemetry

A special PIO must be issued immediately after the switch selector stage and address have been loaded into the switch selector register to telemeter switch selector and discrete output register driver outputs.

### 11.5 DATA COMPRESSION

During orbital mode, significant time intervals will occur during which the vehicle will be out of electromagnetic view

42------------------------------------------------

of a telemetry dump station. During these intervals, data must be processed and stored in the LVDC for later transmission when the vehicle comes within the acquisition region of the next telemetry dump station. The processing and storing of these data is referred to as data compression.

#### 11.5.1 General Data Compression Requirements

The following requirement must apply for all compressed data:

- • Data compression must be programmed for a minimum of nine hours. (A new compressed data time base may be initiated, if necessary.)
- • Storage requirements must be based on a maximum compression period of 5700 seconds.
- • If the maximum storage allocation in a particular table is exceeded between dump stations, the most recent data must be compressed over the oldest data in the affected table. This condition is called table wraparound.
- • Each data word or set of data words must have stored with it a word containing a unique identification (ID) code and an associated time accurate to 1.1 seconds. The ID code and time must be stored in the same relative locations for all data. The associated time must be the time at which the data is stored.

43------------------------------------------------

### 11.5.2 Data to be Compressed

Compressed data can be conveniently divided into three groups:

- • Group A: time compressed data, in which quantities are sampled and stored at regular intervals
- • Group B: occurrence compressed data, in which data describing certain events are stored when the events occur
- • Group C: amplitude compressed data, in which regularly sampled quantities are stored only when a change is detected.

A summary of the compressed data is given in Table 11-6. The Compressed Data Table 11-14 defines the required data, the ID codes, and the data formats.

TABLE 11-6 COMPRESSED DATA SUMMARY

<table border="1"><thead><tr><th>Group</th><th>Data</th></tr></thead><tbody><tr><td>A</td><td><math>\left\{ \begin{array}{l} \text{Fine gimbal angles} \\ \text{Backup gimbal angles} \\ \text{Accelerometer outputs} \end{array} \right.</math></td></tr><tr><td>B</td><td><math>\left\{ \begin{array}{l} \text{Discrete outputs} \\ \text{TLC HOP constants} \\ \text{Switch selector stages and addresses} \end{array} \right.</math></td></tr><tr><td>C</td><td><math>\left\{ \begin{array}{l} \text{Error monitor register contents} \\ \text{Mode code 24} \\ \text{Discrete inputs} \end{array} \right.</math></td></tr></tbody></table>

44------------------------------------------------

#### 11.5.2.1 Group A: Time Compressed Data

Group A data includes the fine and backup gimbal angle readings and the accelerometer readings.

The fine gimbal angles and backup gimbal angles must be read and stored once every 100 (+7.2, -0.0) seconds. The accelerometers must be read and stored once every 50 (+3.6, -0.0) seconds. Each of the three sets of readings must be stored with a unique ID code. The individual readings (X, Y, and Z) must also be distinguished by ID codes.

#### 11.5.2.2 Group B: Occurrence Compressed Data

Group B data includes discrete outputs (D0), switch selector stages and addresses, and TLC HOP constants.

The discrete output register (DOR) must be stored whenever D04 or D06 is set or reset in the register by the flight program.

The stage and address of each switch selector that is issued must be stored.

The TLC HOP constant must be stored when a TLC (simultaneous memory error) is detected.

Each of the above data words must be identifiable by a unique ID code.

#### 11.5.2.3 Group C: Amplitude Compressed Data

Group C data consists of the error monitor register (EMR) contents, Mode Code 24, and discrete inputs (DI).

45------------------------------------------------

The composite error monitor register word must be stored once every 60 (+4.3, -0.0) seconds if any bit is on. This word is formed by ORing the previous composite EMR word with the contents of the EMR at a once per second rate during the 60 (+4.3, -0.0) second interval. The composite EMR word must be reset to zero after its contents have been stored.

Mode Code 24 must be sampled once per second and stored if any bit has changed during the past second.

The discrete input register (DIR) must be sampled once every 50 (+3.6, -0.0) seconds and stored if a change has occurred.

Each of the above data words must have a unique ID code.

### 11.5.3 Telemetry of Compressed Data

Although the compressed data must be telemetered while in acquisition of telemetry dump stations, it has the lowest priority of all orbital functions. Real time telemetry and processing of interrupts will consume about 25 percent of the time over dump stations. The remaining time can be devoted to telemetering compressed data. This will allow all compressed data tables (1650 items) to be telemetered in 36 seconds.

Table 11-7 shows the storage allocation requirements of each compressed data table. The duplex storage requirement defines the minimum size required for each table in order to contain at least the maximum specified number of samples.

The following ground rules apply to compressed data telemetry:

- • The three low order bits of the mode register must be used to distinguish compressed data from other telemetry. The mode register setting for compressed data must be octal 3.

46------------------------------------------------

- • Empty storage locations in the compressed data tables must not be telemetered.
- • Compressed data must be telemetered in the same order that it is stored in each table.
- • Each compressed data table must be telemetered with a different telemetry PIO tag. (The specific PIO tags are defined in the Telemetry Tags Table 11-8.)
- • All items in any one table must be telemetered with the same telemetry PIO tag. In every case, one complete entry must be telemetered before the next is processed. However, since it is possible that the compressed data dump will have to be terminated before all data is telemetered, the program must telemeter the compressed data in such a way as to ensure that each table is at least partially emptied over each dump station.
- • If an entire compressed data table is telemetered over a dump station, a special code must be telemetered to indicate the end of the table. This cycle must be continued until loss of the current dump station. (The specific PIO codes are defined in the Telemetry Tags Table 11-8.)
- • If an entire table is not telemetered over a telemetry dump station, the oldest data that has not been telemetered must be the first data to be telemetered at the next telemetry dump station. If table wrap-around has occurred during the compression period, the oldest data remaining must be the first data to be telemetered.

47------------------------------------------------

- • If table wraparound occurs in any table during a compression period, the entire table must be dumped over the next dump station, beginning with the oldest data.

TABLE 11-7 COMPRESSED DATA STORAGE REQUIREMENTS

<table border="1">
<thead>
<tr>
<th>Table</th>
<th>Locations per Sample</th>
<th>Maximum No. of Samples</th>
<th>Storage Requirement (Duplex Locations)</th>
</tr>
</thead>
<tbody>
<tr>
<td>2 Gimbal angles</td>
<td>8</td>
<td>59</td>
<td>472</td>
</tr>
<tr>
<td>3 Composite EMR word</td>
<td>2</td>
<td>101</td>
<td>202</td>
</tr>
<tr>
<td>4 Discrete inputs, discrete outputs, switch selectors, TLC, and MC24</td>
<td>2</td>
<td>---</td>
<td>512</td>
</tr>
<tr>
<td>5 Accelerometers</td>
<td>4</td>
<td>116</td>
<td>464</td>
</tr>
<tr>
<td>Total</td>
<td></td>
<td></td>
<td>1650</td>
</tr>
</tbody>
</table>

48------------------------------------------------

TABLE 11-8 TELEMETRY TAGS

<table border="1">
<thead>
<tr>
<th colspan="7">LVDC DATA</th>
</tr>
<tr>
<th>Mode<br/>Reg. Tag</th>
<th>PIO<br/>Tag</th>
<th>PCM<br/>Tag</th>
<th>EDD<br/>Symbol</th>
<th>Description</th>
<th>Frequency<br/>(Segments)</th>
<th>Scaling<br/>Units</th>
</tr>
</thead>
<tbody>
<tr>
<td>0</td>
<td>000</td>
<td>0400</td>
<td>T<sub>as</sub></td>
<td>Time from GRR at accelerometer read time during boost and initiation of orbit navigation pass in orbit</td>
<td>RS<sub>ML</sub>(1-5)<br/>RS<sub>8</sub>(6)</td>
<td>15<br/>sec</td>
</tr>
<tr>
<td>0</td>
<td>001</td>
<td>1400</td>
<td>X<sub>z</sub></td>
<td>Yaw guidance command</td>
<td>C<sub>1</sub>(1-6)</td>
<td>0<br/>pirads</td>
</tr>
<tr>
<td>0</td>
<td>004</td>
<td>0402</td>
<td>N/A</td>
<td>Z accelerometer reading used in navigation computations during boost</td>
<td>RS<sub>ML</sub>(1-5)</td>
<td>N/A<br/>.05m/sec per bit</td>
</tr>
<tr>
<td>0</td>
<td>005</td>
<td>1402</td>
<td>X<sub>x</sub></td>
<td>Roll guidance command</td>
<td>C<sub>1</sub>(1-6)</td>
<td>0<br/>pirads</td>
</tr>
<tr>
<td>0</td>
<td>010</td>
<td>0404</td>
<td>N/A</td>
<td>X accelerometer reading used in navigation computations during boost</td>
<td>RS<sub>ML</sub>(1-5)</td>
<td>N/A<br/>.05m/sec per bit</td>
</tr>
<tr>
<td>0</td>
<td>011</td>
<td>1404</td>
<td>X<sub>y</sub></td>
<td>Pitch guidance command</td>
<td>C<sub>1</sub>(1-6)</td>
<td>0<br/>pirads</td>
</tr>
<tr>
<td>0</td>
<td>014</td>
<td>0406</td>
<td>N/A</td>
<td>Y accelerometer reading used in navigation computations during boost</td>
<td>RS<sub>ML</sub>(1-5)</td>
<td>N/A<br/>.05m/sec per bit</td>
</tr>
<tr>
<td>0</td>
<td>015</td>
<td>1406</td>
<td>Θ<sub>z</sub></td>
<td>Yaw gimbal angle</td>
<td>C<sub>1</sub>(1-6)</td>
<td>0<br/>pirads</td>
</tr>
<tr>
<td>0</td>
<td>021</td>
<td>1410</td>
<td>Θ<sub>x</sub></td>
<td>Roll gimbal angle</td>
<td>C<sub>1</sub>(1-6)</td>
<td>0<br/>pirads</td>
</tr>
<tr>
<td>0</td>
<td>024</td>
<td>0412</td>
<td>λ</td>
<td>Angle from longitude of launch meridian to descending node of the desired orbital plane</td>
<td>C<sub>1</sub>(1)</td>
<td>0<br/>pirads</td>
</tr>
</tbody>
</table>

49------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="7">LVDC DATA</th>
</tr>
<tr>
<th>Mode</th>
<th>PIO</th>
<th>PCM</th>
<th>EDD</th>
<th>Description</th>
<th>Frequency (Segments)</th>
<th>Scaling</th>
<th>Units</th>
</tr>
<tr>
<th>Reg.</th>
<th>Tag</th>
<th>Tag</th>
<th>Symbol</th>
<th></th>
<th></th>
<th></th>
<th></th>
</tr>
</thead>
<tbody>
<tr>
<td>0</td>
<td>025</td>
<td>1412</td>
<td><math>\Theta_y</math></td>
<td>Pitch gimbal angle</td>
<td><math>C_1(1-6)</math></td>
<td>0</td>
<td>pirads</td>
</tr>
<tr>
<td>0</td>
<td>030</td>
<td>0414</td>
<td>TBB</td>
<td>Updated time in current time base</td>
<td><math>RS_1(6)</math></td>
<td>15</td>
<td>sec</td>
</tr>
<tr>
<td>0</td>
<td>031</td>
<td>1414</td>
<td>TB</td>
<td>Time in current time base</td>
<td><math>C_1(1-6)</math></td>
<td>15</td>
<td>sec</td>
</tr>
<tr>
<td>0</td>
<td>034</td>
<td>0416</td>
<td><math>Z_S</math></td>
<td><math>Z_S</math> component of radius vector <math>\bar{R}</math> computed by either boost navigation or orbital navigation</td>
<td><math>C_1(1-6)</math></td>
<td>23</td>
<td>m</td>
</tr>
<tr>
<td>0</td>
<td>035</td>
<td>1416</td>
<td><math>T_D</math></td>
<td>Deviation from nominal GRR time</td>
<td><math>C_1(1)</math></td>
<td>15</td>
<td>sec</td>
</tr>
<tr>
<td>0</td>
<td>041</td>
<td>1420</td>
<td><math>i</math></td>
<td>Inclination of orbital plane</td>
<td><math>C_1(1)</math></td>
<td>0</td>
<td>pirads</td>
</tr>
<tr>
<td>0</td>
<td>044</td>
<td>0422</td>
<td><math>X_S</math></td>
<td><math>X_S</math> component of radius vector <math>\bar{R}</math> computed by either boost navigation or orbital navigation</td>
<td><math>C_1(1-6)</math></td>
<td>23</td>
<td>m</td>
</tr>
<tr>
<td>0</td>
<td>050</td>
<td>0424</td>
<td><math>Y_S</math></td>
<td><math>Y_S</math> component of radius vector <math>\bar{R}</math> computed in boost navigation or orbital navigation</td>
<td><math>C_1(1-6)</math></td>
<td>23</td>
<td>m</td>
</tr>
<tr>
<td>0</td>
<td>054</td>
<td>0426</td>
<td>1/R</td>
<td>Reciprocal of radius</td>
<td><math>C_1(1-6)</math></td>
<td>-22</td>
<td>1/m</td>
</tr>
<tr>
<td>0</td>
<td>060</td>
<td>0430</td>
<td><math>\ddot{Z}_{Gg}</math></td>
<td><math>Z_G</math> component of gravity acceleration</td>
<td><math>C_1(1-6)</math></td>
<td>4</td>
<td>m/sec<sup>2</sup></td>
</tr>
<tr>
<td>0</td>
<td>064</td>
<td>0432</td>
<td><math>\ddot{X}_{Gg}</math></td>
<td><math>X_G</math> component of gravity acceleration</td>
<td><math>C_1(1-6)</math></td>
<td>4</td>
<td>m/sec<sup>2</sup></td>
</tr>
<tr>
<td>0</td>
<td>070</td>
<td>0434</td>
<td><math>\ddot{Y}_{Gg}</math></td>
<td><math>Y_G</math> component of gravity acceleration</td>
<td><math>C_1(1-6)</math></td>
<td>4</td>
<td>m/sec<sup>2</sup></td>
</tr>
</tbody>
</table>

50------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="8">LVDC DATA</th>
</tr>
<tr>
<th>Mode<br/>Reg.</th>
<th>PIO<br/>Tag</th>
<th>PCM<br/>Tag</th>
<th>EDD<br/>Symbol</th>
<th>Description</th>
<th>Frequency<br/>(Segments)</th>
<th>Scal-<br/>ing</th>
<th>Units</th>
</tr>
</thead>
<tbody>
<tr>
<td>0</td>
<td>074</td>
<td>0436</td>
<td>TBA</td>
<td>Time of station acquisition in TB4 including biases (+50ms, -0ms)</td>
<td>C<sub>1</sub>(6)</td>
<td>15</td>
<td>sec</td>
</tr>
<tr>
<td>0</td>
<td>075</td>
<td>1436</td>
<td>S'S</td>
<td>Switch selector function indication at time the read command is issued</td>
<td>O<sub>1</sub>(1-6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>0</td>
<td>100</td>
<td>0440</td>
<td>TLTSEC</td>
<td>Time of position extrapolation</td>
<td>RS<sub>1</sub>(6)</td>
<td>15</td>
<td>sec</td>
</tr>
<tr>
<td>0</td>
<td>105</td>
<td>1442</td>
<td>TBL</td>
<td>Time of station loss in TB4 including biases (+50ms, -0ms)</td>
<td>C<sub>1</sub>(6)</td>
<td>15</td>
<td>sec</td>
</tr>
<tr>
<td>0</td>
<td>110</td>
<td>0444</td>
<td><math>\dot{z}_S</math></td>
<td>Z<sub>S</sub> component of velocity vector <math>\vec{v}</math></td>
<td>C<sub>1</sub>(1-6)</td>
<td>14</td>
<td>m/sec</td>
</tr>
<tr>
<td>0</td>
<td>114</td>
<td>0446</td>
<td><math>\dot{x}_S</math></td>
<td>X<sub>S</sub> component of velocity vector <math>\vec{v}</math></td>
<td>C<sub>1</sub>(1-6)</td>
<td>14</td>
<td>m/sec</td>
</tr>
<tr>
<td>0</td>
<td>120</td>
<td>0450</td>
<td><math>\dot{y}_S</math></td>
<td>Y<sub>S</sub> component of velocity vector <math>\vec{v}</math></td>
<td>C<sub>1</sub>(1-6)</td>
<td>14</td>
<td>m/sec</td>
</tr>
<tr>
<td>0</td>
<td>124</td>
<td>0452</td>
<td>V</td>
<td>Total space-fixed velocity</td>
<td>C<sub>1</sub>(1-6)</td>
<td>14</td>
<td>m/sec</td>
</tr>
<tr>
<td>0</td>
<td>134</td>
<td>0456</td>
<td>Z<sub>S</sub></td>
<td>Z<sub>S</sub> extrapolated position</td>
<td>C<sub>1</sub>(6)</td>
<td>23</td>
<td>m</td>
</tr>
<tr>
<td>0</td>
<td>144</td>
<td>0462</td>
<td>X<sub>S</sub></td>
<td>X<sub>S</sub> extrapolated position</td>
<td>C<sub>1</sub>(6)</td>
<td>23</td>
<td>m</td>
</tr>
<tr>
<td>0</td>
<td>150</td>
<td>0464</td>
<td>Y<sub>S</sub></td>
<td>Y<sub>S</sub> extrapolated position</td>
<td>C<sub>1</sub>(6)</td>
<td>23</td>
<td>m</td>
</tr>
<tr>
<td>0</td>
<td>174</td>
<td>0476</td>
<td>N/A</td>
<td>Real time clock reading associated with T<sub>1</sub> in boost and TLTSEC in orbit</td>
<td>C<sub>1</sub>(1-6)</td>
<td>N/A</td>
<td>qms</td>
</tr>
</tbody>
</table>

\*

\*

51------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="10">LVDC DATA</th>
</tr>
<tr>
<th>Mode<br/>Reg.</th>
<th>PIO<br/>Tag</th>
<th>PCM<br/>Tag</th>
<th>EDD<br/>Symbol</th>
<th>Description</th>
<th>Frequency<br/>(Segments)</th>
<th>Scale-<br/>ing</th>
<th>Units</th>
<th></th>
<th></th>
</tr>
</thead>
<tbody>
<tr>
<td>0</td>
<td>400</td>
<td>0500</td>
<td>T<sub>1i</sub></td>
<td>S-IVB IGM time-to-go before<br/>EMRC</td>
<td>C<sub>1</sub>(3)</td>
<td>10</td>
<td>sec</td>
<td></td>
<td></td>
</tr>
<tr>
<td>0</td>
<td>415</td>
<td>1506</td>
<td>MC24</td>
<td>Mode Code 24</td>
<td>RS<sub>ML</sub>(1-5)</td>
<td>N/A</td>
<td>N/A</td>
<td></td>
<td></td>
</tr>
<tr>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td>RS<sub>1</sub>(6)</td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr>
<td>0</td>
<td>420</td>
<td>0510</td>
<td>MC27</td>
<td>Mode Code 27</td>
<td>RS<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
<td></td>
<td></td>
</tr>
<tr>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td>RS<sub>ML</sub>(1-5)</td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr>
<td>0</td>
<td>421</td>
<td>1510</td>
<td>MC25</td>
<td>Mode Code 25</td>
<td>RS<sub>ML</sub>(1-5)</td>
<td>N/A</td>
<td>N/A</td>
<td></td>
<td></td>
</tr>
<tr>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td>RS<sub>1</sub>(6)</td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr>
<td>0</td>
<td>425</td>
<td>1512</td>
<td>DOR</td>
<td>Discrete output register (DOR)<br/>status word</td>
<td>RS<sub>ML</sub>(1-5)</td>
<td>N/A</td>
<td>N/A</td>
<td></td>
<td></td>
</tr>
<tr>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td>RS<sub>1</sub>(6)</td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr>
<td>0</td>
<td>431</td>
<td>1514</td>
<td>ICR</td>
<td>Internal control register<br/>(ICR) status word</td>
<td>RS<sub>ML</sub>(1-5)</td>
<td>N/A</td>
<td>N/A</td>
<td></td>
<td></td>
</tr>
<tr>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td>RS<sub>1</sub>(6)</td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr>
<td>0</td>
<td>434</td>
<td>0516</td>
<td>Z<sub>4</sub></td>
<td>Z<sub>4</sub> component of radius vector <math>\bar{R}</math></td>
<td>C<sub>1</sub>(3-6)</td>
<td>23</td>
<td>m</td>
<td></td>
<td></td>
</tr>
<tr>
<td>0</td>
<td>435</td>
<td>1516</td>
<td>EMR<sub>1</sub></td>
<td>Error monitor register (EMR)<br/>status word</td>
<td>RS<sub>ML</sub>(1-5)</td>
<td>N/A</td>
<td>N/A</td>
<td></td>
<td></td>
</tr>
<tr>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td>RS<sub>1</sub>(6)</td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
</tbody>
</table>

52------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="10">LVDC DATA</th>
</tr>
<tr>
<th>Mode<br/>Reg.</th>
<th>PIO<br/>Tag</th>
<th>PCM<br/>Tag</th>
<th>EDD<br/>Symbol</th>
<th>Description</th>
<th>Frequency<br/>(Segments)</th>
<th>Scaling</th>
<th>Units</th>
<th></th>
<th></th>
</tr>
</thead>
<tbody>
<tr>
<td>0</td>
<td>441</td>
<td>1520</td>
<td>INT</td>
<td>System interrupt indication:</td>
<td>0<sub>2</sub>(1-6)</td>
<td>N/A</td>
<td>N/A</td>
<td></td>
<td></td>
</tr>
<tr>
<td></td>
<td></td>
<td></td>
<td></td>
<td>
<table border="1">
<thead>
<tr>
<th>Octal</th>
<th>Interrupt</th>
</tr>
</thead>
<tbody>
<tr><td>00000</td><td>INT7</td></tr>
<tr><td>20200</td><td>INT2</td></tr>
<tr><td>30300</td><td>INT5</td></tr>
<tr><td>22220</td><td>INT6</td></tr>
<tr><td>40400</td><td>INT4</td></tr>
<tr><td>35774</td><td>INT9</td></tr>
</tbody>
</table>
</td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr>
<td>0</td>
<td>444</td>
<td>0522</td>
<td>X<sub>4</sub></td>
<td>X<sub>4</sub> component of radius vector <math>\bar{R}</math></td>
<td>C<sub>1</sub>(3-6)</td>
<td>23</td>
<td>m</td>
<td></td>
<td></td>
</tr>
<tr>
<td>0</td>
<td>445</td>
<td>1522</td>
<td>DIR</td>
<td>Discrete input register status</td>
<td>RS<sub>ML</sub>(1-5)</td>
<td>N/A</td>
<td>N/A</td>
<td></td>
<td></td>
</tr>
<tr>
<td></td>
<td></td>
<td></td>
<td></td>
<td></td>
<td>RS<sub>1</sub>(6)</td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr>
<td>0</td>
<td>450</td>
<td>0524</td>
<td>Y<sub>V</sub></td>
<td>Y<sub>V</sub> component of radius vector <math>\bar{R}</math></td>
<td>C<sub>1</sub>(3-4)</td>
<td>23</td>
<td>m</td>
<td></td>
<td></td>
</tr>
<tr>
<td>0</td>
<td>455</td>
<td>1526</td>
<td>TBi</td>
<td>Time base start:</td>
<td>0<sub>2</sub>(1-5)</td>
<td>N/A</td>
<td>N/A</td>
<td></td>
<td></td>
</tr>
<tr>
<td></td>
<td></td>
<td></td>
<td></td>
<td>
<table border="1">
<thead>
<tr>
<th>Octal Code</th>
<th>Time Base</th>
</tr>
</thead>
<tbody>
<tr><td>0000</td><td>TB0</td></tr>
<tr><td>1010</td><td>TB1</td></tr>
<tr><td>2020</td><td>TB2</td></tr>
<tr><td>3030</td><td>TB3</td></tr>
</tbody>
</table>
</td>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
</tbody>
</table>

53------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="7">LVDC DATA</th>
</tr>
<tr>
<th>Mode<br/>Reg.</th>
<th>PIO<br/>Tag</th>
<th>PCM<br/>Tag</th>
<th>EDD<br/>Symbol</th>
<th>Description</th>
<th>Frequency<br/>(Segments)</th>
<th>Scale-<br/>ing</th>
<th>Units</th>
</tr>
</thead>
<tbody>
<tr>
<td colspan="8">
<p>The two conditions on which TB4 was initiated must be indicated by <u>OR</u>ing the associated codes:</p>
</td>
</tr>
<tr>
<td colspan="8">
<table border="1">
<thead>
<tr>
<th>Octal Code</th>
<th>Condition</th>
</tr>
</thead>
<tbody>
<tr>
<td>1000</td>
<td>INT4 (S-IVB Engine Out "B")</td>
</tr>
<tr>
<td>0200</td>
<td>DI5 (S-IVB Engine Out "A")</td>
</tr>
<tr>
<td>0030</td>
<td>Low accelerometer pulse counts</td>
</tr>
<tr>
<td>0004</td>
<td>Guidance cutoff issued</td>
</tr>
</tbody>
</table>
</td>
</tr>
<tr>
<td>0</td>
<td>460</td>
<td>0530</td>
<td>T*</td>
<td>Time-to-go to S-IVB cutoff</td>
<td>C<sub>1</sub>(3-4)</td>
<td>10</td>
<td>sec</td>
</tr>
<tr>
<td>0</td>
<td>461</td>
<td>1530</td>
<td>N/A</td>
<td>HOP save of TLC interrupt</td>
<td>O<sub>1</sub>(1-6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>0</td>
<td>464</td>
<td>0532</td>
<td>T<sub>31</sub></td>
<td>S-IVB IGM time-to-go after EMRC</td>
<td>C<sub>1</sub>(3-4)</td>
<td>10</td>
<td>sec</td>
</tr>
<tr>
<td>0</td>
<td>465</td>
<td>1532</td>
<td>DI</td>
<td>Discrete input indication:</td>
<td>O<sub>2</sub>(1-4)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td colspan="8">
<table border="1">
<thead>
<tr>
<th>Octal Code</th>
<th>Discrete</th>
</tr>
</thead>
<tbody>
<tr>
<td>0002</td>
<td>DI14</td>
</tr>
<tr>
<td>0004</td>
<td>DI15</td>
</tr>
<tr>
<td>1010</td>
<td>DI24</td>
</tr>
<tr>
<td>2020</td>
<td>DI21</td>
</tr>
<tr>
<td>2222</td>
<td>DI22</td>
</tr>
<tr>
<td>3030</td>
<td>DI23</td>
</tr>
<tr>
<td>4040</td>
<td>DI5</td>
</tr>
</tbody>
</table>
</td>
</tr>
<tr>
<td>0</td>
<td>470</td>
<td>0534</td>
<td>Az</td>
<td>Platform azimuth</td>
<td>once (1)</td>
<td>0</td>
<td>pirads</td>
</tr>
</tbody>
</table>

\* \* \* \* \*

54------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="7">LVDC DATA</th>
</tr>
<tr>
<th>Mode</th>
<th>PIO</th>
<th>PCM</th>
<th>EDD</th>
<th>Description</th>
<th>Frequency (Segments)</th>
<th>Scaling Units</th>
</tr>
<tr>
<th>Reg. Tag</th>
<th>Tag</th>
<th></th>
<th>Symbol</th>
<th></th>
<th></th>
<th></th>
</tr>
</thead>
<tbody>
<tr>
<td>0</td>
<td>475</td>
<td>1536</td>
<td>N/A</td>
<td>Indication of Time Base 1 start on the backup test with the following code:</td>
<td>0<sub>2</sub>(1-3)</td>
<td>N/A N/A</td>
</tr>
<tr>
<td colspan="7">
<table border="1">
<thead>
<tr>
<th>Octal Code</th>
<th>Event</th>
</tr>
</thead>
<tbody>
<tr>
<td>1010</td>
<td>TBI</td>
</tr>
</tbody>
</table>
</td>
</tr>
</tbody>
</table>

\* \* \*

\* \*

55------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="8">LVDC DATA</th>
</tr>
<tr>
<th>Mode<br/>Reg.</th>
<th>PIO<br/>Tag</th>
<th>PCM<br/>Tag</th>
<th>EDD<br/>Symbol</th>
<th>Description</th>
<th>Frequency<br/>(Segments)</th>
<th>Scaling</th>
<th>Units</th>
</tr>
</thead>
<tbody>
<tr>
<td>0</td>
<td>500</td>
<td>0540</td>
<td>N/A</td>
<td>SS feedback in error</td>
<td>0<sub>1</sub>(1-6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>0</td>
<td>501</td>
<td>1540</td>
<td>X'<sub>z</sub></td>
<td>Minor loop rate-limited X<sub>z</sub></td>
<td>C<sub>1</sub>(1-6)</td>
<td>0</td>
<td>pirads</td>
</tr>
<tr>
<td>0</td>
<td>505</td>
<td>1542</td>
<td>X'<sub>x</sub></td>
<td>Minor loop rate-limited X<sub>x</sub></td>
<td>C<sub>1</sub>(1-6)</td>
<td>0</td>
<td>pirads</td>
</tr>
<tr>
<td>0</td>
<td>511</td>
<td>1544</td>
<td>X'<sub>y</sub></td>
<td>Minor loop rate-limited X<sub>y</sub></td>
<td>C<sub>1</sub>(1-6)</td>
<td>0</td>
<td>pirads</td>
</tr>
<tr>
<td>0</td>
<td>520</td>
<td>0550</td>
<td>Z''<sub>DS</sub></td>
<td>Z<sub>S</sub> component of drag acceleration</td>
<td>C<sub>1</sub>(6)</td>
<td>4</td>
<td>m/sec<sup>2</sup></td>
</tr>
<tr>
<td>0</td>
<td>524</td>
<td>0552</td>
<td>X''<sub>DS</sub></td>
<td>X<sub>S</sub> component of drag acceleration</td>
<td>C<sub>1</sub>(6)</td>
<td>4</td>
<td>m/sec<sup>2</sup></td>
</tr>
<tr>
<td>0</td>
<td>530</td>
<td>0554</td>
<td>Y''<sub>DS</sub></td>
<td>Y<sub>S</sub> component of drag acceleration</td>
<td>C<sub>1</sub>(6)</td>
<td>4</td>
<td>m/sec<sup>2</sup></td>
</tr>
<tr>
<td>0</td>
<td>534</td>
<td>0556</td>
<td>h</td>
<td>Vehicle altitude</td>
<td>C<sub>1</sub>(6)</td>
<td>19</td>
<td>m</td>
</tr>
<tr>
<td>0</td>
<td>535</td>
<td>1556</td>
<td>EMR<sub>8</sub></td>
<td>Accumulated EMR <u>OR</u>ed during orbital navigation passes</td>
<td>RS<sub>8</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
</tbody>
</table>

56------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="7">LVDC DATA</th>
</tr>
<tr>
<th>Mode<br/>Reg.</th>
<th>PIO<br/>Tag</th>
<th>PCM<br/>Tag</th>
<th>EDD<br/>Symbol</th>
<th>Description</th>
<th>Frequency<br/>(Segments)</th>
<th>Scaling<br/>Units</th>
</tr>
</thead>
<tbody>
<tr>
<td>0</td>
<td>544</td>
<td>0562</td>
<td>T<sub>31</sub></td>
<td>High speed loop time-to-go</td>
<td>C<sub>1</sub>(4)</td>
<td>10 sec</td>
</tr>
<tr>
<td>0</td>
<td>561</td>
<td>1570</td>
<td>TI</td>
<td>Start time of current time base measured from GRR</td>
<td>RS<sub>ML</sub>(1-5)<br/>RS<sub>1</sub>(6)</td>
<td>15 sec</td>
</tr>
<tr>
<td>0</td>
<td>570</td>
<td>0574</td>
<td>N/A</td>
<td>Minor loop error (format specified in Section 8)</td>
<td>O<sub>1</sub>(1-6)</td>
<td>N/A N/A</td>
</tr>
<tr>
<td>0</td>
<td>571</td>
<td>1574</td>
<td>ETC</td>
<td>End of telemetry cycle: Octal 707070706</td>
<td>RS<sub>ML</sub>(1-5)<br/>RS<sub>1</sub>(6)</td>
<td>N/A N/A</td>
</tr>
<tr>
<td>0</td>
<td>575</td>
<td>1576</td>
<td>BTC</td>
<td>Beginning of telemetry cycle</td>
<td>RS<sub>ML</sub>(1-5)<br/>RS<sub>1</sub>(6)</td>
<td>N/A N/A</td>
</tr>
<tr>
<td>1</td>
<td>020</td>
<td>2410</td>
<td><math>\dot{Z}_m</math></td>
<td>Z<sub>S</sub> component of measured velocity</td>
<td>C<sub>1</sub>(1-5)</td>
<td>14 m/sec</td>
</tr>
<tr>
<td>1</td>
<td>024</td>
<td>2412</td>
<td><math>\dot{X}_m</math></td>
<td>X<sub>S</sub> component of measured velocity</td>
<td>C<sub>1</sub>(1-5)</td>
<td>14 m/sec</td>
</tr>
<tr>
<td>1</td>
<td>030</td>
<td>2414</td>
<td><math>\dot{Y}_m</math></td>
<td>Y<sub>S</sub> component of measured velocity</td>
<td>C<sub>1</sub>(1-5)</td>
<td>14 m/sec</td>
</tr>
<tr>
<td>1</td>
<td>074</td>
<td>2436</td>
<td><math>\dot{Z}_{Sg}</math></td>
<td>Z<sub>S</sub> component of gravity velocity</td>
<td>C<sub>1</sub>(1-5)</td>
<td>14 m/sec</td>
</tr>
<tr>
<td>1</td>
<td>100</td>
<td>2440</td>
<td><math>\dot{X}_{Sg}</math></td>
<td>X<sub>S</sub> component of gravity velocity</td>
<td>C<sub>1</sub>(1-5)</td>
<td>14 m/sec</td>
</tr>
<tr>
<td>1</td>
<td>104</td>
<td>2442</td>
<td><math>\dot{Y}_{Sg}</math></td>
<td>Y<sub>S</sub> component of gravity velocity</td>
<td>C<sub>1</sub>(1-5)</td>
<td>14 m/sec</td>
</tr>
</tbody>
</table>

57------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="8">LVDC DATA</th>
</tr>
<tr>
<th>Mode</th>
<th>PIO</th>
<th>PCM</th>
<th>EDD</th>
<th>Description</th>
<th>Frequency (Segments)</th>
<th>Scaling</th>
<th>Units</th>
</tr>
<tr>
<th>Reg.</th>
<th>Tag</th>
<th>Tag</th>
<th>Symbol</th>
<th></th>
<th></th>
<th></th>
<th></th>
</tr>
</thead>
<tbody>
<tr>
<td>1</td>
<td>130</td>
<td>2454</td>
<td><math>\ddot{Z}_m</math></td>
<td>Z<sub>S</sub> component of measured acceleration</td>
<td>C<sub>1</sub>(1-5)</td>
<td>6</td>
<td>m/sec<sup>2</sup></td>
</tr>
<tr>
<td>1</td>
<td>134</td>
<td>2456</td>
<td><math>\ddot{X}_m</math></td>
<td>X<sub>S</sub> component of measured acceleration</td>
<td>C<sub>1</sub>(1-5)</td>
<td>6</td>
<td>m/sec<sup>2</sup></td>
</tr>
<tr>
<td>1</td>
<td>140</td>
<td>2460</td>
<td><math>\ddot{Y}_m</math></td>
<td>Y<sub>S</sub> component of measured acceleration</td>
<td>C<sub>1</sub>(1-5)</td>
<td>6</td>
<td>m/sec<sup>2</sup></td>
</tr>
<tr>
<td>1</td>
<td>144</td>
<td>2462</td>
<td>(F/M)</td>
<td>Total measured acceleration</td>
<td>C<sub>1</sub>(1-5)</td>
<td>6</td>
<td>m/sec<sup>2</sup></td>
</tr>
<tr>
<td>1</td>
<td>425</td>
<td>3512</td>
<td>N/A</td>
<td>Indication of a change in DOR</td>
<td>O<sub>2</sub>(1-6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>1</td>
<td>445</td>
<td>3522</td>
<td><math>\Delta X_z</math></td>
<td>Computed change in X'<sub>Z</sub> per boost major loop</td>
<td>C<sub>1</sub>(1-6)</td>
<td>0</td>
<td>pirads</td>
</tr>
<tr>
<td>1</td>
<td>451</td>
<td>3524</td>
<td><math>\Delta X_x</math></td>
<td>Computed change in X'<sub>X</sub> per boost major loop</td>
<td>C<sub>1</sub>(1-6)</td>
<td>0</td>
<td>pirads</td>
</tr>
<tr>
<td>1</td>
<td>455</td>
<td>3526</td>
<td><math>\Delta X_y</math></td>
<td>Computed change in X'<sub>Y</sub> per boost major loop</td>
<td>C<sub>1</sub>(1-6)</td>
<td>0</td>
<td>pirads</td>
</tr>
<tr>
<td>1</td>
<td>501</td>
<td>3540</td>
<td>N/A</td>
<td>Extra Z accelerometer reading (not required during HSL)</td>
<td>RS<sub>ML</sub>(1-5)<br/>RS<sub>1</sub>(6)</td>
<td>N/A</td>
<td>.05m/sec</td>
</tr>
<tr>
<td>1</td>
<td>505</td>
<td>3542</td>
<td>N/A</td>
<td>Extra X accelerometer reading (not required during HSL)</td>
<td>RS<sub>ML</sub>(1-5)<br/>RS<sub>1</sub>(6)</td>
<td>N/A</td>
<td>.05m/sec</td>
</tr>
<tr>
<td>1</td>
<td>511</td>
<td>3544</td>
<td>N/A</td>
<td>Extra Y accelerometer reading (not required during HSL)</td>
<td>RS<sub>ML</sub>(1-5)<br/>RS<sub>1</sub>(6)</td>
<td>N/A</td>
<td>.05m/sec</td>
</tr>
</tbody>
</table>

\*

58------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="8">LVDC DATA</th>
</tr>
<tr>
<th>Mode<br/>Reg.</th>
<th>PIO<br/>Tag</th>
<th>PCM<br/>Tag</th>
<th>EDD<br/>Symbol</th>
<th>Description</th>
<th>Frequency<br/>(Segments)</th>
<th>Scaling</th>
<th>Units</th>
</tr>
</thead>
<tbody>
<tr>
<td>1</td>
<td>515</td>
<td>3546</td>
<td>N/A</td>
<td>Real time clock reading associated with the extra accelerometer readings (not required during HSL)</td>
<td>RS<sub>ML</sub>(1-5)<br/>RS<sub>1</sub>(6)</td>
<td>N/A</td>
<td>qms</td>
</tr>
<tr>
<td>2</td>
<td>024</td>
<td>4412</td>
<td>DCRTC</td>
<td>Real time clock reading associated with <math>\Delta X</math> updating (not required during HSL)</td>
<td>RS<sub>ML</sub>(1-5)<br/>RS<sub>1</sub>(6)</td>
<td>N/A</td>
<td>qms</td>
</tr>
<tr>
<td>2</td>
<td>150</td>
<td>4464</td>
<td>(M/F)</td>
<td>Reciprocal of total acceleration (F/M)</td>
<td>C<sub>1</sub>(3-4)</td>
<td>-02</td>
<td>sec<sup>2</sup>/m</td>
</tr>
<tr>
<td>2</td>
<td>154</td>
<td>4466</td>
<td>(M/F)<sub>s</sub></td>
<td>Smoothed value of (M/F)</td>
<td>C<sub>1</sub>(3-4)</td>
<td>-02</td>
<td>sec<sup>2</sup>/m</td>
</tr>
<tr>
<td>2</td>
<td>404</td>
<td>4502</td>
<td><math>\tau_1</math></td>
<td>Ratio of mass to mass flow rate for the IGM phase before S-IVB EMRC</td>
<td>C<sub>1</sub>(3)</td>
<td>10</td>
<td>sec</td>
</tr>
<tr>
<td>2</td>
<td>410</td>
<td>4504</td>
<td><math>\tau_3</math></td>
<td>Ratio of mass to mass flow rate for the IGM phase after S-IVB EMRC</td>
<td>C<sub>1</sub>(4)</td>
<td>10</td>
<td>sec</td>
</tr>
<tr>
<td>2</td>
<td>414</td>
<td>4506</td>
<td><math>\phi_T</math></td>
<td>Terminal range angle</td>
<td>C<sub>1</sub>(3-4)</td>
<td>0</td>
<td>pirads</td>
</tr>
<tr>
<td>2</td>
<td>424</td>
<td>4512</td>
<td>X<sub>V</sub></td>
<td>X<sub>V</sub> component of position vector R</td>
<td>C<sub>1</sub>(3-4)</td>
<td>23</td>
<td>m</td>
</tr>
<tr>
<td>2</td>
<td>434</td>
<td>4516</td>
<td><math>\dot{Z}_V</math></td>
<td>Z<sub>V</sub> component of velocity vector <math>\vec{V}</math></td>
<td>C<sub>1</sub>(3-4)</td>
<td>14</td>
<td>m/sec</td>
</tr>
<tr>
<td>2</td>
<td>444</td>
<td>4522</td>
<td><math>\dot{X}_V</math></td>
<td>X<sub>V</sub> component of velocity vector <math>\vec{V}</math></td>
<td>C<sub>1</sub>(3-4)</td>
<td>14</td>
<td>m/sec</td>
</tr>
</tbody>
</table>

59------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="7">LVDC DATA</th>
</tr>
<tr>
<th>Mode<br/>Reg.</th>
<th>PIO<br/>Tag</th>
<th>PCM<br/>Tag</th>
<th>EDD<br/>Symbol</th>
<th>Description</th>
<th>Frequency<br/>(Segments)</th>
<th>Scaling<br/>Units</th>
</tr>
</thead>
<tbody>
<tr>
<td>2</td>
<td>450</td>
<td>4524</td>
<td><math>\dot{Y}_V</math></td>
<td><math>Y_V</math> component of velocity vector</td>
<td>C<sub>1</sub>(3-4)</td>
<td>14 m/sec</td>
</tr>
<tr>
<td>2</td>
<td>454</td>
<td>4526</td>
<td><math>\Delta T'_3</math></td>
<td>Correction to the IGM time-to-go (T<sub>3i</sub>)</td>
<td>C<sub>1</sub>(3-4)</td>
<td>10 sec</td>
</tr>
<tr>
<td>2</td>
<td>464</td>
<td>4532</td>
<td><math>\dot{\Delta Z}_V</math></td>
<td><math>Z_V</math> component of velocity-to-be-gained vector</td>
<td>C<sub>1</sub>(3-4)</td>
<td>14 m/sec</td>
</tr>
<tr>
<td>2</td>
<td>470</td>
<td>4534</td>
<td><math>\dot{\Delta X}_V</math></td>
<td><math>X_V</math> component of velocity-to-be-gained vector</td>
<td>C<sub>1</sub>(3-4)</td>
<td>14 m/sec</td>
</tr>
<tr>
<td>2</td>
<td>474</td>
<td>4536</td>
<td><math>\dot{\Delta Y}_V</math></td>
<td><math>Y_V</math> component of velocity-to-be-gained vector</td>
<td>C<sub>1</sub>(3-4)</td>
<td>14 m/sec</td>
</tr>
<tr>
<td>2</td>
<td>500</td>
<td>4540</td>
<td><math>\ddot{X}_Z</math></td>
<td>Velocity correction in yaw guidance angle</td>
<td>C<sub>1</sub>(3-4)</td>
<td>0 pirads</td>
</tr>
<tr>
<td>2</td>
<td>504</td>
<td>4542</td>
<td><math>\ddot{X}_Y</math></td>
<td>Velocity correction in pitch guidance angle</td>
<td>C<sub>1</sub>(3-4)</td>
<td>0 pirads</td>
</tr>
<tr>
<td>2</td>
<td>510</td>
<td>4544</td>
<td><math>\Delta Y_V</math></td>
<td><math>Y_V</math> component of distance-to-be-gained vector</td>
<td>C<sub>1</sub>(3-4)</td>
<td>23 m</td>
</tr>
<tr>
<td>2</td>
<td>514</td>
<td>4546</td>
<td>K<sub>1</sub></td>
<td>Altitude correction in pitch guidance command</td>
<td>C<sub>1</sub>(3-4)</td>
<td>1 rad</td>
</tr>
<tr>
<td>2</td>
<td>520</td>
<td>4550</td>
<td>K<sub>2</sub></td>
<td>IGM pitch rate correction</td>
<td>C<sub>1</sub>(3-4)</td>
<td>-1 rad/sec</td>
</tr>
<tr>
<td>2</td>
<td>524</td>
<td>4552</td>
<td><math>\Delta X_V</math></td>
<td><math>X_V</math> component of distance-to-be-gained vector</td>
<td>C<sub>1</sub>(3-4)</td>
<td>24 m</td>
</tr>
</tbody>
</table>

60------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="7">LVDC DATA</th>
</tr>
<tr>
<th>Mode<br/>Reg.</th>
<th>PIO<br/>Tag</th>
<th>PCM<br/>Tag</th>
<th>EDD<br/>Symbol</th>
<th>Description</th>
<th>Frequency<br/>(Segments)</th>
<th>Scaling<br/>Units</th>
</tr>
</thead>
<tbody>
<tr>
<td>2</td>
<td>530</td>
<td>4554</td>
<td>K<sub>3</sub></td>
<td>Altitude correction in yaw guidance angle</td>
<td>C<sub>1</sub>(3-4)</td>
<td>1 rad</td>
</tr>
<tr>
<td>2</td>
<td>534</td>
<td>4556</td>
<td>K<sub>4</sub></td>
<td>IGM yaw rate correction</td>
<td>C<sub>1</sub>(3-4)</td>
<td>-2 rad/sec</td>
</tr>
<tr>
<td>2</td>
<td>550</td>
<td>4564</td>
<td>X<sub>z4</sub></td>
<td>Yaw guidance command in target plane system</td>
<td>C<sub>1</sub>(3-4)</td>
<td>0 pirads</td>
</tr>
<tr>
<td>2</td>
<td>554</td>
<td>4566</td>
<td>X<sub>y4</sub></td>
<td>Pitch guidance command in target plane system</td>
<td>C<sub>1</sub>(3-4)</td>
<td>0 pirads</td>
</tr>
<tr>
<td>2</td>
<td>560</td>
<td>4570</td>
<td>SMCZ</td>
<td>Yaw steering misalignment correction</td>
<td>C<sub>1</sub>(3-4)</td>
<td>0 pirads</td>
</tr>
<tr>
<td>2</td>
<td>564</td>
<td>4572</td>
<td>SMCY</td>
<td>Pitch steering misalignment correction</td>
<td>C<sub>1</sub>(3-4)</td>
<td>0 pirads</td>
</tr>
<tr>
<td>3</td>
<td>021</td>
<td>7410</td>
<td>N/A</td>
<td>End of Compressed Data Table 2</td>
<td colspan="2" rowspan="5">
<div style="border: 1px solid black; padding: 5px; display: inline-block;">
                    Telemetered as specified in Section 11.5.3
                </div>
</td>
</tr>
<tr>
<td>3</td>
<td>024</td>
<td>6412</td>
<td>N/A</td>
<td>End of Compressed Data Table 3</td>
</tr>
<tr>
<td>3</td>
<td>025</td>
<td>7412</td>
<td>N/A</td>
<td>End of Compressed Data Table 4</td>
</tr>
<tr>
<td>3</td>
<td>030</td>
<td>6414</td>
<td>N/A</td>
<td>End of Compressed Data Table 5</td>
</tr>
<tr>
<td>3</td>
<td>041</td>
<td>7420</td>
<td>N/A</td>
<td>Compressed Data Table 4</td>
</tr>
<tr>
<td>3</td>
<td>404</td>
<td>6502</td>
<td>N/A</td>
<td>Compressed Data Table 2</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>3</td>
<td>410</td>
<td>6504</td>
<td>N/A</td>
<td>Compressed Data Table 5</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>3</td>
<td>414</td>
<td>6506</td>
<td>N/A</td>
<td>Compressed Data Table 3</td>
<td>N/A</td>
<td>N/A</td>
</tr>
</tbody>
</table>

61------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="8">LVDC DATA</th>
</tr>
<tr>
<th>Mode<br/>Reg.</th>
<th>PIO<br/>Tag</th>
<th>PCM<br/>Tag</th>
<th>EDD<br/>Symbol</th>
<th>Description</th>
<th>Frequency<br/>(Segments)</th>
<th>Scaling</th>
<th>Units</th>
</tr>
</thead>
<tbody>
<tr>
<td>4</td>
<td>030</td>
<td>0415</td>
<td>N/A</td>
<td>DCS mode status word</td>
<td>O<sub>2</sub>(5,6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>055</td>
<td>1427</td>
<td>N/A</td>
<td>DCS error word</td>
<td>O<sub>2</sub>(5,6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>470</td>
<td>0535</td>
<td>N/A</td>
<td>Header word/memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>474</td>
<td>0537</td>
<td>N/A</td>
<td>Word 1 of memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>500</td>
<td>0541</td>
<td>N/A</td>
<td>Word 2 of memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>504</td>
<td>0543</td>
<td>N/A</td>
<td>Word 3 of memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>510</td>
<td>0545</td>
<td>N/A</td>
<td>Word 4 of memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>514</td>
<td>0547</td>
<td>N/A</td>
<td>Word 5 of memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>520</td>
<td>0551</td>
<td>N/A</td>
<td>Word 6 of memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>524</td>
<td>0553</td>
<td>N/A</td>
<td>Word 7 of memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>530</td>
<td>0555</td>
<td>N/A</td>
<td>Word 8 of memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>534</td>
<td>0557</td>
<td>N/A</td>
<td>Word 9 of memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>540</td>
<td>0561</td>
<td>N/A</td>
<td>Word 10 of memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
<tr>
<td>4</td>
<td>544</td>
<td>0563</td>
<td>N/A</td>
<td>Word 11 of memory dump</td>
<td>O<sub>1</sub>(6)</td>
<td>N/A</td>
<td>N/A</td>
</tr>
</tbody>
</table>

\* \* \* \* \*

62------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="7">LVDC DATA</th>
</tr>
<tr>
<th>Mode</th>
<th>PIO</th>
<th>PCM</th>
<th>EDD</th>
<th>Description</th>
<th>Frequency (Segments)</th>
<th>Scaling Units</th>
</tr>
<tr>
<th>Reg.</th>
<th>Tag</th>
<th>Tag</th>
<th>Symbol</th>
<th></th>
<th></th>
<th></th>
</tr>
</thead>
<tbody>
<tr>
<td>4</td>
<td>550</td>
<td>0565</td>
<td>N/A</td>
<td>Word 12 of memory dump</td>
<td>0<sub>1</sub>(6)</td>
<td>N/A N/A</td>
</tr>
<tr>
<td>4</td>
<td>554</td>
<td>0567</td>
<td>N/A</td>
<td>Word 13 of memory dump</td>
<td>0<sub>1</sub>(6)</td>
<td>N/A N/A</td>
</tr>
<tr>
<td>4</td>
<td>560</td>
<td>0571</td>
<td>N/A</td>
<td>Word 14 of memory dump</td>
<td>0<sub>1</sub>(6)</td>
<td>N/A N/A</td>
</tr>
<tr>
<td>4</td>
<td>564</td>
<td>0573</td>
<td>N/A</td>
<td>Word 15 of memory dump</td>
<td>0<sub>1</sub>(6)</td>
<td>N/A N/A</td>
</tr>
<tr>
<td>4</td>
<td>570</td>
<td>0575</td>
<td>N/A</td>
<td>Word 16 of memory dump</td>
<td>0<sub>1</sub>(6)</td>
<td>N/A N/A</td>
</tr>
<tr>
<td>4</td>
<td>574</td>
<td>0577</td>
<td>N/A</td>
<td>DCS data status word</td>
<td>0<sub>2</sub>(5,6)</td>
<td>N/A N/A</td>
</tr>
</tbody>
</table>

\* \* \* \* \*

63------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="4">LVDA DATA</th>
</tr>
<tr>
<th>Program<br/>PIO Tag</th>
<th>PCM<br/>Tag</th>
<th>Description</th>
<th></th>
</tr>
</thead>
<tbody>
<tr>
<td>002</td>
<td>000</td>
<td>Error time word</td>
<td></td>
</tr>
<tr>
<td>006</td>
<td>041</td>
<td>Mode register</td>
<td></td>
</tr>
<tr>
<td>012</td>
<td>042</td>
<td>Discrete output register reset</td>
<td></td>
</tr>
<tr>
<td>016</td>
<td>043</td>
<td>Discrete output register set</td>
<td></td>
</tr>
<tr>
<td>022</td>
<td>044</td>
<td>Internal control register set (not required during HSL)</td>
<td>*</td>
</tr>
<tr>
<td>023</td>
<td>104</td>
<td>Error monitor register (not required during HSL)</td>
<td>*</td>
</tr>
<tr>
<td>026</td>
<td>045</td>
<td>Internal control register reset</td>
<td></td>
</tr>
<tr>
<td>036</td>
<td>047</td>
<td>Switch selector register</td>
<td></td>
</tr>
<tr>
<td>042</td>
<td>050</td>
<td>CIU address</td>
<td></td>
</tr>
<tr>
<td>043</td>
<td>110</td>
<td>Command receiver</td>
<td></td>
</tr>
<tr>
<td>052</td>
<td>052</td>
<td>Switch selector and discrete output drivers</td>
<td></td>
</tr>
<tr>
<td>053</td>
<td>112</td>
<td>Discrete input spares (not required during HSL)</td>
<td>*</td>
</tr>
<tr>
<td>057</td>
<td>113</td>
<td>Discrete input register (not required during HSL)</td>
<td>*</td>
</tr>
<tr>
<td>066</td>
<td>055</td>
<td>Crossover detector and power supply word (not required during HSL)</td>
<td>*</td>
</tr>
<tr>
<td>067</td>
<td>115</td>
<td>CIU feedback</td>
<td></td>
</tr>
<tr>
<td>077</td>
<td>117</td>
<td>Switch selector feedback</td>
<td></td>
</tr>
</tbody>
</table>

64------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="3">LVDA DATA</th>
</tr>
<tr>
<th>Program<br/>PIO Tag</th>
<th>PCM<br/>Tag</th>
<th>Description</th>
</tr>
</thead>
<tbody>
<tr>
<td>103</td>
<td>120</td>
<td>Real time clock reading (not required during HSL)</td>
</tr>
<tr>
<td>107</td>
<td>121</td>
<td>Z accelerometer reading (not required during HSL)</td>
</tr>
<tr>
<td>117</td>
<td>123</td>
<td>Y accelerometer reading (not required during HSL)</td>
</tr>
<tr>
<td>127</td>
<td>125</td>
<td>X accelerometer reading (not required during HSL)</td>
</tr>
<tr>
<td>137</td>
<td>127</td>
<td>Interrupt storage register</td>
</tr>
<tr>
<td>203</td>
<td>140</td>
<td>Backup gimbal 1 (Z) (not required during HSL)</td>
</tr>
<tr>
<td>223</td>
<td>144</td>
<td>Z fine gimbal</td>
</tr>
<tr>
<td>233</td>
<td>146</td>
<td>Y coarse gimbal</td>
</tr>
<tr>
<td>243</td>
<td>150</td>
<td>Z coarse gimbal</td>
</tr>
<tr>
<td>247</td>
<td>151</td>
<td>Crossover detector spare 1</td>
</tr>
<tr>
<td>273</td>
<td>156</td>
<td>Backup gimbal 3 (Y) (not required during HSL)</td>
</tr>
<tr>
<td>303</td>
<td>160</td>
<td>Backup gimbal 2 (X) (not required during HSL)</td>
</tr>
<tr>
<td>313</td>
<td>162</td>
<td>Crossover detector spare 3</td>
</tr>
<tr>
<td>323</td>
<td>176</td>
<td>Spare backup gimbal 4</td>
</tr>
<tr>
<td>327</td>
<td>165</td>
<td>Crossover detector spare 2</td>
</tr>
<tr>
<td>333</td>
<td>166</td>
<td>Spare coarse gimbal 4</td>
</tr>
</tbody>
</table>

\* \* \* \* \*

65------------------------------------------------

TABLE 11-8 TELEMETRY TAGS (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="4">LVDA DATA</th>
</tr>
<tr>
<th>Program<br/>PIO Tag</th>
<th>PCM<br/>Tag</th>
<th></th>
<th>Description</th>
</tr>
</thead>
<tbody>
<tr>
<td>343</td>
<td>170</td>
<td>X coarse gimbal</td>
<td></td>
</tr>
<tr>
<td>353</td>
<td>172</td>
<td>Y fine gimbal</td>
<td></td>
</tr>
<tr>
<td>363</td>
<td>174</td>
<td>X fine gimbal</td>
<td></td>
</tr>
<tr>
<td>367</td>
<td>175</td>
<td>Crossover detector spare 4</td>
<td></td>
</tr>
</tbody>
</table>

66------------------------------------------------

## NOTES ON TABLE 11-8

### LVDC Telemetry Data

The requirements for each LVDC telemetry quantity are specified in Table 11-8 as follows:

- • Unique combination of mode register and PIO tag
- • Unique PCM tags
- • Telemetry frequency: at regular intervals (RS), when computed (C), or on occurrence (O) as defined in Table 11-9
- • Telemetry time intervals: In order to clearly specify the time interval during which data is to be telemetered, it is convenient to subdivide the mission into the six segments defined in Table 11-10.
- • Binary scaling, when applicable
- • Units, when applicable
- • Special codes for PIOs 0441, 0455, 0465, and 0475 to be used as specified in Section 11.4.1.2.

67------------------------------------------------

NOTES ON TABLE 11-8 (CONTINUED)

TABLE 11-9 TELEMETRY FREQUENCY DEFINITION

<table border="1">
<thead>
<tr>
<th>Frequency</th>
<th>Associated Subscript</th>
</tr>
</thead>
<tbody>
<tr>
<td>RS</td>
<td>ML - once per BML<br/>1 - once per second<br/>8 - once per eight seconds</td>
</tr>
<tr>
<td>C</td>
<td>1 - once<br/>5 - five times</td>
</tr>
<tr>
<td>O</td>
<td>1 - once<br/>2 - twice<br/>5 - five times</td>
</tr>
</tbody>
</table>

TABLE 11-10 TELEMETRY SEGMENT DEFINITION

<table border="1">
<thead>
<tr>
<th>Segment</th>
<th>Description</th>
</tr>
</thead>
<tbody>
<tr>
<td>1</td>
<td>T0+0 until T1+0</td>
</tr>
<tr>
<td>2</td>
<td>T1+0 until T3+0</td>
</tr>
<tr>
<td>3</td>
<td>T3+0 until IGM phase change at S-IVB EMRC</td>
</tr>
<tr>
<td>4</td>
<td>IGM phase change at S-IVB EMRC until T4+0</td>
</tr>
<tr>
<td>5</td>
<td>T4+0 until T4+BN<sub>5</sub> (orbit initialize)</td>
</tr>
<tr>
<td>6</td>
<td>T4+BN<sub>5</sub> until end of mission</td>
</tr>
</tbody>
</table>

68------------------------------------------------

## NOTES ON TABLE 11-8 (CONTINUED)

### LVDA Telemetry Data

All LVDA PIO tags, and the corresponding PCM tags, are given.

### Telemetry Format

Table 11-11 shows the format of a 40-bit telemetry word:

- • D1-D26 represent the data in order of MSB (most significant bit) to LSB (least significant bit)
- • T1-T12 represent the PCM tag, in order of MSB to LSB, for LVDC data; T4-T11 represent the PCM tag, right justified, for LVDA data. The specific PCM tags for all data are defined in Table 11-8.

Table 11-12 depicts how the tag bits are determined for both LVDC and LVDA data:

- • RT3-RT6 represent the Real Time Clock bit 3 through 6, in order of LSB to MSB.
- • M1-M3 represent the mode register contents in order of LSB to MSB.
- • A1-A9 represent the PIO tag in order of LSB to MSB. (An attached "c" designates the complement.)

69------------------------------------------------

NOTES ON TABLE 11-8 (CONTINUED)

TABLE 11-11 40-BIT TELEMETRY WORD

<table border="1">
<tr>
<td>1 2 3 4 5 6 7 8 9 0</td>
<td>1 1 1 1 1 1 1 1 1 2</td>
<td>1 2 3 4 5 6 7 8 9 0</td>
<td>2 2 2 2 2 2 2 2 2 3</td>
<td>3 3 3 3 3 3 3 3 3 4</td>
</tr>
<tr>
<td></td>
<td>1 2 3 4 5 6 7 8 9 0</td>
<td></td>
<td>1 2 3 4 5 6 7 8 9 0</td>
<td>1 2 3 4 5 6 7 8 9 0</td>
</tr>
<tr>
<td>T T T</td>
<td>T T T T T T T T T T</td>
<td>VALIDITY</td>
<td>D D D D D D D D D D</td>
<td>D D D D D D D D D D</td>
</tr>
<tr>
<td></td>
<td>1 1 1 1 1 1 1 1 1 2</td>
<td></td>
<td>1 1 1 1 1 1 1 1 1 2</td>
<td>1</td>
</tr>
<tr>
<td>1 2 3</td>
<td>4 5 6 7 8 9 0 1 2</td>
<td></td>
<td>1 2 3 4 5 6 7 8 9 0</td>
<td>1 2 3 4 5 6 7 8 9 0</td>
</tr>
</table>

TABLE 11-12 TAG BITS

<table border="1">
<tr>
<td>Tag Bits</td>
<td>T1</td>
<td>T2</td>
<td>T3</td>
<td>T4</td>
<td>T5</td>
<td>T6</td>
<td>T7</td>
<td>T8</td>
<td>T9</td>
<td>T10</td>
<td>T11</td>
<td>T12</td>
</tr>
<tr>
<td>LVDC Telemetry</td>
<td>M2</td>
<td>M1</td>
<td>A1</td>
<td>A2c<br/>="1"</td>
<td>A8</td>
<td>A9</td>
<td>A7</td>
<td>A6</td>
<td>A5</td>
<td>A4</td>
<td>A3</td>
<td>M3</td>
</tr>
<tr>
<td>LVDA Telemetry</td>
<td>RT5</td>
<td>RT4</td>
<td>RT3</td>
<td>A2c<br/>="0"</td>
<td>A1</td>
<td>A1c<br/>OR<br/>A8</td>
<td>A7</td>
<td>A6</td>
<td>A5</td>
<td>A4</td>
<td>A3</td>
<td>RT6</td>
</tr>
</table>

70------------------------------------------------

TABLE 11-13 MODE CODE DESCRIPTION

<table border="1">
<thead>
<tr>
<th colspan="4">Mode Code 24</th>
</tr>
<tr>
<th>Bit No.</th>
<th>Set Bit Indication</th>
<th>Reset Bit</th>
<th></th>
</tr>
</thead>
<tbody>
<tr>
<td>S<br/>1</td>
<td>Channel disagreement in Z accelerometer reading or unreasonable Z reading</td>
<td>These bits must be reset once per BML during boost mode.</td>
<td></td>
</tr>
<tr>
<td>2<br/>3</td>
<td>Channel disagreement in X accelerometer reading or unreasonable X reading</td>
<td>These bits must be reset once per BML during boost mode.</td>
<td></td>
</tr>
<tr>
<td>4<br/>5</td>
<td>Channel disagreement in Y accelerometer reading or unreasonable Y reading</td>
<td>These bits must be reset once per BML during boost mode.</td>
<td></td>
</tr>
<tr>
<td>6<br/>7</td>
<td>Unreasonable Z (yaw) gimbal angle readings</td>
<td>These bits must never be reset after they have been set.</td>
<td>*<br/>*</td>
</tr>
<tr>
<td>8<br/>9</td>
<td>Unreasonable X (roll) gimbal angle readings</td>
<td>These bits must never be reset after they have been set.</td>
<td>*<br/>*</td>
</tr>
<tr>
<td>10<br/>11</td>
<td>Unreasonable Y (pitch) gimbal angle readings</td>
<td>These bits must never be reset after they have been set.</td>
<td>*<br/>*</td>
</tr>
<tr>
<td>12</td>
<td>Spare</td>
<td></td>
<td></td>
</tr>
<tr>
<td>13</td>
<td>Gimbal Angle Disagreement Bit status</td>
<td>This bit must never be reset once it has been set.</td>
<td></td>
</tr>
<tr>
<td>14</td>
<td>A COD Counter Failure</td>
<td>This bit must never be reset after it has been set.</td>
<td></td>
</tr>
</tbody>
</table>

71------------------------------------------------

TABLE 11-13 MODE CODE DESCRIPTION (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="3">Mode Code 24</th>
</tr>
<tr>
<th>Bit No.</th>
<th><u>Set Bit Indication</u></th>
<th><u>Reset Bit</u></th>
</tr>
</thead>
<tbody>
<tr>
<td>15</td>
<td>B COD Counter Failure</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>16</td>
<td>Continual selection of ladder B</td>
<td>This bit must never be reset once it has been set.</td>
</tr>
<tr>
<td>17</td>
<td>Switch selector feedback error, Channel B selected</td>
<td>This bit must never be reset once it has been set.</td>
</tr>
<tr>
<td>18</td>
<td>Expanded accelerometer zero test in use</td>
<td>This bit must be reset at T3+0.0.</td>
</tr>
<tr>
<td>19</td>
<td>Unacceptable zero reading in Y accelerometer channel.</td>
<td>This bit must be reset once per BML during boost mode.</td>
</tr>
<tr>
<td>20</td>
<td>"A" COD (crossover detector) multiplexer failure</td>
<td rowspan="2">These bits must never be reset after they have been set.</td>
</tr>
<tr>
<td>21</td>
<td>"B" COD (crossover detector) multiplexer failure</td>
</tr>
<tr>
<td>22</td>
<td>Unacceptable zero reading in Z accelerometer channel</td>
<td rowspan="2">These bits must be reset once per BML during boost mode.</td>
</tr>
<tr>
<td>23</td>
<td>Unacceptable zero reading in X accelerometer channel</td>
</tr>
<tr>
<td>24-25</td>
<td>Spare</td>
<td></td>
</tr>
</tbody>
</table>

\*  
\*

72------------------------------------------------

TABLE 11-13 MODE CODE DESCRIPTION (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="3">Mode Code 25</th>
</tr>
<tr>
<th>Bit No.</th>
<th>Set Bit Indication</th>
<th>Reset Bit</th>
</tr>
</thead>
<tbody>
<tr>
<td>S</td>
<td><u>Begin Time Base 0.</u> Guidance Reference Release (INT7)</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>1</td>
<td><u>Begin Time Base 1.</u> Liftoff Discrete (DI24), or vertical acceleration is greater than or equal to the acceleration due to gravity by <math>2 \text{ m/sec}^2</math> for 4 BMLs.</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>2</td>
<td>Initiate S-IB pitch and roll guidance (Vehicle clears LUT or <math>T_c \geq T_{S0}</math>)</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>3</td>
<td>Roll maneuver complete (<math>|\theta_x| \leq 0.5^\circ</math>)</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>4</td>
<td>Spare</td>
<td></td>
</tr>
<tr>
<td>5</td>
<td>Terminate S-IB time-tilt pitch program and freeze pitch command. (<math>T_c \geq T_{ar}</math>)</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>6</td>
<td><u>Begin Time Base 2.</u> S-IB Low Level Sensors Dry "A" (INT2) or S-IB Low Level Sensors Dry "B" (DI21), plus down-range velocity <math>&gt; 500 \text{ m/sec}</math> (bypass in case of GRF).</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>7</td>
<td><u>Begin Time Base 3.</u> S-IB Outboard Engines Cutoff "A" (INT5) or S-IB Outboard Engines Cutoff "B" (DI23).</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>8</td>
<td>S-IB Inboard Engine Out. (DI15 recognized)</td>
<td>This bit must never be reset after it has been set. (This bit is set nominally upon detection of DI15 at S-IB IECO).</td>
</tr>
<tr>
<td>9</td>
<td>S-IB Outboard Engine Out. (DI14 recognized)</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
</tbody>
</table>

\*  
\*

\*  
\*  
\*  
\*  
\*

\*  
\*  
\*

73------------------------------------------------

TABLE 11-13 MODE CODE DESCRIPTION (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="3">Mode Code 25</th>
</tr>
<tr>
<th>Bit No.</th>
<th>Set Bit Indication</th>
<th>Reset Bit</th>
</tr>
</thead>
<tbody>
<tr>
<td>10</td>
<td>Steering misalignment correction (SMC) calculations active.</td>
<td>This bit must be reset during periods in which SMC calculations are inhibited.</td>
</tr>
<tr>
<td>11</td>
<td>Spare</td>
<td></td>
</tr>
<tr>
<td>12</td>
<td>S-IVB IGM Before EMRC. This bit is set on initiation of IGM (<math>T3+T3_{IGM}</math>).</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>13</td>
<td>S-IVB IGM After EMRC. This bit is set when <math>T_{li} \leq 0</math>.</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>14-17</td>
<td>Spare</td>
<td></td>
</tr>
<tr>
<td>18</td>
<td>Manual initiation of S-IVB cutoff. (Recognition of INT6 or DI22)</td>
<td>This bit must never be reset after it has been set. *</td>
</tr>
<tr>
<td>19</td>
<td>Spare</td>
<td></td>
</tr>
<tr>
<td>20</td>
<td>Issued read command for S-IVB Engine Start On switch selector in Time Base 3.</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>21</td>
<td>Spare</td>
<td></td>
</tr>
<tr>
<td>22</td>
<td>Begin terminal (<math>\tilde{X}</math>-Steering) guidance in Time Base 3. (Time-to-go to S-IVB cutoff is less than <math>\epsilon_2</math> seconds.)</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>23</td>
<td>Issued read command for S-IVB Cutoff No. 1 On switch selector in Time Base 3.</td>
<td>This bit must never be reset after it has been set. *</td>
</tr>
<tr>
<td>24</td>
<td>Begin Time Base 4. Recognition of two of the following in Time Base 3:<br/>
                    1) INT4<br/>
                    2) DI5<br/>
                    3) Issued S-IVB cutoff switch selector command<br/>
                    4) Low accelerometer pulse count (velocity change less than or equal to 1 m/sec for one BML)</td>
<td>This bit must never be reset after it has been set. *<br/>*<br/>*</td>
</tr>
</tbody>
</table>

74------------------------------------------------

TABLE 11-13 MODE CODE DESCRIPTION (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="3">Mode Code 25</th>
</tr>
<tr>
<th><u>Bit No.</u></th>
<th><u>Bit Set Indication</u></th>
<th><u>Reset Bit</u></th>
</tr>
</thead>
<tbody>
<tr>
<td>25</td>
<td>TLC (INT9), simultaneous memory error</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
</tbody>
</table>

75------------------------------------------------

TABLE 11-13 MODE CODE DESCRIPTION (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="3">Mode Code 27</th>
</tr>
<tr>
<th>Bit No.</th>
<th>Bit Set Indication</th>
<th>Reset Bit</th>
</tr>
</thead>
<tbody>
<tr>
<td>S</td>
<td>Powered flight DCS inhibit removed</td>
<td>This bit must be reset each time the DCS is inhibited.</td>
</tr>
<tr>
<td>1-7</td>
<td>Spare</td>
<td></td>
</tr>
<tr>
<td>8</td>
<td>Navigation Update DCS command accepted</td>
<td>This bit must be reset after the update is incorporated.</td>
</tr>
<tr>
<td>9</td>
<td>Time Base Update DCS command accepted</td>
<td>The state of this bit will initially be zero and must be changed each time an update is accepted.</td>
</tr>
<tr>
<td>10</td>
<td>Local reference maneuver in progress</td>
<td>This bit must be reset when bit 11, 12, 16 or 17 of MC27 is set.</td>
</tr>
<tr>
<td>11</td>
<td>Inertial attitude hold in progress</td>
<td>This bit must be reset when bit 10, 12, 16 or 17 of MC27 is set.</td>
</tr>
<tr>
<td>12</td>
<td>S/C in control (DI9 recognized)</td>
<td>This bit must be reset when control is returned to the LVDC. If GRF occurs while this bit is set, it must remain set.</td>
</tr>
<tr>
<td>13</td>
<td>Spare</td>
<td></td>
</tr>
<tr>
<td>14</td>
<td>Guidance Reference Failure discretes (D04/D06) are set.</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>15</td>
<td>S/C has assumed control of the Saturn after GRF detection (DI9 recognized after D04 and D06 set.)</td>
<td>This bit must never be reset after it has been set.</td>
</tr>
<tr>
<td>16</td>
<td>Tracking local reference after S/C control (This bit remains set until time for next programmed maneuver.)</td>
<td>This bit must be reset when bit 10, 11, 12 or 17 of MC27 is set.</td>
</tr>
<tr>
<td>17</td>
<td>Inertial attitude hold maneuver after S/C control (This bit remains set until time for next programmed maneuver.)</td>
<td>This bit must be reset when bit 10, 11, 12 or 16 of MC27 is set.</td>
</tr>
</tbody>
</table>

76------------------------------------------------

TABLE 11-13 MODE CODE DESCRIPTION (CONTINUED)

<table border="1">
<thead>
<tr>
<th colspan="3">Mode Code 27</th>
</tr>
<tr>
<th><u>Bit No.</u></th>
<th><u>Bit Set Indication</u></th>
<th><u>Reset Bit</u></th>
</tr>
</thead>
<tbody>
<tr>
<td>18</td>
<td>Water Control Valve Logic Inhibit DCS command not accepted (This bit is nominally set.)</td>
<td>This bit must be reset upon acceptance of the Water Control Valve Logic Inhibit DCS command. This bit will never be set after it has been reset.</td>
</tr>
<tr>
<td>19</td>
<td>Execute Generalized Maneuver DCS command accepted</td>
<td>This bit must be reset upon acceptance of a Return to Nominal Timeline DCS command or Execute Special Maneuver command.</td>
</tr>
<tr>
<td>20-25</td>
<td>Spare</td>
<td></td>
</tr>
</tbody>
</table>

\*  
\*

77------------------------------------------------

TABLE 11-14 COMPRESSED DATA

<table border="1">
<thead>
<tr>
<th>Group</th>
<th>Description</th>
<th>Sample Rate</th>
<th>Table No.</th>
<th>Word No.</th>
<th>LVDC Bit Assignment</th>
<th>Bit Description</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="4">A</td>
<td rowspan="4">Three Fine Gimbal Angles</td>
<td rowspan="4">Once every 100 (+7.2, -0.0) seconds</td>
<td rowspan="2">2</td>
<td>1</td>
<td>S 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5<br/>0 0 0 0 0 0 0 0 1 T T T T T T T T T T T T T T</td>
<td>Bits S-9 - Identification Code<br/>T - Time from GRR in 1/2 sec per bit</td>
</tr>
<tr>
<td>2</td>
<td>0 0 1 D A A A A A A A A A A B B B B B B B B B B</td>
<td>Bits S-2 - Identification Code<br/>D - Disagreement Bit<br/>A - Yaw Fine Gimbal Angle Reading (Channel A)<br/>B - Yaw Fine Gimbal Angle Reading (Channel B)</td>
</tr>
<tr>
<td rowspan="2">3</td>
<td>1</td>
<td>0 1 0 D A A A A A A A A A A B B B B B B B B B B</td>
<td>Bits S-2 - Identification Code<br/>D - Disagreement Bit<br/>A - Roll Fine Gimbal Angle Reading (Channel A)<br/>B - Roll Fine Gimbal Angle Reading (Channel B)</td>
</tr>
<tr>
<td>2</td>
<td>0 1 1 D A A A A A A A A A A B B B B B B B B B B</td>
<td>Bits S-2 - Identification Code<br/>D - Disagreement Bit<br/>A - Pitch Fine Gimbal Angle Reading (Channel A)<br/>B - Pitch Fine Gimbal Angle Reading (Channel B)</td>
</tr>
</tbody>
</table>

\*

78------------------------------------------------

TABLE II-14 COMPRESSED DATA (CONTINUED)

<table border="1">
<thead>
<tr>
<th>Group</th>
<th>Description</th>
<th>Sample Rate</th>
<th>Table No.</th>
<th>Word No.</th>
<th>LVDC Bit Assignment</th>
<th>Bit Description</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="3">A</td>
<td rowspan="3">Three Back-up Gimbal Angles</td>
<td rowspan="3">Once every 100 (+7.2, -0.0) seconds</td>
<td rowspan="3">2</td>
<td>5</td>
<td>S 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5<br/>1 0 0 0 0 0 0 0 1 0 T T T T T T T T T T T T T T T T T T T T</td>
<td>Bits S-9 - Identification Code<br/>T - Time from GRR in 1/2 sec per bit</td>
</tr>
<tr>
<td>6</td>
<td>1 0 1 D A A A A A A A A A A A A B B B B B B B B B B B B B B B B</td>
<td>Bits S-2 - Identification Code<br/>D - Disagreement Bit<br/>A - Yaw Backup Gimbal Angle Reading (Channel A)<br/>B - Yaw Backup Gimbal Angle Reading (Channel B)</td>
</tr>
<tr>
<td>7</td>
<td>1 1 0 D A A A A A A A A A A A A B B B B B B B B B B B B B B B B</td>
<td>Bits S-2 - Identification Code<br/>D - Disagreement Bit<br/>A - Roll Backup Gimbal Angle Reading (Channel A)<br/>B - Roll Backup Gimbal Angle Reading (Channel B)</td>
</tr>
<tr>
<td></td>
<td></td>
<td></td>
<td></td>
<td>8</td>
<td>1 1 1 D A A A A A A A A A A A A B B B B B B B B B B B B B B B B</td>
<td>Bits S-2 - Identification Code<br/>D - Disagreement Bit<br/>A - Pitch Backup Gimbal Angle Reading (Channel A)<br/>B - Pitch Backup Gimbal Angle Reading (Channel B)</td>
</tr>
</tbody>
</table>

\*

79------------------------------------------------

TABLE 11-14 COMPRESSED DATA (CONTINUED)

<table border="1">
<thead>
<tr>
<th>Group</th>
<th>Description</th>
<th>Sample Rate</th>
<th>Table No.</th>
<th>Word No.</th>
<th>LVDC Bit Assignment</th>
<th>Bit Description</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="4">A</td>
<td rowspan="4">X, Y, Z Accelerometers</td>
<td rowspan="4">Once every 50 (+3.6, -0.0) seconds</td>
<td rowspan="4">5</td>
<td>1</td>
<td>S 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5<br/>0 0 0 0 0 0 0 0 1 1 T T T T T T T T T T T T T T</td>
<td>Bits S-9 - Identification Code<br/>T - Time from GRR in 1/2 sec per bit</td>
</tr>
<tr>
<td>2</td>
<td>0 1 A A A A A A A A A A A A B B B B B B B B B B B B</td>
<td>Bits S-1 - Identification Code<br/>A - Z Accelerometer Channel A<br/>B - Z Accelerometer Channel B</td>
</tr>
<tr>
<td>3</td>
<td>1 0 A A A A A A A A A A A A B B B B B B B B B B B B</td>
<td>Bits S-1 - Identification Code<br/>A - X Accelerometer Channel A<br/>B - X Accelerometer Channel B</td>
</tr>
<tr>
<td>4</td>
<td>1 1 A A A A A A A A A A A A B B B B B B B B B B B B</td>
<td>Bits S-1 - Identification Code<br/>A - Y Accelerometer Channel A<br/>B - Y Accelerometer Channel B</td>
</tr>
</tbody>
</table>

80------------------------------------------------

TABLE 11-14 COMPRESSED DATA (CONTINUED)

<table border="1">
<thead>
<tr>
<th>Group</th>
<th>Description</th>
<th>Sample Rate</th>
<th>Table No.</th>
<th>Word No.</th>
<th>LVDC Bit Assignment</th>
<th>Bit Description</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="4">B</td>
<td rowspan="2">DO4 and DO6</td>
<td rowspan="2">At Occurrence</td>
<td rowspan="2">4</td>
<td>1</td>
<td>S 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5</td>
<td>Bits S-9 - Identification Code<br/>T - Time from GRR in 1/2 sec per bit</td>
</tr>
<tr>
<td>2a</td>
<td>0 0 0 0 0 0 1 0 0 1 T T T T T T T T T T T T T T T</td>
<td>Bits S-2 - Identification Code<br/>D - DO13 - DO1</td>
</tr>
<tr>
<td rowspan="2">Switch Selector Stage and Address</td>
<td rowspan="2">At Occurrence</td>
<td rowspan="2">4</td>
<td>5</td>
<td>0 0 1 D D D D D D D D D D D 0 0 0 0 0 0 0 0 0 0</td>
<td>Bits S-9 - Identification Code<br/>T - Time from GRR in 1/2 sec per bit</td>
</tr>
<tr>
<td>6</td>
<td>1 0 0 0 0 0 1 0 1 1 T T T T T T T T T T T T T T T</td>
<td>Bits S-2 - Identification Code<br/>S - Stage<br/>A - Address</td>
</tr>
<tr>
<td rowspan="2">TLC HOP Constant Identifier</td>
<td rowspan="2">At Occurrence</td>
<td rowspan="2">4</td>
<td>7</td>
<td>1 0 1 0 0 S S S S S A A A A A A 0 0 0 0 0 0 0 0</td>
<td>Bits S-9 - Identification Code<br/>T - Time from GRR in 1/2 sec per bit</td>
</tr>
<tr>
<td>8</td>
<td>1 1 0 0 0 0 1 0 1 0 T T T T T T T T T T T T T T T</td>
<td>Bits S-2 - Identification Code<br/>A - Instruction Module<br/>B - Data Sector<br/>C - Data Module<br/>D - Instruction Address<br/>E - Instruction Sector<br/>S - Instruction Syllable</td>
</tr>
</tbody>
</table>

81------------------------------------------------

TABLE 11-14 COMPRESSED DATA (CONTINUED)

<table border="1">
<thead>
<tr>
<th>Group</th>
<th>Description</th>
<th>Sample Rate</th>
<th>Table No.</th>
<th>Word No.</th>
<th>LVDC Bit Assignment</th>
<th>Bit Description</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="4">C</td>
<td rowspan="2">Composite Error Monitor Register</td>
<td rowspan="2">Once every 60 (+4.3, -0.0) seconds if a change has occurred</td>
<td rowspan="2">3</td>
<td>1</td>
<td>S 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5<br/>1 0 0 0 0 0 1 1 0 T T T T T T T T T T T T T T</td>
<td>Bits S-9 - Identification Code<br/>T - Time from GRR in 1/2 sec per bit</td>
</tr>
<tr>
<td>2</td>
<td>E E E E E E E E E E E E E E E E E E E E E E E E E E</td>
<td>E - EMR Image</td>
</tr>
<tr>
<td rowspan="2">Discrete Inputs</td>
<td rowspan="2">Once every 50 (+3.6, -0.0) seconds if a change has occurred</td>
<td rowspan="2">4</td>
<td>1</td>
<td>0 0 0 0 0 0 1 0 0 1 T T T T T T T T T T T T T T</td>
<td>Bits S-9 - Identification Code<br/>T - Time from GRR in 1/2 sec per bit</td>
</tr>
<tr>
<td>2b</td>
<td>0 0 1 D D D D D D D D D D D D D D D D D D D 1 1</td>
<td>Bits S-2 - Identification Code<br/>D - DI21 - DI1</td>
</tr>
<tr>
<td rowspan="2">Mode Code 24</td>
<td rowspan="2">Mode Code 24</td>
<td rowspan="2">Once every second if a change has occurred</td>
<td rowspan="2">4</td>
<td>3</td>
<td>0 1 0 0 0 0 1 0 0 0 T T T T T T T T T T T T T T</td>
<td>Bits S-9 - Identification Code<br/>T - Time from GRR in 1/2 sec per bit</td>
</tr>
<tr>
<td>4</td>
<td>0 1 1 S S S S S S S S S S S S S S S S S S S S S S</td>
<td>Bits S-2 - Identification Code<br/>S - Bits S-22 of MC24</td>
</tr>
</tbody>
</table>

\*

\*