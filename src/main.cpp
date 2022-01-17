#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266AutoIOT.h> // https://github.com/RobertMcReed/ESP8266AutoIOT.
#include "DHT.h"
#include "html.h"
#include "json.h"


// DHTTYPE is used by the Adafruit sensor library and MUST NOT be renamed!
#define DHTTYPE DHT22
#define DHTPIN D3

DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);
ESP8266AutoIOT app(__SSID__, __PW__);

uint32_t SENSOR_READ_DELAY = 2000;
uint32_t MQTT_PUBLISH_DELAY = 10000;
uint32_t MQTT_DISCOVERY_DELAY = 300000;
unsigned long lastReading = millis();
unsigned long lastPublish = millis();
unsigned long lastDiscovery = millis();

float lastTemp = 0;
float lastHumidity = 0;
float lastHeatIndex = 0;

const char* client_id = "back_basement_sensor";
const char *temperature_topic = "back_basement/temperature";
const char *humidity_topic = "back_basement/humidity";
const char *heat_index_topic = "back_basement/heat_index";

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

void publishHomeAssistantDiscovery() {
  lastDiscovery = millis();
  Serial.println("Sending Home Assistant Configuration Topics...");
  mqtt_client.publish("homeassistant/sensor/back_basement_temperature/config",
    "{\"dev_cla\":\"temperature\",\"name\":\"Back Basement Temperature\",\"stat_t\":\"homeassistant/sensor/back_basement_temperature/state\",\"unit_of_meas\":\"°F\",\"val_tpl\":\"{{value_json.t}}\"}"
  );
  mqtt_client.publish("homeassistant/sensor/back_basement_humidity/config",
    "{\"dev_cla\":\"humidity\",\"name\":\"Back Basement Humidity\",\"stat_t\":\"homeassistant/sensor/back_basement_humidity/state\",\"unit_of_meas\":\"%\",\"val_tpl\":\"{{value_json.h}}\"}"
  );
  mqtt_client.publish("homeassistant/sensor/back_basement_heat_index/config",
    "{\"dev_cla\":\"temperature\",\"name\":\"Back Basement Heat Index\",\"stat_t\":\"homeassistant/sensor/back_basement_heat_index/state\",\"unit_of_meas\":\"°F\",\"val_tpl\":\"{{value_json.hi}}\"}"
  );
}

void reconnect() {
  while (!mqtt_client.connected()) {
    if (mqtt_client.connect(client_id, __MQTT_USERNAME__, __MQTT_PASSWORD__)) {
      Serial.println("Connected to MQTT broker!");
      publishHomeAssistantDiscovery();
    } else {
      Serial.print("Could not connext to broker: ");
      Serial.println(mqtt_client.state());
      delay(5000);
    }
  }
}

void setupMQTT() {
  mqtt_client.setServer(__MQTT_BROKER__, 1883);
}

void mqttLoop() {
  if (!mqtt_client.connected()) {
    reconnect();
  }
    
  if (!mqtt_client.loop()) {
    mqtt_client.connect(client_id, __MQTT_USERNAME__, __MQTT_PASSWORD__);
  }

  unsigned long now = millis();

  if (now - lastPublish < MQTT_PUBLISH_DELAY) { return; }

  if (now - lastDiscovery > MQTT_DISCOVERY_DELAY) {
    // publish the discovery config every 5 minutes in case HA was down
    publishHomeAssistantDiscovery();
  }

  lastPublish = now;
  String status = getStatus();
  const char *state = status.c_str();
  mqtt_client.publish("homeassistant/sensor/back_basement_temperature/state", state);
  mqtt_client.publish("homeassistant/sensor/back_basement_humidity/state", state);
  mqtt_client.publish("homeassistant/sensor/back_basement_heat_index/state", state);
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

void setup() {
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(1000);
  setupWifi();
  delay(500);
  setupSensor();
  delay(500);
  setupMQTT();
  delay(500);
}

void loop() {
  sensorLoop();
  mqttLoop();
  app.loop();
}
