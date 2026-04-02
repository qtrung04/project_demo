#include "passwordManager.h"

#include "Display.h"
#include "RFID.h"
#include "blynkHandler.h"
#include "config.h"
#include "keypadHandler.h"

char new_pass1[6];
char new_pass2[6];

void writeEpprom(char data[]) {
  for (int i = 0; i < 5; i++) EEPROM.write(i, data[i]);
  EEPROM.commit();
}

void readEpprom() {
  for (int i = 0; i < 5; i++) password[i] = EEPROM.read(i);
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

void clear_data_input() {
  for (int i = 0; i < 6; i++) data_input[i] = '\0';
  in_num = 0;
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
    } else if (compareData(data_input, mode_manageRFID)) {
      tft.fillScreen(ST77XX_BLACK);
      clear_data_input();
      index_t = 4;  // RFID management mode
    } else {
      clear_data_input();
      error_pass++;
      tft.fillScreen(ST77XX_BLACK);
      tftprint(2, "Wrong Pass", 20, 54);
      delay(1000);
      if (error_pass >= max_attempts) {
        index_t = 3;  // trigger locktime
      } else {
        tft.fillScreen(ST77XX_BLACK);
      }
    }
  }
}

void changePass() {
  char current_pass_input[6];
  clear_data_input();
  tft.fillScreen(ST77XX_BLACK);
  centerText("CHANGE PASS", 30);
  delay(500);
  tftprint(2, "Enter Pass:", 0, 56);
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
  tftprint(2, "New Pass:", 0, 56);
  while (1) {
    getData();
    if (isBufferdata(data_input)) {
      insertData(new_pass1, data_input);
      clear_data_input();
      break;
    }
  }

  tft.fillRect(0, 56, 160, 40, ST77XX_BLACK);
  tftprint(2, "Again newPass", 0, 56);
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
    blynkWriteV1(String() + new_pass2);

  } else {
    tft.fillScreen(ST77XX_BLACK);
    centerText("Mismatched", 54);
  }

  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  index_t = 0;
}

void manageRFID() {
  clear_data_input();
  tft.fillScreen(ST77XX_BLACK);
  centerText("RFID MANAGER", 10);

  int option = 0;
  bool selecting = true;

  while (selecting) {
    char key = keypad.getKey();

    tft.fillScreen(ST77XX_BLACK);
    centerText("RFID MANAGER", 10);

    // Display options
    tftprint(1, "1. Add Card", 5, 35);
    tftprint(1, "2. Delete Card", 5, 55);
    tftprint(1, "3. View Cards", 5, 75);
    tftprint(1, "4. Exit", 5, 95);

    if (key == '1') {
      selecting = false;
      tft.fillScreen(ST77XX_BLACK);
      centerText("SCANI CARD", 30);
      char uid[30] = {0};

      // Wait for RFID card
      unsigned long timeout = millis();
      while (millis() - timeout < 10000) {  // 10 second timeout
        if (scanRFID(uid)) {
          if (addRFIDCard(uid)) {
            tft.fillScreen(ST77XX_BLACK);
            centerText("Card Added!", 54);
            delay(2000);
          } else {
            tft.fillScreen(ST77XX_BLACK);
            centerText("ADD FAILED", 54);
            tftprint(1, "Card exists or full", 5, 70);
            delay(2000);
          }
          break;
        }
      }

      if (millis() - timeout >= 10000) {
        tft.fillScreen(ST77XX_BLACK);
        centerText("TIMEOUT!", 54);
        delay(1000);
      }
    } else if (key == '2') {
      selecting = false;
      tft.fillScreen(ST77XX_BLACK);
      centerText("SCAN TO DELETE", 30);
      char uid[30] = {0};

      // Wait for RFID card
      unsigned long timeout = millis();
      while (millis() - timeout < 10000) {  // 10 second timeout
        if (scanRFID(uid)) {
          if (deleteRFIDCard(uid)) {
            tft.fillScreen(ST77XX_BLACK);
            centerText("Deleted!", 54);
            delay(2000);
          } else {
            tft.fillScreen(ST77XX_BLACK);
            centerText("NOT FOUND", 54);
            delay(2000);
          }
          break;
        }
      }

      if (millis() - timeout >= 10000) {
        tft.fillScreen(ST77XX_BLACK);
        centerText("TIMEOUT!", 54);
        delay(1000);
      }
    } else if (key == '3') {
      selecting = false;
      int count = getRFIDCardCount();

      tft.fillScreen(ST77XX_BLACK);
      if (count == 0) {
        centerText("No Cards", 54);
        delay(2000);
      } else {
        tftprint(1, "CARD LIST", 5, 10);
        int page = 0;
        int cards_per_page = 4;

        while (true) {
          tft.fillScreen(ST77XX_BLACK);
          centerText("CARD LIST", 10);

          int start = page * cards_per_page;
          int end = start + cards_per_page;
          if (end > count) end = count;

          int y = 35;
          for (int i = start; i < end; i++) {
            char uid[30];
            if (getRFIDCard(i, uid)) {
              tftprint(1, String(i + 1) + ": " + String(uid), 5, y);
              y += 20;
            }
          }

          // Navigation
          tftprint(1, "A:Prev B:Next C:Exit", 2, 110);
          char key = keypad.getKey();

          if (key == 'A' && page > 0) {
            page--;
          } else if (key == 'B' && end < count) {
            page++;
          } else if (key == 'C') {
            break;
          }

          delay(200);
        }
      }
    } else if (key == '4') {
      selecting = false;
    }

    delay(100);
  }

  tft.fillScreen(ST77XX_BLACK);
  index_t = 0;
}