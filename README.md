# ESP32 MQTT Template

A ready-to-use ESP32 template with WiFi, MQTT, OTA updates, and a custom timer library.

## Features

- **Auto WiFi Connection** - Automatically connects to the strongest available network from your list
- **MQTT Integration** - PubSubClient with JSON message handling
- **OTA Updates** - Upload new firmware wirelessly
- **Custom Timer Library** - Non-blocking timers with callback support (`Korzhenevskiy_tick_tack_2`)
- **Multi-timer Management** - Start/stop/tick multiple timers at once

## Quick Start

1. **Configure credentials** in `secrets.h`:
   - WiFi networks (SSID & passwords)
   - MQTT broker details
   - OTA password

2. **Upload** to ESP32

3. **Monitor** via Serial (115200 baud)

## Timer Usage

```cpp
void my_callback() {
    Serial.println("Timer fired!");
}

Korzhenevskiy_tick_tack_2 timer(my_callback, 5000); // 5 second timer

void setup() {
    timer.start();
    // OR start all timers at once:
    // Korzhenevskiy_tick_tack_2::mass_start();
}

void loop() {
    timer.tick();
    // OR tick all timers at once:
    // Korzhenevskiy_tick_tack_2::mass_tick();
}
```

## MQTT Commands

Send JSON to control the ESP:

```json
{"command": "reset"}
{"enable_something": 1}
{"some_integer": 42}
```

## Files

- `template.ino` - Main sketch
- `Korzhenevskiy_tick_tack_2.*` - Timer library
- `Korzhenevskiy_utils.*` - WiFi and MQTT utilities
- `secrets.h` - Configuration (add your credentials here)

## Requirements

- ESP32 board
- Arduino IDE / PlatformIO
- Libraries: WiFi, PubSubClient, ArduinoJson, ArduinoOTA

---

*Enjoy the cat ASCII art on boot! 🐱*
