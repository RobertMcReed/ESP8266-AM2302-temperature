#include <ArduinoJson.h>

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
