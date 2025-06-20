//Configuration file for Azure IoT Hub integration
// Copy azure_config_template.h to azure_config.h and update with your values

#ifndef AZURE_CONFIG_H
#define AZURE_CONFIG_H

// Include certificate definitions when using X.509 authentication
#include "azure_certs.h"

// Azure IoT Hub Configuration
// Replace these values with your actual Azure IoT Hub settings
const String AZURE_IOT_HUB_NAME = "AQM-IOTHub1";           // Just the hub name, not the full URL
const String DEVICE_ID = "Argon-AQM";                      // Your device ID
const String AZURE_IOT_HUB_HOST = AZURE_IOT_HUB_NAME + ".azure-devices.net";

// Authentication Configuration
// Choose ONE authentication method by setting the corresponding flag to true

// Method 1: Symmetric Key Authentication (for device keys - least secure)
const bool USE_SYMMETRIC_KEY_AUTH = false;
const String DEVICE_KEY = "your-device-primary-key";       // Only used if USE_SYMMETRIC_KEY_AUTH = true

// Method 2: X.509 Certificate Authentication (more secure)
const bool USE_X509_AUTH = true;  // Set to true to use X.509 certificates
const String DEVICE_CERT_THUMBPRINT = "your-cert-thumbprint-from-iot-hub";  // Certificate thumbprint from IoT Hub

// X.509 Certificate Configuration
// The certificate and key content are embedded in azure_certs.h
// The certificate must be uploaded to Azure IoT Hub device settings
// To get the thumbprint: Azure Portal > IoT Hub > Device > Authentication Type > X.509 Self-Signed

// Method 3: Connection String Authentication (includes SAS token)
const bool USE_CONNECTION_STRING_AUTH = false;  // Disabled when using X.509
const String DEVICE_CONNECTION_STRING = "HostName=AQM-IOTHub1.azure-devices.net;DeviceId=Argon-AQM;SharedAccessKey=your-key-here";

// Method 4: Azure IoT Hub Device Provisioning Service (DPS) - Most secure for production
const bool USE_DPS_AUTH = false;
const String DPS_ENDPOINT = "global.azure-devices-provisioning.net";
const String DPS_SCOPE_ID = "your-dps-scope-id";
const String DPS_REGISTRATION_ID = "your-registration-id";

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

#endif // AZURE_CONFIG_H
