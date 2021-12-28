# ESP8266-AM2302-temperature

Wire a DHT22 or AM2302 temperature sensor to an ESP8266 on pin D3, including a 10k ohm pull-up resistor.

Create a `.env` file with values for `SSID` and `PW`. These will be used for the initial WiFi name and PW.

Contents of `var/www` will be merged into a single HTML file which will be written to `include/html.h` and available in PROGMEM as var `HTML`.
