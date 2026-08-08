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

## Known issues

- ~~**LCD backlight on but no text**: adjust the contrast potentiometer through its~~
  ~~full range, or wire V0 directly to GND for a fixed max-contrast fallback.~~
- ~~**Distance always reads 0**: double-check Trig/Echo aren't swapped, and that~~
  ~~all breadboard connections are firmly seated (a split power rail on some
  breadboards is a common culprit).~~
- ~~**Erratic/huge distance values**: usually a loose or noisy connection on the~~
  ~~Echo line; reseat wires and consider adding a 10k pull-down resistor between
  Echo and GND.~~
## Version history

- **v1** — Initial working version: servo sweep, distance measurement, and
  live LCD display of distance + angle.
