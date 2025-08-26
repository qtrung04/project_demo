#include "DoorControl.h"
#include "Config.h"

void DoorControl() {
  digitalWrite(PIN_door, HIGH);
  delay(3000);
  digitalWrite(PIN_door, LOW);
}
