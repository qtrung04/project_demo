#include "doorControl.h"
#include "Config.h"
#include "Display.h"
#include "blynkHandler.h"

void doorInit() {
  pinMode(PIN_door, OUTPUT);
  digitalWrite(PIN_door, LOW);
}

void openDoor() {
  tft.fillScreen(ST77XX_BLACK);
  tftprint("OPEN DOOR", 26, 54);
  digitalWrite(PIN_door, HIGH);
  delay(3000);
  digitalWrite(PIN_door, LOW);
  blynkWriteV0(0);
  tft.fillScreen(ST77XX_BLACK);
  index_t = 0;
}