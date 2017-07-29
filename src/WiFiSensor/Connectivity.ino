int trySTAWiFi() {
  // Use the congigured variables to try connect to the wifi ap
  long startt = millis();
  long curtime = startt;
  WiFi.disconnect(true); 
  WiFi.setAutoConnect(false);
  // Okay now we need to stop any existing softAP
  WiFi.softAPdisconnect(true);
  
  WiFi.begin(wifissid.c_str(), wifipassword.c_str());
  while ( (WiFi.status() != WL_CONNECTED) && ((curtime - startt) / 1000) < 15 )  {
    delay(500);
    Serial.print(".");
    curtime = millis();
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Sta Connect Success");
    Serial.print("IP Address is: ");
    Serial.println(WiFi.localIP());
    return (1); // Connect Success
  } else {
    Serial.println("WiFi Sta Connect Failed");
    return (0); // Connect Fail
  }
  return (0);
}


int trySoftSSID() {
  WiFi.softAPdisconnect(true);
  // Use the configured variables to establish a wireless ap
  Serial.print("Starting a softAP, with no password of SSID: ");
  Serial.println(softapssid);
  boolean result = WiFi.softAP(softapssid.c_str());
  if ( result == true ) {
    Serial.println("SoftAP Success");
    return(1);
  } else {
    Serial.println("SoftAP Failed");
    return(0);
  }
  return (0);
}

int closeSoftSSID() {
  WiFi.softAPdisconnect(true);
  return(1):
}


