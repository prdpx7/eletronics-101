# Week 1 - Build a WiFi-Controlled 4WD Robot

## Setup

- Raspberry Pi 5 (16GB) stays on desk as main computer
- ESP32 is the robot's onboard brain (tiny, 10 grams)
- MacBook or Pi 5 sends commands over WiFi to ESP32
- ESP32 drives motors through TB6612FNG motor driver

## Architecture

```
MacBook --> SSH --> Pi 5 (on desk)
                     |
                     | WiFi
                     |
                   ESP32 (on robot)
                     |
                   TB6612FNG Motor Driver
                   /          \
          Left Motors (x2)   Right Motors (x2)
          (wired parallel)   (wired parallel)

Power on robot:
  18650 batteries (7.4V) --> TB6612FNG VM --> Motors
  ESP32 powered via USB from laptop (during dev)
  TB6612FNG logic (VCC) powered from ESP32 3.3V
```

## Parts List (Acquired)

| # | Item | Qty |
|-|-|-|
| 1 | ESP32 Dev Board (30-pin) | 1 |
| 2 | 4WD Smart Car Chassis Kit (4 BO motors, 4 wheels, chassis plate, screws) | 1 |
| 3 | TB6612FNG Motor Driver Module | 1 |
| 4 | 18650 Li-ion Cells (3.7V) | 2 |
| 5 | 18650 2-cell Battery Holder with DC jack | 1 |
| 6 | Male-to-Female Jumper Wires (40-pin bundle) | 1 |
| 7 | Male-to-Male Jumper Wires (40-pin bundle) | 1 |
| 8 | Mini Breadboard (SYB-170, self-adhesive) | 1 |

## What Each Part Does

**ESP32 Dev Board** - Microcontroller with built-in WiFi and Bluetooth. The robot's brain. You flash code onto it from MacBook. It receives WiFi commands and controls the motors.

**4WD Chassis Kit** - The robot's body. Four wheels driven by four BO motors (simple DC motors, 3-6V). Two per side, wired in parallel -- TB6612FNG has 2 channels so left pair shares Channel A, right pair shares Channel B. This gives tank/differential steering.

**TB6612FNG Motor Driver** - Dual H-bridge module using MOSFETs. Takes logic signals from ESP32 (3.3V) and switches motor power (7.4V). Advantages over L298N: near-zero voltage drop (MOSFETs vs BJTs), smaller, more efficient, no heatsink needed. 1.2A continuous per channel (3.2A peak). Pins: AIN1/AIN2/PWMA for Channel A, BIN1/BIN2/PWMB for Channel B, STBY to enable, VM for motor power, VCC for logic power.

**18650 Batteries** - Rechargeable lithium-ion cells. Two in series = 7.4V (8.4V fully charged). Powers the motors through TB6612FNG VM pin. ESP32 runs off USB during development.

**Jumper Wires** - M-F connects ESP32 pins to breadboard. M-M connects breadboard points to each other or for short hops on the breadboard.

**Mini Breadboard (SYB-170)** - TB6612FNG straddles the center gap. Remaining holes in each column connect to the same pin, so you plug jumper wires into those free holes.

## Weekend 1 Plan

### Saturday

- Assemble chassis, mount motors, attach wheels
- Place TB6612FNG on breadboard
- Wire motors to TB6612FNG outputs (left pair parallel to AO1/AO2, right pair parallel to BO1/BO2)
- Wire battery holder to VM/GND
- Wire ESP32 to TB6612FNG logic pins (see wiring-guide.html for exact pin map)
- Flash motor test sketch -- verify all 4 wheels spin correctly (wheels off ground)

### Sunday

- Add WiFi control: ESP32 hosts a web server in AP mode, you send forward/backward/left/right commands from browser on phone
- Drive the robot around your room from your phone

## Wiring Reference

Full visual wiring guide with diagrams: `wiring-guide.html` (open in browser)

Quick pin map:

| ESP32 | TB6612FNG | Purpose |
|-|-|-|
| GND | GND | Common ground (wire first) |
| 3V3 | VCC | Logic power for driver |
| 3V3 or GPIO 32 | STBY | Driver enable (HIGH = active) |
| GPIO 27 | AIN1 | Left motors direction 1 |
| GPIO 26 | AIN2 | Left motors direction 2 |
| GPIO 25 | BIN1 | Right motors direction 1 |
| GPIO 33 | BIN2 | Right motors direction 2 |
| GPIO 14 | PWMA | Left motors speed (PWM) |
| GPIO 12 | PWMB | Right motors speed (PWM) |
| -- | VM | Battery + (7.4V) |
| -- | AO1, AO2 | Left motors (front+rear parallel) |
| -- | BO1, BO2 | Right motors (front+rear parallel) |

## Study Material

### Understanding the Components

1. **How DC Motors work** - YouTube: "How DC Motors Work" by Jared Owen (~5 min)
2. **H-Bridge explained** - YouTube: "H Bridge Motor Driver" by DroneBot Workshop (~15 min)
3. **TB6612FNG vs L298N** - TB6612FNG uses MOSFETs (near-zero voltage drop) vs L298N's BJTs (~1.4V drop per bridge). Smaller, cooler, more efficient. Same concept, better execution.
4. **ESP32 getting started** - YouTube: "ESP32 getting started" by DroneBot Workshop - setup, flashing, WiFi basics
5. **ESP32 robot car** - YouTube: "ESP32 robot car WiFi control" - multiple tutorials showing this build

### Reading

- https://randomnerdtutorials.com/getting-started-with-esp32/ - best ESP32 beginner guide
- https://randomnerdtutorials.com/esp32-pinout-reference/ - ESP32 pin diagram (bookmark this)

## What Comes After Week 1

| Weekend | Goal |
|-|-|
| 2 | Add HC-SR04 ultrasonic sensor + logic level converter for obstacle avoidance |
| 3 | Add camera module on Pi 5, process video feed, send movement commands to ESP32 |
| 4 | Web/mobile dashboard to control and monitor robot, demo-ready prototype |
