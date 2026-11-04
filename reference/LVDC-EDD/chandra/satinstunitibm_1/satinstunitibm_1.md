**IBM**

*C.E. Relf Jr. 43072*

## **Saturn Instrument Unit**

**LVDC Equation Defining Document (EDD)  
for the Saturn IB Flight Program  
Parts I and II  
Revision A**

**11 April 1972**

**MSFC-DRL-008A**

**Line Item 083**

**IBM No. 70-207-0001**

![A large, circular Saturn Instrument Unit (SIU) is shown from a high-angle perspective. The unit is a massive, dark, cylindrical structure with a thick, ribbed outer ring and a smooth inner surface. It is supported by several large, black, tripod-style stands. Inside the unit, several people in hard hats and work clothes are visible, appearing small in comparison to the massive structure. The unit is set against a solid black background.](ebe7fc884ef25c355c97dadabf4e401f_9_img.webp)

1------------------------------------------------

CONTRACT NO. NAS8-14000

MSFC-DRL-008A

Line Item 083

IBM No. 70-207-0001

Prepared for the  
GEORGE C. MARSHALL  
SPACE FLIGHT CENTER  
Huntsville, Alabama

LVDC Equation Defining Document (EDD)  
for the Saturn IB Flight Program  
Parts I and II  
Revision A

11 April 1972

Classification and Content Approval

WD Carson

Data Manager Approval

Henry Rockowski

Program Office Approval

J-E LQ

**IBM**

Federal Systems Division, Electronics Systems Center/Huntsville, Alabama

2------------------------------------------------

PART I

TABLE OF CONTENTS

<table border="0">
<thead>
<tr>
<th>Paragraph</th>
<th>Title</th>
<th>Page</th>
</tr>
</thead>
<tbody>
<tr>
<td>N/A</td>
<td>EDD CONFIGURATION CONTROL PAGE LISTING</td>
<td>viii</td>
</tr>
<tr>
<td>SECTION 1:</td>
<td>GENERAL MISSION DESCRIPTION AND PROGRAM REQUIREMENTS</td>
<td>--</td>
</tr>
<tr>
<td>1.1</td>
<td>INTRODUCTION</td>
<td>I-1-1</td>
</tr>
<tr>
<td>1.2</td>
<td>TYPICAL MISSION REQUIREMENTS</td>
<td>1</td>
</tr>
<tr>
<td>1.3</td>
<td>GENERAL FLIGHT PROGRAM REQUIREMENTS</td>
<td>2</td>
</tr>
<tr>
<td>1.3.1</td>
<td><u>Applicable Document List</u></td>
<td>2</td>
</tr>
<tr>
<td>1.3.2</td>
<td><u>Program Functional Requirements</u></td>
<td>3</td>
</tr>
<tr>
<td>1.3.3</td>
<td><u>Requirements Interaction</u></td>
<td>15</td>
</tr>
<tr>
<td>1.4</td>
<td>PROGRAMMING GROUND RULES</td>
<td>16</td>
</tr>
<tr>
<td>SECTION 2:</td>
<td>REFERENCE SYSTEMS AND TRANSFORMATIONS</td>
<td>--</td>
</tr>
<tr>
<td>2.1</td>
<td>INTRODUCTION</td>
<td>I-2-1</td>
</tr>
<tr>
<td>2.2</td>
<td>REFERENCE SYSTEMS</td>
<td>1</td>
</tr>
<tr>
<td>2.2.1</td>
<td><u>Plumbline Coordinate System</u><br/><u><math>(X_S, Y_S, Z_S)</math></u></td>
<td>1</td>
</tr>
<tr>
<td>2.2.2</td>
<td><u>Target Plane Coordinate System</u><br/><u><math>(X_4, Y_4, Z_4)</math></u></td>
<td>2</td>
</tr>
<tr>
<td>2.2.3</td>
<td><u>Injection Plane Coordinate System</u><br/><u><math>(X_V, Y_V, Z_V)</math></u></td>
<td>2</td>
</tr>
<tr>
<td>2.2.4</td>
<td><u>Gravitational Coordinate System</u><br/><u><math>(X_G, Y_G, Z_G)</math></u></td>
<td>7</td>
</tr>
<tr>
<td>2.2.5</td>
<td><u>Ephemerid Coordinate System</u><br/><u><math>(X_E, Y_E, Z_E)</math></u></td>
<td>7</td>
</tr>
<tr>
<td>2.2.6</td>
<td><u>Telemetry Station Coordinate System</u><br/><u><math>(X_A, Y_A, Z_A)</math></u></td>
<td>8</td>
</tr>
<tr>
<td>2.2.7</td>
<td><u>Body Coordinate System <math>(X_B, Y_B, Z_B)</math></u></td>
<td>8</td>
</tr>
<tr>
<td>2.2.8</td>
<td><u>Inertial Platform Gimbal System</u><br/><u><math>(X_P, Y_P, Z_P)</math></u></td>
<td>8</td>
</tr>
</tbody>
</table>

3------------------------------------------------

<table>
<tr>
<td>2.3</td>
<td>TRANSFORMATION MATRICES</td>
<td>10</td>
</tr>
<tr>
<td>2.3.1</td>
<td><u>[MSG] Matrix</u></td>
<td>14</td>
</tr>
<tr>
<td>2.3.2</td>
<td><u>[MG4] Matrix</u></td>
<td>14</td>
</tr>
<tr>
<td>2.3.3</td>
<td><u>[MBS] Matrix</u></td>
<td>15</td>
</tr>
<tr>
<td>2.3.4</td>
<td><u>[M4V] Matrix</u></td>
<td>15</td>
</tr>
<tr>
<td>2.3.5</td>
<td><u>[MS4] Matrix</u></td>
<td>15</td>
</tr>
<tr>
<td>2.3.6</td>
<td><u>[MGA] Matrix</u></td>
<td>15</td>
</tr>
<tr>
<td>2.3.7</td>
<td><u>[MSA] Matrix</u></td>
<td>15</td>
</tr>
<tr>
<td>2.3.8</td>
<td><u>[MSV] Matrix</u></td>
<td>16</td>
</tr>
<tr>
<td>2.3.9</td>
<td><u>[MEG] Matrix</u></td>
<td>16</td>
</tr>
<tr>
<td>2.3.10</td>
<td><u>[MES] Matrix</u></td>
<td>16</td>
</tr>
</table>

SECTION 3: PREPARE TO LAUNCH --

<table>
<tr>
<td>3.1</td>
<td>INTRODUCTION</td>
<td>I-3-1</td>
</tr>
<tr>
<td>3.2</td>
<td>TARGETING LOAD</td>
<td>1</td>
</tr>
<tr>
<td>3.3</td>
<td>PREFLIGHT PREPARE TO LAUNCH</td>
<td>3</td>
</tr>
<tr>
<td>3.3.1</td>
<td><u>GMT Synchronizing</u></td>
<td>4</td>
</tr>
<tr>
<td>3.3.2</td>
<td><u>Azimuth Laying Support</u></td>
<td>5</td>
</tr>
<tr>
<td>3.4</td>
<td>FLIGHT PROGRAM INITIALIZATION</td>
<td>7</td>
</tr>
</table>

SECTION 4: BOOST NAVIGATION AND GUIDANCE --

<table>
<tr>
<td>4.1</td>
<td>INTRODUCTION</td>
<td>I-4-1</td>
</tr>
<tr>
<td>4.2</td>
<td>ACCELEROMETER DATA DESCRIPTION</td>
<td>2</td>
</tr>
<tr>
<td>4.2.1</td>
<td><u>Accelerometer Data Format</u></td>
<td>2</td>
</tr>
<tr>
<td>4.2.2</td>
<td><u>Accelerometer Data Processing</u></td>
<td>3</td>
</tr>
<tr>
<td>4.2.3</td>
<td><u>Velocity Accumulation</u></td>
<td>8</td>
</tr>
<tr>
<td>4.2.4</td>
<td><u>F/M Calculations</u></td>
<td>8</td>
</tr>
<tr>
<td>4.2.5</td>
<td><u>M/F Smoothing Calculations</u></td>
<td>8</td>
</tr>
<tr>
<td>4.2.6</td>
<td><u>(F/M)<sub>c</sub> Acceleration</u></td>
<td>9</td>
</tr>
<tr>
<td>4.3</td>
<td>BOOST NAVIGATION</td>
<td>10</td>
</tr>
<tr>
<td>4.3.1</td>
<td><u>Integration</u></td>
<td>10</td>
</tr>
<tr>
<td>4.3.2</td>
<td><u>Gravitational Acceleration</u></td>
<td>11</td>
</tr>
<tr>
<td>4.4</td>
<td>BOOST GUIDANCE</td>
<td>12</td>
</tr>
<tr>
<td>4.4.1</td>
<td><u>First Stage Guidance</u></td>
<td>14</td>
</tr>
<tr>
<td>4.4.2</td>
<td><u>IGM Guidance</u></td>
<td>18</td>
</tr>
<tr>
<td>4.4.3</td>
<td><u>Guidance Reference Failure (GRF)</u></td>
<td>28</td>
</tr>
<tr>
<td>4.4.4</td>
<td><u>Steering Misalignment Correction (SMC)</u></td>
<td>29</td>
</tr>
<tr>
<td>4.4.5</td>
<td><u>Chi Computations</u></td>
<td>30</td>
</tr>
</table>

4------------------------------------------------

