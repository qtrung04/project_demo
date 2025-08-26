#include "passwordManager.h"
#include "config.h"

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