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
- Optional Azure IoT Hub payload creation for integration with cloud telemetry (toggle via `ENABLE_AZURE_IOT` and configure `azure_config.h`).
- Modular code structure in `src/AQM.cpp` for easy extension and sensor replacement.

Configuration & build notes
- Review and set device metadata constants in `azure_config.h` (for Azure payloads): `DEVICE_ID`, `DEVICE_TYPE`, `SENSOR_VERSION`, `DEVICE_LOCATION`, and `ENABLE_AZURE_IOT`.
- TLS certificates and related Azure settings are stored in `azure_certs.h`.
- Build and flash using Particle Workbench (VS Code) or the Particle CLI.

Where to look
- Primary firmware implementation: `src/AQM.cpp` (sensor reads, display updates, payload creation).
- Azure integration helpers: `azure_config.h`, `azure_certs.h`.

License and contribution
- See repository root for license and contribution guidelines. If missing, add `CONTRIBUTING.md` and `.editorconfig` per project standards

