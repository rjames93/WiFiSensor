void dhtmeasure(){
  struct measurements tmp;
  dht.read();
  lastmeasurement.temperature = dht.readTemperature() + tempoffset;
  lastmeasurement.humidity = dht.readHumidity() + humidityoffset;
}

int dhtinit(){
  dht.begin();
}

