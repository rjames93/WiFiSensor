#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include "measurements.h"
//#include <GDBStub.h>
extern "C" {
#include "user_interface.h"
}

/* Global Definitions */
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;
PubSubClient client(espClient);
String sensorname;
char msg[25];
String wifissid;
String wifipassword;
int wifichannel;
String softapssid;
String mqttserver;
int mqttport;
String mqttclientname;
String mqttusername;
String mqttpassword;
String mqtthumiditytopic;
String mqtttemperaturetopic;
float tempoffset;
float humidityoffset;
float temp2offset;
float humidity2offset;
int battvalue;
long normalupdate = 30;
long deepsleepupdate = 600;
bool deepsleepmode = false;
bool mqttmode = false;
bool serialmode = false;
bool configloaded = false;
bool dsWebOverride = true;
int dsOverrideCycleCount = 0;
long lastmodification = 0;
struct measurements lastmeasurement;

/* board configuation booleans */
bool ledGRNd1 = false;
bool ledREDd2 = false;
bool dht1d5 = true;
bool dht2d6 = false;
bool batt1a0 = false; 

/* Software Version Management */
String firmwareversion = "0.0.7";

/* PIN definitions */
const int GREENLED = 4;
const int REDLED = 5;
const int DHTPIN = 14;
const int DHT2PIN = 12;
const int BATTPIN = A0;


/* DHT Class */
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHT2PIN, DHTTYPE);

void setup() {

  Serial.begin(115200);
  Serial.println("");
  Serial.println("");
  Serial.println("Sensor Starting");
  delay(500);
  configloaded = false;
  /* This is for FS debugging only */
  /*
    SPIFFS.begin();
    if (SPIFFS.exists("/configuration.cfg")) {
    SPIFFS.format();
    Serial.println("Formatted the FS");
    }
    SPIFFS.end();
  */
  SPIFFS.begin();
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.print(dir.fileName());
    File f = dir.openFile("r");
    Serial.println(f.size());
    f.close();
  }
  SPIFFS.end();
  Serial.println("");
  pinMode(GREENLED, OUTPUT);
  pinMode(REDLED, OUTPUT);
  digitalWrite(GREENLED, LOW);
  digitalWrite(REDLED, LOW);

  // Get reset reason
  struct rst_info *rstinfo = ESP.getResetInfoPtr();
  Serial.print("Restart Reason: ");
  Serial.println((*rstinfo).reason);
  switch ((rst_reason)rstinfo->reason) {
    case 0:
      // Normal Startup by Power On
      Serial.println("Normal Power On");
	  dsWebOverride = true;
	  dsOverrideCycleCount = 0;
      ESP.restart();
      break;
    case 1:
      // Hardware Watchdog Reset
      Serial.println("Hardware Watchdog Reset");
      ESP.restart();
      break;
    case 2:
      // Exception Reset, GPIO status won't change
      Serial.println("Exception Reset");
	  dsWebOverride = true;
	  dsOverrideCycleCount = 0;
      ESP.restart();
      break;
    case 3:
      // Software watch dog reset, GPIO status won't change
      Serial.println("Software Watchdog");
	  
      ESP.restart();
      break;
    case 4:
      // Software Restart, system_restart, GPIO status won't change
      Serial.println("Software Restart");
      extsysreset();
      break;
    case 5:
      // Wake up from Deep-Sleep
      Serial.println("Deep Sleep");
      deepsleep();
      break;
    case 6:
      // External System Reset
      Serial.println("External System Reset");
	  dsWebOverride = true;
	  dsOverrideCycleCount = 0;
      extsysreset();
      break;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
	if(!deepsleepmode){
		//DeepSleep Is disabled so enable Webserver
		httpServer.handleClient();
		 // This will reset every 30000 milliseconds a.k.a every 30 seconds by default
		long timer = (millis() % (1000*normalupdate));
		  
		if ( timer <= 100 ) { // This if statement allows for some variance in the timing for each loop. Could be tuned down further but I don't really see the issue of a 0.1 second variance in when the data 'could' be measured.
			dhtmeasure();
			if (serialmode == true) { // Print to Serial
			  Serial.print("Temp: ");
			  Serial.print(lastmeasurement.temperature);
			  Serial.print("°C, Relative Humidity: ");
			  Serial.print(lastmeasurement.humidity);
			  Serial.println("%");
			  //Serial.println(millis());
			}

			if (mqttmode == true) { // Send data to MQTT Broker
			  Serial.println("Handling MQTT");
			  if ( timer <= 100 ) {
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
			}
		}
	} else {
		//If DeepSleep Enabled this is run 
		if( dsWebOverride){
			//After the Initial setup the web interface will be available for 60s and the green LED will flash
			httpServer.handleClient();
			// This will reset every 2000 milliseconds a.k.a every 2 seconds
			long dstimer = (millis() % 2000);
			if ( dstimer <= 100 ){
				dsOverrideCycleCount++;
			}
			if ( dstimer <= 1000 ){
				//Flashes the LED ON/OFF every second
				if(ledGRNd1){
					  digitalWrite(GREENLED, HIGH);
				  }
				  if(ledREDd2){
					  digitalWrite(REDLED, HIGH);
				  }

			} else{
				digitalWrite(GREENLED, LOW);
				digitalWrite(REDLED, LOW);
			}
			delay(200);
			if( dsOverrideCycleCount >= 30){
				dsWebOverride = false;
				Serial.println("Deep Sleep Web Override Finished, sleep for 10s");
				digitalWrite(GREENLED, LOW);
				digitalWrite(REDLED, LOW);
				ESP.deepSleep(10 * 1000000);
			}
			
		}
	}
		
	
	
}