<table border="0">
<tr>
<td>SECTION 5:</td>
<td>ORBITAL NAVIGATION AND GUIDANCE</td>
<td>--</td>
</tr>
<tr>
<td>5.1</td>
<td>INTRODUCTION</td>
<td>I-5-1</td>
</tr>
<tr>
<td>5.2</td>
<td>ORBITAL PROCESSING RATES</td>
<td>1</td>
</tr>
<tr>
<td>5.3</td>
<td>ORBITAL INITIALIZATION</td>
<td>2</td>
</tr>
<tr>
<td>5.4</td>
<td>ORBITAL NAVIGATION</td>
<td>2</td>
</tr>
<tr>
<td>5.4.1</td>
<td><u>Integration</u></td>
<td>3</td>
</tr>
<tr>
<td>5.4.2</td>
<td><u>Acceleration Models</u></td>
<td>5</td>
</tr>
<tr>
<td>5.4.3</td>
<td><u>Navigation Update</u></td>
<td>6</td>
</tr>
<tr>
<td>5.5</td>
<td>ORBITAL GUIDANCE</td>
<td>8 *</td>
</tr>
<tr>
<td>5.5.1</td>
<td><u>X Freeze Maneuver</u></td>
<td>9</td>
</tr>
<tr>
<td>5.5.2</td>
<td><u>Inertial Attitude Hold Maneuver</u></td>
<td>9</td>
</tr>
<tr>
<td>5.5.3</td>
<td><u>Track Local Reference Maneuver</u></td>
<td>9</td>
</tr>
<tr>
<td>5.5.4</td>
<td><u>Inertial Hold of Local Reference<br/>Maneuver</u></td>
<td>10</td>
</tr>
<tr>
<td>5.5.5</td>
<td><u>Nominal Control Switchover<br/>Capability</u></td>
<td>10</td>
</tr>
<tr>
<td>5.5.6</td>
<td><u>Guidance Reference Failure (GRF)</u></td>
<td>12</td>
</tr>
<tr>
<td>5.5.7</td>
<td><u>DCS Commanded Functions</u></td>
<td>12</td>
</tr>
<tr>
<td>5.6</td>
<td>TELEMETRY ACQUISITION AND LOSS</td>
<td>12</td>
</tr>
<tr>
<td>5.6.1</td>
<td><u>Acquisition and Loss Calculations</u></td>
<td>13</td>
</tr>
<tr>
<td>5.6.2</td>
<td><u>Acquisition and Loss Sequence</u></td>
<td>13</td>
</tr>
<tr>
<td>5.6.3</td>
<td><u>Telemetry Dumps</u></td>
<td>13</td>
</tr>
</table>

