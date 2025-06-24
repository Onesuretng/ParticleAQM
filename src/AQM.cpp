/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include <math.h>
#include "Air_Quality_Sensor.h"
#include "Adafruit_BME280.h"
#include "SeeedOLED.h"
#include "JsonParserGeneratorRK.h"
#include "azure_config.h"
#include "azure_certs.h"
#include "MQTT.h" 


// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
//SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

#define AQS_PIN A2
#define DUST_SENSOR_PIN D4
#define SENSOR_READING_INTERVAL 60000 //update every 60 seconds

/**
 * Instance of AirQualitySensor used to measure air quality parameters.
 *This sensor object interfaces with the hardware connected to the specified pin (AQS_PIN)
 * and provides methods to read air quality data such as pollutant concentration, air quality index,
 * or other relevant metrics supported by the AirQualitySensor class.
 */
AirQualitySensor aqSensor(AQS_PIN);

/**
 * Instance of Adafruit_BME280 sensor object.
 * This object provides access to temperature, humidity, pressure, and altitude measurements from the BME280 sensor. 
 * Ensure the sensor is properly initialized by calling the `begin()` method before attempting to read sensor data.
 */
Adafruit_BME280 bme;

unsigned long lastInterval; // Last time we updated the sensor readings
unsigned long lowpulseoccupancy = 0; //Accumulates the duration of low pulses from the dust sensor
unsigned long last_lpo = 0; //Remembers the last non-zero low pulse occupancy value (for filtering)
unsigned long duration; // Holds the duration of the most recent pulse
float ratio = 0; //Calculated dust ratio based on occupancy and interval
float concentration = 0; //Calculated dust concentration from the ratio
MQTT mqttClient;
bool mqttConnected = false;

int getBMEValues(int &temp, int &humidity, int &pressure);
void getDustSensorReadings();
String getAirQuality();
void createEventPayload(int temp, int humidity, int pressure, String airQuality);
void updateDisplay(int temp, int humidity, int pressure, String airQuality);
String createAzurePayload(int temp, int humidity, int pressure, String airQuality);
bool sendToAzureIoTHub(String payload);
void sendBothPayloads(int temp, int humidity, int pressure, String airQuality);


void setup()
{
  Serial.begin(9600);
  delay(50);

  // Configure the dust sensor pin as an input
  pinMode(DUST_SENSOR_PIN, INPUT);

  if (aqSensor.init())
  {
    Serial.println("Air Quality Sensor ready.");
  }
  else
  {
    Serial.println("Air Quality Sensor ERROR!");
  }

  Wire.begin();
  SeeedOled.init();

  SeeedOled.clearDisplay();
  SeeedOled.setNormalDisplay();
  SeeedOled.setPageMode();

  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("Particle");
  SeeedOled.setTextXY(3, 0);
  SeeedOled.putString("Air Quality");
  SeeedOled.setTextXY(4, 0);
  SeeedOled.putString("Monitor");

  if (bme.begin())
  {
    Serial.println("BME280 Sensor ready.");
  }
  else
  {
    Serial.println("BME280 Sensor ERROR!");
  }
}

