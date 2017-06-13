void deepsleep() {
  if (!configloaded) {
    loadconfig();
  }
  dhtinit();
  dhtmeasure();
  int sleepinterval = 600; // seconds
  // Basically Deepsleep needs MQTT on in order to do stuff else it's going to do nothing cause it won't be able to respond to http requests

  if (mqttmode == true) {
	  Serial.println("Handling MQTT");
	  
	Serial.println("Gathering Data");
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


	  
  }


  ESP.deepSleep(sleepinterval * 1000000, WAKE_RF_DEFAULT);
}

