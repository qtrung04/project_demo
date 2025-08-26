#include "printfOut.h"

SPIClass spi = SPIClass(VSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&spi, TFT_CS, TFT_DC, TFT_RST);

void tftInit() {
  spi.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
}

void centerText(String text, int y) {
  tft.setTextSize(2);  // Kích thước chữ
  tft.setTextColor(ST77XX_WHITE);

  int16_t x1, y1;
  uint16_t w, h;

  // Lấy kích thước pixel của chữ
  tft.getTextBounds(text, 0, y, &x1, &y1, &w, &h);

  // Tính tọa độ X để căn giữa
  int x = (tft.width() - w) / 2;

  // Hiển thị
  tft.setCursor(x, y);
  tft.print(text);
}
void tftprint(String text, int x, int y) {
  tft.setCursor(x, y);
  tft.print(text);
}
