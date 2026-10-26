# Architecture and Conventions

How projects built from this template are organised. Written for humans and for AI coding assistants:
follow these conventions when adding code to a project made from this template.

## Platform

- ESP32 family (tested to compile on ESP32 and ESP32-C6, board package 3.x), Arduino framework.
- Arduino IDE sketch layout: one flat folder, the `.ino` named exactly like the folder, no `src/`.
- **Everything the project needs lives in the sketch folder**, so a fresh `git clone` builds anywhere.
  Nothing comes from the Arduino `libraries` folder:
  - Our own modules sit next to the `.ino`.
  - Public libraries are **pinned**: an exact copy goes into `src/<LibraryName>/` and is included by path,
    e.g. `#include "src/PubSubClient/PubSubClient.h"`, never `#include <PubSubClient.h>`.
  - Record each pinned library's version and source in `src/README.md`.
  - To upgrade, replace the files on purpose and test.
  - New libraries (hardware drivers etc.) are added the same way.
- The ESP32 board package is the only external dependency (tested with 3.0.5).

## Files

```
project_name/
  project_name.ino             setup(), loop(), callback(), project logic
  secrets.h                    gitignored: all logins, passwords and the MQTT identity
  secrets_example.h            committed copy of secrets.h with placeholder values
  Korzhenevskiy_utils.h/.cpp   WiFi + MQTT connection
  Korzhenevskiy_tick_tack_2.h/.cpp   non-blocking timers
  src/                         pinned public libraries + README.md with their versions
  ... project-specific modules (one .h/.cpp pair per device or subsystem, e.g. Radar.h/.cpp) ...
```

## secrets.h

The single place for credentials and the device's identity on the broker:

```cpp
#include "Korzhenevskiy_utils.h"  // WifiNetwork and MqttSettings structs

inline constexpr WifiNetwork WIFI_NETWORKS[] = {
  {"ssid_1", "pass_1"},
  {"ssid_2", "pass_2"},
};

inline constexpr MqttSettings MQTT = {
  .server          = "broker.address",
  .port            = 1883,
  .user            = "user",
  .password        = "password",
  .client_name     = "device_name",       // unique per device on the broker
  .topic_subscribe = "device_name",       // commands to the device
  .topic_publish   = "device_name_send",  // messages from the device
  .will_message    = "{\"online\": false}",
  .hello_message   = "{\"online\": true}",
};
```

- `inline constexpr` means `secrets.h` can be included from any number of `.cpp` files without
  "multiple definition" link errors.
- Designated initializers (`.server = ...`) must stay in the same order as the fields in `MqttSettings`.
- For a new project only `client_name`, the topics and the two messages change.

## Korzhenevskiy_utils

- `connect_to_best_wifi(WIFI_NETWORKS, std::size(WIFI_NETWORKS))` scans, picks the known network with the
  strongest signal and **blocks** until connected. Fine for a skeleton; replace it if the project must keep
  running without WiFi. After the first connection the ESP32 core reconnects WiFi by itself
  (`WiFi.setAutoReconnect` is on by default).
- `connect_to_mqtt(client, MQTT)` makes one attempt, no retry. On success it subscribes to
  `topic_subscribe` and publishes `hello_message`; the broker publishes `will_message` if the device drops.
  Returns `true` when connected.
- `draw_cat()` prints the boot cat.

The utils use no globals: the client and settings are passed in.

## Korzhenevskiy_tick_tack_2 (timers)

Non-blocking timers that replace `delay()`. **Nothing in `loop()` may block.**

```cpp
void publish_sensor();                                          // 1. declare the callback
Korzhenevskiy_tick_tack_2 timer_sensor(publish_sensor, 5000);   // 2. global timer, interval in ms
void publish_sensor() { ... }                                   // 3. define it anywhere below

// setup():  Korzhenevskiy_tick_tack_2::mass_start();   or   timer_sensor.start();
// loop():   Korzhenevskiy_tick_tack_2::mass_tick();    (required every iteration)
```

| Method | Description |
|---|---|
| `start()` / `start(ms)` | Start (optionally with a new interval) |
| `stop()` | Stop |
| `restart()` | Reset the clock without stopping |
| `setinterval(ms)` | Change the interval |
| `tick()` | Fire the callback if the interval has passed |
| `counts()` / `zero_count()` | How many times it fired / reset that |
| `enabled()` | Whether it is running |
| `mass_start()` / `mass_tick()` / `mass_stop()` | Static, act on every registered timer |
| `registry_overflow()` | Static, true if more than `MAX_TIMERS` (10) timers were created |

