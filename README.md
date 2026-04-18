# 4WD Robot Car

WiFi-controlled 4WD robot. ESP32 brain, TB6612FNG motor driver, 18650 battery power.

## Architecture

```
MacBook / Pi 5 (desk)
      |
      | WiFi
      |
    ESP32 (on robot)
      |
    TB6612FNG Motor Driver
    /          \
Left Motors    Right Motors
(x2 parallel)  (x2 parallel)
```

## Power

```
2x 18650 (8.4V) ──┬──> TB6612FNG VM ──> Motors (direct, full voltage)
                   │
                   └──> Buck Converter (LM2596S) ──> 5V ──> ESP32 VIN
                                                              |
                                                          3.3V regulator
                                                              |
                                                          TB6612FNG VCC
```

Two modes:
- **Dev mode**: ESP32 on USB (for flashing + serial monitor). Disconnect buck OUT+ from VIN.
- **Untethered mode**: USB unplugged, buck converter feeds 5V to VIN. Everything runs from battery.

## Parts

| Part | Spec |
|-|-|
| ESP32 DevKit | 30-pin |
| TB6612FNG | Dual H-bridge motor driver (MOSFET, 1.2A/ch) |
| 4WD Chassis Kit | 4 BO motors, 4 wheels, chassis plates |
| 18650 Cells | 2x 3.7V (8.4V series), in 2-cell holder with DC jack |
| Buck Converter | LM2596S with voltage display |
| Breadboard | Full-size, TB6612FNG mounted on it |
| Jumper Wires | M-M and M-F bundles |

## Wiring

Open `wiring-guide.html` in a browser for the full color-coded schematic, pin maps, and step-by-step instructions.

### Quick Reference (Phase 1: Left-Side Motors)

ESP32 to TB6612FNG (M-F wires):

| ESP32 | TB6612FNG | Purpose |
|-|-|-|
| GND | GND | Common ground |
| 3V3 | VCC | Logic power |
| GPIO 27 | AIN1 | Direction control 1 |
| GPIO 26 | AIN2 | Direction control 2 |
| GPIO 14 | PWMA | Speed control (PWM) |

Battery to TB6612FNG:

| From | To | Purpose |
|-|-|-|
| Battery + | VM | Motor power (8.4V) |
| Battery - | GND | Battery ground |

Motors (anti-parallel splice) to TB6612FNG:

| Spliced Wire | To | Purpose |
|-|-|-|
| Front Red + Rear Black | AO1 | Motor output 1 |
| Rear Red + Front Black | AO2 | Motor output 2 |

Buck converter (battery power for ESP32):

| From | To | Purpose |
|-|-|-|
| Breadboard VM row | Buck IN+ | Battery 8.4V in |
| Breadboard GND row | Buck IN- | Ground in |
| Buck OUT+ (5V) | ESP32 VIN | Regulated 5V to ESP32 |
| Buck OUT- | Breadboard GND row | Output ground |

### Phase 2: Right-Side Motors

| ESP32 | TB6612FNG | Purpose |
|-|-|-|
| GPIO 25 | BIN1 | Right motors direction 1 |
| GPIO 33 | BIN2 | Right motors direction 2 |
| GPIO 12 | PWMB | Right motors speed (PWM) |

Right-side motors get the same anti-parallel splice, connected to BO1/BO2.

## Motor Direction

| AIN1 | AIN2 | PWMA | Result |
|-|-|-|-|
| HIGH | LOW | 0-255 | Forward at speed |
| LOW | HIGH | 0-255 | Backward at speed |
| LOW | LOW | any | Coast (free-wheel) |
| HIGH | HIGH | any | Brake (locked stop) |

## Motor Wiring

The front and rear motors on each side spin opposite directions with the same polarity (due to mounting orientation). Splicing them anti-parallel (front-red + rear-black, rear-red + front-black) makes both wheels move in the same direction with a single TB6612FNG channel.

## Firmware

Test sketch at `firmware/motor_test/motor_test.ino`. Currently running code at `../robotics/motor_test/motor_test.ino`.

```bash
# compile
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/motor_test

# flash
arduino-cli upload --fqbn esp32:esp32:esp32 -p /dev/cu.usbserial-0001 firmware/motor_test

# serial monitor
arduino-cli monitor -p /dev/cu.usbserial-0001 -b 115200
```

## Docs

| File | What |
|-|-|
| `wiring-guide.html` | Color-coded schematic, pin maps, voltage reference, wiring steps |
| `week1-plan.md` | Week 1 build plan and study material |
| `week1-reading.md` | First-principles reading guide for electronics concepts |

