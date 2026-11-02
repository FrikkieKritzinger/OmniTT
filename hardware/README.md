# Hardware (KiCad 10)

KiCad project for the OmniTT controller: main board (`OmniTT Turntable Controller.kicad_pcb`),
the root schematic, and the two sub-sheets:

- `tt_control.kicad_sch`: button board (J5, ladder, shortcut buttons)
- `loconet_interface.kicad_sch`: LocoNet interface (J7, Q3, U5, R9 to R14), unpopulated in this revision

Open `OmniTT Turntable Controller.kicad_pro` in KiCad 10.

## External libraries

The design uses two custom libraries. They are deliberately not in the repo: they are
modified third-party work, and the repo keeps only the design files. Install them in
KiCad before opening the project, or the footprints show as missing.

| Library nickname | Used for | Origin |
|---|---|---|
| `DFRobot` (symbol) and `library` (footprint) | DFRobot TB6612FNG motor driver module (U3) | Based on the d1_mini_kicad library (CC BY-SA 3.0), modified by the author |
| `TTGO_T7_MINI32_V1.3` (symbol and footprint) | ESP32 D1 Mini (WROOM-32) module (U4) | Downloaded footprint (SnapEDA basis), modified by the author |

Standard KiCad libraries (Resistor_THT, Capacitor_THT, Connector_PinHeader, Package_TO_SOT, and so on)
ship with KiCad and need no separate install.

## Notes

- Schematic symbols are embedded in each `.kicad_sch`, so the schematic opens without the
  symbol libraries. The PCB footprints still need them.
- Local KiCad state (`*.kicad_prl`), lock files and backups are excluded in `.gitignore`.
