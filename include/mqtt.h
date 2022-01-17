#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "sensor.h"

#ifndef MQTT_HA_h
#define MQTT_HA_h

void setupMQTT();
void mqttLoop();

#endif
