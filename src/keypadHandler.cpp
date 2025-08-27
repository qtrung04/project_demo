#include "keypadHandler.h"
#include "Display.h"

// Keypad
#define ROWS 4
#define COLS 4

static byte rowPins[ROWS] = {14, 27, 26, 25};
static byte colPins[COLS] = {33, 32, 19, 21};

static char keys[ROWS][COLS] = {{'1', '2', '3', 'A'},
                                {'4', '5', '6', 'B'},
                                {'7', '8', '9', 'C'},
                                {'*', '0', '#', 'D'}};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

void getData() {
  char key = keypad.getKey();
  if (key) {
    if (in_num < 5) {
      data_input[in_num] = key;
      tftprint(String(data_input[in_num]), 48 + 12 * in_num, 80);
      delay(200);
      tft.fillRect(48 + 12 * in_num, 80, 12, 16, ST77XX_BLACK);
      tftprint("*", 48 + 12 * in_num, 80);
      in_num++;
    }
  }
}
