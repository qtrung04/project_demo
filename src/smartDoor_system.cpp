#include "smartDoor_system.h"

#include <WiFi.h>
#include <WiFiManager.h>

#include "Display.h"
#include "RFID.h"
#include "blynkHandler.h"
#include "config.h"
#include "doorControl.h"
#include "keypadHandler.h"
#include "passwordManager.h"

void startLocktime() {
  isLocked = true;
  lock_start_time = millis();
  tft.fillScreen(ST77XX_BLACK);
  tftprint(2, "WRONG " + String(max_attempts) + " TIMES", 0, 30);
  tftprint(2, "Try again in", 0, 60);
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
  RFIDinit();
}

void smartDoorUpdate() {
  blynkUpdate();
  if (!isLocked) {
    tftprint(1, "Enter Password or scanRFID", 0, 30);
    tftprint(1, "Attempts: " + String(max_attempts - error_pass), 0, 45);

    // Check RFID card
    char uid[30] = {0};
    if (scanRFID(uid)) {
      if (isValidRFIDCard(uid)) {
        // RFID card is valid, unlock door
        tft.fillScreen(ST77XX_BLACK);
        tftprint(2, "Card Valid!", 15, 54);
        delay(1000);
        error_pass = 0;
        index_t = 2;
      } else {
        // Invalid card, show error
        tft.fillScreen(ST77XX_BLACK);
        tftprint(2, "Invalid Card", 15, 54);
        delay(1000);
        error_pass++;
        tft.fillScreen(ST77XX_BLACK);

        if (error_pass >= max_attempts) {
          index_t = 3;  // trigger locktime
        }
      }
    }

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

  if (index_t == 4) {
    manageRFID();
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
