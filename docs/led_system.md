# LED System Documentation

This project now uses a lightweight SimpleLED helper instead of the previous LEDController class.

## Audio visualization LEDs

- External LEDs on GPIO 11 and 12 are driven with PWM.
- Brightness is updated by a repeating timer in `src/main.cpp` using `AudioController::getAudioIntensity()` with a deadzone, gamma, and a short peak-hold.
- Onboard LED is not used.

See `src/main.cpp` and `include/SimpleLED.h` for the current implementation and API.

Wiring (example):

```text
Pico W          LEDs
------          ----
GPIO 11  -----> LED 1 Anode (via 220Ω)
GPIO 12  -----> LED 2 Anode (via 220Ω)
GND      -----> Both cathodes
```
