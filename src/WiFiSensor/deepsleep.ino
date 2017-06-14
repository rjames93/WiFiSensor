void deepsleep() {
  if (!configloaded) {
    loadconfig();
  }
  int sleepinterval = 10; // seconds
  
  if ( !trySTAWiFi() ) {
    // STA Failed so load softAP
    Serial.println("Wireless Station Failed");
    digitalWrite(REDLED, HIGH);
    delay(2000);
	if ( !trySTAWiFi()  ) {
		Serial.println("SoftAP failed 2");
		Serial.println("Give Up and DeepSleep");
		digitalWrite(REDLED, LOW);
		digitalWrite(GREENLED, LOW);
		ESP.deepSleep(sleepinterval * 1000000);
	}
  }
  dhtinit();
  dhtmeasure();
  
  if (serialmode == true) { // Print to Serial
			  Serial.print("Temp: ");
			  Serial.print(lastmeasurement.temperature);
			  Serial.print("°C, Relative Humidity: ");
			  Serial.print(lastmeasurement.humidity);
			  Serial.println("%");
			  //Serial.println(millis());
			}
  
  // Basically Deepsleep needs MQTT on in order to do stuff else it's going to do nothing cause it won't be able to respond to http requests

  
	Serial.println("Handling MQTT");
	  
	
	// We need to report to the MQTT Server
	if (mqttconnect() != 0) {
	  Serial.println("Unable to connect to MQTT Server");
	} else {
	  // Connect to MQTT Server
	  //client.connect(mqttclientname.c_str(), mqttusername.c_str(), mqttpassword.c_str() );
	  //Following is original
	  String topicname("sensors/");
	  topicname += String(sensorname);
	  topicname += String("/");
	  /*
	  //Manual Variable Setting
	  mqtttemperaturetopic = "temperature";
	  mqtthumiditytopic = "humidity";
	  */
	  snprintf(msg, 25, "%s", String(lastmeasurement.temperature).c_str());
	  mqttpublish((topicname + mqtttemperaturetopic), msg);
	  snprintf(msg, 75, "%s", String(lastmeasurement.humidity).c_str());
	  mqttpublish((topicname + mqtthumiditytopic), msg);
	}


	  
  


  digitalWrite(REDLED, LOW);
  digitalWrite(GREENLED, LOW);
  ESP.deepSleep(sleepinterval * 1000000);
}

