# OmniTT Changelog

Dated session history, newest first. `CLAUDE.md` holds current state only.

## 2026-10-14 - Pin map finalised with resistor ladder (session 5)

- **Button board**: MENU on TCK (13) and E-STOP on 35, both direct. E-STOP is wired
  normally closed. Four shortcut buttons (PRESET A, PRESET B, LAST EXIT, SPEED) share the
  ladder on 34, with levels 1410, 1980, 2480 and 2880 mV (7.5, 15, 30 and 68 kΩ, 10 kΩ
  pull-up). Readings within 150 mV of a level are accepted. Below 1000 mV is a fault.
- **Motor and solenoid**: DIR1 moves to 25, DIR2 to 33 (set by a 3-way jumper). PWM1 (32)
  and PWM2 (4) are unchanged.
- **Home sensor** moves to TDO (15) with the internal pull-up. It's the only strapping pin
  in use.
- **Status ring** moves to TMS (14), which isn't a strapping pin. That frees 5.
- **Pins now used**: 18. Free: 0, 2, 5, 12 (all strapping), and 36, 39 (input-only).
- **SRD v0.26**: HW-14 (normally closed E-STOP), new HW-17 (ladder), OI-08, OI-12 and OI-14
  updated.
- **Files**: `src/config.h` (pin map and ladder levels), `docs/GPIO_REFERENCE.md`,
  `docs/BUILD.md`, and `CLAUDE.md` to-do list.

## 2026-10-12 - First-move rule and move direction (session 4, part 4)

- **Direction**: every move, including the first after power-up, uses the fastest path
  from the assumed position. A configurable direction preference is deferred to a later
  phase.
- **First move** (RUN-23): step counting and release timing start at the home pass. If
  the target is passed before home, the bridge keeps moving without releasing the
  solenoid until home is passed.
- **Move timeout** (CFG-14): must cover a full revolution plus the home-to-target distance.
- **SRD v0.25**: RUN-23 updated.

## 2026-10-12 - Index hold and release rule resolved (session 4, part 3)

- **Only fitted index pieces are stops.** The 48 positions are 7.5° apart, and exits are
  stored as step numbers 0 to 47 (EXT-01).
- **Lock is passive.** Releasing the solenoid lets the pin rub until it drops into the
  next index, which cuts motor power.
- **Release rule** (RUN-25): release mid-gap, after the previous fitted index and before
  the target. Timing tolerance is half the gap.
- **Motor stop** (RUN-26): keep running after release, then stop PWM at the expected
  arrival time plus a margin, with CFG-14 as the backstop.
- **CFG-08 redefined**: calibrated time per 7.5° step, separately for NORMAL and RAPID.
  Learned once by a full-revolution calibration (RUN-27), not at power-up.
- **SRD v0.24**: EXT-01, CFG-08, RUN-25 to RUN-27 and OI-15 updated.

## 2026-10-12 - Board pins verified and referenced by silkscreen label (session 4, part 2)

- **Board verified on the physical D1 ESP32**: every planned output and input was
  tested with the interactive probe (touch test with GND and 3.3 V on the inputs).
  Some pins appear under JTAG and sensor names: TCK (GPIO13), TMS (GPIO14), TDO
  (GPIO15), SVP (GPIO36) and SVN (GPIO39).
- **Documentation now uses the silkscreen label first**, with the GPIO number in
  brackets where useful: `docs/GPIO_REFERENCE.md`, `src/config.h` comments, SRD
  OI-08, OI-12 and OI-14, and `docs/BUILD.md`. The BUILD.md pin list was from the
  old ESP8266 plan and has been replaced.
- **Pull-up and pull-down guidance** is in GPIO_REFERENCE.md: internal pull-ups for
  switches on 19, 21, 22, 25, 33 and TDO; external 10 kΩ pull-ups for labels 34, 35,
  SVP and SVN; external 10 kΩ pull-downs on TCK, TMS, 32 and 4.
- **SRD v0.23**.

## 2026-10-12 - Consistency fixes and open items (session 4)

- **SRD section 2** updated to ESP32, SPI OLED and home microswitch; the diagram now
  shows the TB6612FNG and track pass-through.
- **CFG-05** marked deferred (relay build).
- **HW-24/25** now cover the motor and the lock solenoid; the duplicate Motor 1 and
  Motor 2 entries are removed. **HW-26** tidied.
- **RUN-14**: local E-stop sends STOP to LocoNet only when LocoNet is enabled.
- **config.h**: `DEFAULT_NUM_EXITS` is 0 (set per turntable), and the EEPROM storage
  backend is marked for decision in Phase 1.
- **New open items**: OI-15 (index hold and index spacing, which needs discussion) and
  OI-16 (hardware design review: silkscreen and exposed pins, level shifter for the
  status ring, resistors, power, enclosure wiring).
