#include <Arduino.h>
#include "DHT.h"
#include "json.h"

#ifndef SENSOR_DHT_h
#define SENSOR_DHT_h

String getLatestReadingAsJson();
void setupSensor();
void sensorLoop();

#endif
