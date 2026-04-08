#include "Config.h"
#include "blynkHandler.h"
#include "Display.h"
#include "doorControl.h"
#include "passwordManager.h"

#include <BlynkSimpleEsp32.h>

void blynkInit() {
  Serial.println("Initializing Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, WiFi.SSID().c_str(), WiFi.psk().c_str());
  Serial.println("Blynk initialized");
}

void blynkUpdate() {
  Blynk.run();
}

void blynkWriteV0(int state) {
  Blynk.virtualWrite(V0, state);
}

void blynkWriteV1(String data) {
  Blynk.virtualWrite(V1, data);
}

void logevent(String eventName, String data) {
  Blynk.logEvent(eventName.c_str(), data.c_str());
}

BLYNK_WRITE(V0) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    Serial.println("Unlocking via Blynk");

    if (isLocked) {
      isLocked = false;
      index_t = 0;
      tft.fillScreen(ST77XX_BLACK);
      Serial.println("Locktime canceled");
    }

    index_t = 2;
  }
}

BLYNK_WRITE(V1) {
  char new_passBlynk[6];
  String data = param.asStr();
  for (int i = 0; i < data.length() && i < 5; i++) {
    new_passBlynk[i] = data[i];
  }
  writeEpprom(new_passBlynk);
  insertData(password, new_passBlynk);
  Blynk.logEvent("change_pass", String("Password changed via Blynk: ") + new_passBlynk);
  tftprint(2, "Changed pass", 0, 60);
  delay(2000);
  tft.fillScreen(ST77XX_BLACK);
}

BLYNK_WRITE(V2) {
  Blynk.logEvent("check_pass", String("Current pass: ") + password);
}

BLYNK_WRITE(V3) {
  max_attempts = param.asInt();
  EEPROM.write(EEPROM_ADDR_ATTEMPTS, max_attempts);
  EEPROM.commit();
  Serial.printf("Max attempts set to %d\n", max_attempts);
  tftprint(2, "Attempts = " + String(max_attempts), 0, 60);
  delay(2000);
  tft.fillRect(0, 60, 160, 16, ST77XX_BLACK);
}

BLYNK_WRITE(V4) {
  lock_time_seconds = param.asInt();
  EEPROM.put(EEPROM_ADDR_LOCKTIME, lock_time_seconds);
  EEPROM.commit();
  Serial.printf("Lock time set to: %d\n", lock_time_seconds);
  tftprint(2, "Set lockTime ", 0, 60);
  tftprint(2, "to = " + String(lock_time_seconds) + "s", 0, 80);
  delay(2000);
  tft.fillRect(0, 60, 160, 50, ST77XX_BLACK);
}