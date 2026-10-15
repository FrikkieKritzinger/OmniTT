# Universal Model Railroad Turntable Controller
# Project name OmniTT
## System Requirements Specification

| | |
|---|---|
| **Document status** | Draft v0.30 |
| **Date** | 2026-10-07 |
| **Target platform** | ESP32 (Wemos D1 Mini form factor) |

---

## 1. Introduction

### 1.1 Purpose
This document specifies the requirements for a universal controller for model railroad turntables and traversers. The controller operates standalone through a local user interface, or accepts DCC accessory commands through a LocoNet interface.

### 1.2 Scope
The controller covers:
- Positioning the bridge (turntable) or deck (traverser) at configured exit positions.
- Driving either a stepper motor or a standard DC motor.
- Switching bridge track polarity, using an internal relay or an external reverse-loop module.
- Local configuration through a colour display and rotary encoder.
- Remote control through LocoNet. **[DEFERRED – later phase]**
- Firmware updates over Wi-Fi.

Out of scope: accessory and layout control (for example roundhouse doors or shed lights). These belong to separate decoders, not to the turntable controller.

### 1.3 Definitions

| Term | Definition |
|---|---|
| **TT** | Turntable. In this document it also covers a traverser unless stated otherwise. |
| **Bridge** | The rotating (turntable) or sliding (traverser) track section. |
| **A end / Stall end** | The end of the turntable bridge with the control cabin. Used as the reference for polarity and orientation. |
| **Exit** | A configured stopping position where the bridge lines up with an approach track. |
| **Index** | A physical reference point detected by the index sensor, normally one per exit on indexed turntables. |
| **Home** | The reference position all other positions are measured from. |
| **RUN mode** | The normal operating mode. |
| **MENU mode** | The configuration mode. |
| **AP mode** | Wi-Fi Access Point mode. The controller hosts its own network. |

