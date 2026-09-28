#pragma once

#include "Particle.h"
#include <cstring>
#include <stdexcept>
#include <vector>

constexpr uint8_t HX8357D = 0xD;
constexpr uint16_t HX8357_BLACK = 0x0000;
constexpr uint16_t HX8357_RED = 0xF800;
constexpr uint16_t HX8357_GREEN = 0x07E0;
constexpr uint16_t HX8357_CYAN = 0x07FF;
constexpr uint16_t HX8357_YELLOW = 0xFFE0;
constexpr uint16_t HX8357_WHITE = 0xFFFF;

class Adafruit_HX8357
{
public:
  struct Rect
  {
    int16_t x, y, width, height;
  };

  struct Text
  {
    std::string value;
    Rect bounds;
    uint16_t color;
    uint8_t size;
  };

  static Adafruit_HX8357 *instance;
  int8_t cs, dc, reset;
  uint8_t type, rotation = 0;
  uint32_t beginArgument = 0;
  bool wrap = true;
  unsigned screenClears = 0;
  std::vector<Rect> clears;
  std::vector<Text> texts;

  Adafruit_HX8357(int8_t cs, int8_t dc, int8_t reset, uint8_t type)
      : cs(cs), dc(dc), reset(reset), type(type)
  {
    instance = this;
  }

  void begin(uint32_t argument)
  {
    if (TestPins::modes.at(D2) != OUTPUT || TestPins::levels.at(D2) != HIGH)
    {
      throw std::runtime_error("SD must be deselected before starting SPI");
    }
    beginArgument = argument;
  }

  void setRotation(uint8_t value) { rotation = value; }
  int16_t width() const { return rotation % 2 ? 480 : 320; }
  int16_t height() const { return rotation % 2 ? 320 : 480; }
  void setTextWrap(bool value) { wrap = value; }
  void setTextSize(uint8_t value) { textSize = value; }
  void setTextColor(uint16_t foreground, uint16_t) { color = foreground; }
  void setCursor(int16_t x, int16_t y) { cursorX = x; cursorY = y; }
  void fillScreen(uint16_t) { ++screenClears; }

  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t)
  {
    checkBounds({x, y, w, h});
    clears.push_back({x, y, w, h});
  }

  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t)
  {
    checkBounds({x, y, w, 1});
  }

  void getTextBounds(const char *text, int16_t x, int16_t y, int16_t *x1,
                     int16_t *y1, uint16_t *w, uint16_t *h)
  {
    // Adafruit GFX's built-in font uses a 6x8 cell at text size 1.
    *x1 = x;
    *y1 = y;
    *w = static_cast<uint16_t>(std::strlen(text) * 6 * textSize);
    *h = 8 * textSize;
  }

  void print(const char *text)
  {
    Rect bounds{cursorX, cursorY, static_cast<int16_t>(std::strlen(text) * 6 * textSize),
                static_cast<int16_t>(8 * textSize)};
    checkBounds(bounds);
    texts.push_back({text, bounds, color, textSize});
  }

  void clearRecorded()
  {
    clears.clear();
    texts.clear();
  }

private:
  int16_t cursorX = 0, cursorY = 0;
  uint8_t textSize = 1;
  uint16_t color = HX8357_WHITE;

  void checkBounds(const Rect &rect) const
  {
    if (rect.x < 0 || rect.y < 0 || rect.width <= 0 || rect.height <= 0 ||
        rect.x + rect.width > width() || rect.y + rect.height > height())
    {
      throw std::runtime_error("Drawing exceeds display bounds");
    }
  }
};
