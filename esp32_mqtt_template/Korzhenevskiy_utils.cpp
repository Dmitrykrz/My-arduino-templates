#include "Korzhenevskiy_utils.h"

void draw_cat() {

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");
  Serial.println("   xx                xx");
  Serial.println("   xxxx            xxxx                 xxxxxxx");
  Serial.println("  x   xxxxxxxxxxxxxx  xx             xxxx     xxxxxxx");
  Serial.println(" xx                    x             xx              xxx");
  Serial.println("xx   xxxx     xxxxxx   xx             xxxxxxxxxxxx     xx");
  Serial.println("x  xxx  xx    x    xx   x                        xxx    xx");
  Serial.println("x   xxxxxx    xxxxxxx   x                       xxx      x");
  Serial.println("xxx                     xxxxxxxxxxxxxxxxxxxxxxxxx        x");
  Serial.println("  xxxxxxx        xxxxxxxx                xxx             x");
  Serial.println("        xxxxxxxxxx  x                   xx               x");
  Serial.println("                    x                   xx               x");
  Serial.println("                    x                    x               x");
  Serial.println("              xxxxxx             xxxxxxxxx              xx");
  Serial.println("             x                  xx                 xxxxx");
  Serial.println("             xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");
}

void connect_to_best_wifi(const WifiNetwork networks[], size_t count) {

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Scan start");
    int n = WiFi.scanNetworks();
    Serial.println("Scan done");

    int best_index = -1;
    int best_rssi = -500;

    if (n <= 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            for (size_t k = 0; k < count; k++) {
                if (WiFi.SSID(i) == networks[k].ssid) {
                    int rssi = WiFi.RSSI(i);
                    Serial.print("found ");
                    Serial.print(networks[k].ssid);
                    Serial.print(" ");
                    Serial.println(rssi);
                    if (rssi > best_rssi) {
                        best_rssi = rssi;
                        best_index = k;
                    }
                }
            }
        }
    }
    WiFi.scanDelete();

    if (best_index == -1) {
        Serial.println("No known network found");
        return;
    }

    Serial.print("Connecting to: ");
    Serial.println(networks[best_index].ssid);
    WiFi.begin(networks[best_index].ssid, networks[best_index].pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

bool connect_to_mqtt(PubSubClient& client, const MqttSettings& settings) {
  if (client.connected()) return true;

  Serial.println("Attempting MQTT connection...");

  // connect(clientID, username, password, willTopic, willQoS, willRetain, willMessage)
  if (client.connect(settings.client_name, settings.user, settings.password,
                     settings.topic_publish, 1, false, settings.will_message)) {
    Serial.println("MQTT connected");
    client.subscribe(settings.topic_subscribe);
    client.publish(settings.topic_publish, settings.hello_message);
    return true;
  }

  Serial.print("MQTT connect failed, state ");
  Serial.println(client.state());
  return false;
}
