void dhtmeasure(){
  //struct measurements tmp;
  bool dhtOK = false;
  int retries = 0;
  if(dht1d5){
	while( !dhtOK){ //Loop to wait for good DHT value
		dhtOK = true; //assume it is going to work
		dht.read();
		lastmeasurement.humidity = dht.readHumidity();
		if( isnan(lastmeasurement.humidity)){
			dhtOK = false;
		} else if(lastmeasurement.humidity == 0.0){
			dhtOK = false;
		}
		retries ++;
		if(!dhtOK){
			Serial.println("DHT Not Ready");
			delay(200);
		}
		if(retries == 200){
			// Give up after 200 tries
			dhtOK = true;
		}
	
	}
	  
	  // Then get values and apply offsets
	lastmeasurement.temperature = dht.readTemperature() + tempoffset;
	lastmeasurement.humidity = dht.readHumidity() + humidityoffset;
  }
 
  if(dht2d6){
	dhtok = false;
	while( !dhtOK){ //Loop to wait for good DHT value
		dhtOK = true; //assume it is going to work
		dht.read();
		lastmeasurement.humidity2 = dht2.readHumidity();
		if( isnan(lastmeasurement.humidity2)){
			dhtOK = false;
		} else if(lastmeasurement.humidity2 == 0.0){
			dhtOK = false;
		}
		retries ++;
		if(!dhtOK){
			Serial.println("DHT Not Ready");
			delay(200);
		}
		if(retries == 200){
			// Give up after 200 tries
			dhtOK = true;
		}
	
	}
	  
	  // Then get values and apply offsets
	lastmeasurement.temperature2 = dht2.readTemperature() + tempoffset;
	lastmeasurement.humidity2 = dht2.readHumidity() + humidityoffset;
  }

  if(batt1a0){
    lastmeasurement.voltage = analogRead(BATTPIN);
  }
}

int dhtinit(){
  dht.begin();
}

