#ifndef KORZHENEVSKIY_UTILS_H
#define KORZHENEVSKIY_UTILS_H

#include <Arduino.h>
#include <WiFi.h>
#include "src/PubSubClient/PubSubClient.h"  // pinned 2.8, see src/README.md

// One known WiFi network. The list lives in secrets.h.
struct WifiNetwork {
  const char* ssid;
  const char* pass;
};

// Everything needed to connect to the broker. The values live in secrets.h.
struct MqttSettings {
  const char* server;
  uint16_t port;
  const char* user;
  const char* password;
  const char* client_name;      // must be unique on the broker
  const char* topic_subscribe;  // commands arrive here, e.g. "device_name"
  const char* topic_publish;    // device reports here, e.g. "device_name_send"
  const char* will_message;     // published by the broker if the device drops off
  const char* hello_message;    // published by the device after connecting
};

// Scans, picks the known network with the strongest signal and blocks until connected.
void connect_to_best_wifi(const WifiNetwork networks[], size_t count);

// One connection attempt, no retry. On success subscribes and publishes the hello message.
bool connect_to_mqtt(PubSubClient& client, const MqttSettings& settings);

void draw_cat();

#endif // KORZHENEVSKIY_UTILS_H
