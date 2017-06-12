void dhtmeasure(){
  struct measurements tmp;
  bool dhtOK = false
  
  while( !dhtOK){ //Loop to wait for good DHT value
	dhtOK = true; //assume it is going to work
	dht.read();
	lastmeasurement.humidity = dht.readHumidity();
	if( isnan(lastmeasurement.humidity)){
		dhtOK = false;
	} else if(lastmeasurement.humidity == 0.0){
		dhtOK = false;
	}
	
	if(!dhtOK){
		delay(200);
	}
	
  }
  
  // Then get values and apply offsets
  lastmeasurement.temperature = dht.readTemperature() + tempoffset;
  lastmeasurement.humidity = dht.readHumidity() + humidityoffset;
}

int dhtinit(){
  dht.begin();
}

