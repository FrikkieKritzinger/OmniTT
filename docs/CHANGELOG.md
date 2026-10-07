# OmniTT Changelog

Dated session history, newest first. `CLAUDE.md` holds current state only.

## 2026-10-07 - Hardware decisions & GPIO mapping (session 2, part 2)

- **Resolved OI-01 (Display)**: Chose SPI over I²C. Using 2.44" SPI OLED (128×64)
  from stock. Meets 2.4" minimum requirement, provides better visibility than I²C
  alternatives, and frees the I²C bus for sensors.
- **Resolved OI-11 (GPIO Budget)**: Upgraded from ESP8266 D1 Mini to ESP32 D1 Mini.
  Provides 30+ usable GPIO, eliminates need for I/O expander, gives headroom for
  future expansion. No pin constraints.
- **Hardware stack finalized**:
  - MCU: ESP32 D1 Mini (Wemos form factor, WROOM-32)
  - Display: 2.44" SPI OLED (128×64)
  - Motor: DC motor (stepper TBD in later issues)
  - Sensors: Index, home, encoder, 2 buttons
  - LocoNet: UART2 at 16,667 bps
- **GPIO map created**: 15 pins assigned (SPI display, encoder, buttons, motor,
  LocoNet, relay), 15+ pins still free. See `docs/GPIO_REFERENCE.md`.
- **SRD updated to v0.12**: HW-01 and HW-10/11 updated for ESP32 + SPI display.
  OI-01 and OI-11 marked resolved.

## 2026-10-07 - Build system & skeleton (session 2, part 1)

- **PlatformIO configuration**: `platformio.ini` with `d1_mini` (firmware) and
  `native` (host tests) environments. Verified both build cleanly.
- **Project structure**: `src/`, `test/`, `lib/` directories. Skeleton `main.cpp`
  with TODO stubs for subsystems.
- **Header files**: `version.h` (semver + build timestamp), `config.h` (compile-time
  defaults: pins, timing, EEPROM layout, feature flags). Hardware pins assigned
  per current SRD (pending OI-11 GPIO budget).
- **Templates**: `wifi_credentials.local.h.example` for secrets; copied to real
  file locally and kept out of git via `.gitignore`.
- **Test skeleton**: `test_config.cpp` (Unity framework, basic placeholder). Unit
  test environment compiles and runs on the host.
- **Development guide**: `docs/BUILD.md` with build commands, pin reference, memory
  status (RAM 34.4%, Flash 25.4%), flashing instructions.

**Memory baseline** (skeleton only): ~28 KB RAM / 81 KB (34.4%), ~266 KB Flash / 1044 KB (25.4%).
Headroom available for subsystems; OI-11 will determine if I/O expander or ESP32 upgrade needed.

## 2026-10-07 - Project created

- Turned the original turntable controller notes into a structured System
  Requirements Specification (`docs/Turntable_Controller_SRD.md`). The user then
  refined it to draft v0.11: added HW-03..06 (power handling, LocoNet vs local
  priority), HW-15, MD-04/05, RUN-07, RUN-14, separated Wi-Fi setup into its
  own menu, and named the project OmniTT.
- Set up the project: git repository, `CLAUDE.md`, this changelog, and links to
  the shared cross-project notes in `E:\Claude\shared\` (ESP8266 platform,
  LocoNet).
