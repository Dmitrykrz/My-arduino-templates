// Copy this file to secrets.h (gitignored) and fill it in.
// All logins, passwords and the device's MQTT identity live here.

#ifndef SECRETS_H
#define SECRETS_H

#include "Korzhenevskiy_utils.h"  // WifiNetwork and MqttSettings structs

// --- WiFi: connects to the strongest of these ---
inline constexpr WifiNetwork WIFI_NETWORKS[] = {
  {"your_ssid_1", "your_pass_1"},
  {"your_ssid_2", "your_pass_2"},
};

// --- MQTT ---
inline constexpr MqttSettings MQTT = {
  .server          = "your.broker.address",
  .port            = 1883,
  .user            = "",
  .password        = "",
  .client_name     = "esp32_template",       // unique per device
  .topic_subscribe = "esp32_template",       // commands to the device
  .topic_publish   = "esp32_template_send",  // messages from the device
  .will_message    = "{\"online\": false}",
  .hello_message   = "{\"online\": true}",
};

#endif // SECRETS_H
