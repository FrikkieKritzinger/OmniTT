#ifndef CONFIG_H
#define CONFIG_H

// Hardware pins (ESP32 D1 Mini - WROOM-32 variant)
// Platform: ESP32 Dual-Core, 30+ GPIO, no expander needed
// SPI Display: 2.44" OLED (128×64)

// SPI Display (2.44" OLED) - uses SPI2
#define PIN_OLED_CLK   18   // GPIO18 - SPI2 Clock
#define PIN_OLED_MOSI  23   // GPIO23 - SPI2 MOSI (Data)
#define PIN_OLED_CS    5    // GPIO5  - Chip Select
#define PIN_OLED_DC    27   // GPIO27 - Data/Command
#define PIN_OLED_RES   26   // GPIO26 - Reset

// Rotary Encoder (with integrated button)
#define PIN_ENCODER_A   19  // GPIO19 - Encoder A (CLK)
#define PIN_ENCODER_B   21  // GPIO21 - Encoder B (DT)
#define PIN_ENCODER_BTN 22  // GPIO22 - Encoder button

// User Input Buttons
#define PIN_MENU_BTN    25  // GPIO25 - MENU button
#define PIN_ESTOP       33  // GPIO33 - Emergency STOP button

// LocoNet Interface (UART2)
#define PIN_LOCONET_RX  16  // GPIO16 - LocoNet RX
#define PIN_LOCONET_TX  17  // GPIO17 - LocoNet TX

// Motor Control (DC motor with PWM speed)
#define PIN_MOTOR_A     12  // GPIO12 - Motor direction A
#define PIN_MOTOR_B     13  // GPIO13 - Motor direction B
#define PIN_MOTOR_PWM   32  // GPIO32 - Motor PWM speed (0-255)

// Index / Home Sensor
#define PIN_INDEX_SENSOR 34 // GPIO34 - ADC capable, index pulse input

// Bridge Power Relay (polarity invert, optional)
#define PIN_RELAY_CTRL  2   // GPIO2  - Relay control output

// Note on ESP32 strapping pins (safe to use after boot):
// GPIO0, GPIO2, GPIO4, GPIO5, GPIO12, GPIO15 have internal pulls but can be used
// as outputs. GPIO34-39 are input-only (no output capability).

// Motor control defaults
#define MOTOR_PWM_MAX 1023
#define MOTOR_PWM_DEFAULT 512
#define MOTOR_STEP_DURATION_MS 50  // Time between motor steps

// Display
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define I2C_ADDRESS_DISPLAY 0x3C

// EEPROM
#define EEPROM_SIZE 4096
#define EEPROM_CONFIG_OFFSET 0
#define EEPROM_CONFIG_SIZE 256
#define EEPROM_DEFAULTS_OFFSET 256  // Factory defaults region (MNU-10/11)
#define EEPROM_CONFIG_MAGIC 0xA5C3
#define EEPROM_CONFIG_VERSION 1

// Default configuration values
#define DEFAULT_LOCONET_BASE_ADDRESS 200
#define DEFAULT_NUM_EXITS 6
#define DEFAULT_HOME_POSITION 0

// Wi-Fi
#define WIFI_SETUP_MODE_HOLD_TIME_MS 3000
#define WIFI_CONNECT_TIMEOUT_MS 300

// LocoNet
#define LOCONET_STOP_BROADCAST_ENABLED 1  // Local e-stop sends to LocoNet (RUN-10..14)

#endif // CONFIG_H
