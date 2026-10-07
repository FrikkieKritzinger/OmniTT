#ifndef CONFIG_H
#define CONFIG_H

// Hardware pins (Wemos D1 Mini GPIO)
#define PIN_MOTOR_A D6      // GPIO12 - Motor direction A
#define PIN_MOTOR_B D5      // GPIO14 - Motor direction B
#define PIN_MOTOR_PWM D8    // GPIO15 - Motor PWM (avoid as input!)
#define PIN_ENCODER_A D1    // GPIO5 - Rotary encoder A
#define PIN_ENCODER_B D2    // GPIO4 - Rotary encoder B
#define PIN_ENCODER_BTN D3  // GPIO0 - Encoder button (avoid as input!)
#define PIN_ESTOP D7        // GPIO13 - E-stop button
#define PIN_OLED_SDA D4     // GPIO2 - I2C SDA (avoid as input!)
#define PIN_OLED_SCL D0     // GPIO16 - I2C SCL
#define PIN_LOCONET_RX D0   // GPIO16 - LocoNet RX
#define PIN_LOCONET_TX D8   // GPIO15 - LocoNet TX (avoid as input!)

// Note: GPIO0, GPIO2, GPIO15 are boot-strapping pins - verify usage carefully.
// GPIO layout may need revision pending OI-11 (GPIO budget / I/O expander decision).

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
