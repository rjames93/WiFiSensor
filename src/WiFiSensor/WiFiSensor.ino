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
bool deepsleepmode = false;
bool mqttmode = false;
bool serialmode = false;
bool configloaded = false;
long lastmodification = 0;
struct measurements lastmeasurement;

/* PIN definitions */
const int GREENLED = 4;
const int REDLED = 5;
const int DHTPIN = 14;

/* DHT Class */
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

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
      extsysreset();
      break;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  httpServer.handleClient();
  long timer = (millis() % 30000); // This will reset every 30000 milliseconds a.k.a every 30 seconds
  if ( timer <= 100 ) {
    dhtmeasure();
    if (serialmode == true) {
      Serial.print("Temp: ");
      Serial.print(lastmeasurement.temperature);
      Serial.print("°C, Relative Humidity: ");
      Serial.print(lastmeasurement.humidity);
      Serial.println("%");
      //Serial.println(millis());
    }

    if (mqttmode == true) {
      if ( timer <= 100 ) {
        // We need to report to the MQTT Server
        if(!mqttconnect()){
          Serial.print("Unable to connect to MQTT Server");
        }
        // Connected to MQTT Server
        String topicname("sensors/");
        topicname += String(sensorname);
        topicname += String("/");
        snprintf(msg, 25, "%s", String(lastmeasurement.temperature).c_str());
        client.publish( (topicname + mqtttemperaturetopic).c_str(), msg);
        snprintf(msg, 75, "%s", String(lastmeasurement.humidity).c_str());
        client.publish((topicname + mqtthumiditytopic)).c_str(), msg);
      }
    }
    //delay(250);
  }
}
