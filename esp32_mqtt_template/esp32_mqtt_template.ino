// ESP32 + MQTT project skeleton.
// Before the first build: copy secrets_example.h to secrets.h and fill it in.

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
// Pinned library versions, kept inside the sketch (see src/README.md)
#include "src/PubSubClient/PubSubClient.h"     // 2.8
#include "src/ArduinoJson/ArduinoJson-v7.4.3.h"
#include "secrets.h"
#include "Korzhenevskiy_utils.h"
#include "Korzhenevskiy_tick_tack_2.h"

// ===================================================================================
// PIN DEFINITIONS
// ===================================================================================

// constexpr int LED_PIN = 8;

// ===================================================================================
// CONFIGURATION (saved in Preferences, changeable over MQTT)
// ===================================================================================

constexpr const char* PREFS_NAMESPACE = "config";

Preferences preferences;
uint32_t status_interval_ms = 60000;
bool some_flag = false;

// ===================================================================================
// NETWORK
// ===================================================================================

WiFiClient espClient;
PubSubClient client(espClient);

// ===================================================================================
// FUNCTION DECLARATIONS
// ===================================================================================

void callback(char* topic, byte* payload, unsigned int length);
void publish_json(const JsonDocument& doc);
void publish_status();
void read_config();
void save_config();

// ===================================================================================
// TIMERS
// ===================================================================================

Korzhenevskiy_tick_tack_2 timer_status(publish_status, status_interval_ms);

// ===================================================================================
// TIMER CALLBACKS
// ===================================================================================

void publish_status() {
  JsonDocument doc;
  doc["uptime_s"] = millis() / 1000;
  doc["rssi"] = WiFi.RSSI();
  doc["free_heap"] = ESP.getFreeHeap();
  doc["status_interval_ms"] = status_interval_ms;
  doc["some_flag"] = some_flag;
  publish_json(doc);
}

// ===================================================================================
// HELPERS
// ===================================================================================

void publish_json(const JsonDocument& doc) {
  String out;
  serializeJson(doc, out);
  client.publish(MQTT.topic_publish, out.c_str());
  Serial.print("Published: ");
  Serial.println(out);
}

void read_config() {
  preferences.begin(PREFS_NAMESPACE, true);  // true = read-only
  status_interval_ms = preferences.getUInt("status_ms", status_interval_ms);
  some_flag = preferences.getBool("some_flag", some_flag);
  preferences.end();
}

void save_config() {
  preferences.begin(PREFS_NAMESPACE, false);  // false = read-write
  preferences.putUInt("status_ms", status_interval_ms);
  preferences.putBool("some_flag", some_flag);
  preferences.end();
  Serial.println("Config saved");
}

// ===================================================================================
// SETUP
// ===================================================================================

void setup() {
  // pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  delay(100);
  Serial.println("\n=== BOOT ===");

  read_config();

  client.setBufferSize(1024);  // default is 256 bytes, too small for most JSON
  client.setServer(MQTT.server, MQTT.port);
  client.setCallback(callback);

  draw_cat();
  connect_to_best_wifi(WIFI_NETWORKS, std::size(WIFI_NETWORKS));
  connect_to_mqtt(client, MQTT);

  timer_status.setinterval(status_interval_ms);
  Korzhenevskiy_tick_tack_2::mass_start();

  Serial.println("=== SETUP COMPLETE ===");
}

// ===================================================================================
// LOOP
// ===================================================================================

void loop() {
  Korzhenevskiy_tick_tack_2::mass_tick();

  if (!client.connected()) {
    connect_to_mqtt(client, MQTT);
  }
  client.loop();
}

// ===================================================================================
// MQTT CALLBACK
// ===================================================================================

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT message: ");
  Serial.write(payload, length);
  Serial.println();

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.print(F("JSON parse failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Each key is one command. Plain "if" (not "else if") so one message can carry several.
  // is<T>() checks that the key exists AND has the right type.

  // ---------------------------------------------------------------------------------
  // SYSTEM COMMANDS   {"command": "reset"}   {"command": "status"}
  // ---------------------------------------------------------------------------------
  if (doc["command"].is<const char*>()) {
    String command = doc["command"].as<String>();

    if (command == "reset") {
      Serial.println("Restarting...");
      client.disconnect();
      ESP.restart();
    }

    if (command == "status") {
      publish_status();
    }
  }

  // ---------------------------------------------------------------------------------
  // SET STATUS INTERVAL   {"set_status_interval": 30000}
  // ---------------------------------------------------------------------------------
  if (doc["set_status_interval"].is<uint32_t>()) {
    uint32_t value = doc["set_status_interval"];
    if (value >= 1000) {
      status_interval_ms = value;
      timer_status.setinterval(status_interval_ms);
      save_config();
      Serial.print("status_interval_ms set to: ");
      Serial.println(status_interval_ms);
    }
  }

  // ---------------------------------------------------------------------------------
  // SOME FLAG   {"some_flag": true}   (true/false only, 1/0 is not a bool)
  // ---------------------------------------------------------------------------------
  if (doc["some_flag"].is<bool>()) {
    some_flag = doc["some_flag"];
    save_config();
    Serial.print("some_flag set to: ");
    Serial.println(some_flag ? "true" : "false");
  }
}
