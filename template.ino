#include <Arduino.h>
#include "Korzhenevskiy_utils.h"
#include "Korzhenevskiy_tick_tack_2.h"
#include "secrets.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>


WiFiClient espClient;
PubSubClient client;

void callback(char* topic, byte* payload, unsigned int length);

void some_callback();
Korzhenevskiy_tick_tack_2 some_timer(some_callback, 15000);


void some_callback(){
   Serial.println("timer fires");
   client.publish(MQTTChannelToPublish, "timer fires");
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//                    SETUP                   SETUP                     SETUP                   SETUP
//                    SETUP                   SETUP                     SETUP                   SETUP
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\n=== BOOT SUCCESS ===");
  
  // Initialize MQTT client
  client.setClient(espClient);
  client.setServer(mqtt_server, mqttport);
  client.setCallback(callback);
  
  draw_cat();
 
  connecttobestwifi(networks, numNetworks);
 
  connecttoMQTT();
 
  ArduinoOTA.setPassword(ota_password);
  
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

  ArduinoOTA.begin();  
  Serial.println("=== SETUP COMPLETE ===");
  

 
  Korzhenevskiy_tick_tack_2::mass_start(); //     OR      some_timer.start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//                    LOOP                   LOOP                     LOOP                   LOOP
//                    LOOP                   LOOP                     LOOP                   LOOP
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  
  ArduinoOTA.handle();  
  if (!client.connected()) {connecttoMQTT();}
  client.loop();
  Korzhenevskiy_tick_tack_2::mass_tick();

  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//                    LOOP END                  LOOP END                     LOOP END                 
//                    LOOP END                  LOOP END                     LOOP END      
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////


void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("MQTT message: ");
  Serial.println(message);
  
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.print(F("JSON parse failed: "));
    Serial.println(error.c_str());
    return;
  }
  
  Serial.println("JSON parse success");
  
 
  if (doc.containsKey("command")) {
    String command = doc["command"];
    if (command == "reset") {
      Serial.println("Restarting...");
      ESP.restart();
    }
  }

  if (doc.containsKey("enable_something")) {
      int Value = doc["radarenable"];
      //enable_something = (Value == 1);
      Serial.print("enable_something set to: ");
      Serial.println(Value ? "true" : "false");
    }

  if (doc.containsKey("some_integer")) {
      int Value = doc["some_integer"];
      //stepper.moveTo(Value);
      Serial.print("some_integer ");
      Serial.println(Value);
    }


  
  
  
}