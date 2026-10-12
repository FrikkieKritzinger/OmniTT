#ifndef CONFIG_H
#define CONFIG_H

// Hardware pins (ESP32 D1 Mini - WROOM-32 variant)
// Comments give the board silkscreen label first, then the GPIO number.
// Strapping pins are used only where their state at boot is harmless. See docs/GPIO_REFERENCE.md.

// SPI display (2.44" OLED, 128x64). CS is hard-wired low: the OLED is the only SPI device.
#define PIN_OLED_CLK    18  // label 18 (GPIO18) - SPI clock
#define PIN_OLED_MOSI   23  // label 23 (GPIO23) - SPI data
#define PIN_OLED_DC     27  // label 27 (GPIO27) - Data/command
#define PIN_OLED_RES    26  // label 26 (GPIO26) - Reset

// Rotary encoder (with integrated button)
#define PIN_ENCODER_A    19  // label 19 (GPIO19) - Encoder A
#define PIN_ENCODER_B    21  // label 21 (GPIO21) - Encoder B
#define PIN_ENCODER_BTN  22  // label 22 (GPIO22) - Encoder button

// Console buttons
#define PIN_MENU_BTN      25  // label 25 (GPIO25) - MENU
#define PIN_ESTOP         33  // label 33 (GPIO33) - Emergency STOP
#define PIN_PRESET_A_BTN  34  // label 34 (GPIO34) - PRESET A (input-only, external pull-up)
#define PIN_PRESET_B_BTN  36  // label SVP (GPIO36) - PRESET B (input-only, external pull-up)
#define PIN_LAST_EXIT_BTN 39  // label SVN (GPIO39) - LAST EXIT (input-only, external pull-up)
#define PIN_SPEED_BTN     15  // label TDO (GPIO15) - SPEED toggle (strapping; pulled up at boot)

// LocoNet interface (UART2)
#define PIN_LOCONET_RX   16  // label 16 (GPIO16) - LocoNet RX
#define PIN_LOCONET_TX   17  // label 17 (GPIO17) - LocoNet TX

// Motor (TB6612FNG channel A)
#define PIN_MOTOR_DIR1   13  // label TCK (GPIO13) - Motor direction
#define PIN_MOTOR_PWM1   32  // label 32 (GPIO32) - Motor PWM speed

// Lock solenoid (TB6612FNG channel B)
#define PIN_SOLENOID_DIR2 14 // label TMS (GPIO14) - Solenoid direction (drive high to hold open)
#define PIN_SOLENOID_PWM2  4 // label 4 (GPIO4) - Solenoid PWM (drive high to hold open)

// Home sensor (input-only, external pull-up)
#define PIN_HOME_SENSOR  35  // label 35 (GPIO35) - Microswitch at home position (exit 0)

// Status ring (NeoPixel data, strapping pin; boot-time level only flickers the LEDs)
#define PIN_STATUS_RING   5  // label 5 (GPIO5) - WS2812 data

// Motor control defaults
#define MOTOR_PWM_MAX 1023
#define MOTOR_PWM_DEFAULT 512
#define MOTOR_STEP_DURATION_MS 50  // Time between motor steps

// Display
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

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

// LocoNet
#define LOCONET_STOP_BROADCAST_ENABLED 1  // Local e-stop sends to LocoNet (RUN-10..14)

#endif // CONFIG_H
