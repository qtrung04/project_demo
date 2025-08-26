#include "DoorControl.h"
#include "Config.h"

void doorInit() {
  pinMode(PIN_door, OUTPUT);
  digitalWrite(PIN_door, LOW);
}

void DoorControl() {
  digitalWrite(PIN_door, HIGH);
  delay(3000);
  digitalWrite(PIN_door, LOW);
}
