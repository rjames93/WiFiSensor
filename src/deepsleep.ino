void deepsleep() {
  if (!configloaded) {
    loadconfig();
  }
  dhtinit();
  dhtmeasure();
  int sleepinterval = 60; // seconds
  // Basically Deepsleep needs MQTT on in order to do stuff else it's going to do nothing cause it won't be able to respond to http requests

  if (mqttmode == true) {
    client.setServer(mqttserver.c_str(), mqttport );
    client.connect("loungesensor", "smartsensor1", "wibble");
    memset( msg, 0, sizeof( msg ) );
    
    // We need to report to the MQTT Server
    String topicname("sensors/");
    topicname += String(sensorname);
    topicname += String("/");
    snprintf(msg, 25, "%s", String(lastmeasurement.temperature).c_str());
    client.publish( (topicname + String("temp")).c_str(), msg);

    snprintf(msg, 75, "%s", String(lastmeasurement.humidity).c_str());
    client.publish((topicname + String("temp")).c_str(), msg);
  }


  ESP.deepSleep(sleepinterval * 1000000, WAKE_RF_DEFAULT);
}

