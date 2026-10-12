# Pin Reference

**Target**: ESP32 D1 Mini (Wemos form factor, WROOM-32), supplied board
**Source of truth**: `src/config.h` (the `PIN_*` constants). This page is a summary.
**Naming**: use the board's silkscreen label (for example SVN, TDO). The GPIO number is given in brackets for reference.
**Verified**: every output and input listed below was tested on the physical board (2026-10-12).
**Pin map finalised**: 2026-10-11 (SRD v0.21, OI-14).

---

## Initial build: 21 pins

| Function | Board label | Interface | Notes |
|---|---|---|---|
| OLED clock | 18 | SPI | 2.44" SPI OLED, 128x64 |
| OLED data (MOSI) | 23 | SPI | |
| OLED data/command | 27 | GPIO | |
| OLED reset | 26 | GPIO | |
| OLED chip select | — | — | Hard-wired low. The OLED is the only SPI device |
| Encoder A | 19 | GPIO | EC11. Internal pull-up |
| Encoder B | 21 | GPIO | EC11. Internal pull-up |
| Encoder button | 22 | GPIO | Internal pull-up |
| MENU button | 25 | GPIO | Internal pull-up |
| E-STOP button | 33 | GPIO | Internal pull-up |
| PRESET A button | 34 | GPIO, input-only | External 10 kΩ pull-up to 3.3 V required |
| PRESET B button | SVP [36] | GPIO, input-only | External 10 kΩ pull-up to 3.3 V required |
| LAST EXIT button | SVN [39] | GPIO, input-only | External 10 kΩ pull-up to 3.3 V required |
| SPEED button | TDO [15] | GPIO, strapping | Internal pull-up. Holding it at power-up only silences the boot log |
| LocoNet RX | 16 | UART2 RX | |
| LocoNet TX | 17 | UART2 TX | |
| Motor direction (DIR1) | TCK [13] | GPIO | TB6612FNG channel A. External 10 kΩ pull-down |
| Motor PWM (PWM1) | 32 | PWM | TB6612FNG channel A. External 10 kΩ pull-down |
| Solenoid direction (DIR2) | TMS [14] | GPIO | TB6612FNG channel B. External 10 kΩ pull-down |
| Solenoid PWM (PWM2) | 4 | PWM | TB6612FNG channel B. External 10 kΩ pull-down |
| Home sensor | 35 | GPIO, input-only | Microswitch. External 10 kΩ pull-up to 3.3 V required |
| Status ring data | 5 | GPIO, strapping | WS2812 data, needs a 5 V level shifter. A wrong level at boot only flickers the LEDs |

---

## Board pins not used by the initial build

| Board label | GPIO | Status |
|---|---|---|
| 0 | 0 | Strapping (pulled up). Low at reset enters download mode. Leave unused |
| 2 | 2 | Strapping. Unused. Reserved for the optional relay in a later phase |
| TDI | 12 | Strapping. Must stay low at boot (flash voltage). Keep unused |
| TX, RX | 1, 3 | UART0, serial debugging. Keep |
| CLK, CMD, SD2, SD3, SDI, SDO | 6 to 11 | Internal flash. Do not use |
| SVP, SVN | 36, 39 | Used (see above) |

---

## Strapping pins in use

| Board label | GPIO | Rule |
|---|---|---|
| 5 | 5 | Status ring data. Harmless at boot |
| TDO | 15 | SPEED button. Input only, pulled up at boot |

---

## Reserved for later phases

- **Relay** (optional, HW-05/CFG-05): not in the initial build. Needs a pin when a relay build is added. Label 2 is the candidate, with a transistor driver and pull-down so the relay stays off at boot.
- **Other turntable types** (phase 6): stepper driver and Hall-effect indexing will need more pins. Re-check the budget at that point.

---

## JTAG

TCK, TMS, TDO and TDI are the ESP32's JTAG pins. Using them as GPIO disables JTAG debugging. Serial debugging is unaffected. JTAG is not planned.

---

## Input-only pins

Labels 34, 35, SVP and SVN have no internal pull-ups or pull-downs. They are used for PRESET A, home sensor, PRESET B and LAST EXIT. Each needs an external 10 kΩ pull-up to 3.3 V.

---

## Boot safety

- The motor and solenoid direction and PWM pins (TCK, TMS, 32, 4) need external 10 kΩ pull-downs so the TB6612FNG stays off during reset and boot.
- Strapping pins are used only where their boot state is harmless (labels 5 and TDO).
