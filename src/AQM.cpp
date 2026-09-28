// Include Particle Device OS APIs
#include "Particle.h"
#include <math.h>
#include "Air_Quality_Sensor.h"
#include "Adafruit_BME280.h"
#include "AqmDisplay.h"
#include "JsonParserGeneratorRK.h"

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


int getBMEValues(int &temp, int &pressure, int &humidity);
void getDustSensorReadings();
String getAirQuality();
void createEventPayload(int temp, int humidity, int pressure, String airQuality);

void setup()
{
  Serial.begin(9600);
  delay(50);

  // Configure the dust sensor pin as an input
  pinMode(DUST_SENSOR_PIN, INPUT);

  Wire.begin();
  AqmDisplay::begin();

  if (aqSensor.init())
  {
    Serial.println("Air Quality Sensor ready.");
  }
  else
  {
    Serial.println("Air Quality Sensor ERROR!");
  }

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

    AqmDisplay::update(temp, humidity, pressure, quality, concentration);

    createEventPayload(temp, humidity, pressure, quality);

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
