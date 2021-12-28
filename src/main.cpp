#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266AutoIOT.h> // https://github.com/RobertMcReed/ESP8266AutoIOT.
#include "DHT.h"
#include "html.h"

#define DHTPIN D3

// DHTTYPE is used by the Adafruit sensor library and must not be renamed!
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

uint32_t delayMS = 2000;
unsigned long lastReading = millis();

float lastTemp = 0;
float lastHumidity = 0;
float lastHeatIndex = 0;

// __SSID__ and __PW__ are injected from dotenv.py
ESP8266AutoIOT app((char*)__SSID__, (char*)__PW__);

String jsonReading(float t, float h, float hi) {
  DynamicJsonDocument jDoc(256);
  // temperature
  jDoc["t"] = t;

  // humidity
  jDoc["h"] = h;

  // heat index
  jDoc["hi"] = hi;

  char jChar[128];

  serializeJson(jDoc, jChar);

  return jChar;
}

String getStatus() {
  return jsonReading(lastTemp, lastHumidity, lastHeatIndex);
}

void setupWifi() {
  app.disableLED();
  app.root(HTML);
  app.get("/status", getStatus);
  // if you need to reset the wifi config, uncomment below
  // app.resetAllSettings();
  app.begin();
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
  if (now - lastReading < delayMS) { return; }

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
  
  lastReading = now;
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

void setup() {
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(1000);
  setupWifi();
  setupSensor();
}

void loop() {
  sensorLoop();
  app.loop();
}
