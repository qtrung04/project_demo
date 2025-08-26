#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <EEPROM.h>

void writeEpprom(char data[]);
void readEpprom();
bool isBufferdata(char data[]);
bool compareData(char d1[], char d2[]);
void insertData(char dest[], char src[]);
void clear_data_input();
#endif
