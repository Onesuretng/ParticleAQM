#include "AqmDisplay.h"
#include "Adafruit_HX8357_RK.h"
#include <climits>
#include <iostream>
#include <limits>

std::map<int, int> TestPins::modes;
std::map<int, int> TestPins::levels;
Adafruit_HX8357 *Adafruit_HX8357::instance = nullptr;

void require(bool condition, const char *message)
{
  if (!condition)
  {
    throw std::runtime_error(message);
  }
}

bool contains(const Adafruit_HX8357::Rect &outer, const Adafruit_HX8357::Rect &inner)
{
  return inner.x >= outer.x && inner.y >= outer.y &&
         inner.x + inner.width <= outer.x + outer.width &&
         inner.y + inner.height <= outer.y + outer.height;
}

void checkRefresh(const Adafruit_HX8357 &tft)
{
  require(tft.screenClears == 1, "Updates must not clear the entire screen");
  require(tft.texts.size() == 5 && tft.clears.size() == 5, "Refresh must replace all five values");
  for (size_t i = 0; i < tft.texts.size(); ++i)
  {
    require(contains(tft.clears[i], tft.texts[i].bounds), "Text must fit its cleared value rectangle");
    require(tft.clears[i].y >= 56, "Refresh must preserve the title");
    if (i > 0)
    {
      require(tft.clears[i].x >= 204, "Refresh must preserve the sensor labels");
      const auto &previous = tft.clears[i - 1];
      require(tft.clears[i].y >= previous.y + previous.height, "Value rows must not overlap");
    }
  }
}

int main()
{
  try
  {
    auto &tft = *Adafruit_HX8357::instance;
    require(tft.cs == D6 && tft.dc == D5, "Incorrect Argon TFT pin mapping");
    require(tft.reset == -1 && tft.type == HX8357D, "Incorrect reset or controller");
    AqmDisplay::begin();
    require(tft.beginArgument == HX8357D, "HX8357D initialization is required");
    require(tft.width() == 480 && tft.height() == 320, "Display must be landscape");
    require(!tft.wrap, "Display must disable text wrapping");
    require(TestPins::modes.at(D3) == INPUT, "V2 touch IRQ must remain an input");
    require(TestPins::levels.count(D3) == 0, "V2 touch IRQ must not be driven as V1 CS");
    require(TestPins::modes.count(D4) == 0 && TestPins::levels.count(D4) == 0,
            "Display must not configure or drive the dust sensor pin");
    require(TestPins::modes.count(A2) == 0, "Display must not configure the air-quality pin");
    require(tft.texts.size() == 10, "Startup must render title, labels, status and placeholders");
    require(tft.texts[0].value == "Particle Air Quality", "Startup title missing");
    require(tft.texts[5].value == "Waiting for readings", "Startup should not display fake readings");
    require(tft.texts[9].value == "-- pcs/L", "Startup dust placeholder missing");

    tft.clearRecorded();
    AqmDisplay::update(-12, 45, 1013, "High Pollution", 12345.9f);
    checkRefresh(tft);
    require(tft.texts[0].value == "High Pollution", "Long quality label truncated");
    require(tft.texts[1].value == "-12 F", "Temperature units or sign incorrect");
    require(tft.texts[2].value == "45 %", "Humidity units or ordering incorrect");
    require(tft.texts[3].value == "1013 hPa", "Pressure units or ordering incorrect");
    require(tft.texts[4].value == "12345 pcs/L", "Dust should use whole particles");
    const auto previousClears = tft.clears;

    tft.clearRecorded();
    AqmDisplay::update(7, 1, 999, "None", 0);
    checkRefresh(tft);
    require(tft.texts[4].value == "-- pcs/L", "Absent dust must replace the previous value");
    for (size_t i = 0; i < tft.clears.size(); ++i)
    {
      require(contains(tft.clears[i], previousClears[i]), "Shorter readings must erase the old region");
    }

    const char *qualities[] = {"Danger", "High Pollution", "Low Pollution", "Fresh Air", "None"};
    const uint16_t colors[] = {HX8357_RED, HX8357_RED, HX8357_YELLOW, HX8357_GREEN, HX8357_WHITE};
    for (size_t i = 0; i < 5; ++i)
    {
      tft.clearRecorded();
      AqmDisplay::update(72, 100, 1100, qualities[i], 2);
      checkRefresh(tft);
      require(tft.texts[0].value == qualities[i] && tft.texts[0].color == colors[i],
              "Air quality color or label mismatch");
    }

    const float missingDust[] = {0, 1, -1, std::numeric_limits<float>::infinity(),
                                std::numeric_limits<float>::quiet_NaN()};
    for (float dust : missingDust)
    {
      tft.clearRecorded();
      AqmDisplay::update(72, 50, 1000, "Fresh Air", dust);
      checkRefresh(tft);
      require(tft.texts[4].value == "-- pcs/L", "Invalid or below-threshold dust must show a placeholder");
    }

    tft.clearRecorded();
    AqmDisplay::update(72, 50, 1000, "Fresh Air", 1.9f);
    checkRefresh(tft);
    require(tft.texts[4].value == "1 pcs/L", "Above-threshold dust must retain the existing truncation");

    tft.clearRecorded();
    AqmDisplay::update(INT_MIN, INT_MAX, INT_MIN, "High Pollution", std::numeric_limits<float>::max());
    checkRefresh(tft);
    require(tft.texts[3].size < 3, "Wide numbers must shrink to fit");
    require(tft.texts[4].value.find("e+") != std::string::npos, "Large dust should use scientific notation");

    std::cout << "PASS: landscape, initialization, pins, units, quality labels/colors, "
                 "refresh erasure, thresholds and extreme values\n";
    return 0;
  }
  catch (const std::exception &error)
  {
    std::cerr << "FAIL: " << error.what() << '\n';
    return 1;
  }
}
