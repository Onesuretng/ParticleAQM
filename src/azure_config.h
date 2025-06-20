#ifndef AZURE_CONFIG_H
#define AZURE_CONFIG_H

// Include certificate definitions when using X.509 authentication
#include "azure_certs.h"

// Azure IoT Hub Configuration
// Replace these values with your actual Azure IoT Hub settings
const String AZURE_IOT_HUB_NAME = "AQM-IOTHub1";           // Just the hub name, not the full URL
const String DEVICE_ID = "e00fce685b4e3315eed6afd5";                      // Your device ID
const String AZURE_IOT_HUB_HOST = AZURE_IOT_HUB_NAME + ".azure-devices.net";

// Method 2: X.509 Certificate Authentication (more secure)
const bool USE_X509_AUTH = true;  // Set to true to use X.509 certificates

// X.509 Certificate Configuration
// The certificate and key content are embedded in azure_certs.h
// The certificate must be uploaded to Azure IoT Hub device settings
// To get the thumbprint: Azure Portal > IoT Hub > Device > Authentication Type > X.509 Self-Signed

// Disable other auth methods when using X.509
const bool USE_SYMMETRIC_KEY_AUTH = false;
const bool USE_CONNECTION_STRING_AUTH = false;
const bool USE_DPS_AUTH = false;

// Token Management
const int TOKEN_EXPIRY_HOURS = 1;                          // How long tokens are valid
const bool AUTO_REFRESH_TOKENS = true;                     // Automatically refresh tokens before expiry

// Optional: Advanced Configuration
const int MAX_RETRY_ATTEMPTS = 3;
const int RETRY_DELAY_MS = 2000;
const int HTTP_TIMEOUT_MS = 30000;

// Device Metadata
const String DEVICE_TYPE = "ParticleAQM";
const String SENSOR_VERSION = "1.0";
const String DEVICE_LOCATION = "Indoor";  // Customize as needed

// Feature Flags
const bool ENABLE_AZURE_IOT = true;       // Set to false to disable Azure IoT Hub
const bool ENABLE_PARTICLE_CLOUD = true; // Set to false to disable Particle Cloud
const bool ENABLE_DEBUG_LOGS = true;     // Set to false to reduce serial output
const bool ENABLE_SECURE_AUTH = true;    // Enable enhanced security features

#endif