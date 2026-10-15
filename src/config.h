#ifndef CONFIG_H
#define CONFIG_H

// Hardware pins (ESP32 D1 Mini - WROOM-32 variant)
// Comments give the board silkscreen label first, then the GPIO number.
// Strapping pins in use: TDO (home sensor, pulled up at boot) only. See docs/GPIO_REFERENCE.md.

// SPI display (Waveshare 2.0" IPS, 240x320, ST7789V). CS is hard-wired low: the display is the only SPI device.
// RST is tied to the ESP32 RST line. BL is driven by PWM through a MOSFET (backlight draws about 45 mA).
#define PIN_DISPLAY_CLK   18  // label 18 (GPIO18) - SPI clock
#define PIN_DISPLAY_MOSI  23  // label 23 (GPIO23) - SPI data (DIN)
#define PIN_DISPLAY_DC    27  // label 27 (GPIO27) - Data/command
#define PIN_DISPLAY_BL    26  // label 26 (GPIO26) - Backlight PWM (via MOSFET)

// Rotary encoder (with integrated button)
#define PIN_ENCODER_A    19  // label 19 (GPIO19) - Encoder A
#define PIN_ENCODER_B    21  // label 21 (GPIO21) - Encoder B
#define PIN_ENCODER_BTN  22  // label 22 (GPIO22) - Encoder button

// Button board (external pull-ups)
#define PIN_MENU_BTN     13  // label TCK (GPIO13) - MENU
#define PIN_ESTOP        35  // label 35 (GPIO35) - Emergency STOP, normally open, active LOW (input-only, external pull-up)
#define PIN_BUTTON_LADDER 34 // label 34 (GPIO34) - Resistor ladder: PRESET A, PRESET B, LAST EXIT, SPEED (ADC1, input-only)

// Resistor ladder: series string (430, 270, 430, 680 and 1.3k ohm to GND) fed from 3.3 V through 1 kΩ,
// with a 0.1 µF filter on the ADC node. Levels in millivolts. Button assignment is set here.
#define LADDER_MV_PRESET_A  992   // Button 4: 430 Ω to GND
#define LADDER_MV_PRESET_B  1359  // Button 3: 700 Ω
#define LADDER_MV_LAST_EXIT 1755  // Button 2: 1.13 kΩ
#define LADDER_MV_SPEED     2123  // Button 1: 1.81 kΩ
#define LADDER_MV_NONE      2497  // no button: 3.11 kΩ
#define LADDER_TOLERANCE_MV  150  // reading must be within this of a level
#define LADDER_FAULT_LOW_MV  700  // below this is a fault (short to GND)

// LocoNet (later phase, not in the initial build): RX label 16 (GPIO16) and TX label 17 (GPIO17) are reserved.

// Motor (TB6612FNG channel A)
#define PIN_MOTOR_DIR1   25  // label 25 (GPIO25) - Motor direction
#define PIN_MOTOR_PWM1   32  // label 32 (GPIO32) - Motor PWM speed

// Lock solenoid (TB6612FNG channel B)
#define PIN_SOLENOID_DIR2 33 // label 33 (GPIO33) - Solenoid direction, set by jumper (see OI-12 / to-do)
#define PIN_SOLENOID_PWM2  4 // label 4 (GPIO4) - Solenoid PWM (drive high to hold open)

// Home sensor (microswitch, internal pull-up)
#define PIN_HOME_SENSOR  15  // label TDO (GPIO15) - Microswitch at home position (exit 0). Strapping pin, pulled up at boot

// Status ring (NeoPixel data)
#define PIN_STATUS_RING  14  // label TMS (GPIO14) - WS2812 data

// Motor control defaults
#define MOTOR_PWM_MAX 1023
#define MOTOR_PWM_DEFAULT 512
#define MOTOR_STEP_DURATION_MS 50  // Time between motor steps

// Display
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

// EEPROM
// Storage backend (EEPROM emulation or NVS) to be decided in Phase 1
#define EEPROM_SIZE 4096
#define EEPROM_CONFIG_OFFSET 0
#define EEPROM_CONFIG_SIZE 256
#define EEPROM_DEFAULTS_OFFSET 256  // Factory defaults region (MNU-10/11)
#define EEPROM_CONFIG_MAGIC 0xA5C3
#define EEPROM_CONFIG_VERSION 1

// Default configuration values
#define DEFAULT_LOCONET_BASE_ADDRESS 200
#define DEFAULT_NUM_EXITS 0  // Set per turntable (CFG-03); 0 = not configured
#define DEFAULT_HOME_POSITION 0

// Wi-Fi
#define WIFI_SETUP_MODE_HOLD_TIME_MS 3000
#define WIFI_CONNECT_TIMEOUT_MS 300

// LocoNet (later phase)
#define LOCONET_STOP_BROADCAST_ENABLED 1  // Local e-stop sends to LocoNet (RUN-10..14)

#endif // CONFIG_H