void loop()
{
  int temp, pressure, humidity;

  duration = pulseIn(DUST_SENSOR_PIN, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration;

  if ((millis() - lastInterval) > SENSOR_READING_INTERVAL)
  {
    String quality = getAirQuality();
    Serial.printlnf("Air Quality: %s", quality.c_str());

    getBMEValues(temp, pressure, humidity);
    
    Serial.printlnf("Temp: %d", temp);
    Serial.printlnf("Pressure: %d", pressure);
    Serial.printlnf("Humidity: %d", humidity);

    getDustSensorReadings();

    updateDisplay(temp, humidity, pressure, quality);

    createEventPayload(temp, humidity, pressure, quality);

    // Send to Azure IoT Hub
    if (ENABLE_AZURE_IOT) {
      String azurePayload = createAzurePayload(temp, humidity, pressure, quality);
      sendToAzureIoTHub(azurePayload);
    }

    lowpulseoccupancy = 0;
    lastInterval = millis();
  }
}

String getAirQuality()
{
  int quality = aqSensor.slope();
  String qual = "None";

  if (quality == AirQualitySensor::FORCE_SIGNAL)
  {
    qual = "Danger";
  }
  else if (quality == AirQualitySensor::HIGH_POLLUTION)
  {
    qual = "High Pollution";
  }
  else if (quality == AirQualitySensor::LOW_POLLUTION)
  {
    qual = "Low Pollution";
  }
  else if (quality == AirQualitySensor::FRESH_AIR)
  {
    qual = "Fresh Air";
  }

  return qual;
}

int getBMEValues(int &temp, int &pressure, int &humidity)
{
  temp = (int)(bme.readTemperature() * 9 / 5 + 32); //convert to Fahrenheit from Celsius
  //temp = (int)bme.readTemperature(); // Uncomment this line if you want Celsius instead
  
  // Note: Pressure is returned in Pascals, so we divide by 100 to convert to hPa
  // This is a common unit for atmospheric pressure.
  // If you want to keep it in Pascals, you can remove the division by 100.
  pressure = (int)(bme.readPressure() / 100.0F);
  humidity = (int)bme.readHumidity();
  return 1;
}

void getDustSensorReadings()
{
  // This particular dust sensor returns 0s often, so let's filter them out by making sure we only
  // capture and use non-zero LPO values for our calculations once we get a good reading.
  if (lowpulseoccupancy == 0)
  {
    lowpulseoccupancy = last_lpo;
  }
  else
  {
    last_lpo = lowpulseoccupancy;
  }

  ratio = lowpulseoccupancy / (SENSOR_READING_INTERVAL * 10.0);                   // Integer percentage 0=>100
  concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve

  Serial.printlnf("LPO: %lu", lowpulseoccupancy);
  Serial.printlnf("Ratio: %f%%", ratio);
  Serial.printlnf("Concentration: %f pcs/L", concentration);
}

void createEventPayload(int temp, int humidity, int pressure, String airQuality)
{
  JsonWriterStatic<256> jw;
  {
    JsonWriterAutoObject obj(&jw);

    jw.insertKeyValue("Temp", temp);
    jw.insertKeyValue("Humidity", humidity);
    jw.insertKeyValue("Pressure", pressure);
    jw.insertKeyValue("AirQuality", airQuality);

    if (lowpulseoccupancy > 0)
    {
      jw.insertKeyValue("DustLpo", lowpulseoccupancy);
      jw.insertKeyValue("DustRatio", ratio);
      jw.insertKeyValue("DustConcentration", concentration);
    }
  }
  
  Particle.publish("AQM-Values", jw.getBuffer(), PRIVATE);
}

void updateDisplay(int temp, int humidity, int pressure, String airQuality)
{
  SeeedOled.clearDisplay();

  SeeedOled.setTextXY(0, 3);
  SeeedOled.putString(airQuality);

  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("Temp: ");
  SeeedOled.putNumber(temp);  //SeeedOled.putNumber((int)(temp * 9 / 5 + 32)); converted to Fahrenheit in getBMEValues
  SeeedOled.putString("F"); //SeeedOled.putString("C");
  
  SeeedOled.setTextXY(3, 0);
  SeeedOled.putString("Humidity: ");
  SeeedOled.putNumber(humidity);
  SeeedOled.putString("%");

  SeeedOled.setTextXY(4, 0);
  SeeedOled.putString("Press: ");
  SeeedOled.putNumber(pressure);
  SeeedOled.putString(" hPa");

  if (concentration > 1)
  {
    SeeedOled.setTextXY(5, 0);
    SeeedOled.putString("Dust: ");
    SeeedOled.putNumber(concentration); // Will cast our float to an int to make it more compact
    SeeedOled.putString(" pcs/L");
  }
}

String createAzurePayload(int temp, int humidity, int pressure, String airQuality)
{
  JsonWriterStatic<512> jw;  // Larger buffer for Azure metadata
  {
    JsonWriterAutoObject obj(&jw);
    
    // Device metadata
    jw.insertKeyValue("deviceId", DEVICE_ID);
    jw.insertKeyValue("deviceType", DEVICE_TYPE);
    jw.insertKeyValue("timestamp", Time.format(TIME_FORMAT_ISO8601_FULL));
    
    // Sensor data (same as Particle payload)
    jw.insertKeyValue("temperature", temp);
    jw.insertKeyValue("humidity", humidity);
    jw.insertKeyValue("pressure", pressure);
    jw.insertKeyValue("airQuality", airQuality);
    
    if (lowpulseoccupancy > 0)
    {
      jw.insertKeyValue("dustLpo", lowpulseoccupancy);
      jw.insertKeyValue("dustRatio", ratio);
      jw.insertKeyValue("dustConcentration", concentration);
    }
    
    // Additional metadata for Azure
    jw.insertKeyValue("sensorVersion", SENSOR_VERSION);
    jw.insertKeyValue("location", DEVICE_LOCATION);
  }
  
  return String(jw.getBuffer());
}

bool sendToAzureIoTHub(String payload)
{
  if (!USE_X509_AUTH || !ENABLE_AZURE_IOT) {
    Serial.println("Azure IoT Hub disabled or not configured");
    return false;
  }
  
  // Configure MQTT client with X.509 certificates
  if (!mqttConnected) {
    // Use enableTls with the certificate chain for Particle MQTT library
    //mqttClient.enableTls(AZURE_IOT_CLIENT_CERT, AZURE_IOT_CLIENT_KEY, AZURE_IOT_ROOT_CA);
    mqttclient.enableTlsVerification(true); // Enable TLS verification
    
    String mqttServer = String(AZURE_IOT_HUB_HOST);
    String clientId = String(DEVICE_ID);
    String username = mqttServer + "/" + clientId + "/?api-version=2021-04-12";
    
    // Connect with X.509 authentication (no password needed for cert auth)
    if (mqttClient.connect(mqttServer, 8883, clientId, username, "")) {
      mqttConnected = true;
      if (ENABLE_DEBUG_LOGS) {
        Serial.println("Connected to Azure IoT Hub via MQTT");
      }
    } else {
      if (ENABLE_DEBUG_LOGS) {
        Serial.println("Failed to connect to Azure IoT Hub via MQTT");
      }
      return false;
    }
  }
  
  // Check if still connected
  if (!mqttClient.isConnected()) {
    mqttConnected = false;
    if (ENABLE_DEBUG_LOGS) {
      Serial.println("MQTT connection lost, attempting reconnect...");
    }
    return sendToAzureIoTHub(payload); // Recursive call to reconnect
  }
  
  // Publish telemetry data
  String topic = "devices/" + String(DEVICE_ID) + "/messages/events/";
  bool success = mqttClient.publish(topic, payload);
  
  if (ENABLE_DEBUG_LOGS) {
    Serial.printlnf("Azure IoT Hub MQTT publish %s", success ? "successful" : "failed");
  }
  
  return success;
}