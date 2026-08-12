# Ultrasonic Radar (v1)

An Arduino project that sweeps an HC-SR04 ultrasonic sensor back and forth using
a servo motor and displays live distance + angle readings on a 16x2 LCD. Videos and photos are attached.

## Hardware

- Arduino Uno (or compatible)
- HC-SR04 ultrasonic distance sensor
- SG90 (or similar) servo motor
- 16x2 character LCD, HD44780-compatible (pin-header / non-I2C version)
- 10k potentiometer (for LCD contrast) — or a direct wire from V0 to GND
- 220 ohm resistor (for LCD backlight current limiting)
- Breadboard + jumper wires

## Wiring

### HC-SR04 Ultrasonic Sensor

| Sensor Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| Trig | Pin 6 |
| Echo | Pin 5 |

### Servo Motor

| Servo Wire | Arduino Pin |
|---|---|
| Signal | Pin 3 |
| Power (red) | 5V |
| Ground (brown/black) | GND |

### 16x2 LCD (4-bit mode)

| LCD Pin | Connects to |
|---|---|
| VSS | GND |
| VDD | 5V |
| V0 (contrast) | Potentiometer wiper (or straight to GND) |
| RS | Pin 12 |
| RW | GND |
| E | Pin 11 |
| D4 | Pin 7 |
| D5 | Pin 8 |
| D6 | Pin 9 |
| D7 | Pin 4 |
| A (backlight +) | 5V, through a 220 ohm resistor |
| K (backlight -) | GND |

D0–D3 on the LCD are left unconnected since this project uses 4-bit mode.

## How it works

1. The servo sweeps from 0° to 180° and back, one degree at a time.
2. At every step, the HC-SR04 sends an ultrasonic pulse (`trigPin`) and measures
   how long it takes to receive the echo (`echoPin`) using `pulseIn()`.
3. That duration is converted to a distance in cm using the speed of sound
   (343 m/s ≈ 0.0343 cm/microsecond), divided by 2 to account for the round trip.
4. The current distance and servo angle are printed to both the Serial Monitor
   (9600 baud) and the LCD — distance on the top row, angle on the bottom row.

Top down view of the circuit:
<img width="6048" height="8064" alt="IMG_2137" src="https://github.com/user-attachments/assets/84da7ff0-77b8-468b-8e93-de90a48e868a" />
Video of radar in practice:
https://github.com/user-attachments/assets/6ea901d2-bd42-4f4f-8545-09d28539632b

## Known issues





- ~~**LCD backlight on but no text**: adjust the contrast potentiometer through its~~
  ~~full range, or wire V0 directly to GND for a fixed max-contrast fallback.~~
- ~~**Distance always reads 0**: double-check Trig/Echo aren't swapped, and that~~
  ~~all breadboard connections are firmly seated (a split power rail on some
  breadboards is a common culprit).~~
- ~~**Erratic/huge distance values**: usually a loose or noisy connection on the~~
  ~~Echo line; reseat wires and consider adding a 10k pull-down resistor between
  Echo and GND.~~

## v1.1: Armed/Disarmed mode

V1.1 adds an armed/disarmed mode on top of everything
in v1, toggled with a push button.
Also an LED severity system has been added with a buzzer which changes frequencies based on proximity to other objects. 

- **Disarmed** (default on boot): sensor, servo, and LCD behave exactly like
  v1. LEDs and buzzer stay off no matter how close an object gets.
video in disarmed mode:

https://github.com/user-attachments/assets/8b57ed2e-3701-4053-8193-3ca0dde0644d



- **Armed**: LEDs light up in proportion to proximity, and the buzzer sounds
  depending on the proximity.
video in armed mode:

https://github.com/user-attachments/assets/f2e0f46c-fa36-4150-9da3-ce450dcb2d88



### New hardware

| Component | Arduino Pin | Other leg |
|---|---|---|
| Push button | Pin 2 | GND (uses internal pull-up, no resistor needed) |
| LED 1 (farthest band) | A1 | GND, through 220 ohm resistor |
| LED 2 (mid band) | A2 | GND, through 220 ohm resistor |
| LED 3 (closest band) | A3 | GND, through 220 ohm resistor |
| Buzzer | Pin 10 | GND |
Picture of circuit:
<img width="6048" height="8064" alt="IMG_2157" src="https://github.com/user-attachments/assets/5f8e18fe-1ec5-4cde-ab5a-c9a81f6ed194" />

### Proximity bands

Default thresholds (edit `NEAR_CM`, `MID_CM`, `FAR_CM` in the sketch to tune):

