#include "AqmDisplay.h"
#include "Adafruit_HX8357_RK.h"
#include <math.h>
#include <stdio.h>

namespace
{
// The factory TCS jumper connects Feather pin 9, which is Argon D4.
constexpr int8_t TFT_CS = D4;
constexpr int8_t TFT_DC = D5;
constexpr int8_t SD_CS = D2;
constexpr int8_t TOUCH_IRQ = D3;
constexpr int16_t MARGIN = 12;
constexpr int16_t STATUS_Y = 56;
constexpr int16_t ROW_Y = 112;
constexpr int16_t ROW_HEIGHT = 44;
constexpr int16_t VALUE_X = 204;
constexpr uint16_t BACKGROUND = HX8357_BLACK;

Adafruit_HX8357 tft(TFT_CS, TFT_DC, -1, HX8357D);

void drawValue(int16_t x, int16_t y, int16_t width, const char *text, uint16_t color)
{
  tft.fillRect(x, y, width, ROW_HEIGHT, BACKGROUND);

  uint8_t size = 3;
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  do
  {
    tft.setTextSize(size);
    tft.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
    if (textWidth <= width || size == 1)
    {
      break;
    }
    --size;
  } while (true);

  tft.setTextColor(color, BACKGROUND);
  tft.setCursor(x, y + (ROW_HEIGHT - textHeight) / 2);
  tft.print(text);
}

uint16_t qualityColor(const String &quality)
{
  if (quality == "Danger" || quality == "High Pollution")
  {
    return HX8357_RED;
  }
  if (quality == "Low Pollution")
  {
    return HX8357_YELLOW;
  }
  if (quality == "Fresh Air")
  {
    return HX8357_GREEN;
  }
  return HX8357_WHITE;
}

void drawReading(uint8_t row, const char *value)
{
  drawValue(VALUE_X, ROW_Y + row * ROW_HEIGHT, tft.width() - VALUE_X - MARGIN,
            value, HX8357_WHITE);
}
}

void AqmDisplay::begin()
{
  // The unused SD card must not drive MISO; V2 touch uses I2C, not SPI CS.
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  pinMode(TOUCH_IRQ, INPUT);

  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.setTextWrap(false);
  tft.fillScreen(BACKGROUND);
  tft.setTextColor(HX8357_CYAN, BACKGROUND);
  tft.setTextSize(3);
  tft.setCursor(MARGIN, 12);
  tft.print("Particle Air Quality");
  tft.drawFastHLine(MARGIN, 48, tft.width() - 2 * MARGIN, HX8357_CYAN);

  const char *labels[] = {"Temperature", "Humidity", "Pressure", "Dust"};
  tft.setTextColor(HX8357_WHITE, BACKGROUND);
  tft.setTextSize(2);
  for (uint8_t row = 0; row < 4; ++row)
  {
    tft.setCursor(MARGIN, ROW_Y + row * ROW_HEIGHT + 14);
    tft.print(labels[row]);
  }

  drawValue(MARGIN, STATUS_Y, tft.width() - 2 * MARGIN, "Waiting for readings", HX8357_WHITE);
  drawReading(0, "-- F");
  drawReading(1, "-- %");
  drawReading(2, "-- hPa");
  drawReading(3, "-- pcs/L");
}

void AqmDisplay::update(int temp, int humidity, int pressure, const String &airQuality,
                        float concentration)
{
  drawValue(MARGIN, STATUS_Y, tft.width() - 2 * MARGIN, airQuality.c_str(),
            qualityColor(airQuality));

  char value[32];
  snprintf(value, sizeof(value), "%d F", temp);
  drawReading(0, value);
  snprintf(value, sizeof(value), "%d %%", humidity);
  drawReading(1, value);
  snprintf(value, sizeof(value), "%d hPa", pressure);
  drawReading(2, value);

  if (isfinite(concentration) && concentration > 1)
  {
    // Keep whole particles; scientific notation keeps large values on screen.
    snprintf(value, sizeof(value), "%.6g pcs/L", static_cast<double>(floorf(concentration)));
  }
  else
  {
    snprintf(value, sizeof(value), "-- pcs/L");
  }
  drawReading(3, value);
}
