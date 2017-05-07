void dhtmeasure(){
  struct measurements tmp;
  dht.read();
  lastmeasurement.temperature = dht.readTemperature();
  lastmeasurement.humidity = dht.readHumidity();
}

int dhtinit(){
  dht.begin();
}

