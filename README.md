# ParticleAQM

## Overview

ParticleAQM is an embedded air quality monitoring application for Particle devices. It collects environmental data from multiple sensors, displays values on an onboard OLED, publishes JSON events to the Particle Cloud.
This project is based on https://github.com/particle-iot/air-quality-kit but updated to use modern libraries and coding practices. 

Key components and sensors
- `Air_Quality_Sensor` (analog): measures general air quality / pollution level.
- `Adafruit_BME280`: provides temperature, humidity, and barometric pressure readings.
- Dust/particulate sensor (digital pulse input): computes particle concentration using low-pulse occupancy.
- `SeeedOLED`: small OLED used to display current readings locally.

Features
- Periodic sensor sampling (configurable interval via `SENSOR_READING_INTERVAL`).
- Local display of air quality, temperature, humidity, pressure, and dust concentration.
- JSON event publishing to the Particle Cloud using `Particle.publish`.
- Modular code structure in `src/AQM.cpp` for easy extension and sensor replacement.