# WiFiSensor ![Build Status](https://api.travis-ci.org/rjames93/WiFiSensor.svg?branch=master)

ESP8266 Based Smart Home Sensor

Device will automatically try to connect to WiFi station defined in JSON script. If this fails it will setup a SoftAP for the user to connect to. 

The IP of the ESP8266 in SoftAP mode is:
  192.168.4.1
  
Here you can configure the device.

Default JSON (formatted by the UI and can be copy/pasted back in) is:
{
  "wifi": {
    "ssid": "default",
    "password": "default",
    "channel": 1
  },
  "softap": {
    "ssid": "SmartSensor"
  },
  "features": {
    "deepsleep": false,
    "mqtt": false,
    "serial": true
  },
  "mqtt-settings": {
    "mqtt_server": "127.0.0.1",
    "mqtt_port": 1883,
    "client_name": "SmartSensor",
    "username": "",
    "password": "",
    "temperaturetopic": "temperature",
    "humiditytopic": "humidity"
  },
  "calibration": {
    "temperature-offset": "0",
    "humidity-offset": "0"
  },
  "update_rates": {
    "normal_rate_s": "30",
    "deepsleep_rate_s": "600"
  },
  "last_modification": 0,
  "sensorname": "SmartSensor"
}


