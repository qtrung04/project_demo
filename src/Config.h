#ifndef CONFIG_H
#define CONFIG_H

// === BLYNK ===
#define BLYNK_TEMPLATE_ID "TMPL6Pa74tSP8"
#define BLYNK_TEMPLATE_NAME "Smart door"
#define BLYNK_AUTH_TOKEN "WHCRY6Rcm76o659_eWg94N4bqJfJDF3e"

#include <Arduino.h> 

// === PINOUT ===
#define PIN_door 13

// TFT LCD
#define TFT_CS 5
#define TFT_RST 4
#define TFT_DC 2
#define TFT_SCLK 18
#define TFT_MOSI 23

inline char data_input[6];
inline int max_attempts = 3;
inline int lock_time_seconds = 60;
inline unsigned char index_t = 0, in_num = 0, error_pass = 0;
inline bool isLocked = false;
inline unsigned long lock_start_time = 0;

inline char password[6] = "12345";
inline char mode_changePass[6] = "*#01#";

// EEPROM
#define EEPROM_ADDR_ATTEMPTS 10
#define EEPROM_ADDR_LOCKTIME 20

#endif