### 1.4 Requirement conventions
- **shall** means mandatory.
- **should** means recommended or preferred.
- **[OPEN]** marks an item that still needs a decision. All open items are collected in [Section 10](#10-open-issues).

---

## 2. System Overview

The controller has these parts:
- An ESP32 microcontroller in a D1 Mini form factor.
- A graphical SPI colour display (2.0" IPS, 240×320, ST7789V).
- An EC11 rotary encoder with a push button.
- A MENU button.
- An emergency STOP input.
- A home microswitch input.
- A TB6612FNG dual H-bridge driving the motor and the lock solenoid.
- Track and bridge power switching.
- A LocoNet interface (later phase).

```
           ┌──────────────┐
 (LocoNet, later)┤           ├── Display (SPI)
 Main PWR ─┤              ├── Rotary encoder + button
 Track PWR ┤   ESP32      ├── MENU button / E-STOP
 Home ─────┤ (D1 Mini)    ├── Motor + lock solenoid (TB6612FNG)
           │              ├── Bridge power OUT (track pass-through)
           └──────┬───────┘
                  └── Wi-Fi (config / firmware update)
```

---

## 3. Hardware Requirements

### 3.1 Controller

| ID | Requirement |
|---|---|
| HW-01 | The controller shall use an ESP32 microcontroller in a Wemos D1 Mini form factor (e.g., ESP32-WROOM-32 with D1 Mini pinout). |
| HW-02 | The controller shall provide Wi-Fi in both station (client) and Access Point modes. |
| HW-03 | The power input shall only be used to power the D1, display, and motor control modules and motors. |
| HW-04 | The controller shall not provide power to the bridge, but switch power from supplied DCC or DC track power only |
| HW-05 | The default configuration will pass track power through, and rely on the TT to invert polarity. This can be changed in config to allow an onboard relay to handle power inverting to the bridge. **[DEFERRED – relay build, later phase]** |
| HW-06 | **[DEFERRED – later phase, with LocoNet]** Local selections and command will take preference over Loconet. ie loconet will only be applied if the bridge is not already busy with another movement |

### 3.2 User interface hardware

| ID | Requirement |
|---|---|
| HW-10 | The controller shall have a graphical colour display connected over SPI: Waveshare 2.0" IPS, 240×320, ST7789V controller (spec: `docs/Waveshare_2inch_LCD_Module.pdf`, local copy, not committed). RST is tied to the ESP32 RST line, CS is tied low, and the backlight (BL, about 45 mA, four white LEDs) is driven by PWM from GPIO26 through a MOSFET. |
| HW-11 | The display is a 2.0" IPS (240×320), selected 2026-10-14. This is below the 2.4" minimum recorded under OI-01. **[CONFIRM – see OI-01 note]** |
| HW-12 | The controller shall have one EC11-type rotary encoder module with an integrated push button. |
| HW-13 | The controller shall have a dedicated MENU push button to switch between RUN mode and MENU mode. **[RESOLVED – OI-02]** |
| HW-14 | The controller shall have a dedicated emergency STOP push button, wired normally closed so that a broken wire stops the bridge. **[RESOLVED – OI-03]** |
| HW-15 | The eStop button shall stop bridge movement immediately |
| HW-16 | The controller shall have a 12-pixel addressable RGB (NeoPixel) status ring mounted under the rotary encoder knob. The ring's 35 mm inside diameter fits the 35 mm aluminium knob, leaving the pixels visible around the knob edge. Implementation deferred to a later phase. |
| HW-17 | The four shortcut buttons (PRESET A, PRESET B, LAST EXIT, SPEED) shall share one ADC input through a series resistor ladder: 1 kΩ from 3.3 V, then 430 Ω, 270 Ω, 430 Ω, 680 Ω and 1.3 kΩ to GND, with a 0.1 µF filter on the ADC node. Each button taps a point on the ladder. The levels are 992, 1359, 1755 and 2123 mV for the four buttons, and 2497 mV for no button. A reading shall be accepted only within 150 mV of a level. A reading below 700 mV is a fault and shall never be treated as a button press. |

### 3.3 Electrical interfaces

| ID | Interface | Direction | Description |
|---|---|---|---|
| HW-20 | LocoNet | Bidirectional | **[DEFERRED – later phase]** LocoNet bus connection for DCC control. |
| HW-21 | Main power | IN | Supply for the controller and motor. |
| HW-22 | Track power | IN | DC or DCC track power feed. |
| HW-23 | Bridge power | OUT | Switched and polarity-controlled track power to the bridge. |
| HW-24 | Motor | OUT | DC motor terminals, driven from TB6612FNG channel A (HW-26). |
| HW-25 | Lock solenoid terminals | OUT | Driven from TB6612FNG channel B (HW-28). |
| HW-26 | Motor driver | OUT | DFRobot TB6612FNG dual H-bridge board: 4 GPIO (2 dir, 2 PWM). Channel A drives the motor, channel B drives the lock solenoid (HW-28). Each channel is rated 1.2 A at 12 V. The board has no standby line. **[RESOLVED – OI-04, OI-13]** |
| HW-27 | Home sensor | IN | Microswitch at home position (exit 0). Detects home for position recalibration. **[RESOLVED – OI-08/09]** |
| HW-28 | Lock solenoid | OUT | Lock solenoid driven from TB6612FNG channel B (HW-26). Typically 12 V at a few hundred mA. Held open while the motor runs and released to lock at an index. Matches the MOVE line of the Fleischmann 6152, and of similar Fleischmann, Roco and Märklin turntables. **[RESOLVED – OI-12]** |

### 3.4 Physical layout

| ID | Requirement |
|---|---|
| HW-30 | The enclosure is a Sanhe 18-33 console enclosure. The display sits in the rectangular display window. |
| HW-31 | The rotary encoder with knob sits to the right of the display window, as the enclosure provides. |
| HW-32 | Six button positions sit in the bottom-right area of the console. The MENU and E-STOP buttons are placed here; the remaining four are assigned to shortcuts (section 5.5). The MENU button does not need to be adjacent to the display. **[RESOLVED – OI-02/03]** |

---

## 4. Operating Modes

The controller shall have two top-level modes:

| Mode | Purpose |
|---|---|
| **RUN** | Normal operation: shows the bridge position, selects and drives to exits, and handles stop events. |
| **MENU (Config)** | Configures the turntable, exits, DCC/LocoNet, network, and maintenance functions. |

| ID | Requirement |
|---|---|
| MD-01 | The controller shall start up in RUN mode. |
| MD-02 | Activating the MENU input in RUN mode shall switch to MENU mode. |
| MD-03 | MENU mode shall provide a way to return to RUN mode. |
| MD-04 | MENU mode shall not be activated if a move is in progress. |
| MD-05 | All local and LocoNet commands to control the bridge movement shall be ignored when in MENU mode. |

---

## 5. RUN Mode Requirements

### 5.1 Display and selection

| ID | Requirement |
|---|---|
| RUN-01 | The display shall show the current bridge position graphically. |
| RUN-02 | The graphic shall match the configured TT type (turntable or traverser). |
| RUN-03 | The display shall show a **selection ring** (cursor) that marks the target exit in TT mode, or 2 brackets to highlight the target exit in Traverser mode. |
| RUN-04 | Turning the rotary encoder shall move the selection ring (or selection brackets) through the configured exits. |
| RUN-05 | Pressing the encoder button shall confirm the selected exit as the target. |
| RUN-06 | After a target is confirmed, the controller shall rotate or move the bridge to that exit automatically. |
| RUN-07 | The display must update to indicate the movement of the bridge from current position to target exit aligned with bridge movement. |


### 5.2 Emergency stop

| ID | Requirement |
|---|---|
| RUN-10 | The controller shall have an emergency STOP function that halts bridge motion immediately. |
| RUN-11 | An emergency STOP shall be triggered by the local STOP input. A DCC/LocoNet STOP command shall also trigger it, once LocoNet is added (later phase). |
| RUN-12 | While a STOP is active, the display shall change to a distinct stop screen. |
| RUN-13 | The stop screen shall show whether the stop came from DCC or from the local input. **[DCC source deferred – later phase]** |
| RUN-14 | **[DEFERRED – later phase]** When LocoNet is enabled (DCC-01), a local eStop shall push a STOP command to LocoNet. |

### 5.3 Position tracking

| ID | Requirement |
|---|---|
| RUN-20 | The controller shall keep the current bridge position in non-volatile memory (EEPROM), so it knows where it is at startup without seeking home every time. |
| RUN-21 | During normal operation, the controller shall keep track of the home position so it cannot lose its position. For example, it shall re-verify home whenever the bridge passes it. |
| RUN-22 | After power-up, the bridge position shall be taken as the last saved position (DAT-02) and shown as uncertain (red pulse, RUN-30). An orderly restart and a power interruption, including one during a move, shall be treated the same way. |
| RUN-23 | On the first move after power-up, the assumed position (RUN-22) is treated as the start of the move. The bridge shall move by the fastest path from that assumed position, as for any move. Step counting and solenoid release timing shall not start until the home sensor is passed, which sets the position to HOME. If the target is reached before home is passed, the bridge shall keep moving in the same direction, without releasing the solenoid, until home is passed. It shall then continue to the selected exit (RUN-06). The move timeout (CFG-14) shall cover a full revolution plus the home-to-target distance. |
| RUN-24 | After the position is determined (RUN-23), the controller shall track position by dead reckoning and re-confirm it at every home pass. |
| RUN-25 | A move shall release the lock solenoid after the bridge passes the previous fitted index in the direction of travel, and before the target index. The release shall be timed mid-gap using CFG-08, so that the timing tolerance is half the gap. Releasing earlier would let the pin drop into the earlier index. |
| RUN-26 | After the solenoid is released, the motor shall keep running until the lock pin drops into the target index, which cuts motor power. The controller shall stop the motor PWM after the expected arrival time plus a margin. The move timeout (CFG-14) shall apply as the backstop. |
| RUN-27 | Calibration of CFG-08 shall be a one-time setup procedure, run from the menu or the web page (MNU-24 rules), that drives one full revolution from home. It shall not run at power-up. |
| RUN-28 | Every move shall use the fastest path from the current position to the target, except the first move after power-up (RUN-23). A configurable direction preference is deferred to a later phase. |

---

### 5.4 Status ring

The status ring (HW-16) shows the controller state from across the room. Detailed information stays on the display.

| ID | Requirement |
|---|---|
| RUN-30 | While the bridge position is unknown (before the first home pass, OI-09), the status ring shall pulse red. |
| RUN-31 | While a move is in progress, the status ring shall show amber. |
| RUN-32 | When the bridge is locked at a known position and idle, the status ring shall show green. |
| RUN-33 | While an emergency STOP is active, the status ring shall show solid red. |

### 5.5 Shortcut buttons

The four console buttons not used for MENU or E-STOP (HW-32) are assigned to shortcuts.

| ID | Requirement |
|---|---|
| RUN-40 | Pressing the PRESET A or PRESET B button shall move the bridge to the exit stored for that preset, as a local selection (RUN-06). |
| RUN-41 | Holding a PRESET button for the preset hold time (CFG-11) shall store the current exit as that preset. |
| RUN-42 | Pressing the LAST EXIT button shall move the bridge to the exit it occupied before the current one. |
| RUN-43 | Shortcut actions shall follow the same rules as encoder selection: ignored in MENU mode (MD-05), and not acted on while the bridge is moving (HW-06). |
| RUN-44 | Pressing the SPEED button shall toggle the move mode between NORMAL and RAPID. The selected mode applies to the next move and persists across power cycles (DAT-06). RAPID is a hard lockout: it is disabled until the bridge position is determined (the first home pass, OI-09). Once determined, by first home pass, dead reckoning or subsequent home passes, RAPID is available. No configuration setting or operator action overrides this lockout. |

---

## 6. MENU Mode Requirements

### 6.1 General menu behaviour

| ID | Requirement |
|---|---|
| MNU-01 | Menu navigation shall follow the structure defined in [Section 7](#7-menu-structure). |
| MNU-02 | The rotary encoder shall navigate menu items and change values. |
| MNU-03 | The encoder button shall select menu items or function as ENTER or CONFIRM button. |
| MNU-04 | Every screen where data can be changed shall offer **SAVE** and **CANCEL** options. |
| MNU-05 | **SAVE** shall write the changed values to EEPROM. |
| MNU-06 | **CANCEL** shall discard any unsaved changes. |
| MNU-07 | Destructive operations (Clear exits, Factory reset) shall require explicit user confirmation before they run. |

### 6.2 Factory defaults

| ID | Requirement |
|---|---|
| MNU-10 | A clean factory configuration shall be stored in a reserved memory region that is never overwritten during normal operation. |
| MNU-11 | A factory reset shall restore all configuration from that reserved region. |

### 6.3 Web configuration

| ID | Requirement |
|---|---|
| MNU-20 | Configuration through a web page shall be provided in addition to the local menu. **[RESOLVED – OI-06]** |
| MNU-21 | The web configuration shall offer the same settings, with the same labels, as the local menu. |
| MNU-22 | Secret values (Wi-Fi password) shall be write-only on the web page: once saved, they shall never be displayed again. |
| MNU-23 | The access point and web service shall start only when the user selects Setup Wi-Fi or Firmware Update (UPD-01), and shall stop after an idle timeout or on leaving the menu. A blank or incomplete configuration shall enter setup mode automatically. |
| MNU-24 | Calibration moves started from the web page (to set CFG-08) shall be allowed only when the bridge is idle, not in MENU mode, and not in E-stop. The web page shall offer a STOP action during such a move. |

Deferred to a later stage: password protection (admin PIN) for the web configuration, and access from the home network (station mode).

---

## 7. Menu Structure

### 7.1 Menu tree

```
├── Run
└── Config
    ├── TT Setup
    │   ├── TT Name
    │   ├── TT Type
    │   ├── No. of Exits
    │   ├── Stall End
    │   ├── Enable Internal Relay?
    │   ├── Motor Type ── [Stepper | Standard]
    │   ├── Configure Steps
    │   ├── Index Hold Time
    │   └── Home
    ├── Exit Configuration
    │   ├── Add
    │   ├── Remove
    │   └── Clear
    ├── DCC Setup
    │   ├── Enable LocoNet?
    │   └── DCC Address
    ├── Setup Wi-fi
    │   ├── Wi-Fi SSID/Name
    │   ├── Wi-Fi Password
    │   └── IP Address
    ├── Reset
    ├── Firmware Update
    └── Info
```

### 7.2 Run
The normal operating status screen. See [Section 5](#5-run-mode-requirements).

### 7.3 Config → TT Setup
Opens the turntable setup values.

| ID | Parameter | Values | Description |
|---|---|---|---|
| CFG-01 | **TT Name** | Text | A unique name for this turntable, typically the station or service terminal it serves. |
| CFG-02 | **TT Type** | Turntable / Traverser | Sets the on-screen graphics and the related behaviour. |
| CFG-03 | **No. of Exits** | 0 – n | The total number of exits. For fixed-position (indexed) turntables such as Roco, Fleischmann or Märklin, enter the number of exits. Enter **0** for unindexed or infinite-position turntables such as PECO, Walthers or DIY builds. |
| CFG-04 | **Stall End** | A end reference | Sets the A end of the bridge (the control-cabin end). Applies to turntables only; a traverser has no polarity inversion. |
| CFG-05 | **Enable Internal Relay?** | Yes / No | **Yes:** the controller inverts bridge power at the crossover point using its internal relay. **No:** an external reverse-loop module handles track power changeover. **[DEFERRED – relay build, later phase]** |
| CFG-06 | **Motor Type** | Stepper / Standard | **Stepper:** a standard stepper motor drives the bridge. **Standard:** a standard DC motor drives the bridge. |
| CFG-07 | **Configure Steps** | Integer | The number of steps per 360° rotation. Applies to stepper motors only. |
| CFG-08 | **Index Hold Time** | Time per step, NORMAL and RAPID | Calibrated time for the bridge to travel one 7.5° step, measured separately for each speed mode. Learned in the one-time setup calibration (RUN-27). Used to predict the release point and arrival time (RUN-25, RUN-26). |
| CFG-09 | **Home** | Procedure | Sets the home position, which is the reference for all other positions. |
| CFG-10 | *Other* | — | Reserved for future configuration values. |
| CFG-11 | **Preset Hold Time** | Time | How long a PRESET button must be held to store the current exit as that preset (RUN-41). |
| CFG-12 | **Normal Move Speed** | 0–100 % | Motor PWM speed for NORMAL moves (RUN-44). Used for short moves and when the position is still being found. |
| CFG-13 | **Rapid Move Speed** | 0–100 % | Motor PWM speed for RAPID moves (RUN-44). |
| CFG-14 | **Move Timeout** | Time | Maximum duration of a move. If exceeded, the controller stops the motor and reports an error. |
| CFG-15 | **Ramp Time** | Time | Soft-start acceleration time to reduce mechanical wear. 0 disables the ramp. |
| CFG-16 | **Status Ring Brightness** | 0–100 % | Brightness of the status ring (HW-16). |
| CFG-17 | **Display Brightness** | 0–100 % | Backlight brightness, set by software PWM on GPIO26. 0 turns the backlight off. |

### 7.4 Config → Exit configuration
Sets the position, number and name of each exit.

| ID | Option | Description |
|---|---|---|
| EXT-01 | **Add** | Saves a new exit at a position step number 0 to 47, where each step is 7.5°. Only steps with a fitted index piece are stops. |
| EXT-02 | **Remove** | Removes an existing exit position. |
| EXT-03 | **Clear** | Removes **all** exit positions. ⚠️ This cannot be undone and shall require confirmation (MNU-07). |

### 7.5 Config → DCC Setup **[DEFERRED – later phase]**
Enables or disables DCC control and sets the DCC base address. LocoNet is the intended DCC interface, not a direct DCC track decoder.

| ID | Parameter | Values | Description |
|---|---|---|---|
| DCC-01 | **Enable LocoNet?** | Yes / No | **Yes:** the turntable can be controlled through LocoNet and locally. **No:** local control only. |
| DCC-02 | **DCC Address** | Integer (base) | Applies only when LocoNet is enabled. Sets the base accessory address. Each exit's address is **base address + exit number**. Example: base 200, exit 3 → address **203**. A round base number (200, 250, 500, …) is recommended so exit numbers are easy to work out. |

### 7.6 Config → Wi-Fi Setup
Opens the Wi-Fi setup values.

| ID | Parameter | Values | Description |
|---|---|---|---|
| WIFI-01 | **Wi-Fi Name** | SSID | Selects the Wi-Fi network to join. |
| WIFI-02 | **Wi-Fi Password** | Text | The password for the selected Wi-Fi network. |
| WIFI-03 | **IP Address** | DHCP / Static | Chooses DHCP or a static IP address. |

### 7.7 Config → Reset
| ID | Requirement |
|---|---|
| RST-01 | Performs a factory reset. ⚠️ **This erases all user data.** It shall require confirmation (MNU-07) and shall restore the factory configuration (MNU-11). |

### 7.8 Config → Update
| ID | Requirement |
|---|---|
| UPD-01 | Starts Wi-Fi Access Point mode and serves a local web page for uploading new firmware to the device. |

### 7.9 Config → Info
| ID | Requirement |
|---|---|
| INF-01 | Shows details of the firmware on the unit, such as version and build information. |

---

## 8. LocoNet / DCC Requirements **[DEFERRED – later phase. Not in the initial build]**

| ID | Requirement |
|---|---|
| LN-01 | When LocoNet is enabled (DCC-01), the controller shall accept accessory commands addressed to the configured exit addresses (DCC-02). |
| LN-02 | Receiving a command for an exit address shall move the bridge to that exit, exactly as a local selection would (RUN-06). |
| LN-03 | A DCC/LocoNet STOP command shall trigger the emergency stop (RUN-11). |
| LN-04 | Local control shall stay available while LocoNet is enabled. |

---

## 9. Data Persistence Requirements

| ID | Requirement |
|---|---|
| DAT-01 | All configuration values shall be stored in EEPROM and kept across power cycles. |
| DAT-02 | The current bridge position shall be stored in EEPROM (RUN-20). |
| DAT-03 | Factory defaults shall be stored in a reserved region that normal operation never overwrites (MNU-10). |
| DAT-04 | Position writes should be designed to limit flash wear, for example by writing only when motion completes rather than continuously. |
| DAT-05 | Preset exits (RUN-40/41) and the last exit (RUN-42) shall be stored in EEPROM and kept across power cycles. |
| DAT-06 | The selected move mode (RUN-44) shall be stored in EEPROM and kept across power cycles. |

---

## 10. Open Issues

Questions and design considerations from the source material, still to be resolved.

| ID | Topic | Status & Decision |
|---|---|---|
| OI-01 | **Display** | ✅ **RESOLVED (2026-10-07):** Display priority is SIZE. Use an SPI display. **Superseded 2026-10-14:** the Waveshare 2.0" IPS (240×320, ST7789V) is selected (HW-10). It is below the 2.4" minimum, so HW-11 needs confirming. |
| OI-11 | **GPIO availability** | ✅ **RESOLVED (2026-10-07):** Upgrade from ESP8266 D1 Mini to ESP32 D1 Mini. Provides 30+ usable GPIO with no expander needed. Eliminates pin constraints, simplifies hardware, enables future expansion. |
| OI-04 | **Motor driver** | ✅ **RESOLVED (2026-10-07):** Use TB6612FNG Dual Channel H-Bridge modules. 4 GPIO pins per module (2 direction, 2 PWM). Can drive stepper or DC motor up to 12V/1.2A. Scope Phase 1: Fleischmann 6152 (48 positions, solenoid-locked, bidirectional DC motor). |
| OI-05 | **Position tracking** | ✅ **RESOLVED (2026-10-07):** Time-based tracking via calibrated motor timing. Learn milliseconds-per-position via menu option "Index Hold Time". Dead-reckoning between home passes. Solenoid auto-locks at each index. First move after power-up recalibrates position via home sensor. |
| OI-08 | **Home detection** | ✅ **RESOLVED (2026-10-07):** Microswitch at home position (exit 0). Detects when bridge passes home. Used to recalibrate position on first move after power-up. The home sensor is on board label TDO (GPIO15). |
| OI-09 | **Startup behavior** | ✅ **RESOLVED (2026-10-07):** Display bridge icon flashing RED from startup until first home pass. Once home sensor detects, icon turns GREEN and position is set to HOME. Then continue to target exit. Subsequent moves use dead-reckoning (no red flash). Most bridges remain where left, so this is pragmatic and fast. |
| OI-02 | **MENU input** | ✅ **RESOLVED (2026-10-11):** Dedicated MENU push button. The encoder stays for navigation, with its click providing haptic feedback. Button placement does not need to be next to the display. |
| OI-03 | **Emergency STOP input** | ✅ **RESOLVED (2026-10-11):** Dedicated E-STOP push button, placed among the six console buttons. |
| OI-06 | **Web configuration** | ✅ **RESOLVED (2026-10-11):** Web configuration mirrors the local menu (MNU-20..24). It runs on the access point, started on demand. Secrets are write-only. Calibration moves are allowed; movement control is not. Password protection and home-network access are deferred. |
| OI-07 | **Additional config values** | ✅ **RESOLVED (2026-10-11):** Added CFG-12..17: normal and rapid move speed, move timeout, ramp time, status ring and OLED brightness. Relay crossover angle deferred until a relay build needs it (CFG-05). |
| OI-10 | **Position persistence** | ✅ **RESOLVED (2026-10-11):** Position is saved in EEPROM when motion completes (DAT-02/04). After power-up, the saved position is assumed and shown uncertain until the first home pass (RUN-22/23). An interrupted move is treated the same as an orderly restart, so no move-in-progress flag is needed. |
| OI-12 | **Lock solenoid output** | ✅ **RESOLVED (2026-10-11):** The solenoid runs from TB6612FNG channel B. The 6152 motor and solenoid are both 12 V and draw a few hundred mA, well within the 1.2 A rating. Pins: channel B uses PWM2 (board label 4) and DIR2 (board label 33, set by a jumper). Both pins must be held low at reset (external pull-downs) so the solenoid stays off during boot. |
| OI-13 | **TB6612FNG standby** | ✅ **RESOLVED (2026-10-11):** The DFRobot TB6612FNG board has no standby line, so no GPIO is used. |
| OI-14 | **Pin budget and strapping pins** | ✅ **RESOLVED (2026-10-11):** The initial build uses 16 pins. Strapping pins in use: TDO only (home sensor, pulled up at boot). Four shortcut buttons share one ADC input (HW-17). MENU and E-STOP have their own pins on the button board. The display's RST uses no GPIO, and BL uses GPIO26 (PWM). Labels 0, 2, 5, TDI, SVP and SVN are unused. LocoNet pins (labels 16 and 17) are reserved for the later phase. The relay is deferred to a later phase (HW-05, CFG-05), so GPIO2 is not needed. No I²C expander is used. |
| OI-15 | **Index hold and index spacing** | ✅ **RESOLVED (2026-10-12):** Only fitted index pieces are stops; the 48 positions are 7.5° apart. The lock is passive: once released, the pin rubs until it drops into the next index, which cuts motor power. The solenoid is released mid-gap after the previous index (RUN-25). CFG-08 is the calibrated time per step at each speed, learned once by a full-revolution calibration (RUN-27). No per-gap table is needed. |
| OI-16 | **Hardware design review** | Open for discussion. Covers the exposed pins and silkscreen of the chosen D1 Mini ESP32 board compared with the pin map, the level shifter for the status ring (WS2812 data at 5 V), the external pull-up and pull-down resistors, power supply and wiring, and the enclosure layout. |
