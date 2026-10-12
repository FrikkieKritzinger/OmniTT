# OmniTT - Development Guide

**Project**: OmniTT - universal model railroad turntable / traverser controller
**Purpose**: Standalone local control (OLED + rotary encoder) of a turntable or
traverser, with optional remote control as a LocoNet accessory decoder
**Language**: C++ (Arduino framework, PlatformIO)
**Target**: ESP32 in a Wemos D1 Mini form factor (WROOM-32)

> ℹ️ **How this file is maintained**: `CLAUDE.md` carries only what's pertinent to
> the current and next session - architecture, conventions, config reference and
> current status. Dated session narratives go to [docs/CHANGELOG.md](docs/CHANGELOG.md).

> ⚠️ OmniTT is a **separate project** from xpressnet-ecos-bridge
> (`E:\Claude\Bridge\files`). Don't reference that project's docs or code
> directly. Reusable knowledge from it is in the shared notes below.

> 🔗 **Shared cross-project notes** - record reusable LocoNet/ESP8266 learnings
> there, not only here:
> @E:/Claude/shared/esp8266-platform.md
> @E:/Claude/shared/loconet.md

---

## Requirements

The authoritative spec is [docs/Turntable_Controller_SRD.md](docs/Turntable_Controller_SRD.md)
(System Requirements Specification). Requirements have stable IDs (`HW-xx`,
`RUN-xx`, `MNU-xx`, `CFG-xx`, `DCC-xx`, `LN-xx`, `DAT-xx`, ...) - reference them
in commits, code comments for non-obvious behaviour, and tests. Open design
questions are tracked there as `OI-xx`. Resolve them in the SRD, not here.

Key behaviours to keep in mind:
- Two modes: **RUN** (select exit with encoder, confirm, bridge moves) and
  **MENU** (configuration). MENU can't be entered mid-move; movement commands
  are ignored while in MENU (MD-04/05).
- **E-stop** halts motion immediately, from a local button or a LocoNet STOP. A
  local e-stop is also sent onto LocoNet (RUN-10..14).
- **Local control takes priority over LocoNet** - a LocoNet command is only
  acted on when the bridge isn't already moving (HW-06).
- **Position persistence**: bridge position kept in EEPROM, written when motion
  completes to limit flash wear; home tracked during normal running (RUN-20/21,
  DAT-02/04). Factory defaults live in a reserved region (MNU-10/11).
- **LocoNet exit addressing**: base address + exit number (DCC-02).
- The controller switches supplied track power to the bridge; it never powers
  the bridge itself (HW-03..05).

---

## Development Status

- **Phase 0 (Requirements)**: ✅ Complete - SRD v0.21 (2026-10-11). All open issues
  OI-01..OI-14 are resolved. The relay is deferred to a later phase (HW-05, CFG-05).
- **Pin map**: finalised, 21 pins. See docs/GPIO_REFERENCE.md and src/config.h.
- **Platform**: ESP32 (D1 Mini form factor). The current `platformio.ini` still
  targets ESP8266 and needs migrating in Phase 1.
- ✅ **Build system set up** (2026-10-07): PlatformIO, skeleton main.cpp, test framework (Unity), config templates.
  Firmware compiles cleanly; native tests pass. RAM 34.4%, Flash 25.4%.

**Implementation plan** (agreed 2026-10-11; detail in docs/CHANGELOG.md):
1. Core bench build: ESP32 build migration, config and EEPROM, motor and solenoid
   drivers, home sensor, position tracker, move controller, native tests, minimal OLED.
2. Local UI: encoder, buttons, RUN and MENU screens, stop screen, calibration, status ring.
3. Wi-Fi: setup mode, AP on demand, web configuration, firmware update, Info screen.
4. LocoNet: accessory addressing, STOP handling, local priority (HW-06).
5. Field hardening: 6152 field test, watchdog, EEPROM verification.
6. Later: other turntable types, traversers, stepper motors, unindexed and Hall-effect indexing, relay builds.

**Open items** (SRD section 10):
- OI-15: index hold time and index spacing (CFG-08 wording to follow).
- OI-16: hardware design review (board silkscreen vs pin map, level shifter for the status ring, pull-up and pull-down resistors, power, enclosure wiring).

