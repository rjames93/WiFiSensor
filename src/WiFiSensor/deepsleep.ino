void deepsleep() {
  if (!configloaded) {
    loadconfig();
  }
  // DeepSleep normal mode LEDS should always be disabled
  digitalWrite(REDLED, LOW);
  digitalWrite(GREENLED, LOW);
  
  bool mqttconnectsuccess = false;
  bool mqtttimeout = false;
  int mqttfailcount = 0;
  
  if ( !trySTAWiFi() ) {
    // STA Failed so load softAP
    Serial.println("Wireless Station Failed 1 of 2");
    delay(2000);
	if ( !trySTAWiFi()  ) {
		Serial.println("Wireless Station Failed 2 of 2");
		Serial.println("Give Up and DeepSleep");
    Serial.print("ESP Sleep for ");
    Serial.print(deepsleepupdate);
    Serial.print(" seconds.");
		ESP.deepSleep(deepsleepupdate * 1000000);
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
  dhtinit();
  dhtmeasure();
  
  if (serialmode == true) { // Print to Serial
          if (dht1d5) {
            Serial.print("Temp: ");
            Serial.print(lastmeasurement.temperature);
            Serial.print("°C, Relative Humidity: ");
            Serial.print(lastmeasurement.humidity);
            Serial.println("%");
          }
          if (dht2d6) {
            Serial.print("Temp2: ");
            Serial.print(lastmeasurement.temperature2);
            Serial.print("°C, Relative Humidity: ");
            Serial.print(lastmeasurement.humidity2);
            Serial.println("%");
          }
          if (batt1a0) {
            Serial.print("Voltage: ");
            Serial.print(lastmeasurement.voltage);
            Serial.println("V");
          }
	}
  
  // Basically Deepsleep needs MQTT on in order to do stuff else it's going to do nothing cause it won't be able to respond to http requests

  
	Serial.println("Handling MQTT");
	  
	
	// We need to report to the MQTT Server
	while (!mqtttimeout) {
	  if(mqttconnect() != 0){
		  Serial.println("Unable to connect to MQTT Server ");
		  Serial.println(mqttfailcount);
		  delay(1000);
	      mqttfailcount++;
	  } else {
		  Serial.println("MQTT Successfully Connected");
		  mqtttimeout = true;
		  mqttconnectsuccess = true;
	  }
	  
	  if(mqttfailcount >= 10 ){
		  Serial.println("Giving up MQTT connection" );
		  mqtttimeout = true;
	  }
	  
	  
	}
	
	if(mqttconnectsuccess){
	  // Connect to MQTT Server
	  //client.connect(mqttclientname.c_str(), mqttusername.c_str(), mqttpassword.c_str() );
	  //Following is original
	  String topicname("sensors/");
	  topicname += String(sensorname);
	  topicname += String("/");
	  
	  mqttPrepAndPublish(topicname);
	}


	  
  

  Serial.print("ESP Sleep for ");
  Serial.print(deepsleepupdate);
  Serial.print(" seconds.");

  ESP.deepSleep(deepsleepupdate * 1000000);
}


