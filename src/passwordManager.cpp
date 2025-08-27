#include "passwordManager.h"
#include "config.h"
#include "keypadHandler.h"
#include "Display.h"

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