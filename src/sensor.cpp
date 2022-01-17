#include "sensor.h"

// DHTTYPE is used by the Adafruit sensor library and MUST NOT be renamed!
#define DHTTYPE DHT22
#define DHTPIN D3

DHT dht(DHTPIN, DHTTYPE);

uint32_t SENSOR_READ_DELAY = 2000;
unsigned long lastReading = millis();

float lastTemp = 0;
float lastHumidity = 0;
float lastHeatIndex = 0;

boolean isValidReading() {
  if (lastTemp == 0) {
    return false;
  } else if (lastHumidity == 0) {
    return false;
  } else if (lastHeatIndex == 0) {
    return false;
  }

  return true;
}

String getLatestReadingAsJson() {
  return jsonReading(lastTemp, lastHumidity, lastHeatIndex);
}

void setupSensor() {
  pinMode(DHTPIN, INPUT);
  delay(100);
  dht.begin();
  Serial.println(F("[INFO] DHT22 Unified Sensor Initialized!"));
}

void sensorLoop() {
  unsigned long now = millis();

  // the sensor updates every 2 seconds
  if (now - lastReading < SENSOR_READ_DELAY) { return; }

  lastReading = now;
  bool useFahrenheit = true;
  float h = dht.readHumidity();
  float t = dht.readTemperature(useFahrenheit);

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));

    if (isnan(h)) { lastHumidity = 0; }
    if (isnan(t)) { lastTemp = 0; }
    lastHeatIndex = 0;

    return;
  }
  
  float hi = dht.computeHeatIndex(t, h, useFahrenheit);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println(F("%"));
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F("°F"));
  Serial.print(F("Heat Index: "));
  Serial.print(hi);
  Serial.println(F("°F"));
  Serial.println();

  lastTemp = t;
  lastHumidity = h;
  lastHeatIndex = hi;
}
