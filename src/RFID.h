#ifndef RFID_H
#define RFID_H

#include <MFRC522.h>
#include <Preferences.h>
#include <SPI.h>

#include "config.h"

// ===== RFID =====
MFRC522 mfrc522(SS_PIN, RST_PIN);

// ===== Preferences =====
Preferences prefs;

#define MAX_RFID_CARDS 10
#define RFID_NAMESPACE "rfid"

// RFID Card structure
struct RFIDCard {
  char uid[30];  // UID in HEX string format
};

// RFID Management Functions
void RFIDinit();
bool scanRFID(char* uid);
bool isValidRFIDCard(char* uid);
bool addRFIDCard(char* uid);
bool deleteRFIDCard(char* uid);
int getRFIDCardCount();
bool getRFIDCard(int index, char* uid);
void clearAllRFIDCards();

#endif