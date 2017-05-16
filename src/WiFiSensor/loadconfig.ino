void createdefaultconfig() {
  Serial.println("Create Default Config: /default.cfg");
  SPIFFS.begin();
  SPIFFS.remove("/default.cfg");
  File f = SPIFFS.open("/default.cfg", "w");
  if (!f) {
    Serial.println("Unable to open the file: /default.cfg");
    return;
  }
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  StaticJsonBuffer<128> wifibuffer;
  JsonObject &wifista = wifibuffer.createObject();
  wifista["ssid"] = "default";
  wifista["password"] = "default";
  wifista["channel"] = 1;
  root["wifi"] = wifista;

  StaticJsonBuffer<128> softapbuffer;
  JsonObject &softap = softapbuffer.createObject();
  String ssidname = String("SmartSensor");// + String(random(256));
  softap["ssid"] = ssidname.c_str();
  root["softap"] = softap;

  StaticJsonBuffer<128> featuresBuffer;
  JsonObject &features = featuresBuffer.createObject();
  features["deepsleep"] = false;
  features["mqtt"] = false;
  features["serial"] = true;
  StaticJsonBuffer<128> mqttbuffer;
  JsonObject &mqtt = mqttbuffer.createObject();
  mqtt["mqtt_server"] = "127.0.0.1";
  mqtt["mqtt_port"] = 1883;
  mqtt["client_name"] = ssidname;
  mqtt["username"] = "";
  mqtt["password"] = "";
  mqtt["temperaturetopic"] = "temp";
  mqtt["humiditytopic"] = "humidity";
  features["mqtt-settings"] = mqtt;
  root["features"] = features;

  root["last_modification"] = 0; // Default values. Basically we just check to see if this value is not 0
  root["sensorname"] = "SmartSensor";

  root.printTo(f);
  Serial.print("Default Config: ");
  root.printTo(Serial);
  Serial.println("");
  delay(0);
  f.close();
  SPIFFS.end();
}

int loadconfig() {
  Serial.println("");
  Serial.println("Loading Config");
  SPIFFS.begin();
  // By this point the configuration file should exist
  File f;
  if (!SPIFFS.exists("/configuration.cfg")) {
    Serial.println("Opening Default Config");
    f = SPIFFS.open("/default.cfg", "r");
  } else {
    Serial.println("Opening Custom Config");
    f = SPIFFS.open("/configuration.cfg", "r");
  }
  if (!f) {
    Serial.println("Unable to open the file: /default.cfg");
    return (1);
  }

  String confjson = f.readStringUntil('\n');
  Serial.println(confjson);
  int jsstrlength = confjson.length();
  Serial.print("JSON String Length: ");
  Serial.println(jsstrlength);
  char *jsstr = (char *)malloc(jsstrlength * sizeof(char) + 5);
  //jsstr = confjson.c_str();
  Serial.println(jsstr);
  strcpy(jsstr, confjson.c_str());
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(jsstr);
  if (!root.success()) {
    Serial.println("JSON Parsing fail");
    f.close();
    return (1);
  }
  f.close();
  Serial.println();
  // Config is now loaded.
  // Use the JSON root object to assign the values to the global variables
  wifissid = root["wifi"]["ssid"].asString();
  Serial.print("WiFi SSID: ");
  Serial.println(wifissid);
  wifipassword = root["wifi"]["password"].asString();
  Serial.print("WiFi Password: ");
  Serial.println(wifipassword);
  wifichannel = root["wifi"]["channel"];
  Serial.print("WiFi Channel: ");
  Serial.println(wifichannel);

  // Use bools for the features
  deepsleepmode = root["features"]["deepsleep"];
  mqttmode = root["features"]["mqtt"];
  serialmode = root["features"]["serial"];

  if (mqttmode == true) {
    mqttserver = root["mqtt-settings"]["mqtt_server"].asString();
    mqttport = root["mqtt-settings"]["mqtt_port"];
    mqttclientname = root["mqtt-settings"]["client_name"].asString();
    mqttusername = root["mqtt-settings"]["username"].asString();
    mqttpassword = root["mqtt-settings"]["password"].asString();
    mqtthumiditytopic = root["mqtt-settings"]["humiditytopic"].asString();
    mqtttemperaturetopic = root["mqtt-settings"]["temperaturetopic"].asString();
  }

  lastmodification = root["last_modification"];
  sensorname = root["sensorname"].asString();

  softapssid = root["softap"]["ssid"].asString();

  configloaded = true;
  Serial.println("Config Loaded");
  SPIFFS.end();
  return (0);
}
