# ESP32 MQTT Template

Skeleton for new ESP32 projects (Arduino framework): WiFi, MQTT with JSON commands, non-blocking timers,
settings saved in flash. Everything the sketch needs is in one folder, so `git clone` builds on any machine.

**Read [TODO.md](TODO.md)**: planned changes (espMqttClient instead of PubSubClient, FastAccelStepper for motors).

## Start a new project

1. Copy the `esp32_mqtt_template/` folder and rename it, e.g. `plant_watering/`.
2. Rename the `.ino` to match the folder (`plant_watering/plant_watering.ino`), as the Arduino IDE requires.
3. Copy `secrets_example.h` to `secrets.h` and fill it in: WiFi networks, broker login, and the device's
   client name and topics (`device_name` for commands, `device_name_send` for what it reports).
   `secrets.h` is gitignored.
4. Build and upload. Serial monitor at 115200 baud, enjoy the cat.

## Libraries

Nothing to install from the Library Manager. The libraries are pinned: exact copies live in the sketch's
`src/` folder and are included by path, so an update elsewhere can't change the build
(details in `esp32_mqtt_template/src/README.md`).

| Library | Pinned version |
|---|---|
| ArduinoJson (Benoit Blanchon) | 7.4.3, contains the March 2026 security fix |
| PubSubClient (Nick O'Leary) | 2.8 |

The only thing installed separately is the ESP32 board package (Espressif), tested with 3.0.5.

## Built-in MQTT commands

```json
{"command": "reset"}
{"command": "status"}
{"set_status_interval": 30000}
{"some_flag": true}
```

`status` replies on the publish topic with uptime, RSSI, free heap and the current settings. The same report
is sent every `status_interval_ms`.

## Files

```
esp32_mqtt_template/
  esp32_mqtt_template.ino       setup(), loop(), MQTT command handler, project logic
  secrets_example.h             copy to secrets.h: WiFi, MQTT login, device identity
  Korzhenevskiy_utils.h/.cpp    connect to strongest known WiFi, connect to MQTT, ASCII cat
  Korzhenevskiy_tick_tack_2.h/.cpp   non-blocking timers
  src/                          pinned copies of ArduinoJson and PubSubClient
ARCHITECTURE.md                 how the pieces fit and the coding conventions (also a guide for AI assistants)
```
