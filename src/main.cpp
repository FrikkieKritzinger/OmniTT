#include <Arduino.h>
#include "version.h"
#include "config.h"

// Include local Wi-Fi credentials if available
#if __has_include("wifi_credentials.local.h")
#include "wifi_credentials.local.h"
#else
#warning "wifi_credentials.local.h not found - copy from .example and fill in your details"
#endif

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.print("\n\nOmniTT Firmware v");
  Serial.println(FIRMWARE_VERSION);
  Serial.print("Built: ");
  Serial.print(FIRMWARE_BUILD_DATE);
  Serial.print(" ");
  Serial.println(FIRMWARE_BUILD_TIME);

  // TODO: Initialize subsystems
  // - Motor controller
  // - Rotary encoder
  // - E-stop button
  // - OLED display
  // - EEPROM config
  // - Wi-Fi / LocoNet interface
}

void loop() {
  // TODO: Main update loop - keep non-blocking
  // - Handle encoder input
  // - Update motor control
  // - Refresh display
  // - Process LocoNet commands
  // - Handle e-stop

  yield();
}