| Distance | LEDs lit | Buzzer |
|---|---|---|
| < 10 cm | All 3 | 1000hz |
| < 20 cm | 2 | 500hz |
| < 40 cm | 1 | 100hz |
| ≥ 40 cm | 0 | 0hz |

### Notes

- The button uses simple debouncing (50ms) so a single press reliably
  registers as one toggle, not several.
- The button is checked both at the top of `loop()` and mid-sweep (inside
  `calcDist()`), so arming/disarming feels responsive even though a full
  servo sweep takes several seconds.
### Known issues
- ~~Button needs to be clicked twice to disarm from armed mode~~
- ~~Button glitches from time to time~~

## v1.2: IR remote arm/disarm + joystick manual control

V1.2 adds two new ways to control the radar. Requires
the **IRremote** library (Sketch → Include Library → Manage Libraries →
search "IRremote" → install the one by Armin Joachimsmeyer). 
- added an ir controller to arm/disarm
- added a joystick to control servo.
- used built in joystick button to switch between manual control and sweep
- added a kill switch and a reset switch on the ir controller


### New hardware

| Component | Arduino Pin | Notes |
|---|---|---|
| IR receiver (e.g. TSOP38238) OUT | A0 | VCC → 5V, GND → GND |
| Joystick VRx | A4 | X axis controls servo angle |
| Joystick SW | Pin 13 | Uses internal pull-up, no resistor needed |

VRy (the joystick's Y axis) isn't used.

Image of circuit:
<img width="5792" height="4344" alt="IMG_2283" src="https://github.com/user-attachments/assets/41ad4eac-acac-46c1-b5e6-fe974ce22854" />

### Arm/disarm

The physical button (Pin 2) **toggles** arm/disarm. The IR remote uses two
dedicated buttons instead — Button 1 always arms, Button 2 always disarms —
so there's no ambiguity about which state you land in. Note this still keeps the button.
Video of Arming and disarming using IR remote:

https://github.com/user-attachments/assets/c265c4aa-e386-4518-bccf-44238a942f19



This project's remote codes are already set in the sketch:
```cpp
const unsigned long ARM_CODE    = 0xF30CFF00UL; // Button 1
const unsigned long DISARM_CODE = 0xE718FF00UL; // Button 2
```
### Sweep vs. manual aiming

Clicking the joystick's SW button toggles between:
- **Sweep mode** (default): servo auto-sweeps 0–180° and back, as in v1.
- **Manual mode**: servo angle follows the joystick's X axis directly.
Video of toggle between sweep mode and manual mode including use of manual mode:

https://github.com/user-attachments/assets/da248ad3-0686-4b68-8ca4-c2cd8ca745f2


### Why the sweep logic changed

Earlier versions used blocking `for` loops with `delay()` calls to sweep the
servo, which meant the Arduino couldn't check for button presses, IR codes,
or joystick input while a sweep was in progress. v1.2 rewrites the sweep as
a non-blocking step (`runSweepStep()`), timed with `millis()` instead of
`delay()`, so all inputs stay responsive at all times.

### Added a kill switch and reset switch on the ir controller

In previous versions, to disarm and detach the servo, you had to unplug the arduino and remove power from the circuit, which means that it wasn't practical in real world use cases. So, a kill switch is added which automatically disarms the radar and detaches the servo. It also stops taking distance readings and angle readings. To have the radar working again a restart button is used on the IR remote which starts the radar as if it was just plugged in.
Video of kill and restart in action:


https://github.com/user-attachments/assets/57ed3a88-ddad-436f-8d90-e7c41e58f124


### Known issues
- ~~issue with ir remote only working while disarmed, as soon as armed it doesn't respond due to sharing the internal timer with the buzzer~~
- ~~joystick toggle and control didn't work as expected~~
- ~~often needs multiple clicks to respond to arm command or a disarm command~~
- ~~joystick makes erratic movements when plugged in~~
- ~~kill switch doesn't kill sometimes~~
- 
## Version history

- **v1** — Initial working version: servo sweep, distance measurement, and
  live LCD display of distance + angle.
- **v1.1** — Added armed/disarmed mode via push button, with LED proximity
  ranking and a buzzer alert when armed.
- **v1.2** — Added IR remote for arm/disarm (Button 1 arms, Button 2
  disarms) alongside the physical button, and a joystick for manual servo
  aiming (toggled against auto-sweep via the joystick's click button).
  Sweep logic rewritten to be non-blocking. A kill switch and restart switch have been added to simplify turning the radar on and off
