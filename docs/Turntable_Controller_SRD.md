# Universal Model Railroad Turntable Controller
# Project name OmniTT
## System Requirements Specification

| | |
|---|---|
| **Document status** | Draft v0.14 |
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
- Local configuration through an OLED display and rotary encoder.
- Remote control through LocoNet.
- Firmware updates over Wi-Fi.

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
- An ESP8266 microcontroller.
- A graphical OLED display.
- An EC11 rotary encoder with a push button.
- A MENU button.
- An emergency STOP input.
- Motor drive outputs for a stepper or DC motor.
- An index sensor input.
- Track and bridge power switching.
- A LocoNet interface.

```
           ┌──────────────┐
 LocoNet ──┤              ├── OLED display (I²C)
 Main PWR ─┤              ├── Rotary encoder + button
 Track PWR ┤   ESP8266    ├── MENU button / E-STOP
 Index ────┤ (D1 Mini)    ├── Motor driver (stepper or DC)
           │              ├── Bridge power OUT (via relay)
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
| HW-03 | The power input shall only be used to power the D1, OLED, and motor control modules and motors. |
| HW-04 | The controller shall not provide power to the bridge, but switch power from supplied DCC or DC track power only |
| HW-05 | The default configuration will pass track power through, and rely on the TT to invert polarity. This can be changed in config to allow an onboard relay to handle power inverting to the bridge |
| HW-06 | Local selections and command will take preference over Loconet. ie loconet will only be applied if the bridge is not already busy with another movement |

### 3.2 User interface hardware

| ID | Requirement |
|---|---|
| HW-10 | The controller shall have a graphical OLED display connected over SPI. |
| HW-11 | The display shall be at least 2.4" diagonal; 2.44" SPI OLED (128×64) is the standard. **[RESOLVED]** |
| HW-12 | The controller shall have one EC11-type rotary encoder module with an integrated push button. |
| HW-13 | The controller shall have a dedicated MENU push button to switch between RUN mode and MENU mode. **[RESOLVED – OI-02]** |
| HW-14 | The controller shall have a dedicated emergency STOP push button. **[RESOLVED – OI-03]** |
| HW-15 | The eStop button shall stop bridge movement immediately |
| HW-16 | The controller shall have a 12-pixel addressable RGB (NeoPixel) status ring mounted under the rotary encoder knob. The ring's 35 mm inside diameter fits the 35 mm aluminium knob, leaving the pixels visible around the knob edge. Implementation deferred to a later phase. |

### 3.3 Electrical interfaces

| ID | Interface | Direction | Description |
|---|---|---|---|
| HW-20 | LocoNet | Bidirectional | LocoNet bus connection for DCC control. |
| HW-21 | Main power | IN | Supply for the controller and motor. |
| HW-22 | Track power | IN | DC or DCC track power feed. |
| HW-23 | Bridge power | OUT | Switched and polarity-controlled track power to the bridge. |
| HW-24 | Motor 1 | OUT | DC motor terminal 1. |
| HW-25 | Motor 2 | OUT | DC motor terminal 2. |
| HW-26 | Motor driver | OUT | TB6612FNG Dual H-Bridge: 4 GPIO (2 dir, 2 PWM). Drives DC or stepper motor up to 12V/1.2A. **[RESOLVED – OI-04]** |
| HW-27 | Home sensor | IN | Microswitch at home position (exit 0). Detects home for position recalibration. **[RESOLVED – OI-08/09]** |

### 3.4 Physical layout

| ID | Requirement |
|---|---|
| HW-30 | The enclosure is a Sanhe 18-33 console enclosure. The OLED display sits in the rectangular display window. |
| HW-31 | The rotary encoder with knob sits to the right of the display window, as the enclosure provides. |
| HW-32 | Six button positions sit in the bottom-right area of the console. The MENU and E-STOP buttons are placed here; the remaining four are spare and not assigned. The MENU button does not need to be adjacent to the display. **[RESOLVED – OI-02/03]** |

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
| RUN-01 | The OLED shall show the current bridge position graphically. |
| RUN-02 | The graphic shall match the configured TT type (turntable or traverser). |
| RUN-03 | The OLED shall show a **selection ring** (cursor) that marks the target exit in TT mode, or 2 brackets to highlight the target exit in Traverser mode. |
| RUN-04 | Turning the rotary encoder shall move the selection ring (or selection brackets) through the configured exits. |
| RUN-05 | Pressing the encoder button shall confirm the selected exit as the target. |
| RUN-06 | After a target is confirmed, the controller shall rotate or move the bridge to that exit automatically. |
| RUN-07 | The OLED must update the display to indicate the movement of the bridge from current position to target exit aligned with bridge movement. |


### 5.2 Emergency stop

| ID | Requirement |
|---|---|
| RUN-10 | The controller shall have an emergency STOP function that halts bridge motion immediately. |
| RUN-11 | An emergency STOP shall be triggered by the local STOP input or by a DCC/LocoNet STOP command. |
| RUN-12 | While a STOP is active, the OLED shall change to a distinct stop screen. |
| RUN-13 | The stop screen shall show whether the stop came from DCC or from the local input. |
| RUN-14 | Local eStop shall push a STOP command to LocoNet. |

### 5.3 Position tracking

| ID | Requirement |
|---|---|
| RUN-20 | The controller shall keep the current bridge position in non-volatile memory (EEPROM), so it knows where it is at startup without seeking home every time. |
| RUN-21 | During normal operation, the controller shall keep track of the home position so it cannot lose its position. For example, it shall re-verify home whenever the bridge passes it. |

---

### 5.4 Status ring

The status ring (HW-16) shows the controller state from across the room. Detailed information stays on the OLED.

| ID | Requirement |
|---|---|
| RUN-30 | While the bridge position is unknown (before the first home pass, OI-09), the status ring shall pulse red. |
| RUN-31 | While a move is in progress, the status ring shall show amber. |
| RUN-32 | When the bridge is locked at a known position and idle, the status ring shall show green. |
| RUN-33 | While an emergency STOP is active, the status ring shall show solid red. |

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
| MNU-20 | Configuration through a web page may be provided in addition to the local menu. **[OPEN – OI-06]** |

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
| CFG-05 | **Enable Internal Relay?** | Yes / No | **Yes:** the controller inverts bridge power at the crossover point using its internal relay. **No:** an external reverse-loop module handles track power changeover. |
| CFG-06 | **Motor Type** | Stepper / Standard | **Stepper:** a standard stepper motor drives the bridge. **Standard:** a standard DC motor drives the bridge. |
| CFG-07 | **Configure Steps** | Integer | The number of steps per 360° rotation. Applies to stepper motors only. |
| CFG-08 | **Index Hold Time** | Time | Applies to DC motors only. The time used to pass over intermediate index points so the bridge stops only at the selected one. Only used when the target is not the next index, for example moving from exit 2 to exit 4. |
| CFG-09 | **Home** | Procedure | Sets the home position, which is the reference for all other positions. |
| CFG-10 | *Other* | — | **[OPEN – OI-07]** Any additional configuration values. |

### 7.4 Config → Exit configuration
Sets the position, number and name of each exit.

| ID | Option | Description |
|---|---|---|
| EXT-01 | **Add** | Saves a new exit position. |
| EXT-02 | **Remove** | Removes an existing exit position. |
| EXT-03 | **Clear** | Removes **all** exit positions. ⚠️ This cannot be undone and shall require confirmation (MNU-07). |

### 7.5 Config → DCC Setup
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

## 8. LocoNet / DCC Requirements

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

---

## 10. Open Issues

Questions and design considerations from the source material, still to be resolved.

| ID | Topic | Status & Decision |
|---|---|---|
| OI-01 | **Display** | ✅ **RESOLVED (2026-10-07):** Display priority is SIZE. Use 2.44" SPI OLED (128×64) to meet the 2.4" minimum requirement. SPI interface frees the I²C bus for sensors. |
| OI-11 | **GPIO availability** | ✅ **RESOLVED (2026-10-07):** Upgrade from ESP8266 D1 Mini to ESP32 D1 Mini. Provides 30+ usable GPIO with no expander needed. Eliminates pin constraints, simplifies hardware, enables future expansion. |
| OI-04 | **Motor driver** | ✅ **RESOLVED (2026-10-07):** Use TB6612FNG Dual Channel H-Bridge modules. 4 GPIO pins per module (2 direction, 2 PWM). Can drive stepper or DC motor up to 12V/1.2A. Scope Phase 1: Fleischmann 6152 (48 positions, solenoid-locked, bidirectional DC motor). |
| OI-05 | **Position tracking** | ✅ **RESOLVED (2026-10-07):** Time-based tracking via calibrated motor timing. Learn milliseconds-per-position via menu option "Index Hold Time". Dead-reckoning between home passes. Solenoid auto-locks at each index. First move after power-up recalibrates position via home sensor. |
| OI-08 | **Home detection** | ✅ **RESOLVED (2026-10-07):** Microswitch at home position (exit 0). Detects when bridge passes home. Used to recalibrate position on first move after power-up. GPIO35 assigned for home sensor input. |
| OI-09 | **Startup behavior** | ✅ **RESOLVED (2026-10-07):** Display bridge icon flashing RED from startup until first home pass. Once home sensor detects, icon turns GREEN and position is set to HOME. Then continue to target exit. Subsequent moves use dead-reckoning (no red flash). Most bridges remain where left, so this is pragmatic and fast. |
| OI-02 | **MENU input** | ✅ **RESOLVED (2026-10-11):** Dedicated MENU push button. The encoder stays for navigation, with its click providing haptic feedback. Button placement does not need to be next to the display. |
| OI-03 | **Emergency STOP input** | ✅ **RESOLVED (2026-10-11):** Dedicated E-STOP push button, placed among the six console buttons. |
| OI-06 | **Web configuration** | Should configuration also be possible from a web page, in addition to the local menu? |
| OI-07 | **Additional config values** | Are any other configuration values needed? Candidates: motor speed, acceleration/deceleration, relay crossover angle, display brightness. |
| OI-10 | **Position persistence** | Position saved in EEPROM when motion completes (DAT-02/04). On startup, EEPROM position used as assumption; first move recalibrates via home sensor. No explicit confirmation needed beyond home sensor pass. |
