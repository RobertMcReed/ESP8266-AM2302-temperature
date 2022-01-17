#include <Arduino.h>
#include "mqtt.h"
#include "wifi.h"
#include "sensor.h"

void setup() {
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(500);
  setupWifi();
  delay(50);
  setupSensor();
  delay(50);
  setupMQTT();
  delay(50);
}

void loop() {
  sensorLoop();
  mqttLoop();
  wifiLoop();
}
