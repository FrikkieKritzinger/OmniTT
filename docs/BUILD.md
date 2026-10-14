# OmniTT Build & Development Guide

## Quick Start

### Prerequisites
- Python 3.7+ and pip
- MinGW-w64 (for native tests)
- PlatformIO: `python -m platformio --version`

### Build Firmware
```bash
cd E:\Claude\OmniTT
python -m platformio run --environment d1_mini
```

Output: `.pio/build/d1_mini/firmware.bin` (ready to flash)

### Run Unit Tests (Host)
```bash
python -m platformio test --environment native
```

Builds with Unity test framework. Current memory usage: **RAM 34.4%, Flash 25.4%** (headroom for subsystems).

### Monitor Serial Output (115200 baud)
```bash
python -m platformio device monitor
# or if you know the port:
python -m platformio device monitor --port COM4
```

The serial monitor will **NOT** auto-reset the board (DTR/RTS disabled in `platformio.ini`), so live EEPROM state persists while debugging.

## Project Structure

```
src/
  main.cpp              Entry point (setup/loop)
  version.h             Firmware version & build info
  config.h              Compile-time defaults (pins, timing, defaults)
  wifi_credentials.local.h.example  Template for secrets
  
test/
  test_config.cpp       Example unit test
  
lib/
  (External libraries from PlatformIO package manager)
  
docs/
  BUILD.md              This file
  CLAUDE.md             Development conventions & status
  Turntable_Controller_SRD.md  System Requirements Specification
  CHANGELOG.md          Dated session history
  
platformio.ini          Build configuration
```

## Configuration

### Wi-Fi Credentials
1. Copy `src/wifi_credentials.local.h.example` → `src/wifi_credentials.local.h`
2. Fill in your SSID and password
3. **Never commit** `wifi_credentials.local.h` (gitignored automatically)

### Hardware Pins
Edit `src/config.h` to match your wiring. The full map, by board silkscreen label, is in [GPIO_REFERENCE.md](GPIO_REFERENCE.md). Summary for the ESP32 D1 Mini:
- Motor: 25 (DIR1), 32 (PWM1)
- Lock solenoid: 33 (DIR2, set by jumper), 4 (PWM2)
- Display (SPI, Waveshare 2.0" IPS): 18 (CLK), 23 (DIN), 27 (DC), 26 (backlight PWM via MOSFET). RST on the ESP32 RST line
- Encoder: 19, 21, 22 (button)
- Button board: MENU on TCK, E-STOP on 35, resistor ladder on 34 (PRESET A, PRESET B, LAST EXIT, SPEED)
- LocoNet: 16 (RX), 17 (TX)
- Home sensor: TDO. Status ring: TMS

The only strapping pin in use is TDO (home sensor, pulled up at boot). See GPIO_REFERENCE.md for the rules and the ladder levels.

### EEPROM Layout
- **0–255**: Runtime configuration (checksum validated)
- **256–511**: Factory defaults (OI-10/11)
- **512+**: Runtime data (position, home, counters)

## Build Flags

The `platformio.ini` enables strict warnings (`-Wall`, `-Wextra`). Fix all compiler warnings before commit.

## Flashing

Once the build succeeds, you can flash over USB:
```bash
python -m platformio run --environment d1_mini --target upload --upload-port COM4
```

Or manually use esptool.py:
```bash
esptool.py --port COM4 --baud 115200 write_flash 0x00000 .pio\build\d1_mini\firmware.bin
```

## Memory Status

**Current (skeleton only):**
- RAM: ~28 KB / 80 KB (34.4%)
- Flash: ~266 KB / 1044 KB (25.4%)

Once subsystems are added, keep an eye on flash usage, especially with LocoNet library and web server. If approaching limits, consider an I/O expander or ESP32 upgrade (OI-11).
