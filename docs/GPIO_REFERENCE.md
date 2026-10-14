# Pin Reference

**Target**: ESP32 D1 Mini (Wemos form factor, WROOM-32), supplied board
**Source of truth**: `src/config.h` (the `PIN_*` constants). This page is a summary.
**Naming**: use the board's silkscreen label (for example TDO, TCK). The GPIO number is in brackets.
**Verified**: the pin labels were checked on the physical board with a touch test (2026-10-12).
**Pin map finalised**: 2026-10-14 (SRD v0.28), updated for the Waveshare display and backlight PWM.

---

## Initial build: 18 pins

| Function | Board label | Interface | Notes |
|---|---|---|---|
| Display clock | 18 | SPI | Waveshare 2.0" IPS, 240x320, ST7789V |
| Display data (DIN, MOSI) | 23 | SPI | |
| Display data/command (DC) | 27 | GPIO | |
| Display reset (RST) | — | — | Tied to the ESP32 RST line. No GPIO used |
| Display chip select (CS) | — | — | Tied to GND. The display is the only SPI device |
| Display backlight (BL) | 26 | GPIO, PWM | Through a logic-level MOSFET (backlight about 45 mA). Software brightness (CFG-17) |
| Encoder A | 19 | GPIO | EC11. Internal pull-up |
| Encoder B | 21 | GPIO | EC11. Internal pull-up |
| Encoder button | 22 | GPIO | Internal pull-up |
| MENU button | TCK [13] | GPIO | Button board, external pull-up |
| E-STOP | 35 | GPIO, input-only | Normally closed, button board, external pull-up. A broken wire reads as stop |
| Resistor ladder | 34 | GPIO, input-only, ADC1 | PRESET A, PRESET B, LAST EXIT and SPEED share this pin (see ladder below) |
| Home sensor | TDO [15] | GPIO, strapping | Microswitch, internal pull-up. Holding it at power-up only silences the boot log |
| LocoNet RX | 16 | UART2 RX | |
| LocoNet TX | 17 | UART2 TX | |
| Motor direction (DIR1) | 25 | GPIO | TB6612FNG channel A. External 10 kΩ pull-down |
| Motor PWM (PWM1) | 32 | PWM | TB6612FNG channel A. External 10 kΩ pull-down |
| Solenoid direction (DIR2) | 33 | GPIO | TB6612FNG channel B. Set by a 3-way jumper (pull-up, pull-down or GPIO). Pull-down by default |
| Solenoid PWM (PWM2) | 4 | PWM | TB6612FNG channel B. External 10 kΩ pull-down |
| Status ring data | TMS [14] | GPIO | WS2812 data, needs a 5 V level shifter |

---

## Resistor ladder (button board)

A 10 kΩ pull-up to 3.3 V sits on the ladder pin. Each button connects its own resistor to GND.

| Button | Resistor | Voltage (mV) |
|---|---|---|
| PRESET A | 7.5 kΩ | 1410 |
| PRESET B | 15 kΩ | 1980 |
| LAST EXIT | 30 kΩ | 2480 |
| SPEED | 68 kΩ | 2880 |
| No button | open | 3300 |

- Accept a reading only if it is within 150 mV of a level.
- Below 1000 mV is a fault (for example a short to GND). Ignore it, and never treat it as a button press.
- Use 1% resistors. Verify each level with a multimeter on the button board.
- Two buttons pressed together give one reading, which matches no level, so ignore it.

---

## Board pins not used by the initial build

| Board label | GPIO | Status |
|---|---|---|
| 0 | 0 | Strapping (pulled up). Low at reset enters download mode. Unused |
| 2 | 2 | Strapping. Unused. Reserved for the optional relay in a later phase |
| 5 | 5 | Strapping. Unused |
| TDI | 12 | Strapping. Must stay low at boot. Unused |
| SVP | 36 | Input-only. Unused, spare input |
| SVN | 39 | Input-only. Unused, spare input |
| TX, RX | 1, 3 | UART0, serial debugging. Keep |
| CLK, CMD, SD2, SD3, SDI, SDO | 6 to 11 | Internal flash. Do not use |

---

## Strapping pins in use

| Board label | GPIO | Rule |
|---|---|---|
| TDO | 15 | Home sensor. Input only, pulled up at boot |

---

## Reserved for later phases

- **Relay** (optional, HW-05/CFG-05): not in the initial build. Label 2 is the candidate, with a transistor driver and pull-down so the relay stays off at boot.
- **Stepper motor** (later phase): needs DIR2 back from the jumper, which is set to GPIO mode.
- **Other turntable types** (phase 6): re-check the pin budget at that point.

---

## JTAG

TCK, TMS, TDO and TDI are the ESP32's JTAG pins. Using them as GPIO disables JTAG debugging. Serial debugging is unaffected. JTAG is not planned.

---

## Pull-up and pull-down

- **Button board** (MENU, E-STOP, ladder): external resistors. The pull-up is on the button board, powered from 3.3 V.
- **Home sensor** (TDO): internal pull-up.
- **TB6612 control inputs** (DIR1, PWM1, DIR2, PWM2): the TB6612FNG datasheet lists an internal 200 kΩ pull-down on each control input. A 10 kΩ external pull-down is an option until the board is measured.
- **Input-only pins** (34, 35, 36, 39): no internal pull-up or pull-down. Each needs an external resistor.

---

## Boot safety

- The motor and solenoid direction and PWM pins keep the TB6612 off during reset when pulled down.
- Strapping pins in use: TDO only, where its boot state is harmless.
- The display's RST follows the ESP32 RST line, so the display resets with the board.
- The backlight MOSFET gate has a pull-down, so the backlight stays off until firmware starts. GPIO26 floats during reset.
