#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// === BLYNK ===
#define BLYNK_TEMPLATE_ID "TMPL6Pa74tSP8"
#define BLYNK_TEMPLATE_NAME "Smart door"
#define BLYNK_AUTH_TOKEN "WHCRY6Rcm76o659_eWg94N4bqJfJDF3e"

// === PINOUT ===
#define PIN_DOOR 13

// TFT LCD
#define TFT_CS 5
#define TFT_RST 4
#define TFT_DC 2
#define TFT_SCLK 18
#define TFT_MOSI 23

int max_attempts = 3;
int lock_time_seconds = 60;

// Keypad
#define ROWS 4
#define COLS 4
static byte rowPins[ROWS] = {14, 27, 26, 25};
static byte colPins[COLS] = {33, 32, 19, 21};

static char keys[ROWS][COLS] = {{'1', '2', '3', 'A'},
                                {'4', '5', '6', 'B'},
                                {'7', '8', '9', 'C'},
                                {'*', '0', '#', 'D'}};

char password[6] = "12345";
char mode_changePass[6] = "*#01#";

// EEPROM
#define EEPROM_ADDR_ATTEMPTS 10
#define EEPROM_ADDR_LOCKTIME 20

#endif