- **CFG-08 wording** is left for the OI-15 discussion.
- **SRD v0.22**.

## 2026-10-11 - Pin map finalised, relay deferred (session 3, part 9)

- **Relay deferred** to a later phase (HW-05, CFG-05). It is needed for relay builds,
  which invert track power when the TT has no slip rings, or with an external reverse
  loop on DCC layouts. The initial build has no relay pin.
- **OLED CS hard-wired low**: the OLED is the only SPI device, which frees GPIO5.
- **Pin map** (21 pins, `src/config.h`): motor GPIO13/32, solenoid GPIO14/4, OLED
  GPIO18/23/27/26, encoder GPIO19/21/22, MENU GPIO25, E-STOP GPIO33, shortcut buttons
  GPIO34/36/39/15, LocoNet GPIO16/17, home GPIO35, status ring GPIO5.
- **Strapping pins**: GPIO5 (status ring) and GPIO15 (SPEED) only, where boot state is
  harmless. GPIO12 removed. GPIO0 and GPIO2 unused.
- **Input-only pins** (GPIO34/35/36/39) need external pull-ups for their buttons and the
  home sensor.
- **OI-14 resolved**, no I²C expander used. **SRD v0.21**.
- `config.h` no longer defines `I2C_ADDRESS_DISPLAY`, which contradicted the SPI decision.

## 2026-10-11 - Lock solenoid and motor driver resolved (session 3, part 8)

- **Resolved OI-12**: the solenoid runs from TB6612FNG channel B and the motor from
  channel A. Both are 12 V and draw a few hundred mA, well within the 1.2 A rating.
  The 6152, Roco and Märklin use the same four-wire arrangement (GND, MOVE, MOTOR A,
  MOTOR B).
- **Resolved OI-13**: the DFRobot TB6612FNG board has no standby line, so no GPIO is
  needed.
- **Motor and solenoid pins**: DIR1 GPIO13, PWM1 GPIO32 (motor); DIR2 GPIO14, PWM2
  GPIO4 (solenoid). GPIO4 and GPIO14 are not strapping pins.
- **Boot safety**: PWM1 and PWM2 need external pull-downs so the outputs stay off
  during reset.
- **New OI-14**: pin budget and strapping pins. About 23 pins are needed and about 19
  clean pins are available. GPIO12 must move. Decision needed before the Phase 1
  pin map.
- **Pending correction**: `docs/GPIO_REFERENCE.md` wrongly lists GPIO4 as strapping,
  and it still shows GPIO12 for motor direction A. To be fixed with the Phase 1 pin map.
- **SRD v0.20**: HW-26, HW-28, OI-12, OI-13, OI-14 updated.

## 2026-10-11 - Architecture and phase plan (session 3, part 7)

- **Architecture** agreed: pure logic (position tracker, move controller, config,
  input, menu logic) kept separate from hardware drivers, so the core can be tested
  on the host.
- **Phase plan** agreed: 1 core bench build, 2 local UI and status ring, 3 Wi-Fi,
  4 LocoNet, 5 field hardening, 6 later turntable types.
- **New open issues before Phase 1**: OI-12 (lock solenoid output; GPIO2 is a
  strapping pin and is also the relay pin) and OI-13 (TB6612FNG standby pin).
- **Platform**: ESP32 is the target. The existing `platformio.ini` still builds for
  ESP8266, and LocoNet moves to the ESP32 library (LocoNet2).
- **SRD v0.19**: HW-26 and HW-28 updated; OI-12 and OI-13 added.
- **CLAUDE.md**: target, status, architecture and phase plan updated.

## 2026-10-11 - Position after power-up resolved (session 3, part 6)

- **Resolved OI-10**: an interrupted move is treated the same as an orderly restart.
  The bridge position is uncertain either way, so no move-in-progress flag is stored.
- **First move after power-up** (RUN-22/23): the bridge keeps moving in the
  operator-selected direction, past the target if needed, until the home sensor is
  passed. It then sets position to HOME and stops at the selected exit. The move
  timeout (CFG-14) must allow for a full revolution.
- **After that** (RUN-24): dead reckoning, re-confirmed at every home pass.
- **SRD v0.18**: section 5.3 and section 10 updated.

## 2026-10-11 - Extra config values and SPEED button (session 3, part 5)

- **Resolved OI-07**: added CFG-12..17. Move speed is split into normal (CFG-12) and
  rapid (CFG-13) so the SPEED toggle has a value to apply. Also added move timeout
  (CFG-14, safety limit on dead-reckoning moves), ramp time (CFG-15), status ring
  brightness (CFG-16) and OLED brightness (CFG-17). Relay crossover angle deferred.
