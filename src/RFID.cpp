#include "RFID.h"

void RFIDinit() {
  SPI.begin();
  mfrc522.PCD_Init();
  prefs.begin("rfid", false);
}
