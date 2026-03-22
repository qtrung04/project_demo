#ifndef RFID_H
#define RFID_H

#include <MFRC522.h>
#include <SPI.h>
#include "config.h"

void saveUID(String uid);
bool checkUID(String uid);
void deleteUID(String uid);


#endif