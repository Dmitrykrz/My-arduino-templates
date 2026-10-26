# TODO

## Replace PubSubClient with espMqttClient

PubSubClient is no longer maintained (last release 2.8, 2020; the author recommends an alternative for
new projects). [espMqttClient](https://github.com/bertmelis/espMqttClient) (Bert Melis) is the replacement:

- On ESP32 the connection runs in its own task, so a broker outage no longer stalls `loop()`
  (with PubSubClient every reconnect attempt blocks, which freezes motors and timers).
- Virtually unlimited payload size, so no more `setBufferSize()`.
- QoS 0, 1 and 2, TLS, MQTT 3.1.1.

Steps:
- Pin an exact release as a copy in `esp32_mqtt_template/src/espMqttClient/` and record it in `src/README.md`.
- Rewrite `connect_to_mqtt()`, the reconnect in `loop()` and `callback()` for the new API.
- While at it, make the will and hello messages retained, so Node-RED always sees online/offline.
- Remove `src/PubSubClient/`, update README.md and ARCHITECTURE.md, test on ESP32-C6 and ESP32.

## Add FastAccelStepper for stepper projects

[FastAccelStepper](https://github.com/gin66/FastAccelStepper) replaces AccelStepper. Step pulses come from
hardware timers or interrupts, not from `loop()` calling `run()`, so motion stays smooth while WiFi, MQTT
or anything else is busy. Supports ESP32 and ESP32-C6.

Steps:
- It does not compile on Arduino-ESP32 core 3.0.x (needs ESP-IDF 5.3+). Upgrade the core first (tested
  with 3.0.5 so far) and pin it, e.g. with an arduino-cli `sketch.yaml` profile.
- Pin an exact release as a copy in `src/FastAccelStepper/` and record it in `src/README.md`.
- Add a small optional motion module (e.g. `Motors.h/.cpp` with `begin()`, `move()`, enable-pin handling)
  plus a documented example, without making motors mandatory in the skeleton.
- Document the pin restrictions per chip from the library's docs.
