#include "wifi.h"

ESP8266AutoIOT app((char*)__SSID__, (char*)__PW__);

void setupWifi() {
  app.disableLED();
  app.root(HTML);
  app.get("/status", getLatestReadingAsJson);
  // if you need to reset the wifi config, uncomment below
  // app.resetAllSettings();
  app.begin();
}

void wifiLoop() {
  app.loop();
}
