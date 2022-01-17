#include "mqtt.h"

// how long to wait between connection attempts
uint32_t MQTT_CONNECT_DELAY = 10000;

// how long to wait between publishing sensor data
uint32_t MQTT_PUBLISH_DELAY = 10000;

// how long to wait before advertising to HA
uint32_t MQTT_DISCOVERY_DELAY = 300000;

const unsigned long before = millis() - MQTT_CONNECT_DELAY - MQTT_PUBLISH_DELAY - MQTT_DISCOVERY_DELAY;
unsigned long last_discovery = before;
unsigned long last_publish = before;
unsigned long last_connect_attempt = before;

const char* client_id = "back_basement_sensor";
const char *temperature_topic = "back_basement/temperature";
const char *humidity_topic = "back_basement/humidity";
const char *heat_index_topic = "back_basement/heat_index";

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

void publishHomeAssistantDiscovery() {
  last_discovery = millis();
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

bool reconnect() {
  last_connect_attempt = millis();
  if (mqtt_client.connect(client_id, __MQTT_USERNAME__, __MQTT_PASSWORD__)) {
    Serial.println("Connected to MQTT broker!");
    publishHomeAssistantDiscovery();
  } else {
    Serial.print("Could not connect to broker: ");
    Serial.println(mqtt_client.state());
  }

  return mqtt_client.connected();
}

void setupMQTT() {
  mqtt_client.setServer(__MQTT_BROKER__, 1883);
}

bool connectionOk() {
  if (mqtt_client.connected()) {
    return true;
  }

  if (millis() - last_connect_attempt < MQTT_CONNECT_DELAY) {
    return false;
  }

  return reconnect();
}

void mqttLoop() {
  if (!connectionOk()) { return; }
    
  unsigned long now = millis();

  if (now - last_discovery > MQTT_DISCOVERY_DELAY) {
    // publish the discovery config every 5 minutes in case HA was down
    publishHomeAssistantDiscovery();
  }

  if (now - last_publish < MQTT_PUBLISH_DELAY) { return; }

  last_publish = now;
  const String state_s = getLatestReadingAsJson();
  const char *state = state_s.c_str();

  mqtt_client.publish("homeassistant/sensor/back_basement_temperature/state", state);
  mqtt_client.publish("homeassistant/sensor/back_basement_humidity/state", state);
  mqtt_client.publish("homeassistant/sensor/back_basement_heat_index/state", state);
}