SECTION 6: (EDITOR'S NOTE: ALL SECTIONS ARE NUMBERED HEREIN FOR COMPATIBILITY WITH SATURN V EDD; THERE IS NO REQUIREMENT IN THIS SATURN IB EDD FOR A SECTION 6.)

<table border="0">
<tr>
<td>SECTION 7:</td>
<td>TIME BASES, DISCRETES, AND INTERRUPTS</td>
<td>--</td>
</tr>
<tr>
<td>7.1</td>
<td>INTRODUCTION</td>
<td>I-7-1</td>
</tr>
<tr>
<td>7.2</td>
<td>TIME BASES</td>
<td>1</td>
</tr>
<tr>
<td>7.2.1</td>
<td><u>Time Base 0 (Guidance Reference<br/>Release)</u></td>
<td>2</td>
</tr>
<tr>
<td>7.2.2</td>
<td><u>Time Base 1 (Liftoff)</u></td>
<td>2</td>
</tr>
<tr>
<td>7.2.3</td>
<td><u>Time Base 2 (S-IB Low Level Sensors<br/>Dry)</u></td>
<td>5 *</td>
</tr>
<tr>
<td>7.2.4</td>
<td><u>Time Base 3 (S-IB Outboard Engines<br/>Cutoff)</u></td>
<td>6 *</td>
</tr>
<tr>
<td>7.2.5</td>
<td><u>Time Base 4 (S-IVB Cutoff)</u></td>
<td>6 *</td>
</tr>
</table>

5------------------------------------------------

<table border="0">
<tr>
<td>7.3</td>
<td>DISCRETE OUTPUTS (DO)</td>
<td>9</td>
</tr>
<tr>
<td>7.4</td>
<td>DISCRETE INPUTS (DI)</td>
<td>11</td>
</tr>
<tr>
<td>7.4.1</td>
<td><u>DI1: RCA-110A Sync</u></td>
<td>11</td>
</tr>
<tr>
<td>7.4.2</td>
<td><u>DI2: Command Decoder OM/D "A" and<br/>Command Decoder OM/D "B"</u></td>
<td>13</td>
</tr>
<tr>
<td>7.4.3</td>
<td><u>DI3: Spare (Wired to IU/S-IVB<br/>Interface ...)</u></td>
<td>13</td>
</tr>
<tr>
<td>7.4.4</td>
<td><u>DI4: Spare (Wired to Control<br/>Distributor)</u></td>
<td>13</td>
</tr>
<tr>
<td>7.4.5</td>
<td><u>DI5: S-IVB Engine Out "A"</u></td>
<td>13</td>
</tr>
<tr>
<td>7.4.6</td>
<td><u>DI6: Spare...</u></td>
<td>13</td>
</tr>
<tr>
<td>7.4.7</td>
<td><u>DI7: Spare (Wired to the ESE)</u></td>
<td>14</td>
</tr>
<tr>
<td>7.4.8</td>
<td><u>DI8: Spare (Wired to the ESE)</u></td>
<td>14</td>
</tr>
<tr>
<td>7.4.9</td>
<td><u>DI9: S/C Control of Saturn</u></td>
<td>14</td>
</tr>
<tr>
<td>7.4.10</td>
<td><u>DI10: Coolant Thermal Switch #1</u></td>
<td>15</td>
</tr>
<tr>
<td>7.4.11</td>
<td><u>DI11: Coolant Thermal Switch #2</u></td>
<td>15</td>
</tr>
<tr>
<td>7.4.12</td>
<td><u>DI12: S-IB/S-IVB Separation</u></td>
<td>16</td>
</tr>
<tr>
<td>7.4.13</td>
<td><u>DI13: Spare...</u></td>
<td>16</td>
</tr>
<tr>
<td>7.4.14</td>
<td><u>DI14: S-IB Outboard Engine Out</u></td>
<td>16</td>
</tr>
<tr>
<td>7.4.15</td>
<td><u>DI15: S-IB Inboard Engine Out "B"</u></td>
<td>16</td>
</tr>
<tr>
<td>7.4.16</td>
<td><u>DI16: Prepare for Guidance<br/>Reference Release</u></td>
<td>17</td>
</tr>
<tr>
<td>7.4.17</td>
<td><u>DI17: Spare</u></td>
<td>17</td>
</tr>
<tr>
<td>7.4.18</td>
<td><u>DI18: Spare</u></td>
<td>17</td>
</tr>
<tr>
<td>7.4.19</td>
<td><u>DI19: Spare (Wired to IU/S-IVB<br/>Interface...)</u></td>
<td>17</td>
</tr>
<tr>
<td>7.4.20</td>
<td><u>DI20: Spacecraft Initiation of S-IVB<br/>Engine Cutoff</u></td>
<td>17</td>
</tr>
<tr>
<td>7.4.21</td>
<td><u>DI21: S-IB Low Level Sensors Dry<br/>"B"</u></td>
<td>18</td>
</tr>
<tr>
<td>7.4.22</td>
<td><u>DI22: Manual Initiation of S-IVB<br/>Engine Cutoff "B"</u></td>
<td>18</td>
</tr>
<tr>
<td>7.4.23</td>
<td><u>DI23: S-IB Outboard Engines<br/>Cutoff "B"</u></td>
<td>19</td>
</tr>
<tr>
<td>7.4.24</td>
<td><u>DI24: Liftoff</u></td>
<td>19</td>
</tr>
<tr>
<td>7.4.25</td>
<td><u>DIS1-DIS8: Spares (Not Wired)</u></td>
<td>19</td>
</tr>
<tr>
<td>7.5</td>
<td>INTERRUPTS</td>
<td>19</td>
</tr>
<tr>
<td>7.5.1</td>
<td><u>INT1: Command LVDA/RCA-110A<br/>Interrupt</u></td>
<td>21</td>
</tr>
<tr>
<td>7.5.2</td>
<td><u>INT2: S-IB Low Level Sensors<br/>Dry "A"</u></td>
<td>21</td>
</tr>
<tr>
<td>7.5.3</td>
<td><u>INT3: RCA-110A Interrupt</u></td>
<td>22</td>
</tr>
<tr>
<td>7.5.4</td>
<td><u>INT4: S-IVB Engine Out "B"</u></td>
<td>22</td>
</tr>
<tr>
<td>7.5.5</td>
<td><u>INT5: S-IB Outboard Engines<br/>Cutoff "A"</u></td>
<td>22</td>
</tr>
<tr>
<td>7.5.6</td>
<td><u>INT6: Manual Initiation of<br/>S-IVB Engine Cutoff "A"</u></td>
<td>23</td>
</tr>
<tr>
<td>7.5.7</td>
<td><u>INT7: Guidance Reference Release<br/>(GRR)</u></td>
<td>23</td>
</tr>
<tr>
<td>7.5.8</td>
<td><u>INT8A and INT8B: Command Decoder<br/>Interrupts "A" and "B"</u></td>
<td>24</td>
</tr>
</table>

6------------------------------------------------

<table border="0">
<tbody>
<tr>
<td>7.5.9</td>
<td><u>INT9: TLC-Simultaneous Memory Error</u></td>
<td>24</td>
<td>*</td>
</tr>
<tr>
<td>7.5.10</td>
<td><u>INT10: Spare</u></td>
<td>25</td>
<td>*</td>
</tr>
<tr>
<td>7.5.11</td>
<td><u>INT11 and INT12: Internal Function of the LVDC</u></td>
<td>25</td>
<td>*</td>
</tr>
<tr>
<td colspan="2">SECTION 8: LAUNCH VEHICLE ATTITUDE CONTROL</td>
<td>--</td>
<td></td>
</tr>
<tr>
<td>8.1</td>
<td>INTRODUCTION</td>
<td>I-8-1</td>
<td></td>
</tr>
<tr>
<td>8.2</td>
<td>MINOR LOOP</td>
<td>1</td>
<td></td>
</tr>
<tr>
<td>8.2.1</td>
<td><u>Gimbal Angle Data</u></td>
<td>2</td>
<td></td>
</tr>
<tr>
<td>8.2.2</td>
<td><u>Gimbal Data Processing</u></td>
<td>4</td>
<td></td>
</tr>
<tr>
<td>8.2.3</td>
<td><u>Attitude Error Commands</u></td>
<td>10</td>
<td></td>
</tr>
<tr>
<td>8.3</td>
<td>MINOR LOOP SUPPORT</td>
<td>11</td>
<td></td>
</tr>
<tr>
<td>8.3.1</td>
<td><u>Attitude Increments</u></td>
<td>12</td>
<td></td>
</tr>
<tr>
<td>8.3.2</td>
<td><u>Gimbal-to-Body Transformation</u></td>
<td>12</td>
<td></td>
</tr>
<tr>
<td colspan="2">SECTION 9: SWITCH SELECTOR PROCESSING</td>
<td>--</td>
<td></td>
</tr>
<tr>
<td>9.1</td>
<td>INTRODUCTION</td>
<td>I-9-1</td>
<td></td>
</tr>
<tr>
<td>9.2</td>
<td>COMMAND EXECUTION OPERATIONS</td>
<td>1</td>
<td></td>
</tr>
<tr>
<td>9.2.1</td>
<td><u>Sequence of Operations</u></td>
<td>2</td>
<td></td>
</tr>
<tr>
<td>9.2.2</td>
<td><u>Termination of a Command Sequence</u></td>
<td>9</td>
<td></td>
</tr>
<tr>
<td>9.3</td>
<td>TIMING</td>
<td>9</td>
<td></td>
</tr>
<tr>
<td>9.4</td>
<td>PRIORITIES</td>
<td>10</td>
<td></td>
</tr>
<tr>
<td>9.4.1</td>
<td><u>Class 1 Alternate Sequence</u></td>
<td>11</td>
<td></td>
</tr>
<tr>
<td>9.4.2</td>
<td><u>Class 2 Alternate Sequence</u></td>
<td>11</td>
<td></td>
</tr>
<tr>
<td>9.4.3</td>
<td><u>Nominal Flight Sequence</u></td>
<td>12</td>
<td></td>
</tr>
<tr>
<td>9.4.4</td>
<td><u>Class 3 Alternate Sequence</u></td>
<td>12</td>
<td></td>
</tr>
<tr>
<td>9.4.5</td>
<td><u>Class 4 Alternate Sequence</u></td>
<td>13</td>
<td></td>
</tr>
<tr>
<td colspan="2">SECTION 10: DIGITAL COMMAND SYSTEM</td>
<td>--</td>
<td></td>
</tr>
<tr>
<td>10.1</td>
<td>INTRODUCTION</td>
<td>I-10-1</td>
<td></td>
</tr>
<tr>
<td>10.2</td>
<td>DCS WORD FORMAT</td>
<td>1</td>
<td></td>
</tr>
<tr>
<td>10.3</td>
<td>DCS COMMAND VERIFICATION</td>
<td>1</td>
<td></td>
</tr>
<tr>
<td>10.3.1</td>
<td><u>DCS Mode Command Verification</u></td>
<td>3</td>
<td></td>
</tr>
<tr>
<td>10.3.2</td>
<td><u>DCS Data Command Verification</u></td>
<td>5</td>
<td></td>
</tr>
<tr>
<td>10.3.3</td>
<td><u>DCS Data Validation</u></td>
<td>6</td>
<td></td>
</tr>
<tr>
<td>10.3.4</td>
<td><u>DCS Error Message</u></td>
<td>7</td>
<td></td>
</tr>
</tbody>
</table>

7------------------------------------------------

<table border="0">
<tr>
<td>10.4</td>
<td>DCS COMMANDS</td>
<td>7</td>
<td></td>
</tr>
<tr>
<td>10.4.1</td>
<td><u>Time Base Update</u></td>
<td>9</td>
<td></td>
</tr>
<tr>
<td>10.4.2</td>
<td><u>Navigation Update</u></td>
<td>11</td>
<td></td>
</tr>
<tr>
<td>10.4.3</td>
<td><u>Generalized Switch Selector</u></td>
<td>13</td>
<td></td>
</tr>
<tr>
<td>10.4.4</td>
<td><u>Memory Dump</u></td>
<td>14</td>
<td>*</td>
</tr>
<tr>
<td>10.4.5</td>
<td><u>Terminate</u></td>
<td>17</td>
<td>*</td>
</tr>
<tr>
<td>10.4.6</td>
<td><u>Execute Generalized Maneuver</u></td>
<td>18</td>
<td>*</td>
</tr>
<tr>
<td>10.4.7</td>
<td><u>Return to Nominal Timeline</u></td>
<td>22</td>
<td>*</td>
</tr>
<tr>
<td>10.4.8</td>
<td><u>ECS Water Control Valve Logic</u></td>
<td>23</td>
<td>*</td>
</tr>
<tr>
<td></td>
<td><u>Inhibit</u></td>
<td></td>
<td>*</td>
</tr>
<tr>
<td>10.4.9</td>
<td><u>Execute Special Maneuvers</u></td>
<td>23</td>
<td>*</td>
</tr>
<tr>
<td>10.4.10</td>
<td><u>Execute Alternate Sequence</u></td>
<td>25</td>
<td>*</td>
</tr>
<tr>
<td>10.4.11</td>
<td><u>Targeting Load</u></td>
<td>28</td>
<td>*</td>
</tr>
</table>

SECTION 11: REAL TIME TELEMETRY AND DATA COMPRESSION --

<table border="0">
<tr>
<td>11.1</td>
<td>INTRODUCTION</td>
<td>I-11-1</td>
</tr>
<tr>
<td>11.2</td>
<td>TELEMETRY SYSTEM INTERFACE</td>
<td>1</td>
</tr>
<tr>
<td>11.3</td>
<td>IDENTIFICATION TAGS</td>
<td>2</td>
</tr>
<tr>
<td>11.4</td>
<td>GENERAL REQUIREMENTS FOR LVDC AND LVDA TELEMETRY</td>
<td>4</td>
</tr>
<tr>
<td>11.4.1</td>
<td><u>LVDC Telemetry</u></td>
<td>4</td>
</tr>
<tr>
<td>11.4.2</td>
<td><u>LVDA Telemetry</u></td>
<td>11</td>
</tr>
<tr>
<td>11.5</td>
<td>DATA COMPRESSION</td>
<td>12</td>
</tr>
<tr>
<td>11.5.1</td>
<td><u>General Data Compression Requirements</u></td>
<td>13</td>
</tr>
<tr>
<td>11.5.2</td>
<td><u>Data to be Compressed</u></td>
<td>14</td>
</tr>
<tr>
<td>11.5.3</td>
<td><u>Telemetry of Compressed Data</u></td>
<td>16</td>
</tr>
</table>

SECTION 12: PREFLIGHT TESTS --

<table border="0">
<tr>
<td>12.1</td>
<td>INTRODUCTION</td>
<td>I-12-1</td>
</tr>
<tr>
<td>12.2</td>
<td>FLIGHT SIMULATION</td>
<td>1</td>
</tr>
<tr>
<td>12.2.1</td>
<td><u>Repeatable</u></td>
<td>1</td>
</tr>
<tr>
<td>12.2.2</td>
<td><u>Non-Repeatable</u></td>
<td>3</td>
</tr>
<tr>
<td>12.3</td>
<td>FLIGHT SIMULATION OPTIONS</td>
<td>7</td>
</tr>
<tr>
<td>12.3.1</td>
<td><u>Flight Simulation Selected</u></td>
<td>7</td>
</tr>
<tr>
<td>12.3.2</td>
<td><u>Switch Selector Status</u></td>
<td>8</td>
</tr>
<tr>
<td>12.3.3</td>
<td><u>Repeatable/Non-Repeatable</u></td>
<td>8</td>
</tr>
<tr>
<td>12.3.4</td>
<td><u>Data Compression Status</u></td>
<td>8</td>
</tr>
<tr>
<td>12.3.5</td>
<td><u>Platform Status</u></td>
<td>8</td>
</tr>
<tr>
<td>12.3.6</td>
<td><u>Test Site Indicator</u></td>
<td>8</td>
</tr>
</table>

8------------------------------------------------

<table border="0">
<tbody>
<tr>
<td>12.4</td>
<td>PREFLIGHT LINKAGE TESTS</td>
<td>9</td>
</tr>
<tr>
<td>12.4.1</td>
<td><u>G&amp;C Steering Linkage</u></td>
<td>9</td>
</tr>
<tr>
<td>12.4.2</td>
<td><u>Preflight Command Receiver Linkage</u></td>
<td>10</td>
</tr>
<tr>
<td colspan="3">SECTION 13: ALGORITHMS --</td>
</tr>
<tr>
<td>13.1</td>
<td>INTRODUCTION</td>
<td>I-13-1</td>
</tr>
<tr>
<td>13.2</td>
<td>SINE-COSINE ALGORITHM</td>
<td>1</td>
</tr>
<tr>
<td>13.3</td>
<td>ARCTANGENT ALGORITHM</td>
<td>3</td>
</tr>
<tr>
<td>13.4</td>
<td>NATURAL LOGARITHM ALGORITHM</td>
<td>5</td>
</tr>
<tr>
<td>13.5</td>
<td>SQUARE ROOT ALGORITHM</td>
<td>5</td>
</tr>
<tr>
<td>13.6</td>
<td>INVERSE SQUARE ROOT ALGORITHM</td>
<td>6</td>
</tr>
<tr>
<td>13.7</td>
<td>VECTOR DOT PRODUCT</td>
<td>7</td>
</tr>
<tr>
<td>13.8</td>
<td>VECTOR CROSS PRODUCT</td>
<td>7</td>
</tr>
<tr>
<td colspan="3">SECTION 14: EQUATIONS --</td>
</tr>
<tr>
<td>14.1</td>
<td>INTRODUCTION</td>
<td>I-14-1</td>
</tr>
<tr>
<td>14.2</td>
<td>EQUATIONS</td>
<td>3</td>
</tr>
<tr>
<td colspan="3">SECTION 15: DEFINITIONS --</td>
</tr>
<tr>
<td>15.1</td>
<td>INTRODUCTION</td>
<td>I-15-1</td>
</tr>
<tr>
<td>15.2</td>
<td>ABBREVIATIONS</td>
<td>1</td>
</tr>
<tr>
<td>15.3</td>
<td>VARIABLE DICTIONARY</td>
<td>1</td>
</tr>
<tr>
<td>15.4</td>
<td>MAXIMUM VALUES OF SELECTED VARIABLES</td>
<td>2</td>
</tr>
</tbody>
</table>

9------------------------------------------------

PART I

SAT IB EDD CONFIGURATION CONTROL PAGE LISTING, REVISION A

This Revision A constitutes a complete reprinting of this volume; pages affected by this revision are identified by the notation "Rev. A" in the lower right corner. The following Flight Program Change Requests (FPCRs) are incorporated in this revision: FPCR 206-1 thru -9; 206-D1, -D2, and -D3.

10------------------------------------------------

## SECTION 1

### GENERAL MISSION DESCRIPTION AND PROGRAM REQUIREMENTS

#### 1.1 INTRODUCTION

This section contains a brief description of a typical Saturn IB mission, general flight program requirements to meet mission requirements and some specific program requirements which might be called programming ground rules to meet mission requirements.

#### 1.2 TYPICAL MISSION REQUIREMENTS

The typical mission of the Saturn IB launch vehicle (LV) is to place the space vehicle, consisting of the Command and Service Modules (CSM), into a specified earth orbit such that CSM rendezvous may be achieved with the orbiting Skylab configuration. The manned CSM will be launched on a northeasterly azimuth into an elliptical orbit. The CSM/LV separation will occur during the first revolution. The Service Module (SM), Service Propulsion System (SPS), and SM Reaction Control System (RCS) will be used to complete the CSM rendezvous maneuvers and to dock with the Skylab.

The launch vehicle mission may be divided conveniently into three major phases, namely: (1) prelaunch targeting, (2) boost to earth orbit, and (3) orbital operations.

Targeting during prelaunch is required because of the time varying relationship between the Skylab orbit and the Saturn IB launch site. The nominal launch time and the launch window, or time interval during the day when launch can take place, are determined by available launch azimuth range, launch and rendezvous lighting requirements, the dry workshop ephemeris, and

11------------------------------------------------

optimum performance considerations. The final targeting is loaded into the launch vehicle digital computer (LVDC) flight program via the command system during the interval from approximately 35 minutes before liftoff until entry is made to the prepare to launch mode. \*

Ignition and liftoff occur shortly after targeting completion and final azimuth determination. As detailed in Section 7, liftoff is a basic time point of flight program reference, actuated by an umbilical disconnect. Approximately fourteen seconds prior to ignition or seventeen seconds prior to lift-off, the inertial reference is released from its earth-fixed constraints and its attitude becomes fixed in inertial space. This point, called guidance reference release (GRR), marks the beginning of the boost to earth orbit phase of the mission. The boost phase consists of burning the S-IB stage to propellant depletion with a subsequent burn of the S-IVB stage to achieve the desired end conditions to place the S-IVB/IU and space vehicle configuration into earth orbit. \*

During flight, the launch vehicle systems are monitored in real time by ground controllers. By observing the performance of the vehicle, they can detect or predict situations requiring contingency action. The command capability from the ground to the launch vehicle is available during orbital flight to alter certain launch vehicle functions or initiate alternate functions. \*

### 1.3 GENERAL FLIGHT PROGRAM REQUIREMENTS

#### 1.3.1 Applicable Document List

General program requirements discussed in this section and the specific requirements of later sections are defined for the Saturn IB mission based on the Mission Definition Document and

12------------------------------------------------

the vehicle dependent Final Mission Definition Documents provided by MSFC. The program requirements are written specifically for the astrionics configuration described in the "Astrionics System Handbook" dated 1 November 1968 (changed 15 November 1969), MSFC No. IV-4-401-1, IBM No. 68-966-0002. Specifications for the LVDC and launch vehicle data adapter (LVDA) included as part of the astrionics system are given in the following documents:

- ● CEI Specification, LVDC Specification No. 6150001  
  Contract No. NAS8-11561
- ● CEI Specification, LVDA Specification No. 6150000  
  Contract No. NAS8-11561

Programming techniques are described in the "Programmer's Operating Manual," Volume I, Revised 15 December 1965, prepared under Contract NAS8-11561. This document has no number.

### 1.3.2 Program Functional Requirements

The flight program is an integral part of: (1) the guidance and control system comprising the LVDC/LVDA, ST-124M inertial platform, body-mounted accelerometers, rate gyros, and the flight control computer; and (2) the launch vehicle sequencing system comprising the discrete input, discrete output, and interrupt registers of the LVDA and the stage switch selectors.

The integration of these systems is accomplished through the flight program. It provides a flexible mechanism by which the system functional and detailed specifications may be altered, within wide limits, to accomplish a wide variety of missions without changes to the launch vehicle hardware.

13------------------------------------------------

All LVDC inputs and outputs pass through the LVDA. All mention of flight program input/output will be interpreted as communication with equipment external to the LVDC/LVDA with the understanding that the external equipment is physically connected to the LVDA rather than the LVDC, and that inputs and outputs are, in actual fact, relative to the LVDC/LVDA hardware and only controlled by the program. The notation is used for convenience only.

Although specific requirements will vary with mission phase, the flight program is required to provide for the following recurring functions, as discussed in Section 1.3.2.2:

- • Navigation
- • Guidance
- • Attitude control
- • Launch vehicle sequencing
- • Telemetry
- • Programmed backups to specific hardware functions
- • Command processing
- • Data compression.

Non-recurring targeting functions are also required, as discussed in Section 1.3.2.1.

In addition to the specifically identified requirements, the routine requirements of program sequencing and control exist.

14------------------------------------------------

However, these are induced requirements of program implementation rather than program requirements and are not discussed further in the requirements section.

#### 1.3.2.1 Non-Recurring Functions

The one-time targeting requirement is performed within the interval from approximately 35 minutes before liftoff until entry is made to the prepare to launch mode. The parking orbit parameters and rates of change are loaded via the command system. The rates of change are used to compute the desired descending node and inclination angle. Detailed targeting requirements are given in Section 3.

#### 1.3.2.2 Recurring Functions

Of the recurring functions listed above, data compression is required only during the orbital mission phase. Command processing is required for the one-time targeting requirement and during the orbital mission phase. The remaining functions are required during both the boost and orbital phases although specific requirements and rates of computation may vary from the boost to the orbital phases. Each requirement is discussed separately except for navigation and guidance which are more closely tied together than the other requirements. In addition, the guidance law and navigation scheme requirements are significantly different during the boost and orbital phases. Therefore, these two requirements are combined in the discussion and this particular discussion is broken into boost and orbit mission phases.

The following definitions of navigation, guidance and control apply in this document.

15------------------------------------------------

Navigation: The calculation of vehicle position and velocity at any time with respect to a specified reference frame.

Guidance: The computation, according to a specified law, of instantaneous vehicle attitude, with respect to a specified reference frame, necessary to achieve a specified state vector and/or vehicle attitude at some future time.

Control: The computation, according to a specified law, of the vehicle commands necessary to achieve the required instantaneous vehicle attitude and operation of the proper hardware to position the vehicle.

#### 1.3.2.2.1 Boost Navigation and Guidance

Boost navigation and guidance are required to begin at GRR since the platform attitude becomes inertially fixed at that time. Although guidance during first stage burn is open loop, navigation data are required for the second (active) stage guidance and navigation calculations must begin at GRR and continue throughout the boost mission phase.

Since variations in acceleration are large during boost, the computation rate for navigation must be higher than during orbit. In determining position and velocity relative to the desired reference frame, gravitational effects are computed as part of the navigation scheme since the sensors cannot measure gravitational acceleration. A mathematical model of the earth's gravitational field, which was empirically derived from satellite measurements, has been selected for use in the gravitational computations. Position and velocity in the desired

\*  
\*  
\*

16------------------------------------------------

reference frame are obtained by differencing the integrated measured and gravitational accelerations. A computation rate, or integration interval, as low as once per two seconds is adequate for the boost phase. With this rate, and the smoothed acceleration function, a simple trapezoidal integration routine yields sufficient accuracy. A single navigation scheme is adequate throughout the boost phase.

Pitch guidance commands during the first stage burn are computed as functions of time from prestored polynomials. The polynomials are designed to yield a gravity turn biased for expected winds during the launch quarter. This helps to control the angle of attack, or correspondingly, aerodynamic loading, during the atmospheric phase of flight. At a specified time in first stage burn, all guidance commands are held constant in order to zero the vehicle angular rates. This time is selected to be immediately prior to nominal S-IB fuel depletion and ensures zero or low vehicle angular rates during stage separation. \*

Yaw guidance commands during the first stage burn are computed as a tabular function of time from prestored tables. Yaw guidance is necessary in order to meet the orbital constraints necessary to rendezvous with the Saturn Workshop. \*

Except for the roll to platform azimuth maneuver during the early part of the S-IB burn, the roll guidance commands are zero during first stage. In general, the vehicle will be aligned on the launch pad to an azimuth different from the platform azimuth since the vehicle alignment is fixed at 90 degrees East of North while the required platform azimuth may vary. Only when the platform azimuth is 90 degrees will the two coincide. The desired flight azimuth is calculated by the pre-flight routines using data from the command system targeting load command. After the platform has been aligned to this azimuth, the roll misalignment must be measured and the initial \*

17------------------------------------------------

roll command set equal to the misalignment. A few seconds \*  
after liftoff, the roll command is set to zero based on an \*  
altitude test which determines that the launch vehicle has \*  
cleared the LUT. Although the roll error is quite large \*  
initially, roll rate is constrained to a low value by flight  
program limits. The roll command is set to zero for the  
remainder of the boost phase.

During the early part of the second stage burn, guidance com-  
mands are held constant. After a preset time guidance becomes  
active, using a path adaptive guidance law (IGM) to achieve the  
desired end conditions. In order to provide knowledge of  
vehicle performance, navigation data are required as an input  
to the guidance law during this portion of the boost phase.  
The total closed loop guidance mode is divided into two phases  
by the propellant mixture ratio shift during S-IVB burn. During  
the phase transition period, a smoothing technique, using arti-  
ficially stabilized performance indications, is required to  
minimize transients in the guidance commands.

In order to achieve the correct orbital inclination and long-  
itude of descending node, active guidance is required in  
both the pitch and yaw planes. Compensation in the guidance  
law is required for abrupt transients in vehicle performance  
and for subtle off-nominal vehicle characteristics such as  
center of gravity offsets. In general, active guidance laws,  
including IGM, tend to become unstable as the end conditions  
are approached. In order to maintain a stable vehicle at  
cutoff, certain of the guidance constraints are dropped shortly  
before S-IVB cutoff. In particular, the position constraints,  
and the lateral and vertical position rate constraints are  
dropped as the time-to-go approaches zero. The component  
velocity-to-be-gained constraints are maintained slightly  
longer and the commands "frozen" just before cutoff to ensure

18------------------------------------------------

zero angular rates and a stable vehicle. The time of cutoff is computed as a function of total velocity-to-be-gained and the cutoff command issued to the S-IVB at the proper time. The primary constraints on the orbit are inclination and longitude of descending node. The constrained insertion conditions are radius, path angle, and velocity.

Detailed boost navigation and guidance requirements are given in Section 4.

#### 1.3.2.2.2 Orbital Navigation and Guidance

Orbital navigation and guidance requirements are relatively simple when compared to boost requirements. Orbital navigation consists primarily of integrating the orbital equations of motion. The required gravitational model is constructed by adding the third and fourth zonal harmonics to the model used during boost.

The only external force of consequence acting on the vehicle and tending to alter its orbit is drag. A mathematical model of drag force is used instead of using the inertial platform sensors. Therefore, pre-stored equations are used in the computation of vehicle drag accelerations.

The integration scheme selected for navigation is a self-starting modified Scarborough scheme. The self-starting feature permits the drag accelerations to be introduced easily at the appropriate times. If navigation errors are large enough to require correction, the navigation state vectors may be updated by ground command. The new state vectors and time are transmitted to the flight program via the command system.

19------------------------------------------------

Orbital guidance consists primarily of following a pre-determined attitude timeline; however, the attitude may be changed at any time by the spacecraft. In addition, variations to the timeline may be made from the ground via the command system.

The acquisition and loss times of specified telemetry ground stations are computed as part of the orbital mode using vehicle position derived from orbital navigation and a stored list of station positions. This computation is necessary in order to schedule, upon starting acquisition, telemetry of the compressed data stored during the preceding dark period. Detailed requirements for the compressed data telemetry are given in Section 11.

Detailed orbital navigation, orbital guidance, and telemetry acquisition and loss requirements are given in Section 5.

#### 1.3.2.2.3 Attitude Control

The primary stabilization loop of the Saturn IB vehicle is the attitude control loop which is closed by the flight program in the LVDC. The control law requires vehicle turning rates and accelerations and commanded and actual vehicle attitudes, with respect to the prescribed reference frame, as inputs. Attitude error commands are issued to the engine actuators through the vehicle control system to effect the control function.

Limits are applied to the rate of change of the commanded attitude, the commanded attitude error magnitude, and the rate at which the commanded attitude error may change. Vehicle angular rates are thereby maintained within safe limits. The

20------------------------------------------------

control function is required throughout the mission although the frequency of control computations is higher during boost than during orbit.

During the boost period, when control is provided by the main propulsive engines, significant control authority with relatively fast vehicle response is available. Therefore, a high iteration rate (25/sec) for the control computations is required. During the orbital phase, control is maintained by a reaction jet system with limited control authority. Thus, since the response of this system is slow, a relatively slow iteration rate (10/sec) for control computations is adequate.

Detailed control requirements are given in Section 8.

#### 1.3.2.2.4 Sequencing

The flight program functional requirements include sequencing of the discrete vehicle events by means of a switch selector in each propulsive stage and one in the IU. A limited number of discrete sequencing requirements are satisfied by use of the discrete output register of the LVDA. The flight program itself must sequence into the different phases in response to interrupts and discrete inputs from the vehicle. These vehicle interrupts and discrete inputs signal the occurrence of specific mission events.

The vehicle sequencing requirements are based on the occurrence of specific mission events such as liftoff. Therefore, the vehicle sequencing requirements are divided into several distinct series of switch selector commands. Each series is referenced to a specified event called a time base. In this manner, vehicle sequencing is correct in spite of perturbations during previous mission phases. Table 1-1 lists the time bases

21------------------------------------------------

required and the primary signal for initiating each time base. Secondary or backup requirements for starting each time base are discussed in Section 1.3.2.2.6.

Detailed sequencing requirements are given in Section 7.

TABLE 1-1 TIME BASES

<table border="1"><thead><tr><th>Time Base Number</th><th>Primary Initiation Event</th></tr></thead><tbody><tr><td>0</td><td>GRR interrupt</td></tr><tr><td>1</td><td>Liftoff Discrete</td></tr><tr><td>2</td><td>S-IB level sensors dry interrupt</td></tr><tr><td>3</td><td>S-IB propellant depletion interrupt</td></tr><tr><td>4</td><td>S-IVB engine out interrupt</td></tr></tbody></table>

#### 1.3.2.2.5 Telemetry and Data Compression

Data are required from the flight program during its operation for real time monitoring of guidance system performance and for detailed postflight evaluation. Real time data generally consist of the state vector, error or backup indications, vehicle attitude, and program/vehicle status, mode, sequencing, and timing information. Detailed data generally consist of intermediate guidance calculations, hardware interface information via the data output multiplexer (DOM), and data compressed during orbital operations when the vehicle is not in range of a telemetry ground station. These data are transmitted to the ground stations through the Instrument Unit (IU) telemetry system.

22------------------------------------------------

Data to be telemetered are loaded into the LVDA buffer register along with a tag which serves as an identifier to the ground processing systems. Since the telemetry system operates at a much lower rate than the instruction execution rate of the computer, a programmed telemetry delay check is used to control the rate at which the buffer register is loaded.

Data compression is defined as the process of storing and selecting significant information about system operation during time periods in which the vehicle is not in range of a telemetry ground station. The significant information is separated from the total data stream according to a predefined rule. Information selected is stored along with a time marker for later transmission to the ground in the normal telemetry stream.

Detailed telemetry and data compression requirements are given in Section 11.

#### 1.3.2.2.6 Backups

The Saturn vehicle was built with the concept of high reliability. The flight program is required to follow this principle when practical and possible. For this reason, all time bases except Time Base 0 initiated in the LVDC by external signals can be initiated by a backup method if the primary signal fails. Generally, the primary signal to start a time base is an interrupt but the backup methods vary.

The GRR interrupt normally starts TBO and a discrete output (DO12) is set a short time later to enable the ignition command. If TBO is not started, DO12 is not set and ignition is inhibited by the ground equipment, thus providing a safe

23------------------------------------------------

vehicle. Time Bases 1 and 2 are provided with protection to prevent their initiation under erroneous circumstances and ensure a safe vehicle on the pad. Time Base 3 is provided with a backup to assure proper sequencing if the primary signal fails.

Because of various failure modes that exist in the engine out signals from the single engine S-IVB stage to the flight program, a special scheme is required for starting TB4. This time base is initiated upon two of four indications to the flight program, as detailed in Section 7.2.5.

#### 1.3.2.2.7 Command Processing

The Digital Command System (DCS) provides the capability to alter certain specified flight program functions and data upon receipt by the flight program of the proper commands and data from the ground. Some commands, such as Terminate and ECS Water Control Valve Logic Inhibit, require only a valid mode command for execution while others, such as Navigation Update and Memory Dump, require a valid mode command and appropriate data for execution. Through use of the command capability, several preplanned alternate modes can be entered or corrections can be made for certain predefined off-nominal performance situations or vehicle failures through the update and generalized switch selector commands. \*

The flight program first validates the mode and data sequence upon receipt of the command. Appropriate data are telemetered to the ground to indicate acceptance and validation of the command. If any of several non-allowable conditions exist upon receipt of a command; such as invalid command, out of sequence mode or data, or valid command at a wrong time; the flight program transmits the proper error message to the ground

24------------------------------------------------

to inform flight controllers of the condition so that appropriate action may be taken.

With the exception of the Targeting Load command and the other commands required to support it, the command capability is only required during the coast phase of the mission. Complete requirements for command processing are given in Section 10.

### 1.3.3 Requirements Interaction

The terms boost major loop (BML) and minor loop provide a convenient means of separating the functional requirements according to the required computational rates. The boost major loop is the basic repetition rate for boost processing. It is so called because the majority of the computations, including navigation and guidance, are performed in this loop. The control computations are performed at a much higher rate in the minor loop -- so called because a relatively short time is required for these computations. To preserve control accuracy, the minor loop rate is rigidly maintained, as discussed in Section 8.2.

The remaining functional requirements have no fixed repetition requirements but are interspersed with the BML and minor loop on an as-required or interrupt basis. Telemetry requirements have no specified repetition rate. The telemetered parameters will appear instead at their respective computational rates. Programmed backup capabilities for hardware malfunctions are required in both the major and minor loops during those mission phases where the particular hardware input is being used or is expected. Sequencing requirements vary with the phase of flight and are performed largely on an interrupt basis during the BML. Program response to certain hardware malfunctions lengthens the time required during the BML for the sequencing process.

25------------------------------------------------

Thus, due to the unpredictable number of computations, logic decisions, sequencing requirements, and backup paths that may be followed during any one BML, it is evident that the BML length will vary. Therefore, the BML has no fixed repetition rate requirement but it is performed at a rate consistent with accuracy requirements.

During the orbital portion of the mission, the computing load is much less than during powered flight. Considerable time is available for all computations and the navigation computations are greatly simplified by a fixed length computation cycle. Therefore, a one second computation cycle rate is adequate with all other fixed repetition computations performed at a multiple or sub-multiple of this rate. As in boost, all non-repetitive operations are performed on an interrupt or an as-required basis.

#### 1.4 PROGRAMMING GROUND RULES

The following general ground rules must be adhered to in programming the computer to meet the requirements set forth in Sections 3 through 13:

1. 1. The program must be executed in the LVDC duplex mode when operated in the flight mode. Simplex instructions or data are not permitted.
2. 2. The program structure must be planned so that new requirements may be added or old requirements deleted with a minimum of effort. All program areas involved with program or vehicle sequencing and guidance presetting are subject to frequent change and the capability must exist to make these changes with a minimum of effort.

26------------------------------------------------

1. 3. Program variables must be scaled to accommodate the maximum values defined in Table 15-3, Maximum Values of Selected Variables. Within this constraint, scaling must provide maximum accuracy obtainable without incurring program inefficiency through excessive shifting.
2. 4. Trigonometric functions and dot products of unit vectors must either be scaled to accommodate values greater than one or logic must be provided to limit these quantities to less than one.

27------------------------------------------------

(THIS PAGE HAS BEEN LEFT BLANK INTENTIONALLY.)

28------------------------------------------------

## SECTION 2

### REFERENCE SYSTEMS AND TRANSFORMATIONS

#### 2.1 INTRODUCTION

Several different reference systems are needed in defining the Saturn IB mission requirements. Each system has some special geometric or physical feature which simplifies the solution of particular problems. Transformation matrices are defined which must be used to convert vectors represented in one system to representation in another system.

#### 2.2 REFERENCE SYSTEMS

Described below are the "proper" coordinate systems S-, 4-, V-, G-, E-, A- and B-. A proper coordinate system is a set of three mutually orthogonal axes which exhibits the property of right-handedness. Also described is the P- reference system, a set of three non-orthogonal axes.

A space-fixed coordinate system (or directional inertial frame) is one which retains its orientation with respect to the celestial sphere; i.e., fixed stars, although the origin may be moving along any general curvilinear path in space. Similarly, an earth-fixed coordinate system retains its orientation with respect to the rotating earth, and a vehicle-fixed coordinate system retains its orientation with respect to the launch vehicle.

##### 2.2.1 Plumbline Coordinate System ( $X_S, Y_S, Z_S$ )

The Plumbline Coordinate System (or S-system) is a space-fixed system with origin at the geocentric center of the earth. The

29------------------------------------------------

$X_S$  axis is parallel to the plumbline direction at the launch site at  $T_{GRR}$  and is positive in the opposite direction of the gravity vector. The  $Z_S$  axis is parallel to the platform azimuth and is positive downrange (see Figure 2-1).

This system is identical to Apollo Standard Coordinate System 13, Launch Vehicle Navigation<sup>1</sup>.

### 2.2.2 Target Plane Coordinate System ( $X_4$ , $Y_4$ , $Z_4$ )

The Target Plane Coordinate System (or 4-system) is a space-fixed system with origin at the geocentric center of the earth. The  $X_4$  axis lies along the intersection of the desired orbit plane and the equatorial plane and is positive toward the descending node of the desired orbit. The positive  $Z_4$  axis lies in the desired orbit plane 90 degrees downrange from the  $X_4$  axis (see Figure 2-1).

The 4-system is an intermediate reference frame used in IGM calculations.

### 2.2.3 Injection Plane Coordinate System ( $X_V$ , $Y_V$ , $Z_V$ )

The Injection Plane Coordinate System (or V-system) is a space-fixed system with origin at the geocentric center of the earth. The positive  $X_V$  axis lies in the desired orbit plane at an angle  $-\phi_T$  from the  $X_4$  axis in the  $X_4$ - $Z_4$  plane, and passes through the predicted insertion point. The positive  $Z_V$  axis lies in the desired orbit plane 90 degrees downrange from the  $X_V$  axis (see Figure 2-1).

---

<sup>1</sup>NASA: "Project Apollo Coordinate System Standards", Office of Manned Space Flight, Report No. SE 008-001-1, June 1965, Washington, D.C.

30------------------------------------------------

![Diagram of coordinate system geometry for S-, 4-, and V- systems on a sphere.](cee42799719a7041cd049a6a23e926f3_1_img.webp)

The diagram illustrates the geometric relationships between three coordinate systems (S, 4, and V) on a spherical body. The sphere's center is the origin for all three systems. The S-system is defined by axes  $x_S$ ,  $y_S$ , and  $z_S$ . The V-system is defined by axes  $x_V$ ,  $y_V$ , and  $z_V$ . The 4-system is defined by axes  $x_4$ ,  $y_4$ , and  $z_4$ . The V and 4 systems are shown as dashed lines, indicating they are rotated relative to the S-system. The angle  $\phi_T$  is the rotation angle between the S and V systems. The angle  $A_{zf}$  is the angle between the S and V systems at the point of intersection. The diagram also shows the N.P. (North Pole), the LAUNCH SITE, and the DESCENDING NODE. The EQUATOR is also indicated. The axes are color-coded: red for S, blue for V, and green for 4. Arrowheads on the axes indicate the positive sense of direction.

Figure 2-1 Coordinate System Geometry for S-, 4-, and V- systems (Arrowheads on axes indicate the positive sense of direction along those axes.)

31------------------------------------------------

(THIS PAGE HAS BEEN LEFT BLANK INTENTIONALLY.)

32------------------------------------------------

(THIS PAGE HAS BEEN LEFT BLANK INTENTIONALLY.)

33------------------------------------------------

![Diagram of coordinate system geometry for E-, G-, and A- systems on a sphere.](ce3fdb625852612af1b1d0d4a456e873_1_img.webp)

The diagram illustrates the coordinate system geometry for E-, G-, and A- systems on a sphere. The sphere is shown with its spin axis, represented by a red dashed line passing through the North Pole (N.P.). The spin axis is labeled "SPIN AXIS" with an arrow pointing to it. The equator is marked with the label "EQUATOR". The sphere is tilted relative to the ecliptic plane, which is represented by a shaded elliptical disk. The ecliptic plane is labeled "TO VERNAL EQUINOX" with an arrow pointing to the direction of the  $X_E$  axis. The E- system is defined by axes  $X_E$  (red),  $Y_E$  (red), and  $Z_E$  (red). The G- system is defined by axes  $X_G$  (blue),  $Y_G$  (blue), and  $Z_G$  (blue). The A- system is defined by axes  $X_A$  (green),  $Y_A$  (green), and  $Z_A$  (green). The  $X_E$  axis is shown at an angle  $\theta_E$  from the  $X_G$  axis. The  $X_G$  axis is shown at an angle  $\theta_R$  from the  $X_A$  axis. The  $Y_A$  and  $Y_G$  axes are shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X_G$  axis is shown pointing downwards and to the left. The  $X_A$  axis is shown pointing downwards and to the left. The  $Y_A$  axis is shown pointing downwards. The  $Y_G$  axis is shown pointing downwards. The  $Z_A$  axis is shown pointing to the right. The  $Z_G$  axis is shown pointing downwards and to the right. The  $Z_E$  axis is shown pointing upwards. The  $X_E$  axis is shown pointing to the left. The  $X$

34------------------------------------------------

The IGM velocity-to-be-gained and the time-to-go to cutoff are determined from quantities calculated in the V-system. Flight equations are thereby simplified since several variables will be equal to zero at the insertion point.

#### 2.2.4 Gravitational Coordinate System ( $X_G, Y_G, Z_G$ )

The Gravitational Coordinate System (or G-system) is a space-fixed system with origin at the geocentric center of the earth. The  $X_G$  axis is positive toward the intersection of the launch site meridian at  $T_{GRR}$  and the equator. The  $Y_G$  axis lies along the earth's spin axis and is positive toward the south pole (see Figure 2-2).

The G-system is used in computing the gravitational and drag accelerations.

#### 2.2.5 Ephermal Coordinate System ( $X_E, Y_E, Z_E$ )

The Ephermal Coordinate System (or E-system) is a space-fixed system with origin at the geocentric center of the earth. The  $X_E$  axis is positive toward the vernal equinox. The  $Z_E$  axis lies along the earth's spin axis and is positive toward the north pole (see Figure 2-2).

The E-system is identical to Apollo Standard Coordinate System 4, Geocentric Inertial<sup>2</sup>.

---

<sup>2</sup> Ibid.

35------------------------------------------------

#### 2.2.6 Telemetry Station Coordinate System ( $X_A$ , $Y_A$ , $Z_A$ )

The Telemetry Station Coordinate System (or A-system) is an earth-fixed system with origin at the geocentric center of the earth. The  $X_A$  axis is positive toward the intersection point of the launch site meridian and the equator. The  $Y_A$  axis lies along the earth's spin axis and is positive toward the south pole (see Figure 2-2).

#### 2.2.7 Body Coordinate System ( $X_B$ , $Y_B$ , $Z_B$ )

The Body Coordinate System (or B-system) is a vehicle-fixed system with origin at the geometrical center of the Instrument Unit. The  $X_B$  axis lies along the longitudinal vehicle axis, and is positive toward the spacecraft. The positive  $Z_B$  axis passes through Position I (see Figure 2-3). Rotations about the  $X_B$ ,  $Y_B$ , and  $Z_B$  axes are identified as roll, pitch, and yaw, respectively.

The B-system is used to implement the attitude error commands.

This system is translatable, along the  $X_B$  axis, to Apollo Standard Coordinate System 8a, Saturn I and IB Launch Vehicle Structural Body Axes<sup>3</sup>.

#### 2.2.8 Inertial Platform Gimbal System ( $X_P$ , $Y_P$ , $Z_P$ )

The inertial platform gimbal system is the set of three non-orthogonal axes about which the gimbal angles are measured. The origin of the system is at the center of the inertial platform.

---

<sup>3</sup>Ibid.

36------------------------------------------------

The diagram illustrates the geometry of the Saturn IB Launch Vehicle and its B-system. The main diagram shows the vehicle from below, with the following labels and features:

- $X_B$ : Vertical axis of the B-system.
- **ROLL**: Indicated by a curved arrow around the  $X_B$  axis.
- **LES**: Launch Escape System.
- **CM**: Center of Mass.
- **SM**: Solid Rocket Motor.
- **CENTER OF IU**: Center of the Instrument Unit.
- **SLA**: Solid Rocket Motor attachment point.
- **IU**: Instrument Unit.
- $Y_B$ : Horizontal axis of the B-system.
- **PITCH**: Indicated by a curved arrow around the  $Y_B$  axis.
- **POSITION I**: A reference point on the vehicle.
- **S-IVB**: Solid Rocket Booster.
- **S-IB**: Solid Rocket Motor.
- **Z\_B**: Horizontal axis of the B-system.
- **YAW**: Indicated by a curved arrow around the  $Z_B$  axis.

The B-system geometry diagram (LOOKING UPWARD) shows the following:

- **FIN 1** to **FIN 8**: Numbered fins arranged in a circle.
- **POSITION I**: Located at the bottom of the circle, aligned with FIN 1.
- **POSITION II**: Located on the left side, aligned with FIN 3.
- **POSITION III**: Located at the top of the circle, aligned with FIN 5.
- **POSITION IV**: Located on the right side, aligned with FIN 7.

Figure 2-3 Saturn IB Launch Vehicle Geometry and B-system Geometry. (Arrowheads indicate the positive sense of direction or rotation.)

37------------------------------------------------

The axes of this system may be defined by translating the S-system and the B-system so that their origins are at the center of the inertial platform (see Figure 2-4). With this geometrical configuration, the  $X_p$  axis of this system is identical to the  $X_B$  axis. The  $Y_p$  axis is identical to the  $Y_S$  axis. The  $Z_p$  axis lies along the line of nodes, which is a line  $\Theta_y$  from the  $Z_S$  axis (being also the intersection of the  $X_S-Z_S$  plane with the  $Y_B-Z_B$  plane). At the time of GRR, the axes of the gimbal system are parallel, respectively, with the axes of the plumbline system (see Figure 2-5).

The angle  $\Theta_x$  is measured about the  $X_B$  axis, and locates the  $Z_B$  axis with respect to the line of nodes. The angle  $\Theta_y$  is measured about the  $Y_S$  axis and locates the line of nodes with respect to the  $Z_S$  axis. The angle  $\Theta_z$  is measured about the line of nodes. The angles  $\Theta_x$ ,  $\Theta_y$ , and  $\Theta_z$  are the outer, inner, and middle gimbal angles, respectively, of the inertial platform and may also be identified as the Euler angles.

The IGM calculations, in Section 4.4, generate steering commands with respect to the 4-system. These must be expressed in terms of gimbal angles for comparison with the real vehicle attitude (Eq. 4.4.85 through 4.4.87).

Further, when the attitude errors have been determined, it is necessary to convert these gimbal angle errors to the B-system since steering is in terms of roll, pitch, and yaw (Eq. 8.3.2).

## 2.3 TRANSFORMATION MATRICES

It is often necessary to transform a vector from one coordinate system to another. This is accomplished by operating on the known vector with an appropriate transformation matrix. The transformation matrix from an original system to a new system

38------------------------------------------------

![Diagram of an Inertial Platform Gimbal System showing three coordinate systems: S (red), B (green), and L.N. (blue). The S system has axes X_S, Y_S, and Z_S. The B system has axes X_B, Y_B, and Z_B. The L.N. system has axes X_B, Y_S, and L.N. Gimbal angles are indicated: theta_x is the angle between X_S and X_B; theta_y is the angle between Z_S and Z_B; and theta_z is the angle between Y_S and Y_B. Arrows indicate the positive sense of rotation.](f77d60421afed0f140ce4a9263ed50f2_1_img.webp)

The diagram illustrates an Inertial Platform Gimbal System with three coordinate systems originating from a common point. The primary system, S, is shown in red with axes  $X_S$  (vertical),  $Z_S$  (horizontal to the right), and  $Y_S$  (diagonal down-left). The secondary system, B, is shown in green with axes  $X_B$  (diagonal up-left),  $Z_B$  (diagonal up-right), and  $Y_B$  (diagonal down-right). The third system, L.N., is shown in blue with axes  $X_B$  (shared with B),  $Y_S$  (shared with S), and  $L.N.$  (diagonal up-right). Gimbal angles are defined as follows:  $\theta_x$  is the angle between  $X_S$  and  $X_B$ ;  $\theta_y$  is the angle between  $Z_S$  and  $Z_B$ ; and  $\theta_z$  is the angle between  $Y_S$  and  $Y_B$ . Arrows indicate the positive sense of rotation for each angle.

Figure 2-4 Inertial Platform Gimbal System (The angles  $\theta_x$ ,  $\theta_y$ , and  $\theta_z$  are the gimbal angles measured about the  $X_B$ ,  $Y_S$ , and  $L.N.$  axes, respectively. Arrowheads indicate the positive sense of direction or rotation.  $L.N.$  is downrange at liftoff at which time  $\theta_y = 0$ .)

39------------------------------------------------

(THIS PAGE HAS BEEN LEFT BLANK INTENTIONALLY.)

40------------------------------------------------

The diagram illustrates the platform configuration at liftoff, showing the gimbal system and associated sensors and servos. The platform is mounted on a vehicle frame and consists of three concentric gimbals: the Outer Gimbal, the Middle Gimbal, and the Inertial Gimbal. The Outer Gimbal is responsible for Roll ( $X_B$ ) and Pitch ( $Y_S$ ) motions, with a yaw motion ( $L.N.$ ) also indicated. The Middle Gimbal is responsible for Yaw ( $L.N.$ ). The Inertial Gimbal is the innermost gimbal. The platform is equipped with several sensors and servos:
 

- **RESOLVER**: Located on the Outer Gimbal, Middle Gimbal, and Inertial Gimbal to measure angular displacement.
- **GYRO**: Located on the Outer Gimbal, Middle Gimbal, and Inertial Gimbal to measure angular velocity.
- **INTEGRATING ACCELEROMETERS**: Located on the Outer Gimbal to measure acceleration.
- **PRELAUNCH VERTICAL ALIGNMENT PENDULUMS**: Located on the Outer Gimbal to provide vertical alignment.
- **SERVOED PRISM**: Located on the Outer Gimbal to control the gimbal's motion.
- **FIXED PRISM**: Located on the Outer Gimbal as a reference.
- **SERVOTORQUE MOTOR**: Located on the Outer Gimbal, Middle Gimbal, and Inertial Gimbal to provide torque for motion control.

 The diagram also shows the **FLIGHT PATH** and the **VEHICLE FRAME**. The platform is shown in a configuration where the yaw motion ( $L.N.$ ) is downrange at liftoff.

Figure 2-5 Platform Configuration at Liftoff (L.N. is downrange at liftoff.)

41------------------------------------------------

is obtained by rotating the original system (according to a prescribed plan about selected axes) until it is coincident with the new system.

Matrix names are composed of three characters, the first being the letter M. The matrix transforms vectors from the system indicated by the second character to the system indicated by the third character. Elements of a matrix are named by adding two numbers to the matrix name. The first number indicates the row of the matrix to which the element belongs. The second number indicates the column to which the element belongs.

The following matrices will be used, as necessary in the flight program, to convert from one coordinate system to another. In the descriptions, the prime symbol is used to denote intermediate systems which are not otherwise defined or used.

#### 2.3.1 [MSG] Matrix

The [MSG] matrix transforms vectors from the S-system to the G-system. The elements are found by two rotations. The first is a rotation of the S-system about the  $X_S$  axis through  $(A_z - 90^\circ)$  to obtain the S'-system. The second is a rotation of the S'-system about its own Z axis through  $\phi_L$ . The elements of the [MSG] matrix are shown in Eq. 2.3.1.

#### 2.3.2 [MG4] Matrix

The [MG4] matrix transforms vectors from the G-system to the 4-system. It is obtained by a rotation of the G-system about the  $Y_G$  axis through  $-\lambda$  to obtain the G'-system, followed by a rotation of the G'-system about its X axis through  $-i$ . The elements of the [MG4] matrix are shown in Eq. 2.3.2.

42------------------------------------------------

### 2.3.3 [MBS] Matrix

The [MBS] matrix transforms vectors from the B-system to the S-system. It is obtained by three rotations of the B-system: (1) a rotation about the  $X_B$  axis through  $-\theta_x$ ; (2) a rotation about the new  $Z_B$  axis through  $-\theta_z$ ; and (3) a rotation about the  $Y_B$  axis through  $-\theta_y$ . The elements of the [MBS] matrix are shown in Eq. 2.3.3.

### 2.3.4 [M4V] Matrix

The [M4V] matrix transforms vectors from the 4-system to the V-system. It is obtained by a single rotation of the 4-system about the  $Y_4$  axis through  $-\phi_T$ . Eq. 2.3.4 shows the elements of the [M4V] matrix.

### 2.3.5 [MS4] Matrix

The [MS4] matrix transforms vectors from the S-system to the 4-system. It is obtained by pre-multiplying the [MSG] matrix by the [MG4] matrix. Eq. 2.3.5 shows the computation of the elements of the [MS4] matrix.

### 2.3.6 [MGA] Matrix

The [MGA] matrix transforms vectors from the G-system to the A-system. This matrix represents a rotation of the G-system about  $Y_G$  through  $\theta_R$ . This yields the elements for the [MGA] matrix shown in Eq. 2.3.6.

### 2.3.7 [MSA] Matrix

The [MSA] matrix transforms vectors from the S-system to the A-system. It is obtained by pre-multiplying the [MSG] matrix

43------------------------------------------------

by the [MGA] matrix, which represents a rotation of the G-system about  $Y_G$  through  $\theta_R$ . This yields the elements for the [MSA] matrix shown in Eq. 2.3.7.

#### 2.3.8 [MSV] Matrix

The [MSV] matrix transforms vectors from the S-system to the V-system. It is obtained by pre-multiplying the [MS4] matrix by the [M4V] matrix. Eq. 2.3.8 shows the computation of the elements of the [MSV] matrix.

#### 2.3.9 [MEG] Matrix

The [MEG] matrix transforms vectors from the E-system to the G-system. Two rotations are needed: (1) a rotation of the E-system about  $Z_E$  through  $\theta_E$  to obtain the E'-system, followed by (2) a rotation of the E'-system about its X axis through -90 degrees. The elements of the [MEG] matrix are shown in Eq. 2.3.9.

#### 2.3.10 [MES] Matrix

The [MES] matrix transforms vectors from the E-system to the S-system. It is obtained by pre-multiplying the [MEG] matrix by the transpose of the [MEG] matrix. The elements of the [MES] matrix are shown in Eq. 2.3.10.

44------------------------------------------------

## SECTION 3

### PREPARE TO LAUNCH

#### 3.1 INTRODUCTION

This section describes the flight program related events which occur before liftoff. This includes the necessary interface between the ground routines and the flight program in order to load the targeting data, to synchronize Greenwich Mean Time (GMT) between the RCA-110A and the LVDC, to perform azimuth laying, and to transfer program control to the flight routines. This transfer is initiated upon receipt of the Guidance Reference Release (GRR) interrupt, at which time the flight program will be initialized. Figure 3-1 is a schematic representation of the sequence of events during the last forty-five minutes before liftoff.

In specifying the LVDC flight program requirements, portions of other programs (RCA-110A and LVDC preflight) will be described when they interface with flight program requirements. These programs are not controlled by this document and are mentioned for completeness only.

#### 3.2 TARGETING LOAD

When the countdown clock shows approximately ten minutes prior to liftoff, a thirty-five minute hold will be initiated in the countdown sequence to load targeting data. The digital command system (DCS) targeting load command is the primary method for loading the data into the LVDC. If this fails, the RCA-110A can load the data from cards.

During the first five minutes of the hold, the LVDC preflight targeting load mode will be entered by selection of entry 3 of the RCA-110A preflight command test. This mode will enable

45------------------------------------------------

12/1/72  
Judy Anderson, KSC

The diagram is a sequence diagram with three columns: LAUNCH SEQUENCER, LVDC, and RCA-110A. A central vertical line represents TIME. The sequence of events is as follows:

- **ENTER HOLD:**
  - LAUNCH SEQUENCER sends a signal to LVDC.
  - LVDC sends a signal to RCA-110A at **T-37 (L-45) MINUTES**.
  - RCA-110A sends **1 ISSUE PREFLIGHT TARGETING LOAD MODE COMMAND** to LVDC.
  - LVDC sends **2 ENABLE TARGETING LOAD, MEMORY DUMP, TERMINATE DCS COMMANDS** to RCA-110A.
  - LVDC sends **3 RECEIVE DATA VIA DCS FROM MCC** to RCA-110A.
  - RCA-110A sends **DCS TARGETING LOAD** to LVDC.
- **EXIT HOLD:**
  - LAUNCH SEQUENCER sends a signal to LVDC.
  - LVDC sends a signal to RCA-110A at **T-10 (L-10) MINUTES**.
  - RCA-110A sends **1 ISSUE PREPARE TO LAUNCH COMMAND** to LVDC.
  - LVDC sends **2 ENTER PREPARE TO LAUNCH** to RCA-110A.
  - LVDC sends **3 READ REAL TIME CLOCK AND ISSUE D02** to RCA-110A.
  - RCA-110A sends **4 READ REAL TIME CLOCK** to LVDC.
  - RCA-110A sends **5 TRANSMIT GMT** to LVDC.
  - LVDC sends **6 SYNCHRONIZE GMT** to RCA-110A.
  - RCA-110A sends **SYNC GMT** to LVDC.
- **LAUNCH SEQUENCER START:**
  - LAUNCH SEQUENCER sends a signal to LVDC at **T-187 SECONDS**.
  - LVDC sends **2 CALCULATE AND TRANSMIT A<sub>Z</sub>** to RCA-110A.
  - RCA-110A sends **1 TRANSMIT T<sub>L</sub>** to LVDC.
  - LVDC sends **3 RECEIVE A<sub>Z</sub>, DO AZIMUTH LAYING, GENERATE PLATFORM ALIGNED INTERRUPT** to RCA-110A.
  - RCA-110A sends **AZIMUTH LAYING** to LVDC.
- **GRR (OUTPUT):**
  - LVDC sends a signal to RCA-110A at **T-22 SECONDS**.
  - RCA-110A sends **GRR (INPUT)** to LVDC.
- **GRR (INPUT), INITIALIZE AND SET D012:**
  - LVDC sends a signal to RCA-110A at **T-17 SECONDS**.
  - RCA-110A sends **GRR (INPUT), INITIALIZE AND SET D012** to LVDC.
- **LIFTOFF, START TIME BASE 1 AND RESET D012:**
  - LVDC sends a signal to RCA-110A at **T-0**.
  - RCA-110A sends **LIFTOFF, START TIME BASE 1 AND RESET D012** to LVDC.

Figure 3-1 Prepare to Launch Events

46------------------------------------------------

the targeting load, memory dump, and terminate DCS commands. \*  
(See Sections 10.4.11, 10.4.4, and 10.4.5.) During the interval \*  
from ten minutes to twenty minutes into the hold, Mission Con-  
trol Center (MCC) will use the targeting load DCS command to  
load the targeting data, defined in Table 10-17, into the LVDC.  
MCC will verify the load using the memory dump DCS command. \*  
The RCA-110A has the capability to verify the data using an  
option within the backup load program.

If three attempts to load the data via DCS have failed, the  
backup load from the RCA-110A is initiated at approximately  
twenty minutes into the hold. This capability remains active  
until the LVDC preflight Prepare to Launch (PTL) routine is  
activated, at the end of the hold.

### 3.3 PREFLIGHT PREPARE TO LAUNCH

The LVDC preflight PTL routine is activated by a PTL mode com-  
mand from the RCA-110A Prepare to Launch test. The LVDC pre-  
flight program performs the prepare to launch functions listed  
below and prepares the LVDC for entering the flight mode.<sup>1</sup>  
During Prepare to Launch the LVDC:

- • Accepts an initial value for Greenwich Mean Time
- • Performs other system monitoring functions not  
  related to the flight program
- • Accepts an initial value or update of predicted  
  liftoff time ( $T_L'$ ) and performs azimuth calcula-  
  tions for azimuth laying
- • Processes the GRR interrupt and transfers LVDC control  
  to the flight program.

---

<sup>1</sup>IBM: LVDC Preflight Program Descriptions Document, NAS8-14000,  
September 1, 1969.

47------------------------------------------------

At approximately 22 seconds before liftoff the GRR discrete, \*  
which originates in the Launch Sequencer, is sent to the LVDC.  
This input shortens the preflight computation cycle by elim-  
inating LVDC self-test and platform off-level computations.  
At approximately 17 seconds before liftoff, the ST-124M plat- \*  
form becomes space stable and the LVDC GRR interrupt is generated.  
When the GRR interrupt is received by the LVDC, the LVDC  
preflight program stores the last reading of the roll gimbal  
angle for flight program use and transfers program control to  
the flight program initialize routine.

### 3.3.1 GMT Synchronizing

Providing a value of Greenwich Mean Time to the LVDC requires coordination with the RCA-110A computer. When the LVDC preflight PTL routine is initiated, it reads and stores the real time clock reading, issues Discrete Output 2 (DO2) and begins to accumulate elapsed time from the initial reading. The setting of DO2 establishes the reference point for synchronizing GMT between the RCA-110A and the LVDC. To synchronize accurately the initial time reading with the setting of DO2, the time corresponding to the fixed number of executed LVDC instructions between these two events is added to the initial real time clock reading.

In the RCA-110A, receipt of DO2 causes the RCA-110A to read GMT. Control is then returned to its PTL program and another GMT reading is made for protection against an erroneous clock reading. If these two RCA-110A readings agree within 500 milliseconds, the RCA-110A PTL routine formats the first GMT reading and loads it into the LVDC. If they do not agree, an option is displayed to continue or to recycle. At GRR, the LVDC flight initialize routine will add this GMT reading to the elapsed time accumulated from the LVDC PTL routine initiation to compute the time of GRR.

48------------------------------------------------

### 3.3.2 Azimuth Laying Support

The azimuth laying program is executed in the RCA-110A Launch Control Computer (LCC). It is used at approximately two hours before predicted liftoff to initially position the ST-124M platform to the platform azimuth,  $A_z$ . (This is the predicted platform azimuth.)

#### 3.3.2.1 Initial Alignment

The azimuth laying program requests the operator to enter the platform azimuth measured from true north in the form of degrees, minutes and seconds. The program validates and stores this input for future use. The program then requests the Theodolite Azimuth in the form of degrees, minutes and seconds. This is the angle measured between the Theodolite and true north. The program validates this angle and stores it for future use.

Next the program ensures that the ST-124M is properly powered up and in a correct condition to perform the azimuth laying function. The program turns on Dual Prism alignment and waits until both the Moving Prism and the Inertial Prisms have been acquired for ten seconds. A series of readings of the azimuth encoder is taken and averaged. This average is used as the azimuth encoder offset.

The program computes the amount and direction that the azimuth encoder must be moved to align the ST-124M platform to the platform azimuth and drives the azimuth encoder to position the platform within 10 arcseconds of the computed value.

49------------------------------------------------

### 3.3.2.2 Repositioning

Repositioning of the ST-124M is accomplished by the RCA-110A azimuth laying fine positioning program executed in the Mobile Launch Computer (MLC). This program is called into execution at approximately T-10 minutes by the RCA-110A PTL test. The fine positioning program ensures positioning of the platform according to the latest computed position. The program continuously monitors GMT and countdown time for any change in  $T'_L$  until GRR.

The predicted liftoff time ( $T'_L$ ) is loaded into the LVDC from the RCA-110A after the LVDC has been commanded to Prepare to Launch. The LVDC PTL routine computes the difference between the predicted time of GRR and the nominal time of GRR ( $T_{GRR0}$ ) using Eq. 3.2.1. The resulting time differential ( $T_D$ ) is used in Eq. 3.2.2 to compute  $\lambda$ , the longitude of the descending node measured from the launch meridian. The longitude of the descending node and the inclination are used in Eq. 3.2.3 to compute the platform azimuth ( $A_z$ ). The computed platform azimuth is transmitted back to the RCA-110A in true form and complemented form where it is used to align the platform.

Any holds greater than 5 seconds, occurring between the beginning of Prepare to Launch and 187 seconds prior to liftoff, require that a revised predicted liftoff time ( $T'_L$ ) be transmitted to the LVDC. In response to the new value of  $T'_L$ , the LVDC will calculate a new platform azimuth and transmit this to the RCA-110A for platform realignment. If it is necessary to recycle to the beginning of Prepare to Launch Mode, a new  $T'_L$  must be loaded into the LVDC and the sequence started again.

The platform may be realigned at times other than when  $T'_L$  changes. The capability to monitor platform gimbal angles exists at the MLC until GRR. Any drift of the azimuth encoder

50------------------------------------------------

greater than two binary bits (10 arcseconds) will be corrected automatically by executing the Azimuth Laying Fine Positioning program. Corrections will be made up until 152 seconds before liftoff. Any encoder drift after this time will result only in an error message indicating that repositioning is required.

### 3.4 FLIGHT PROGRAM INITIALIZATION

When the flight program gains control upon recognition of the GRR interrupt, it must start Time Base 0 (TBO), make initial accelerometer readings, and calculate the descending node of the desired orbit. Subsequently, the remaining flight quantities must be initialized. When the flight program initialization is complete, the LVDC/LVDA Firing Commit Enable Discrete Output (D012) must be set, and the boost mode calculations will begin.

Time Base 0 is started by storing the real time clock reading for use as a reference for the total time in the flight mode ( $T_{as}$ ). The GMT value ( $T_{GMT}$ ) at this time must also be stored.

Immediately upon beginning the initialization, the accelerometers and the real time clock must be read and stored for reference as past values in accelerometer processing computations. The time between reading the clock and the accelerometers must be the same as in accelerometer processing. The last reading of the roll gimbal angle made by LVDC preflight in the PTL routine, must be stored and used to derive the initial roll guidance command for both the boost major loop and minor loop processing. The real time clock reading, accelerometer readings, and initial roll command are telemetered during initialization.

The GRR time delta ( $T_D$ ) from the nominal time of GRR ( $T_{GRR0}$ ) must be recomputed using  $T_{GMT}$  in Eq. 3.4.1. The variable

51------------------------------------------------

longitude of the descending node ( $\lambda$ ) must be computed as a function of  $T_D$  using Eq. 3.4.2.

In order to fly a Saturn IB mission with a preset descending node and platform azimuth (i.e., no targeting load is performed prior to launch),  $\dot{\lambda}$  must be preset to zero.

The transformation matrices between the plumbline and target plane [MS4], and the plumbline and gravitational [MSG] coordinate systems must be computed as functions of the platform azimuth ( $A_z$ ), the orbital inclination ( $i$ ), the descending node ( $\lambda$ ), and the geodetic latitude of the launch site ( $\phi_L$ ).

Upon completing the above computations, the initial values of position, velocity, and gravitation acceleration are computed using Eq. 3.4.3 through 3.4.8. In Eq. 3.4.3,  $\phi'_L$  is the geocentric latitude of the launch site and  $R_L$  is the initial position magnitude. All remaining navigation, guidance, attitude control, and flight sequencing quantities required for the boost mode computations must then be initialized. After boost initialization is complete, the program will immediately enter the boost major loop which begins with the processing of accelerometer data.