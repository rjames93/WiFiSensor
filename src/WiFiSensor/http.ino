void httpConfigure() {
  Serial.println("");
  Serial.println("httpConfigure page");
  SPIFFS.begin();

  File f;
  if (!SPIFFS.exists("/configuration.cfg")) {
    if (!SPIFFS.exists("/default.cfg")) {
      createdefaultconfig();
    }
    Serial.println("Opening /default.cfg");
    f = SPIFFS.open("/default.cfg", "r");
  } else {
    Serial.println("Opening /configuration.cfg");
    f = SPIFFS.open("/configuration.cfg", "r");
  }
  String confjson = f.readStringUntil('\n');
  int jsstrlength = confjson.length();
  char *jsstr = (char *)malloc(jsstrlength * sizeof(char) + 1);
  strcpy(jsstr, confjson.c_str());
  Serial.println(jsstr);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(jsstr);
  if (!root.success()) {
    Serial.println("JSON Parsing fail");
    httpServer.send(200, "text/html", confjson.c_str());
    return;
  }

  String html = String("<html><head><title>Smart Sensor Configurator</title></head><body><form method=\"post\" name=\"Configurator\" id=\"Configurator\" action=\"/testandapplyconfig\"><textarea id=\"json\" name=\"json\" rows=\"20\" cols=\"128\">");
  String jsonform;
  root.prettyPrintTo(jsonform);
  html += jsonform;
  html += String("</textarea><br><button type=\"submit\">Submit</button></div></form>");
  html += String("<br><hr><ul>");
  // Print the output of a wifi scan
  int nnetworks = WiFi.scanNetworks();
  for (int i = 0; i < nnetworks; i++) {
    String ssid;
    uint8_t encryptionType;
    int32_t RSSI;
    uint8_t* BSSID;
    int32_t channel;
    bool isHidden;
    WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);
    char tmp[128];
    snprintf(tmp, 127, "<li>%d: %s, Ch:%d (%ddBm) %s %s</li>", i + 1, ssid.c_str(), channel, RSSI, encryptionType == ENC_TYPE_NONE ? "open" : "", isHidden ? "hidden" : "");
    html += String(tmp);
  }
  html += String("</ul></body></html>");
  Serial.println("/config Requested");
  httpServer.send(200, "text/html", html);
  f.close();
  SPIFFS.end();
};

void testandapplyconfig() {
  SPIFFS.begin();
  DynamicJsonBuffer httpjsonBuffer;
  int lengthofinput = httpServer.arg(0).length();
  Serial.println(lengthofinput);
  char *string = (char *)malloc( (lengthofinput + 1) * sizeof(char) );
  strcpy(string, httpServer.arg(0).c_str());
  JsonObject &root = httpjsonBuffer.parseObject(string);
  if (!root.success()) {
    Serial.println("JSON Parsing Fail");
    httpServer.send(200, "text/html", String("<html><h1>JSON Parsing Fail</h1>") + String(string));
    return;
  }
  String returnhtml("<html></body><h1>Results</h1>");
  returnhtml += String("<br>Recieved SSID: ");
  returnhtml += String(root["wifi"]["ssid"].asString());
  returnhtml += String("<br>Recieved Password: ");
  returnhtml += String(root["wifi"]["password"].asString());
  returnhtml += String("<br>Recieved Channel: ");
  returnhtml += String(root["wifi"]["channel"].as<long>());
  //Serial.println(returnhtml);
  wifissid = String(root["wifi"]["ssid"].asString());
  wifipassword = String(root["wifi"]["password"].asString());
  /*
    returnhtml += String("<br><h2>New IP Address on SSID - ");
    returnhtml += wifissid;
    returnhtml += String("is: ");
    returnhtml += WiFi.localIP();
    returnhtml += String("</h2>");
    returnhtml += String("</body></html");
  */
  Serial.println("New config sent to Server");
  httpServer.send(200, "text/html", returnhtml);

  // Now try to connect to WiFi using provided credentials
  if ( !trySTAWiFi() ) {
    ESP.restart();
  } else {
    Serial.println("WiFi STA Success");
  }


  if ( !trySoftSSID() ) {
    ESP.restart();
  } else {
    Serial.println("SoftAP reinitialised");
  }
  // Okay now to write the json to the file
  File fp = SPIFFS.open("/configuration.cfg", "w+");
  if (!fp) {
    Serial.println("Unable to open the configuration file");
    SPIFFS.remove("/configuration.cfg");
    return;
  }
  root.printTo(fp);
  Serial.print("Printing JSON to file: ");
  root.printTo(Serial);
  fp.close();
  SPIFFS.end();
  loadconfig();
  ESP.restart();
}

void landingPage() {
  String html("<http><head><title>");
  html += sensorname;
  html += String("</title></head><body><h1>");
  html += sensorname;
  html += String(" Status</h1><br>");
  html += String("<p>Firmware Version: ");
  html += firmwareversion;
  html += String("<h2>Last Measurement</h2>");
  html += String("<p>Temperature (°C): ");
  html += String(lastmeasurement.temperature);
  html += String("<br><p>Relative Humidity (%): ");
  html += String(lastmeasurement.humidity);
  html += String("<p>Temperature 2 (°C): ");
  html += String(lastmeasurement.temperature2);
  html += String("<br><p>Relative Humidity 2 (%): ");
  html += String(lastmeasurement.humidity2);
  html += String("<br><p>Voltage (V): ");
  html += String(lastmeasurement.voltage);
  html += String("<br><hr><br><h2> Other Links: <ul><li><a href=\"/config\"> Configuration Page</a></li><li><a href=\"/update\"> Firmware Updater</a></li></ul>");
  html += String("</body></html>");
  Serial.println("/ Requested");
  httpServer.send(200, "text/html", html);
}

void resetConfig() {
  httpServer.send(200, "text/html", String("<html><title>Message</title><body><h1>Deleting the Configuration.cfg</h1></html>"));
  SPIFFS.begin();
  SPIFFS.remove("/configuration.cfg");
  SPIFFS.remove("/default.cfg");
  SPIFFS.end();
  ESP.restart();
}

