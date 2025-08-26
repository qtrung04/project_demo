#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#include "config.h"

extern SPIClass spi;
extern Adafruit_ST7735 tft;

void tftInit();

void centerText(String text, int y);
void tftprint(String text, int x, int y);

#endif
