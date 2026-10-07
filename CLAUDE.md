# OmniTT - Development Guide

**Project**: OmniTT - universal model railroad turntable / traverser controller
**Purpose**: Standalone local control (OLED + rotary encoder) of a turntable or
traverser, with optional remote control as a LocoNet accessory decoder
**Language**: C++ (Arduino framework, PlatformIO)
**Target**: ESP8266 (Wemos D1 Mini) - see open issue OI-11 on GPIO budget

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

- **Phase 0 (Requirements)**: 🔧 In progress - SRD draft v0.11 (2026-10-07).
  Open issues OI-01..OI-11 still to resolve, most importantly OI-01 (display
  size vs I²C), OI-05/08/09 (index and home sensing) and OI-11 (GPIO budget -
  probably needs an I/O expander or an ESP32).
- No firmware code yet.

**Next**: resolve the open issues, then agree an architecture and phase plan.

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
├── CLAUDE.md                          This file
└── docs/
    ├── Turntable_Controller_SRD.md    System Requirements Specification
    └── CHANGELOG.md                   Dated session history
```
