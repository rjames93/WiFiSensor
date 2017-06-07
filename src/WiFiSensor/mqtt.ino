int setupMQTT(){

  if(mqttmode == false){
    return (1);
  }

  client.setServer(mqttserver.c_str(), mqttport);
  /*
  mqttserver = root["mqtt-settings"]["mqtt_server"].asString();
  mqttport = root["mqtt-settings"]["mqtt_port"];
  mqttclientname = root["mqtt-settings"]["client_name"].asString();
  mqttusername = root["mqtt-settings"]["username"].asString();
  mqttpassword = root["mqtt-settings"]["password"].asString();
  mqtthumiditytopic = root["mqtt-settings"]["humiditytopic"].asString();
  mqtttemperaturetopic = root["mqtt-settings"]["temperaturetopic"].asString();
  */
  return(0);
}

int mqttconnect(){
  // This exists so that even if called out of the setup loop (which it might be) that it doesn't do anything if the config isn't loaded
  if(mqttmode == false){
    return (1);
  }

  if (client.connect(mqttclientname.c_str(), mqttusername.c_str(), mqttpassword.c_str() ) ){
    // Client connected successfully
    return(0);
  } else {
    Serial.println("Unable To Connect, Details: ");
    Serial.println("  mqttclientname: ");
    Serial.println(mqttclientname);
    Serial.println("  mqttusername: ");
    Serial.println(mqttusername);
    Serial.println("  mqttpassword: ");
    Serial.println(mqttpassword);
    Serial.println(" ");
    return(2);
  }
}

int mqttpublish(String topic, const char* msg ){
  if(client.publish( topic.c_str(), msg)){
      Serial.println( "Successful MQTT Update");
      return 0;
  }else{
      Serial.println( "Failed MQTT Update, Details:");
      Serial.println("  topic: ");
      Serial.println(topic);
      Serial.println("  msg: ");
      Serial.println(msg );
      Serial.println(" ");
      return 1;
  }
}
  

