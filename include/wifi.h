#include <Arduino.h>
#include <ESP8266AutoIOT.h> // https://github.com/RobertMcReed/ESP8266AutoIOT.
#include "html.h"
#include "sensor.h"

#ifndef AUTO_WIFI_h
#define AUTO_WIFI_h

void setupWifi();
void wifiLoop();

#endif
