# LED System Documentation

This project now uses a lightweight SimpleLED helper instead of the previous LEDController class.

## LEDs overview

- Eye stalk: 1 blue LED (Bluetooth status) on GPIO 15. Blinks while pairing, solid when paired.
- Dome: 2 red LEDs (audio visualization) on GPIO 11 and GPIO 12 driven by PWM.

## Audio visualization LEDs

- External LEDs on GPIO 11 and 12 are driven with PWM.
- Brightness is updated by a repeating timer in `src/main.cpp` using `AudioController::getAudioIntensity()` with a deadzone, gamma, and a short peak-hold.
- Onboard LED is not used.

See `src/main.cpp` and `include/SimpleLED.h` for the current implementation and API.

Wiring (dome LEDs):

```text
Pico W          LEDs
------          ----
GPIO 11  -----> LED 1 (red) Anode (via 150Ω; 100Ω if brighter)
GPIO 12  -----> LED 2 (red) Anode (via 150Ω; 100Ω if brighter)
GND      -----> Both cathodes
```

Wiring (eye-stalk status LED):

```text
Pico W          LED (Blue)
------          ---------
GPIO 15  -----> Anode (via 100–150Ω)
GND      -----> Cathode
```

Resistor and driver guidance:

- Direct from 3.3 V GPIO is safe at modest currents.
	- Red (Vf ≈ 2.0–2.2 V): 150Ω → ~7–9 mA; 100Ω → ~11–13 mA (upper end per pin)
	- Blue (Vf ≈ 3.0–3.2 V): 100–150Ω → ~1–3 mA (limited headroom at 3.3 V)
- For true ~20 mA brightness:
	- Red at 3.3 V: use a transistor/MOSFET driver and 62–68Ω series resistor.
	- Blue: use a 5 V LED supply with a driver and 100Ω series resistor (common ground).
