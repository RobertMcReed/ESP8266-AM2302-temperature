#include "mqtt.h"

unsigned long last_discovery = millis();
unsigned long last_publish = millis();

const char* client_id = "back_basement_sensor";
const char *temperature_topic = "back_basement/temperature";
const char *humidity_topic = "back_basement/humidity";
const char *heat_index_topic = "back_basement/heat_index";

uint32_t MQTT_PUBLISH_DELAY = 10000;
uint32_t MQTT_DISCOVERY_DELAY = 300000;

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

  if (now - last_publish < MQTT_PUBLISH_DELAY) { return; }

  if (now - last_discovery > MQTT_DISCOVERY_DELAY) {
    // publish the discovery config every 5 minutes in case HA was down
    publishHomeAssistantDiscovery();
  }

  last_publish = now;
  const char *state = getLatestReadingAsJson().c_str();
  mqtt_client.publish("homeassistant/sensor/back_basement_temperature/state", state);
  mqtt_client.publish("homeassistant/sensor/back_basement_humidity/state", state);
  mqtt_client.publish("homeassistant/sensor/back_basement_heat_index/state", state);
}
