#ifndef BLYNKHANDLER_H
#define BLYNKHANDLER_H

#include <Arduino.h>

void blynkInit();
void blynkUpdate();

void blynkWriteV0(int state);
void blynkWriteV1(String data);

#endif
