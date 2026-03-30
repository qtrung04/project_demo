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

void RFIDinit();

#endif