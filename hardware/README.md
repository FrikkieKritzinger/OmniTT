# Hardware (KiCad 10)

KiCad project for the OmniTT controller: main board (`OmniTT Turntable Controller.kicad_pcb`),
the root schematic, and the two sub-sheets:

- `tt_control.kicad_sch`: button board (J5, ladder, shortcut buttons)
- `loconet_interface.kicad_sch`: LocoNet interface (J7, Q3, U5, R9 to R14), unpopulated in this revision

Open `OmniTT Turntable Controller.kicad_pro` in KiCad 10.

## External libraries

The design uses these third-party libraries. They are not vendored into the repo (see
Rule 3 in `CLAUDE.md`). Install them in KiCad before opening the project, or the
footprints will show as missing.

| Library nickname | Used for | Local source (author's machine) |
|---|---|---|
| `library` (footprint) and `DFRobot` (symbol) | DFRobot TB6612FNG motor driver module | `E:\Downloads\KiCAD\d1_mini_kicad-master\library` |
| `wemos_mini`, `wemos_CD_power_shield`, `wemos_DC_Power_Shield` | Wemos D1 Mini and power shields (symbols) | `E:\Downloads\KiCAD\d1_mini_kicad-master\library` |
| `TTGO_T7_MINI32_V1.3` | ESP32 D1 Mini (WROOM-32) module used as U4 | `E:\Downloads\KiCAD\TTGO_T7_MINI32_V1.3` |

Upstream URLs and licences for these libraries are not recorded yet. Add them here
before the repo is public, and check each licence allows reuse.

Standard KiCad libraries (Resistor_THT, Capacitor_THT, Connector_PinHeader, Package_TO_SOT, and so on)
ship with KiCad and need no separate install.

## Notes

- Schematic symbols are embedded in each `.kicad_sch`, so the schematic opens without the
  symbol libraries. The PCB footprints still need them.
- The `MAX485` module library is registered in the author's global table but isn't used
  by this board.
- Local KiCad state (`*.kicad_prl`), lock files and backups are excluded in `.gitignore`.