**To-do (hardware, before wiring)**:
- TB6612FNG datasheet (checked 2026-10-12): every control input (AIN1, AIN2, BIN1, BIN2, PWMA, PWMB, STBY) has an internal 200 kΩ pull-down. STBY is active-low. Still to confirm on the DFRobot board with a multimeter (board unpowered, about 200 kΩ to GND per input). If confirmed, the pull-downs on TCK, TMS, 32 and 4 are not needed.
- Confirm the board holds STBY high, since no STBY pin is exposed. Otherwise the chip sits in standby.
- VCC (logic) must be 3.3 V: operating range 2.7 to 5.5 V, absolute maximum 6 V. Never connect 5 V or 12 V to VCC. Check the DFRobot board's VCC wiring.
- Bypass capacitors per the datasheet application circuit: 0.1 µF plus 10 µF on VCC, and 0.1 µF plus 10 µF on VM, placed close to the pins. Check whether the DFRobot board already has them.
- Fit a flyback diode (1N4007 or UF4007) across the lock solenoid, cathode to +12 V, anode to the driver side. One diode is enough because the solenoid is only driven in one direction.
- Optional: 0.1 µF across the motor terminals to reduce brush noise. Common practice, not from the datasheet.
- Input filter on the N7812-1PV (Meanwell switching module, 8 to 20 V in, 12 V at 1 A out): 100 µF electrolytic (35 V or 50 V) plus 0.1 µF ceramic, close to the input pins. Check whether the module needs its own output capacitors.
- Input filter on the Wemos D1 power shield (5 V and 3.3 V rails): confirm whether it is switching or linear, then fit 100 µF plus 0.1 µF at its input.
- Check the 1 A output headroom: motor and solenoid together should stay well under 1 A, including at stall.
- Decide the input reassignment that removes the button and home-sensor pull-up resistors: home sensor on 26 (with OLED RES tied to 3.3 V), PRESET A on TDI (12), PRESET B on 2, LAST EXIT on 0. Trade-offs: labels 0 and 2 are strapping pins, and the relay would lose pin 2.
- Confirm on the OLED module that RES can be tied high.
- Level shifter for the status ring data (label 5).

**Next**: resolve OI-15 and OI-16 for hardware, then start Phase 1 (core bench build).

## Architecture

Pure logic is kept separate from hardware so it can be tested on the host.

| Module | Responsibility | Host-testable |
|---|---|---|
| `position_tracker` | Known/unknown state, first-move homing, dead reckoning, home confirmation, presets, last exit | Yes |
| `move_controller` | Move state machine: idle, homing, moving, stopped, timeout. Non-blocking `update()` | Yes |
| `eeprom_config` / `eeprom_store` | Config struct with magic, version and checksum; validation; EEPROM I/O split out | Logic only |
| `motor_driver` | TB6612FNG direction, PWM, ramp | No |
| `solenoid_driver` | Lock release during moves (pin pending OI-12) | No |
| `home_sensor` | Microswitch input, debounce | No |
| `input` | Encoder decoding, buttons, press and hold detection | Logic only |
| `display` | SPI OLED: bridge graphic, selection ring, stop screen, red and green states | No |
| `ui_menu` | RUN and MENU modes, menu tree, SAVE and CANCEL | Logic only |
| `status_ring` | NeoPixel states (RUN-30..33) | No |
| `wifi_manager` / `web_server` | AP on demand, setup mode, web configuration, firmware update | No |
| `loconet_interface` | Accessory addressing, STOP, priority rules | Partly |

---

## Project Guidelines

These guidelines are established principles reused from successful projects
(particularly xpressnet-ecos-bridge). They ensure security, maintainability,
and legal compliance throughout development.

### Rule 1: Config Rules All
Use compile-time configuration (`config.h`) to enable, disable, and tune features
and capabilities. This allows:
- Feature flags (e.g., `LOCONET_STOP_BROADCAST_ENABLED`)
- Hardware pin assignments (flexible across board variants)
- Timing and tuning parameters without code changes
- Easy build variants without code duplication

Examples:
```cpp
#define PIN_MOTOR_A D6
#define MOTOR_PWM_MAX 1023
#define LOCONET_ENABLED 1
#define WIFI_SETUP_MODE_HOLD_TIME_MS 3000
```

Use compile-time constants, not magic numbers or preprocessor string hacks.

