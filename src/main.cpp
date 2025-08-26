#include "config.h"
#include "printfOut.h"

#include <BlynkSimpleEsp32.h>
#include <EEPROM.h>
#include <Keypad.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiManager.h>


char data_input[6];
char new_pass1[6];
char new_pass2[6];

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

unsigned char index_t = 0, in_num = 0, error_pass = 0;
bool isLocked = false;
unsigned long lock_start_time = 0;

void writeEpprom(char data[]) {
  for (int i = 0; i < 5; i++) EEPROM.write(i, data[i]);
  EEPROM.commit();
}

void readEpprom() {
  for (int i = 0; i < 5; i++) password[i] = EEPROM.read(i);
}

void clear_data_input() {
  for (int i = 0; i < 6; i++) data_input[i] = '\0';
  in_num = 0;
}

bool isBufferdata(char data[]) {
  for (int i = 0; i < 5; i++)
    if (data[i] == '\0') return false;
  return true;
}

bool compareData(char d1[], char d2[]) {
  for (int i = 0; i < 5; i++)
    if (d1[i] != d2[i]) return false;
  return true;
}

void insertData(char dest[], char src[]) {
  for (int i = 0; i < 5; i++) dest[i] = src[i];
}

void getData() {
  char key = keypad.getKey();
  if (key) {
    if (in_num < 5) {
      data_input[in_num] = key;
      tftprint(String(data_input[in_num]), 48 + 12 * in_num, 80);
      delay(200);
      tft.fillRect(48 + 12 * in_num, 80, 12, 16, ST77XX_BLACK);
      tftprint("*", 48 + 12 * in_num, 80);
      in_num++;
    }
  }
}

void checkPass() {
  getData();
  if (isBufferdata(data_input)) {
    if (compareData(data_input, password)) {
      tft.fillScreen(ST77XX_BLACK);
      clear_data_input();
      index_t = 2;
    } else if (compareData(data_input, mode_changePass)) {
      tft.fillScreen(ST77XX_BLACK);
      clear_data_input();
      index_t = 1;
    } else {
      clear_data_input();
      error_pass++;
      tft.fillScreen(ST77XX_BLACK);
      tftprint("Wrong Pass", 20, 54);
      delay(1000);
      if (error_pass >= max_attempts) {
        index_t = 3;  // trigger locktime
      } else {
        tft.fillScreen(ST77XX_BLACK);
      }
    }
  }
}

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

void changePass() {
  char current_pass_input[6];
  clear_data_input();
  tft.fillScreen(ST77XX_BLACK);
  centerText("CHANGE PASS", 30);
  delay(500);
  tftprint("Enter Pass:", 0, 56);
  while (1) {
    getData();
    if (isBufferdata(data_input)) {
      insertData(current_pass_input, data_input);
      clear_data_input();
      break;
    }
  }

  if (!compareData(current_pass_input, password)) {
    tft.fillScreen(ST77XX_BLACK);
    centerText("Wrong Pass", 54);
    delay(2000);
    tft.fillScreen(ST77XX_BLACK);
    index_t = 0;
    return;
  }

  tft.fillRect(0, 56, 160, 40, ST77XX_BLACK);
  tftprint("New Pass:", 0, 56);
  while (1) {
    getData();
    if (isBufferdata(data_input)) {
      insertData(new_pass1, data_input);
      clear_data_input();
      break;
    }
  }

  tft.fillRect(0, 56, 160, 40, ST77XX_BLACK);
  tftprint("Again newPass", 0, 56);
  while (1) {
    getData();
    if (isBufferdata(data_input)) {
      insertData(new_pass2, data_input);
      clear_data_input();
      break;
    }
  }

  if (compareData(new_pass1, new_pass2)) {
    writeEpprom(new_pass2);
    insertData(password, new_pass2);
    tft.fillScreen(ST77XX_BLACK);
    centerText("-- Success --", 54);
  } else {
    tft.fillScreen(ST77XX_BLACK);
    centerText("Mismatched", 54);
  }

  delay(1000);
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

// === BLYNK ===
BLYNK_WRITE(V0) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    Serial.println("🔓 Unlocking via Blynk");

    if (isLocked) {
      isLocked = false;
      index_t = 0;
      tft.fillScreen(ST77XX_BLACK);
      Serial.println("✅ Locktime canceled");
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
  Serial.printf("🔐 Max attempts set to %d\n", max_attempts);
  tftprint("Attempts = " + String(max_attempts), 0, 60);
  delay(2000);
  tft.fillRect(0, 60, 160, 16, ST77XX_BLACK);
}

BLYNK_WRITE(V4) {
  lock_time_seconds = param.asInt();
  EEPROM.put(EEPROM_ADDR_LOCKTIME, lock_time_seconds);
  EEPROM.commit();
  Serial.printf("⏳ Lock time set to: %d\n", lock_time_seconds);
  tftprint("Set lockTime ", 0, 60);
  tftprint("to = " + String(lock_time_seconds) + "s", 0, 80);
  delay(2000);
  tft.fillRect(0, 60, 160, 50, ST77XX_BLACK);
}