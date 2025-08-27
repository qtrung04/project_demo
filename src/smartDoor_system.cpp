#include "config.h"
#include "smartDoor_system.h"
#include "Display.h"
#include "doorControl.h"
#include "keypadHandler.h"
#include "passwordManager.h"
#include "blynkHandler.h"

#include <WiFi.h>
#include <WiFiManager.h>

void smartDoorInit() {
  Serial.begin(115200);
  EEPROM.begin(64);
  readEpprom();

  max_attempts = EEPROM.read(EEPROM_ADDR_ATTEMPTS);
  EEPROM.get(EEPROM_ADDR_LOCKTIME, lock_time_seconds);

  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  if (!wm.autoConnect("SmartDoor_Config")) {
    Serial.println("⚠️ WiFi failed. Restarting...");
    delay(3000);
    ESP.restart();
  }

  doorInit();
  blynkInit();
  tftInit();
}

void smartDoorUpdate() {
  blynkUpdate();
}
