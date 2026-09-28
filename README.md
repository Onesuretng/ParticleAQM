# ParticleAQM

## Overview

ParticleAQM is an embedded air quality monitoring application for the Particle Argon. It collects environmental data from multiple sensors, displays values on an Adafruit 3.5" 480x320 TFT FeatherWing V2, and publishes JSON events to the Particle Cloud.
This project is based on https://github.com/particle-iot/air-quality-kit but updated to use modern libraries and coding practices. 

Key components and sensors
- `Air_Quality_Sensor` (analog): measures general air quality / pollution level.
- `Adafruit_BME280`: provides temperature, humidity, and barometric pressure readings.
- Dust/particulate sensor (digital pulse input): computes particle concentration using low-pulse occupancy.
- `Adafruit_HX8357_RK`: Particle-compatible HX8357 driver for the TFT FeatherWing.

Features
- Periodic sensor sampling (configurable interval via `SENSOR_READING_INTERVAL`).
- Local display of air quality, temperature, humidity, pressure, and dust concentration.
- JSON event publishing to the Particle Cloud using `Particle.publish`.
- Sensor sampling and cloud publishing in `src/AQM.cpp`, with display rendering isolated in `src/AqmDisplay.cpp`.

## FeatherWing V2 connections

The Argon plugs directly into the FeatherWing socket. **The HX8357 TFT uses hardware
SPI through those headers, not I2C.** Connect the FeatherWing STEMMA QT connector
to the Particle/Grove sensor board's `I2C_1` connector using a correctly wired
adapter: SDA to SDA, SCL to SCL, 3.3V to 3.3V, and GND to GND. Check the adapter
pinout rather than assuming Grove and STEMMA QT connector pin orders match.
Do not feed 5V into STEMMA QT or Argon GPIO.

The connector label `I2C_1` does **not** select the Device OS `Wire1` peripheral.
This setup uses the existing `Wire` bus on Argon D0 (SDA) and D1 (SCL).
The BME280 remains on that bus, with its existing library/address configuration.
The V2 touchscreen controller is a TSC2007 at I2C address `0x48`; touch interaction
is not used by this application. Do not initialize the older V1 STMPE610
touchscreen driver.

| Function | Argon pin / connection |
| --- | --- |
| Air quality sensor | A2, unchanged |
| Dust sensor pulse input | D4, unchanged |
| BME280 and STEMMA QT | `Wire`: D0/SDA, D1/SCL |
| TFT SPI | Dedicated SCK, MOSI/MO, MISO/MI header pins |
| TFT data/command (DC) | D5 (Feather pin 10), default jumper |
| TFT chip-select (TCS) | **D6 (Feather pin 11), requires jumper modification below** |
| microSD chip-select (SCS) | D2 (Feather pin 5), held HIGH; SD is unused |
| V2 touch interrupt (IRQ) | D3 (Feather pin 6), left as an input |
| TFT reset / backlight | Software reset; stock always-on backlight, no extra GPIO |

### Required chip-select change

**An unmodified FeatherWing cannot share D4 with the existing dust sensor.**
Its default TFT TCS jumper connects Feather pin 9, which is **Argon D4**, not D9.
Changing a constant in firmware alone cannot disconnect that trace.

With power disconnected, cut the FeatherWing's **TCS** jumper and connect its
signal pad (the pad nearest the TCS label) to **Argon D6 / Feather pin 11**.
Leave DC, SCS, IRQ, and all sensor connectors unchanged. Check that TCS is
disconnected from D4 and connected to D6 before powering up. The firmware's
`TFT_CS = D6` assumes this modification has been made.

**STEMMA QT carries only power and I2C.** If moving the Argon into the display
socket removed the sensor board's header connection, an I2C cable alone will not
connect the A2 air-quality sensor or D4 dust sensor. Preserve those signals with
stacking headers/a FeatherWing expansion board or separate A2 and D4 wiring,
along with the sensors' required power and common ground. A sensor that required
5V on the original board still needs that supply; STEMMA QT supplies only 3.3V.
No software change can read those sensors through a passive I2C connector.

## Display behavior

The display starts in **480x320 landscape** (`setRotation(1)`) with a title,
status message, and placeholders until the first normal 60-second sample.
It shows air quality, temperature in Fahrenheit, humidity in percent, pressure
in hPa, and dust concentration in pcs/L. Air-quality text is color-coded but
retains its full label. Large values use smaller text; large dust concentrations
use scientific notation. Dust at or below the existing threshold of 1 pcs/L,
or a non-finite concentration, shows `-- pcs/L`.

Only the status and value rectangles are cleared on refresh, so shorter readings
cannot leave stale digits or dust values behind. No full-screen framebuffer is
allocated. Sensor pins, sampling interval, dust calculations, and the
`AQM-Values` event name/JSON fields remain unchanged.

## Build and hardware check

The dependency versions are pinned in `project.properties`. Restore them using
Particle Workbench or `particle library copy` for a local build. The HX8357
package includes GFX and a legacy STMPE610 dependency; the latter is not
initialized or used for V2 touch.

With an authenticated Particle CLI, build for Argon (Device OS 6.2.1):

```powershell
New-Item -ItemType Directory -Force target | Out-Null
particle compile argon . --target 6.2.1 --saveTo target\featherwing-feature.bin
```

Alternatively, configure Particle Workbench for Argon and Device OS 6.2.1, restore
the libraries, and run **Particle: Compile application (local)** to build without
the cloud compiler.

Do not flash the old `argon_firmware_1750451115169.bin`; it predates this display
change. Build a new binary. After checking the wiring above, flash that binary
using Particle Workbench or the CLI. With the existing automatic cloud mode,
application startup may wait for the Particle connection.

On the physical device, confirm the title and all four rows fit the landscape
screen, wait for the first sample, and compare the readings with USB serial
output and `AQM-Values`. Confirm the dust pulse input still works on D4 and that
shorter values erase cleanly. A lit but blank TFT warrants checking TCS-to-D6,
DC-to-D5, and the SPI header seating, not switching to `Wire1`.

Hardware references:
- [Adafruit FeatherWing V2 pinouts](https://learn.adafruit.com/adafruit-3-5-tft-featherwing/pinouts-v2)
- [Particle Argon datasheet](https://docs.particle.io/reference/datasheets/wi-fi/argon-datasheet/)
- [Particle HX8357 port and Feather-to-Argon pin mapping](https://github.com/rickkas7/Adafruit_HX8357_RK)

## Display regression tests

The native tests compile the real display module against recording Particle/GFX
stubs. They check 480x320 bounds, startup, pin assignments, text/units/colors,
large numbers, and complete clearing when readings shrink or dust disappears.
They do not replace an Argon firmware build or physical SPI/display checks.

Run from the repository root with a C++11 compiler (for example MinGW g++ on
Windows; `clang++` or `zig c++` can be substituted):

```powershell
New-Item -ItemType Directory -Force target | Out-Null
g++ -std=c++11 -Wall -Wextra -Werror -I tests\stubs -I src tests\display_test.cpp src\AqmDisplay.cpp -o target\display-test.exe
if ($LASTEXITCODE -ne 0) { throw "Display test build failed" }
.\target\display-test.exe
if ($LASTEXITCODE -ne 0) { throw "Display tests failed" }
```