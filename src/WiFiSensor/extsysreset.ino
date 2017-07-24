void extsysreset() {
  // Create the default Config
  createdefaultconfig();
  // Load the Config
  if(loadconfig()== 1){
    Serial.println("Deleting Custom Config");
    SPIFFS.remove("/configuration.cfg");
    Serial.println("Deleting Default Config");
    SPIFFS.remove("/default.cfg");
    createdefaultconfig();
    loadconfig();
  }
  // Config is now loaded.
  delay(1000);
  // Try connecting to the WiFi using the config file
  if ( !trySTAWiFi() ) {
    // STA Failed so load softAP
    Serial.println("Wireless Station Failed");
    Serial.println("Now try a SoftAP");
    if(ledREDd2){
      digitalWrite(REDLED, HIGH);
    }
    delay(1000);
	if ( !trySoftSSID()  ) {
		Serial.println("SoftAP failed");
    if(ledREDd2){
      digitalWrite(REDLED, HIGH);
    }
	}
  }

  

  //  Setup mqtt
  if (setupMQTT() == 1){
    // Ignore this outcome it hasn't done anything
  } else {
    // We have setupMQTT
    if( mqttconnect() != 0 ){
      // Haven't been able to connect :|
      // Handle this exception. Probably reload?
      Serial.println("Unable to connect to MQTT after reset");

    }
  }

  // Should now be connected to the MQTT Server

  if(ledGRNd1){
    digitalWrite(GREENLED, HIGH);
  }

  // Setup the WebServer
  httpUpdater.setup(&httpServer);
  httpServer.on("/config", httpConfigure);
  httpServer.on("/reset", resetConfig);
  httpServer.on("/testandapplyconfig", testandapplyconfig);
  httpServer.on("/", landingPage);
  httpServer.begin();
  dhtinit();
  if (mqttmode == true) {
    client.setServer(mqttserver.c_str(), mqttport);
    if (mqttusername == String("") && mqttpassword == String("") ) {
      client.connect(mqttclientname.c_str());
    } else {
      client.connect(mqttclientname.c_str(), mqttusername.c_str(), mqttpassword.c_str());
    }
    memset( msg, 0, sizeof( msg ) );
  }
}