### Rule 2: No Secrets in Code (Hard Rule)
**Never commit credentials, keys, or sensitive values in code or git history.**
- Real Wi-Fi credentials → `wifi_credentials.local.h` (gitignored)
- Committed version → `wifi_credentials.local.h.example` (template only)
- Real API keys / tokens → `.local.h` file with a committed `.example`
- Check file contents before committing anything that *could* be a secret
  (even innocuous-looking filenames)

If a secret is ever found committed, treat it as **compromised**: immediately
revoke it (change Wi-Fi password, rotate API key) and check git history
(`git log --all -p -- <file>`) to see how long it was exposed.

### Rule 3: External Libraries – Credit, Don't Copy
When using external libraries (e.g., LocoNet, Adafruit SSD1306):
- Declare them in `platformio.ini` (PlatformIO installs from package manager)
- **Do not add `.zip` or source files to the repo** (bloats git history, licensing confusion)
- Credit the publisher in code comments or a `CREDITS.md` file
- Keep library usage isolated (wrapper modules if needed) for easy swaps

Example in `platformio.ini`:
```ini
lib_deps =
    mrrwa/LocoNet@1.1.9
    adafruit/Adafruit SSD1306@2.5.11
```

Rationale: PlatformIO handles versioning and updates cleanly. A 1MB library zip
in git becomes a 10MB history blob; contributors and CI pipelines suffer.

### Rule 4: Honor Document Licenses
Before including any vendor spec, manual, reference, or third-party document
(PDF, datasheet, protocol spec):
1. **Check the license** (ESU, Roco, Digitrax, Digitax, etc. often forbid redistribution)
2. **Confirm it's permissible to commit** to a public/shared repo
3. **If restricted**: gitignore it locally, store it outside the repo, and summarize/reference it in docs instead
4. **If permitted**: commit it with a `LICENSE` header noting the source and terms

Examples:
- ✅ **Can commit**: GPL/MIT/Creative Commons docs, self-authored specs
- ❌ **Cannot commit**: ESU, Roco, Digitrax datasheets (proprietary / commercial use restricted)
- 🟡 **Reference only**: Summarize findings in `docs/` and link to where the real doc lives

Current status: `docs/Turntable_Controller_SRD.md` is user-authored (no license restriction).
Any vendor PDFs must be confirmed before addition.

---

## Code Style & Conventions

(Starting conventions, same style as the user's other ESP8266 projects - adjust
here if OmniTT needs something different.)

- **Files** snake_case, **classes** PascalCase, **methods** camelCase,
  **constants** SCREAMING_SNAKE_CASE, **locals** snake_case.
- Comments only for non-obvious logic (hidden constraints, hardware quirks,
  protocol details). Single-line `//` comments.
- No dynamic allocation (`new`/`delete`); fixed-size arrays, static objects.
- Validate at system boundaries (user input, LocoNet packets, EEPROM); trust
  internal code. Return `bool` for success/failure, no exceptions.
- **Non-blocking everything**: no `delay()` after setup, all `update()` methods
  return quickly. Motor stepping and e-stop must stay responsive while the
  display and Wi-Fi run.
- `config.h` holds compile-time defaults; runtime values come from EEPROM.
- Secrets (Wi-Fi credentials) in a gitignored `*.local.h` with a committed
  `.example` template.

---

## Files & Organization

```
OmniTT/
├── platformio.ini                     Build configuration (d1_mini & native test)
├── CLAUDE.md                          This file
├── .gitignore                         (*.local.h, build artefacts, restricted PDFs)
├── src/
│   ├── main.cpp                       Firmware entry point (setup/loop skeleton)
│   ├── version.h                      Firmware version & build timestamp
│   ├── config.h                       Compile-time defaults (pins, timing, EEPROM layout)
│   └── wifi_credentials.local.h.example  Template for secrets (copy & fill locally)
├── test/
│   └── test_config.cpp                Example unit test (Unity framework)
├── lib/
│   └── (external packages, managed by PlatformIO)
└── docs/
    ├── BUILD.md                       Build commands & development guide
    ├── Turntable_Controller_SRD.md    System Requirements Specification (v0.21)
    ├── GPIO_REFERENCE.md              Pin map and strapping-pin rules
    └── CHANGELOG.md                   Dated session history
```

See [docs/BUILD.md](docs/BUILD.md) for build commands and project structure details.
