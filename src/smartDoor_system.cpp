#include "config.h"
#include "smartDoor_system.h"
#include "Display.h"
#include "doorControl.h"
#include "keypadHandler.h"
#include "passwordManager.h"
#include "blynkHandler.h"

#include <WiFi.h>
#include <WiFiManager.h>

void startLocktime() {
  isLocked = true;
  lock_start_time = millis();
  tft.fillScreen(ST77XX_BLACK);
  tftprint("WRONG " + String(max_attempts) + " TIMES", 0, 30);
  tftprint("Try again in", 0, 60);
}

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
  if (!isLocked) {
    tftprint("Enter Pass", 0, 30);
    checkPass();
  }

  if (index_t == 1) {
    changePass();
    logevent("change_pass", String("Password changed: ") + password);
  }

  if (index_t == 2) {
    blynkWriteV0(HIGH);
    openDoor();
    error_pass = 0;
  }

  if (index_t == 3 && !isLocked) {
    logevent("error", "Cảnh báo đột nhập");
    startLocktime();
  }
  if (isLocked) {
    static int last_remain = -1;
    unsigned long elapsed = (millis() - lock_start_time) / 1000;
    int remain = lock_time_seconds - elapsed;

    if (remain >= 0 && remain != last_remain) {
      tft.fillRect(0, 90, 160, 16, ST77XX_BLACK);
      centerText(String(remain) + "(s)", 90);
      last_remain = remain;
    }
    if (elapsed >= lock_time_seconds) {
      isLocked = false;
      index_t = 0;
      tft.fillScreen(ST77XX_BLACK);
    }
  }
}
