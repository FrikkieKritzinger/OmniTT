# GPIO Pin Reference

**Target Platform**: ESP32 D1 Mini (Wemos form factor with WROOM-32)  
**Resolution**: OI-01 (Display) and OI-11 (GPIO Budget) — 2026-10-07

---

## Pin Assignments Summary

| Function | GPIO | D1 Label | Interface | Notes |
|---|---|---|---|---|
| **Display** | | | **SPI2** | 2.44" OLED (128×64) |
| OLED Clock | 18 | D18 | SPI CLK | Display clock |
| OLED Data (MOSI) | 23 | D23 | SPI MOSI | Display data line |
| OLED Chip Select | 5 | D5 | GPIO | Chip select (active low) |
| OLED Data/Command | 27 | D27 | GPIO | Command vs data mode |
| OLED Reset | 26 | D26 | GPIO | Reset line (active low) |
| **Rotary Encoder** | | | **GPIO** | EC11 with push button |
| Encoder A (CLK) | 19 | D19 | GPIO | Phase A output |
| Encoder B (DT) | 21 | D21 | GPIO | Phase B output |
| Encoder Button | 22 | D22 | GPIO | Push button (active low) |
| **User Buttons** | | | **GPIO** | Direct digital inputs |
| MENU Button | 25 | D25 | GPIO | Config mode trigger (active low) |
| E-STOP Button | 33 | D33 | GPIO | Emergency stop (active low) |
| **LocoNet** | | | **UART2** | Accessory decoder commands |
| RX (input) | 16 | RX2 | UART | Receive from LocoNet bus |
| TX (output) | 17 | TX2 | UART | Transmit to LocoNet bus |
| **Motor Control** | | | **GPIO + PWM** | DC motor (stepper TBD) |
| Direction A | 12 | D12 | GPIO | Motor direction line A |
| Direction B | 13 | D13 | GPIO | Motor direction line B |
| PWM Speed | 32 | D32 | GPIO+PWM | Motor speed (0–255 or 0–1023) |
| **Sensors** | | | **GPIO** | Position feedback |
| Home Sensor | 35 | D35 | GPIO | Microswitch at home (exit 0) — recalibration trigger (input only) |
| **Power & Relay** | | | **GPIO** | Bridge power control |
| Relay Control | 2 | D2 | GPIO | Bridge power/polarity invert |

---

## Usage Notes

### SPI Display (2.44" OLED)
- Uses SPI2 (HSPI) peripheral
- Configuration: see `src/config.h` `PIN_OLED_*` constants
- Driver: Adafruit_SSD1306 or equivalent for SPI mode
- Init sequence: reset low → delay → reset high → begin SPI

### Rotary Encoder
- A (CLK) and B (DT) decode direction via quadrature
- Button is debounced (mechanical switch)
- Pull-ups: internal GPIO pull-ups recommended (10k typical)

### LocoNet UART
- UART2 (separate from debug UART0)
- Baud rate: 16,667 bps (non-standard, requires custom UART config)
- Library: `LocoNet.zip` (mrrwa, ESP32-compatible with SW UART adapter)
- Note: ESP32 hardware UART2 may need pin swaps depending on board variant

### Motor PWM
- GPIO32 supports PWM (ledc peripheral)
- Frequency: typically 1 kHz (configurable)
- Resolution: 8-bit (0–255) or 10-bit (0–1023)
- Direction lines (GPIO12/13) set H-bridge direction

### Index Sensor
- GPIO34 is input-only (ADC capable)
- Can be digital (microswitch) or analog (Hall-effect / optical)
- Pull-up via sensor circuit (not internal GPIO pull-up)

---

## Strapping & Boot Pins

The ESP32 uses internal pull-ups/pull-downs on certain GPIO for boot configuration:

| GPIO | Pull | State at Boot | Usage |
|---|---|---|---|
| GPIO0 | Up | High | JTAG/Flash mode; can use as output after boot |
| GPIO2 | Down | Low | Boot; safe after boot; we use for relay control |
| GPIO4 | Down | Low | Boot; can use after boot |
| GPIO5 | Up | High | Strapping; we use as SPI CS |
| GPIO12 | Down | Low | Boot voltage select; safe after boot; we use for motor |
| GPIO15 | Down | Low | Boot; safe after boot |

**All assigned GPIO are safe after boot**. No conflicts with strap pin requirements.

---

## Total GPIO Usage

- **Assigned**: 16 GPIO pins (display, encoder, buttons, motor, LocoNet, home sensor, relay)
- **Available on ESP32**: 30+ usable GPIO
- **Headroom**: 14+ pins free for future sensors, SD card, expansion

### Pins Still Available
GPIO 1, 3, 6–11 (flash), 14, 15, 24, 28–31, 35–39 (most are input-only)

---

## Future Expansions

If you need additional sensors or features:
- **SD card**: use SPI1 (GPIO8–11)
- **Additional ADC sensors**: GPIO35, GPIO36, GPIO37, GPIO38, GPIO39
- **I²C devices**: GPIO1 (SCL), GPIO3 (SDA) — or use alternate I²C on GPIO21, GPIO22 if needed
- **Extra buttons or digital inputs**: abundant GPIO (14, 15, 24, 28–31, etc.)

---

## Cross-Reference to Code

See `src/config.h` for the `#define PIN_*` constants used throughout the firmware.
