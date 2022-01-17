# ESP8266-AM2302-temperature

Wire a DHT22 or AM2302 temperature sensor to an ESP8266 on pin D3, including a 10k ohm pull-up resistor.

Create an `.env` as follows:

```
SSID=giveTheDeviceAName
PW=pwProtectTheDevice
MQTT_BROKER=addressOfMqttBroker
MQTT_USERNAME=usernameForMqtt
MQTT_PASSWORD=pwForMqtt
```

Contents of `var/www` will be merged into a single HTML file which will be written to `include/html.h` and available in PROGMEM as var `HTML`. Do not edit `include/html.h` manually, your changes will be overwritten.

Look at `platform.ini` to use OTA updates.