- **SPEED button** (RUN-44): the fourth shortcut button toggles NORMAL and RAPID.
  RAPID is a hard lockout until the position is determined, and no setting overrides it.
  The mode persists (DAT-06).
- **All six console buttons are now assigned**: MENU, E-STOP, PRESET A, PRESET B,
  LAST EXIT and SPEED.
- **SRD v0.17**: sections 3.4, 5.5, 7.3, 9 and 10 updated.

## 2026-10-11 - Web configuration resolved (session 3, part 4)

- **Resolved OI-06**: web configuration mirrors the local menu (MNU-20, MNU-21).
- **Secrets write-only** (MNU-22): the Wi-Fi password is never shown on the page
  once saved, following the no-secrets rule.
- **On-demand AP and web service** (MNU-23): started from Setup Wi-Fi or Firmware
  Update, stopped by an idle timeout or on leaving the menu. Blank config enters
  setup mode automatically.
- **Calibration from web** (MNU-24): allowed only when idle, not in MENU, and not in
  E-stop. A STOP action is offered during the move. No other movement control on
  the web page.
- **Deferred**: admin PIN for the web configuration, and home-network access
  (station mode).
- **SRD v0.16**: section 6.3 and section 10 updated.

## 2026-10-11 - Shortcut buttons added (session 3, part 3)

- **Shortcut buttons** (section 5.5, RUN-40..43): PRESET A and PRESET B recall stored
  exits. Holding a preset for CFG-11 stores the current exit. LAST EXIT returns to
  the previous exit. One spare button remains.
- **Scope**: accessory and layout control (doors, lights) stated as out of scope;
  these belong to separate decoders.
- **EEPROM**: presets and last exit stored (DAT-05).
- **SRD v0.15**: section 1.2, section 5.5, CFG-11 and DAT-05 added.

## 2026-10-11 - Status ring added (session 3, part 2)

- **Status ring**: 12-pixel NeoPixel ring under the encoder knob (HW-16). Ring
  inside diameter 35 mm, outside 50 mm. Paired with the 35 mm aluminium knob, so
  every pixel stays visible around the knob edge with about 1 mm clearance. The
  42 mm knob would hide up to half the pixels, so it is not used with this ring.
- **States** (RUN-30..33): red pulse while position is unknown, amber while moving,
  green when locked and idle, solid red during E-stop. Detailed information stays
  on the OLED.
- **Scope**: a planned feature, not optional. Implementation deferred to a later
  phase.
- **SRD v0.14**: HW-16 and section 5.4 added.

## 2026-10-11 - Input layout and enclosure (session 3)

- **Enclosure chosen**: Sanhe 18-33 console enclosure. It has a rectangular display
  window, an encoder with knob to the right, and six button positions in the
  bottom-right.
- **Resolved OI-02 (MENU input)**: dedicated MENU push button. The encoder stays
  for navigation, and its click gives haptic feedback. Mode switching uses the
  button, not encoder gestures.
- **Resolved OI-03 (E-STOP input)**: dedicated E-STOP push button among the six
  console buttons. Four buttons remain spare.
- **Physical layout** (HW-30..32) reworked for the enclosure. The MENU button no
  longer needs to sit next to the display.
- **SRD v0.13**: HW-13, HW-14, HW-30..32 and OI-02/03 updated.

## 2026-10-07 - Motor control & position tracking finalized (session 2, part 3)

- **Resolved OI-04 (Motor driver)**: TB6612FNG Dual H-Bridge modules. 4 GPIO pins
  per module (2 direction, 2 PWM). Supports DC or stepper motors, up to 12V/1.2A.
  Phase 1 scope: Fleischmann 6152 (48 positions, solenoid-locked, bidirectional).
- **Resolved OI-05 (Position tracking)**: Time-based dead-reckoning via calibrated
  motor timing (milliseconds per position). Configurable via menu "Index Hold Time".
  Solenoid auto-locks at each index. First move recalibrates position.
- **Resolved OI-08/09 (Home detection & startup)**: Single microswitch at home
  (exit 0, GPIO35). Startup shows flashing RED bridge icon until first home pass.
  Once home is detected, position is set to HOME, icon turns GREEN, then continue
  to target. Pragmatic: most bridges remain where they're left. Subsequent moves
  use dead-reckoning (no red flash).
- **Scope clarification**: Phase 1 targets Fleischmann 6152 specifically (48 indexed
  positions, mechanical solenoid-lock). Generic turntable support (PECO, Walthers,
  unindexed, Hall-effect indexing) deferred to Phase 2+.
- **SRD updated to v0.12**: OI-04, OI-05, OI-08, OI-09 marked RESOLVED. Open issues
  remaining: OI-02 (MENU input), OI-03 (E-STOP), OI-06 (web config), OI-07 (extra
  config), OI-10 (position persistence, now understood).

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
