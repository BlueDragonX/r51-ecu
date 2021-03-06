# R51 Climate Control

The R51 contains two CAN connected ECUs critical to the operation of the
climate control system: the A/C Auto Amp the AV Control Unit.

The A/C Auto Amp is the computer which is connected to the climate control
hardware. It contains the relays and motor drivers which control the operation
of the front A/C unit's fan and vents. It also connects to the sensors which
monitor the outside and inside temperatures. This enables automatic and dual
zone operation of the front A/C unit.

The A/C Auto Amp also controls the rear A/C unit and its controls. The rear A/C
unit is operated manually via the connected controls.

The A/C Auto Amp forwards signals to the BCM to control activation  of the A/C
compressor.

The AV Control Unit is the car's infotainment system. It consists of the AV
Control Unit itself, the Display Unit (LCD screen), and the A/C and AV Switch
Assembly. The AV Control Unit sends signals over CAN to control the state of
the climate control system.


## Physical Controls

The physical controls are connected to the AV Control Unit via an independent
CAN network. The AV Control Unit forwards button presses to the A/C Auto Amp
over the main CAN bus.

The rear window heating element is not controlled via the independent CAN bus.
Rather, the switch assembly controls this directly over a single wire connected
directly to the A/C Auto Amp.

A good overview of the system is available in [this YouTube
video](https://www.youtube.com/watch?v=P5e_osej-WY). This video is specifically
for the Nissan Armada but applies equally to the same generation Nissan
Pathfinder. This video does leave out some details for recreating state
transitions.

The physical controls consist of the following inputs:
* Driver Temperature Rotary Knob
  * Press to toggle Auto.
  * Turn Left to decrease temperature.
  * Turn right to increase temperature.
* Passenger Temperature Rotary Knob
  * Press to toggle Dual.
  * Turn Left to decrease temperature.
  * Turn right to increase temperature.
* A/C Button: Toggle the A/C compressor request.
* Off: Turn off climate control.
* Fan+: Increase fan speed.
* Fan-: Decrease fan speed.
* Front Defrost: Toggle on/off windshield defrost mode.
* Rear Defrost: Toggle on/off the rear window and side mirror heating elements.
* Mode: Cycle airflow modes.
* Recirculate: Toggle on/off cabin air recirculation.


## Unit State Management

Ths A/C Auto Amp maintains the current operating state of the climate control
system. The AV Control Unit forwards inputs from the A/C And AV Switch Assembly
to the A/c Auto Amp. The A/C Auto Amp updates its internal state and sends
frames to indicate the new state to the AV Control Unit.

This section documents the various states and state changes triggered by control inputs.


### Possible Unit States

The climate control may exist in one of five states:
* Off: The climate control system is off.
* Auto: Automatic mode. Fan speed and air flow is adjusted to achieve the
  desired temperature in the driver and passenger climate zones.
* Manual: Temperature, fan speed, and air flow are controlled manually.
* Manual with Fan Off: The unit allows manual control of airflow and
  recirculation while the fans are off.
* Defrost: The unit acts in windshield defrost mode. Fan speed and temperature
  are controlled manually. Only airflow to the windshield is allowed.


### State Transitions

This section describes state changes caused by button presses when the unit is
in each of the five states.


#### Off

Unit is off. Fan is turned off, vents are closed, and the AV Control Unit
displays no settings. 

* Auto
  * Enter Auto state.
  * Restore previously set temperatures and Dual setting.
  * Turn A/C on.
  * Turn recirculate off.
  * Fan and Mode are automatically chosen.
* Dual
  * Makes no changes to unit state.
* Off
  * Makes no changes to unit state.
* A/C
  * Makes no changes to unit state.
* Driver Temp
  * AV Control Unit sends no control frames.
* Passenger Temp
  * AV Control Unit sends no control frames.
* Fan+
  * Enter Manual state.
  * Restore previously set temperatures and Dual setting.
  * Set fan to speed 1.
  * Turn recirculate off.
  * TODO: A/C state?
* Fan-
  * Enter Manual state.
  * Restore previously set temperatures and Dual setting.
  * Set fan to speed 1.
  * Turn recirculate off.
  * TODO: A/C state?
* Front Defrost
  * Enter Defrost state.
  * Restore previously set temperatures.
  * Enable only windshield vents.
  * Disable A/C, Dual, Recirculate.
* Mode
  * Enter Manual with Fan Off state.
  * Restore previously set temperatures, Dual setting, and Mode.
  * Turn A/C and recirculate off.
* Recirculate
  * Always toggles recirculate regardless of Mode.


#### Auto

The unit controls the fan speed and airflow in order to achieve the target
driver and passenger temperatures.

* Auto
  * No state changes.
* Dual
  * Toggle Dual.
  * Sets passenger temp to driver when Dual toggled off.
* Off
  * Enter Off state.
* A/C
  * Toggle A/C.
* Driver Temp
  * Change driver temperature.
  * Sets passenger temp to driver when Dual disabled.
* Passenger Temp
  * Change passenger temperature.
  * Enables Dual if previously disabled.
* Fan+
  * Enter Manual state.
  * Increase fan speed.
  * Control frames sent even at max speed.
* Fan-
  * Enter Manual state.
  * Decrease fan speed.
  * Control frames sent even at min speed.
* Front Defrost
  * Enter Defrost state.
  * Enable only windshield vents.
  * Disable Dual, Recirculate
  * Keep temperature, fan, and A/C state.
* Mode
  * Enter Manual state.
  * Cycle airflow.
  * Turn off recirculate if face vents close.
* Recirculate
  * Stays in Auto state.
  * Toggle recirculate if face vents open.

#### Manual

The user manually controls air temperature, fan speed, and airflow.

* Auto
  * Enter Auto state.
  * Turn on A/C.
  * Turn off Recirculate.
  * Keep temperature and Dual setting.
  * Fan and Mode are automatically chosen.
* Dual
  * Toggle Dual.
  * Sets passenger temp to driver when Dual toggled off.
* Off
  * Enter Off state.
* A/C
  * Toggle A/C.
* Driver Temp
  * Change driver temperature.
  * Sets passenger temp to driver when Dual disabled.
* Passenger Temp
  * Change passenger temperature.
  * Enables Dual if previously disabled.
* Fan+
  * Increase fan speed.
  * Control frames sent even at max speed.
* Fan-
  * Decrease fan speed.
  * Control frames sent even at min speed.
* Front Defrost
  * Enter Defrost state.
  * Enable only windshield vents.
  * Disable Dual, Recirculate
  * Keep temperature, fan, and A/C state.
* Mode
  * Cycle airflow.
  * Turn off recirculate if face vents close.
* Recirculate
  * Toggle recirculate if face vents open.

#### Manual with Fan Off

The user manually controls temperature and airflow but the fan is off.

* Auto
  * Enter Auto state.
  * Turn on A/C, Fan.
  * Turn off Recirculate.
  * Keep temperature and Dual setting.
  * Fan and Mode are automatically chosen.
* Dual
  * Toggle Dual.
  * Sets passenger temp to driver when Dual toggled off.
* Off
  * Enter Off state.
* A/C
  * Toggle A/C.
* Driver Temp
  * Change driver temperature.
  * Sets passenger temp to driver when Dual disabled.
* Passenger Temp
  * Change passenger temperature.
  * Enables Dual if previously disabled.
* Fan+
  * Enter Manual state.
  * Set fan speed to 1.
* Fan-
  * Enter Manual state.
  * Set fan speed to 1.
* Front Defrost
  * Enter Defrost state.
  * Set fan speed to 3.
  * Enable only windshield vents.
  * Disable Dual, Recirculate
  * Keep temperature, and A/C state.
  * TODO: Double check A/C state.
* Mode
  * Cycle airflow.
  * Does not turn off recirculate.
* Recirculate
  * Toggle recirculate.
  * Does not require that face vents be open.


#### Defrost

The unit blows air only on the windshield. Temperature and fan speeds are
controlled manually. Dual and recirculate is disabled.

* Auto
  * Enter Auto state.
  * Turn on A/C.
  * Turn off Recirculate.
  * Keep temperature and Dual setting.
  * Fan and Mode are automatically chosen.
* Dual
  * No state changes.
  * Does send control frame.
* Off
  * Enter Off state.
* A/C
  * Toggle A/C
* Driver Temp
  * change driver and passenger temperatures.
* Passenger Temp
  * No state changes.
  * Does send control frame.
* Fan+
  * Increase fan speed. 
* Fan-
  * Decrease fan speed.
* Front Defrost
  * If previously Off, Manual No Fan, or AUto:
    * Enter Auto state.
    * Keep Temperatures, A/C, Dual
  * If previously Manual:
    * Enter Manual state.
    * Keep Temperatures, Fan Speed, A/C, Dual
* Mode
  * Enter Manual state.
  * Cycle mode.
  * Turn On A/C
  * Keep Temperatures, Fan Speed, Dual
* Recirculate
  * No state changes.
  * Does send control frame.

### Airflow Transitions

The following state transitions are allowed when cycling airflow via the Mode
button:

* face -> face+feet
* face+feet -> feet
* feet -> feet+windshield
* feet+windshield -> face
* windshield -> feet+windshield

### Rear Defrost

_This section is inferred from the service manual. Functionality has not yet been tested._

The rear defrost turns on/off the rear window and side mirror heating elements.
This is independent of regular HVAC operations and so is controlled via a
separate wire to the BCM. This is the yellow wire on connector M98, pin 16, on
the A/C and AV Switch Assembly. This terminates at the BCM with connector M18,
pin 9.

The BCM provides 5v on this line. When this line is grounded momentarily the
BCM toggles the rear defrost. The BCM sends a CAN frame to the IPDM to enable
or disable to heating elements.

The A/C and AV Switch Assembly grounds this wire when the rear defrost button
is toggled on. The line is allowed to float when toggled off.

## CAN Protocol

The A/C Auto Amp maintains the current state of the climate control system. It
sends two CAN frames (0x54A and 0x54B) with the current state so that connected
systems can display that state to the user. Frames are sent periodically and
any time the state of the system changes. State changes may be in response to
CAN messages received from the AV Control Unit or due to temperature changes
when auto is enabled.

The AV Control Unit listens for CAN frames from the A/C Auto Amp and displays
the current state to the user via the LCD screen and LED's on the A/C buttons.

The AV Control Unit sends frames (0x540 and 0x541) to update the state of the
A/C Auto Amp. These are sent periodically with the current state or with the
updated state when an A/C button or encoder is activated by the user.

The AV Control Unit also controls the rear window and mirror defrost heaters
via CAN. A frame (0x35D) is sent to enable or disable the heating elements. The
heating elements are controlled by the IPDM. A frame (0x625) is sent by the
IPDM to indicate the current state of the heating elements.


### CAN Frame Definitions

The following CAN frames are exchanged by the climate control sytem.

| ID    | Len | Purpose                                                                                  | Sender          | Listener        |
| ----- | --- | ---------------------------------------------------------------------------------------- | --------------- | --------------- |
| 0x35D |   8 | External communication. Used to signal compressor state and control rear defrost heater. | AV Control Unit | BCM, ECU        |
| 0x540 |   8 | Button state. Requests changes to unit power, compressor, temperature, and mode.         | AV Control Unit | A/C Auto Amp    |
| 0x541 |   8 | Button state. Requests changes to fan speed and recirculation.                           | AV Control Unit | A/C Auto Amp    |
| 0x54A |   8 | Climate state. Sends current temperature configuration.                                  | A/C Auto Amp    | AV Control Unit |
| 0x54B |   8 | Climate state. Sends current power state, mode, fan speed, and recirculation.            | A/C Auto Amp    | AV Control Unit |
| 0x625 |   6 | Climate state. Sends current state of the rear defrost.                                  | Unknown         | Unknown         |


#### CAN Frame ID 0x35D

I have not fully decoded this this CAN frame but it appears to be sent form the
AV Control Unit or A/C Auto Amp to control the A/C compressor and defrost
heating elements.

```
    +---------------------- A/C compressor, heating elements
    |
35D#07:00:00:00:00:00:00:00
    0  1  2  3  4  5  6  7
```

_Byte 0: A/C Compressor and Defrost Heating Elements_

Bit meanings (least to most significant):
* Bit 0: Controls the A/C compressor. 0 is off, 1 is on.
* Bit 1: Controls the rear window or side mirror heating element. 0 is off, 1 is on.
* Bit 2: Controls the rear window or side mirror heating element. 0 is off, 1 is on.
* Bits 3-7: Always 0.

Bit 1 and 2 are always seen to have the same value. The rear defrost button
controls both the rear window heating element _and_ the side mirror heating
element. It is assumed that these are controlled independently by the IPDM and
so the two bits control these two items.  

Observed frame values:
* `00000000` defrost off, compressor off
* `00000001` defrost off, compressor on
* `00000110` defrost on, compressor off
* `00000111` defrost on, compressor on

_Bytes 1-7: Unknown_

The other bytes of the payload are repeated back. This may require the most
recently received frame to be stored in order to manipulate compressor and rear
defrost state.  


#### CAN Frame ID 0x540

This frame is sent by the AV Control Unit to initialize the A/C Auto Amp,
enable auto mode (this also turns on the unit), turn off the unit,
enable/disable the compressor, change the airflow direction (the "mode"), and
enable/disable dual zone control.

```
    +---------------------- ECU status
    |
    |  +------------------- ECU status
    |  |
    |  |     +------------- driver temperature control
    |  |     |
    |  |     |  +---------- passenger temperature control
    |  |     |  |
    |  |     |  |  +------- temperature control and A/C compressor
    |  |     |  |  |
    |  |     |  |  |  +---- mode, front defrost, dual zone, units
    |  |     |  |  |  |
540#60:40:00:00:00:00:04:00
    0  1  2  3  4  5  6  7
```

_Byte 0 and 1: Unit Status_

These bytes are changed in order to indicate the status of the ECU unit: either
initializing, acknowledging first packages from the A/C Auto Amp, or normal
operation. Handshake is covered in a later section.

| State | Byte 0 | Byte 1 |
| ----- | ------ | ------ |
| Init  |   0x80 |   0x00 |
| Ack   |   0x00 |   0x00 |
| Run   |   0x60 |   0x40 |

_Byte 3: Driver Temperature Control_

This is an 8-bit unsigned integer indicating the temperature of the driver side
climate zone. It does not correspond to a specific temperature but rather
increments or decrements the temperature by the relative amount. The initial
value at boot is 0x00. Incrementing this by 1 will increase the temperature
by 1. Once the A/C Auto Amp reaches max temperature it will not increment
further though the stored relative value will change. Decrementing by 1 will
decrease the temperature in the same manner.  

_Byte 4: Passenger Temperate Control_

This is an 8-bit unsigned integer indicating the temperature of the passenger
side climate zone. It works identically to the Byte 3.

_Byte 5: Temperature Control and A/C Compressor_

Bit meanings (least to most significant):
* Bit 0: Always 0.
* Bit 1: Always 0.
* Bit 2: Always 0.
* Bit 3: Indicates to the A/C Auto Amp whether or not to enable the A/C compressor. 1 is on, 0 is off as one might expect.
* Bit 4: Always 0.
* Bit 5: Flipped any time a temperature change occurs.
* Bit 6: Always 0.
* Bit 7: Always 0.

Observed values:
* `00000000` temperature change; compressor off
* `00100000` temperature change; compressor off
* `00001000` temperature change; compressor on
* `00101000` temperature change; compressor on

_Byte 6: Mode, Front Defrost, Dual Zone Control, and Units_

Bit meanings (least to most significant):
* Bit 0: Flipped to toggle the "mode" - the direction of the airflow.
* Bit 1: Flipped to toggle front defrost.
* Bit 2: Units. Set to 1 for US, 0 for Metric.
* Bit 3: Flipped to toggle dual zone climate control.
* Bit 4: Always 0.
* Bit 5: Flipped to enable the "auto" mode of the climate control unit.
* Bit 6: Always 0.
* Bit 7: Flipped to toggle the unit off.

Example values:
* `00100100` auto mode toggled
* `00001100` dual zone climate control toggled
* `00000110` front defrost toggled
* `00000101` air flow mode toggled

Flipping bit 3 will change the display units. This will cause a 54A frame to be
sent with the temperature values in the new units.

#### CAN Frame ID 0x541

This frame is sent by the AV Control Unit to change fan speed and toggle
recirculation. 

```
    +---------------------- change fan speed
    |
    |  +------------------- toggle recirculation
    |  |
541#00:00:00:00:00:00:00:00
    0  1  2  3  4  5  6  7
```

_Byte 0: Change Fan Speed_

Bit meanings (least to most significant):
* Bit 0: Always 0.
* Bit 1: Always 0.
* Bit 2: Always 0.
* Bit 3: Always 0.
* Bit 4: Flipped to decrease fan speed.
* Bit 5: Flipped to increase fan speed.
* Bit 6: Always 0.
* Bit 7: Always 0.

_Byte 1: Toggle Recirculation_

Bit meanings (least to most significant):
* Bit 0: Always 0.
* Bit 1: Always 0.
* Bit 2: Always 0.
* Bit 3: Always 0.
* Bit 4: Always 0.
* Bit 5: Always 0.
* Bit 6: Flip to toggle recirculation.
* Bit 7: Always 0.

_Bytes 2-7_

These bytes are always set to 0x00.


#### CAN Frame ID 0x54A

This frame is sent by the A/C Auto Amp to indicate the zone temperatures.

```
    +---------------------- unknown; either 0x3C or 0xDA
    |
    |  +------------------- always 0x3E
    |  |
    |  |  +---------------- always 0x7F
    |  |  |
    |  |  |  +------------- units
    |  |  |  |
    |  |  |  |  +---------- current driver temperature
    |  |  |  |  |
    |  |  |  |  |  +------- current passenger temperature
    |  |  |  |  |  |
    |  |  |  |  |  |     +- outside temperature
    |  |  |  |  |  |     |
54A#3C:3E:7F:80:00:00:00:45
    0  1  2  3  4  5  6  7
```

_Byte 0: Unknown_

This has been observed to flip between the values 0x3C and 0xDA. It is not
known what causes the value to change.

_Byte 1: Unknown_

Always 0x3E.

_Byte 2: Unknown_

Always 0x7F.

_Byte 3: Units_

The display units. Valid values are:
* 0x40: Metric (Celsius)
* 0x80: US (Fahrenheit)

_Byte 4: Driver Temperature_

This is an 8-bit unsigned integer representing the temperate of the driver side
climate zone. Values are 0 when climate unit is off and from 60 to 90F when
on.

_TODO: Determine Celsius temperature range._

_Byte 5: Passenger Temperature_

This is an 8-bit unsigned integer representing the temperate of the passenger
side climate zone. Values are 0 when climate unit is off and from 60 to 90F
when on.

_TODO: Determine Celsius temperature range._

_Byte 6: Unknown_

Always 0x00.

_Byte 7: Unknown_

This is the outside temperature as reported by the temperature sensor connected
to the A/C Auto Amp. This is an 8-bit unsigned integer.


#### CAN Frame ID 0x54B

This frame is sent by the A/C Auto Amp to indicate unit status, mode, front
defrost, fan speed, recirculation, and dual climate zone mode.

```
    +---------------------- A/C compressor, auto mode, power off
    |
    |  +------------------- mode, defrost
    |  |
    |  |  +---------------- fan speed
    |  |  |
    |  |  |  +------------- recirculation, dual zone control
    |  |  |  |
    |  |  |  |           +- unknown; either 0x00 or 0x02
    |  |  |  |           |
54B#59:84:08:12:00:00:00:02
    0  1  2  3  4  5  6  7
```

_Byte 0: A/C Compressor, Auto Mode, Power Off_

Bit meanings (least to most significant):
* Bit 0: Auto setting. Set to 1 when auto is on.
* Bit 1: Manual setting. Set to 1 when manual mode enabled or unit is off.
* Bit 2: Always 0.
* Bit 3: A/C compressor. Set to 1 when A/C compressor is requested.
* Bit 4: Unknown. Set to 1 when A/C is on or when unit is off.
* Bit 5: Unknown.
* Bit 6: Always 1.
* Bit 7: Unit power. Set to 1 when power is fully off.

Observed values:
* `0x41 (01000001)` A/C Off, Auto On
* `0x42 (01000010)` A/C Off, Auto Off
* `0x59 (01011001)` A/C On, Auto On
* `0x5A (01011010)` A/C On, Auto Off
* `0x72 (01110010)` Manual with Fan Off
* `0xF2 (11110010)` Power Off

This value does not seem to be consistent when defrost mode is one. To
determine when the unit is in "manual with no fan" mode, bit 7 and fan speed
must be checked. Otherwise the unit may be in defrost mode - in which case a
value of 0x72 is valid for byte 0 even though the fan is on.

_Byte 1: Mode, Front Defrost_

Observed values:
* `0x00 (00000000)` Power Off
* `0x04 (00000100)` Face
* `0x08 (00001000)` Face and Feet
* `0x0C (00001100)` Feet
* `0x10 (00010000)` Feet and Windshield
* `0x34 (00110100)` Windshield
* `0x84 (10000100)` Auto; Cooling; Face
* `0x88 (10001000)` Auto; Neutral; Face and Feet
* `0x8C (10001100)` Auto; Heating; Feet

Windshield (defrost) is not possible in auto mode. Change the mode or enabling
defrost takes the unit out of auto mode. 

_Byte 2: Fan Speed_

This is an unsigned integer between 0x00 and 0x0F representing eight fan
speeds. The value increments and decrements by 0x02. The unit may sometimes
indicate half speeds. It is assumed that the value is rounded up in these
cases.

```
speed = (x + 1) / 2
```

Fan speed it reported as 0x00 when the unit is off.

_Byte 3: Recirculation and Dual Zone Control_

Bit meanings (least to most significant):
* Bit 0: Always 0.
* Bit 1: Unknown.
* Bit 2: Unknown.
* Bit 3: Always 0.
* Bit 4: Recirculation. 1 is on, 0 is off.
* Bit 5: Recirculation. 0 is on, 1 is off.
* Bit 6: Dual zone control. 0 is off, 1 is on.
* Bit 7: Dual zone control. 0 is off, 1 is on.

Observed values:
* `0x12 (00010010)` recirculation on, dual off
* `0x24 (00100100)` recirculation off, dual off
* `0xD2 (11010010)` recirculation on, dual on
* `0xE2 (11100010)` recirculation off, dual on

_Bytes 4-6_

Always 0x00.

_Byte 7: Unknown_

This has been observed to flip between the values 0x00 and 0x02. It is not
known what causes the value to change. It may be to indicate a state change.


#### CAN Frame ID 0x625

This frame is sent when the rear defrost is toggled. The frame originates from
the IPDM and is acted on by the AV Control Unit to display the rear defrost
status.

```
    +---------------- rear defrost heating element
    |
625#32:60:FF:1D:00:00
    0  1  2  3  4  5
```

_Byte 0: A/C Compressor and Rear Defrost Heater_

Bit meanings (least to most significant):
* Bit 0: Indicates the rear window or side mirror heating element is on. 0 is off, 1 is on.

Observed values:
* `00100000` rear defrost heating element off
* `00100001` rear defrost heating elemnt on


### Unit Initialization

The A/C Auto Amp does not begin operating immediately at power on state. It
first performs a basic handshake with the AV control Unit before becoming
operational.


#### Handshake

The climate control systems boot when the ignition is turned on. At boot time
the AV Control Unit sends a set of initialization frames to the A/C Auto Amp.
After theses frames are sent the AV Control Unit enters its regular operating
mode and sends input state changes to the A/C Auto Amp.

The AV Control Unit sends at least four pairs of `0x540` and `0x541`
initialization frames. These are sent at an interval of about 100ms. These
frames have the most significant bit set:

```
540#80:00:00:00:00:00:00:00
541#80:00:00:00:00:00:00:00
```

These are meant to zero the input state of the A/C Auto Amp so that it can
detect state changes in future frames.

The AV Control Unit then sends a pair of control frames with the initial input
state: 

```
540#60:40:00:00:00:00:04:00
541#00:00:00:00:00:00:00:00
```

At this point the AV Control Unit is fully initialized for A/C control.


#### Normal Operation

Under normal operation the A/C Auto Amp and AV Control Unit send frames at a
regular interval to indicate their current state. A state change in a unit
causes them to send updated frames immediately.

The A/C Auto Amp responds to `0x540` and `0x541` frames from the AV Control
Unit.

![R51 Climate Operation](images/r51_climate_operation.png "R51 Climate Operation")

The `0x540` and `0x541` frames appear to repeat at an interval which varies
between 100ms and 1200ms.

The AV Control Unit flips bits in the control frames to indicate the A/C Auto
Amp that an input was triggered (button press or rotary encoder movement). The
AV Auto Amp also tracks the current temperature settings for driver and
passenger zones and includes them in the control frame. A temperature change is
accompanied by a bit flip.
