void createdefaultconfig() {
  Serial.println("Create Default Config: /default.cfg");
  SPIFFS.begin();
  SPIFFS.remove("/default.cfg");
  File f = SPIFFS.open("/default.cfg", "w");
  if (!f) {
    Serial.println("Unable to open the file: /default.cfg");
    return;
  }
  DynamicJsonBuffer jsonBuffer;
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
  root["features"] = features;
  
  StaticJsonBuffer<256> boardconfBuffer;
  JsonObject &boardconf = boardconfBuffer.createObject();
  boardconf["led-grn-d1"] = true;
  boardconf["led-red-d2"] = true;
  boardconf["dht-1-d5"] = true;
  boardconf["dht-2-d6"] = false;
  boardconf["batt-1-a0"] = false;
  root["board-conf"] = boardconf;
  
  StaticJsonBuffer<256> mqttbuffer;
  JsonObject &mqtt = mqttbuffer.createObject();
  mqtt["mqtt_server"] = "127.0.0.1";
  mqtt["mqtt_port"] = 1883;
  mqtt["client_name"] = ssidname;
  mqtt["username"] = "";
  mqtt["password"] = "";
  mqtt["temperaturetopic"] = "temperature";
  mqtt["humiditytopic"] = "humidity";
  root["mqtt-settings"] = mqtt;
  
  StaticJsonBuffer<256> calbuffer;
  JsonObject &calib = calbuffer.createObject();
  calib["temperature-offset"] = "0";
  calib["humidity-offset"] = "0";
  calib["temperature2-offset"] = "0";
  calib["humidity2-offset"] = "0";
  calib["voltage-multiplier"] = "0.0152";
  root["calibration"] = calib;
  
  StaticJsonBuffer<128> timingbuffer;
  JsonObject &timing = timingbuffer.createObject();
  timing["normal_rate_s"] = "30";
  timing["deepsleep_rate_s"] = "600";
  root["update_rates"] = timing;

  root["last_modification"] = 0; // Default values. Basically we just check to see if this value is not 0
  root["sensorname"] = "SmartSensor";

  root.printTo(f);
  Serial.print("Default Config: ");
  root.printTo(Serial);
  Serial.println("");
  delay(0);
  f.close();
  SPIFFS.end();
  Serial.println("Finished Default Conf Setup");
}

int loadconfig() {
  Serial.println("");
  Serial.println("Loading Config");
  SPIFFS.begin();
  File f;
  // By this point the configuration file should exist
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
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(jsstr);
  if (!root.success()) {
    Serial.println("JSON Parsing fail");
    f.close();
    SPIFFS.end();
    return (1);
  }
  f.close();
  SPIFFS.end();
  Serial.println();
  // Config is now loaded.

  //Check certain parts of the JSON to ensure it is compatible with this version
  if(!root.containsKey("wifi")){
    //String not found error case
    Serial.println("Error: Config does not contain 'wifi'");
    return (1);
  }
  if(!root.containsKey("features")){
    //String not found error case
    Serial.println("Error: Config does not contain 'features'");
    return (1);
  }
  if(!root.containsKey("board-conf")){
    //String not found error case
    Serial.println("Error: Config does not contain 'board-conf'");
    return (1);
  }
  if(!root.containsKey("update_rates")){
    //String not found error case
    Serial.println("Error: Config does not contain 'update_rates'");
    return (1);
  }
  
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

  //Use Bools for the board Config
  ledGRNd1 = root["board-conf"]["led-grn-d1"];
  ledREDd2 = root["board-conf"]["led-red-d2"];
  dht1d5 = root["board-conf"]["dht-1-d5"];
  dht2d6 = root["board-conf"]["dht-2-d6"];
  batt1a0 = root["board-conf"]["batt-1-a0"]; 


  mqttserver = root["mqtt-settings"]["mqtt_server"].asString();
  mqttport = root["mqtt-settings"]["mqtt_port"];
  mqttclientname = root["mqtt-settings"]["client_name"].asString();
  mqttusername = root["mqtt-settings"]["username"].asString();
  mqttpassword = root["mqtt-settings"]["password"].asString();
  mqtthumiditytopic = root["mqtt-settings"]["humiditytopic"].asString();
  mqtttemperaturetopic = root["mqtt-settings"]["temperaturetopic"].asString();

  tempoffset = root["calibration"]["temperature-offset"].as<float>();
  humidityoffset = root["calibration"]["humidity-offset"].as<float>();
  temp2offset = root["calibration"]["temperature2-offset"].as<float>();
  humidity2offset = root["calibration"]["humidity2-offset"].as<float>();
  voltagemultiplier = root["calibration"]["voltage-multiplier"].as<float>();
  
  normalupdate = root["update_rates"]["normal_rate_s"].as<long>();
  deepsleepupdate = root["update_rates"]["deepsleep_rate_s"].as<long>();

  lastmodification = root["last_modification"];
  sensorname = root["sensorname"].asString();

  softapssid = root["softap"]["ssid"].asString();
  // Set configloaded flag
  configloaded = true;
  Serial.println("Config Loaded");
  return (0);
}

