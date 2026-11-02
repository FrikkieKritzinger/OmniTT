# Hardware (KiCad 10)

KiCad project for the OmniTT controller: main board (`OmniTT Turntable Controller.kicad_pcb`),
the root schematic, and the two sub-sheets:

- `tt_control.kicad_sch`: button board (J5, ladder, shortcut buttons)
- `loconet_interface.kicad_sch`: LocoNet interface (J7, Q3, U5, R9 to R14), unpopulated in this revision

Open `OmniTT Turntable Controller.kicad_pro` in KiCad 10.

## External libraries

The design uses two custom libraries.

| Library nickname | Used for | Where it lives | Origin |
|---|---|---|---|
| `DFRobot` (symbol) and `library` (footprint) | DFRobot TB6612FNG motor driver module (U3) | In the repo: `libraries/DFRobot.kicad_sym` and `libraries/library.pretty/` | Designed by the author |
| `TTGO_T7_MINI32_V1.3` (symbol and footprint) | ESP32 D1 Mini (WROOM-32) module (U4) | Not in the repo | Downloaded footprint (SnapEDA basis), modified by the author |

`sym-lib-table` and `fp-lib-table` point KiCad at the in-repo libraries using `${KIPRJMOD}`,
so the project resolves them without any global setup.

The TTGO library isn't in the repo. Install it in KiCad from your own copy before opening
the project, or its footprint shows as missing. Its SnapEDA origin needs checking before
it's added to the repo.

Standard KiCad libraries (Resistor_THT, Capacitor_THT, Connector_PinHeader, Package_TO_SOT, and so on)
ship with KiCad and need no separate install.

## Notes

- Schematic symbols are embedded in each `.kicad_sch`, so the schematic opens without the
  symbol libraries. The PCB footprints still need them.
- Local KiCad state (`*.kicad_prl`), lock files and backups are excluded in `.gitignore`.