Timers register themselves by address, so declare them at global scope and don't copy them.
If more than 10 are created, the extras are not ticked by `mass_tick()` and `mass_start()` prints a warning.

## MQTT callback

```cpp
void callback(char* topic, byte* payload, unsigned int length) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) { /* log and return */ }

  if (doc["set_speed"].is<int>()) {
    int value = doc["set_speed"];
    ...
  }
}
```

- Each JSON key is one command, its value is the parameter.
- Use plain `if`, not `else if`, so one message can carry several commands. A command that must run alone
  (e.g. a long animation) can `return` early.
- Check commands with `doc["key"].is<T>()`: it checks that the key exists **and** has the right type.
  `containsKey()` is deprecated in ArduinoJson 7. Note that `is<bool>()` only accepts `true`/`false`, not `1`/`0`.
- System commands are values of the `"command"` key: `{"command": "reset"}`, `{"command": "status"}`.
- Parse straight from `payload`/`length`; no need to copy it into a `String` first.

## Publishing JSON

Build outgoing messages with ArduinoJson, not string concatenation:

```cpp
JsonDocument doc;
doc["temperature"] = 21.5;
doc["online"] = true;
publish_json(doc);   // serializes and publishes to MQTT.topic_publish
```

`client.setBufferSize(1024)` in `setup()` raises PubSubClient's 256-byte limit, which applies to both
incoming and outgoing messages. Raise it further if messages are big (desk robot animations used 4096).

## ArduinoJson 7 notes

- `JsonDocument doc;` replaces `StaticJsonDocument<N>` and `DynamicJsonDocument(N)`; it grows as needed.
- `doc["key"].is<T>()` replaces `containsKey`.
- `doc["list"].to<JsonArray>()` / `doc["obj"].to<JsonObject>()` replace `createNestedArray/Object`.
- `doc.overflowed()` tells you if memory ran out.
- `serializeJson(doc, str)` replaces the contents of `str`, it doesn't append.
- The pinned 7.4.3 includes the March 2026 fix for a buffer overrun when parsing long numbers.
  Never pin anything older.

## Persistent settings (Preferences)

Settings that MQTT can change are saved to flash and read back in `setup()`:

```cpp
preferences.begin("config", true);    // true = read-only
value = preferences.getUInt("key", default_value);
preferences.end();

preferences.begin("config", false);   // false = read-write
preferences.putUInt("key", value);
preferences.end();
```

Preferences keys are limited to 15 characters.

## setup() order

1. Pin modes, outputs to a safe state
2. `Serial.begin(115200)`
3. `read_config()`
4. Hardware init (sensors, displays). If something essential is missing, log it and halt.
5. MQTT client: `setBufferSize`, `setServer(MQTT.server, MQTT.port)`, `setCallback(callback)`
6. `draw_cat()`, `connect_to_best_wifi(...)`, `connect_to_mqtt(client, MQTT)`
7. Start timers

## loop() order

```cpp
void loop() {
  // 1. Fast hardware work (steppers, etc.)
  // 2. Korzhenevskiy_tick_tack_2::mass_tick();
  // 3. Sensor updates
  if (!client.connected()) connect_to_mqtt(client, MQTT);
  client.loop();   // every iteration, always last
}
```

## Coding style

- Arduino C++: globals at file scope grouped under banner comments, `String` is fine.
- Banners:

```cpp
// ===================================================================================
// SECTION NAME
// ===================================================================================
```

  and inside the callback:

```cpp
  // ---------------------------------------------------------------------------------
  // SUBSECTION NAME
  // ---------------------------------------------------------------------------------
```

- Naming:
  - constants and pins: `constexpr int UPPER_SNAKE_CASE`, not `#define`
  - variables and functions: `lower_snake_case`
  - classes: `PascalCase`
  - timers: `timer_` prefix
  - MQTT topics: `device_name` in, `device_name_send` out
- Log generously to Serial: every command received, every message published.
- Error handling stays minimal: a bad JSON message is logged and ignored.
