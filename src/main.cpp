#include "config.h"
#include "Display.h"
#include "passwordManager.h"
#include "keypadHandler.h"

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiManager.h>

void openDoor() {
  tft.fillScreen(ST77XX_BLACK);
  tftprint("OPEN DOOR", 26, 54);
  digitalWrite(PIN_door, HIGH);
  delay(3000);
  digitalWrite(PIN_door, LOW);
  Blynk.virtualWrite(V0, 0);
  tft.fillScreen(ST77XX_BLACK);
  index_t = 0;
}



void startLocktime() {
  isLocked = true;
  lock_start_time = millis();
  tft.fillScreen(ST77XX_BLACK);
  tftprint("WRONG " + String(max_attempts) + " TIMES", 0, 30);
  tftprint("Try again in", 0, 60);
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(64);
  readEpprom();

  max_attempts = EEPROM.read(EEPROM_ADDR_ATTEMPTS);
  EEPROM.get(EEPROM_ADDR_LOCKTIME, lock_time_seconds);

  pinMode(PIN_door, OUTPUT);
  digitalWrite(PIN_door, LOW);

  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  if (!wm.autoConnect("SmartDoor_Config")) {
    Serial.println("⚠️ WiFi failed. Restarting...");
    delay(3000);
    ESP.restart();
  }

  Blynk.begin(BLYNK_AUTH_TOKEN, WiFi.SSID().c_str(), WiFi.psk().c_str());

  tftInit();
}

void loop() {
  Blynk.run();

  if (!isLocked) {
    tftprint("Enter Pass", 0, 30);
    checkPass();
  }

  if (index_t == 1) {
    changePass();
    Blynk.logEvent("change_pass", String("Password changed: ") + password);
  }

  if (index_t == 2) {
    Blynk.virtualWrite(V0, HIGH);
    openDoor();
    error_pass = 0;
  }

  if (index_t == 3 && !isLocked) {
    Blynk.logEvent("error", "Cảnh báo đột nhập");
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
  centerText("Changed pass", 60);
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
  tftprint("Attempts = " + String(max_attempts), 0, 60);
  delay(2000);
  tft.fillRect(0, 60, 160, 16, ST77XX_BLACK);
}

BLYNK_WRITE(V4) {
  lock_time_seconds = param.asInt();
  EEPROM.put(EEPROM_ADDR_LOCKTIME, lock_time_seconds);
  EEPROM.commit();
  Serial.printf("Lock time set to: %d\n", lock_time_seconds);
  tftprint("Set lockTime ", 0, 60);
  tftprint("to = " + String(lock_time_seconds) + "s", 0, 80);
  delay(2000);
  tft.fillRect(0, 60, 160, 50, ST77XX_BLACK);
}