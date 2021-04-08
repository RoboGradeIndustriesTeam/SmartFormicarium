#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#ifndef STASSID
#define STASSID "net.rob0"
#define STAPSK  "Robo.Grade"
#endif

#define u8 uint8_t

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

DHT_Unified dht(10, DHT11);

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

String handleCmd(DynamicJsonDocument doc) {
  const char* _command = doc["cmd"];
  // Что-бы получить command: (String) _command
  String dataS = "{\"success\": 0}";
  if ((String) _command == "fan") {
    /*
      {
       "cmd": "fan",
       "data": {
         "fan_number": 32,
         "action": 0
       }
      }
    */
    const u8 fan_number = doc["data"]["fan_number"];
    const u8 action = (u8) doc["data"]["action"];
    if (fan_number == 0) {
      digitalWrite(7, action);
    }
    if (fan_number == 1) {
      digitalWrite(8, action);
    }
    Serial.println("Fan command requested: fun_number: [" + String(fan_number) + "], action: [" + String(action) + "]");
    dataS = "{\"success\": 1}";
  }
  else if ((String) _command == "light") {
    /*
      {
       "cmd": "light",
       "data": {
         "action" : 1
       }
      }
    */
    const u8 action = doc["data"]["action"];
    digitalWrite(9, action);
    Serial.println("Light command requested: action: [" + String(action) + "]");
    dataS = "{\"success\": 1}";
  }
  else if ((String) _command == "sensor") {
    /*
      {
       "cmd": "sensor",
       "data": {
         "name": "dht"
       }
      }
    */
    // Что-бы получить name: (String) _name
    const char* _name = doc["data"]["name"];
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    u8 temp = event.temperature;
    dht.humidity().getEvent(&event);
    u8 hum = event.relative_humidity;
    if ((String) _name == "dht") {
      dataS = (String)"{\"success\": 1, \"sensorTemp\": " + (String)temp + ", \"sensorHum\": " + (String)hum + "}";
    }
    Serial.println("Sensor command requested: name: [" +  String(_name) + "]");
  }
  else if ((String) _command == "set") {
    /*
      {
       "cmd": "set",
       "data": {
         "type": "hum",
         "value": 32
       }
      }
    */
    // Что-бы получить name: (String) _name
    const char* _name = doc["data"]["type"];
    const u8 value = doc["data"]["value"];
    if ((String) _name == "hum") {
      dataS = "{\"success\": 0}";
    }
    Serial.println("Set command requested: temp: [" +  String(_name) + "], value: [" +  String(value) + "]");
  }
  return dataS;
}
void handleRoot() {
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    server.send(200, "text/plain", "{\"error\": 0}");
    return;
  }
  server.send(200, "text/plain", handleCmd(doc));
}

void setup(void) {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
