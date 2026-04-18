# Week 1 - Metro Reading Guide (90 min)

First principles for a software engineer getting into robotics. No code -- just the mental models you need before touching hardware.

## Part 1: Electricity Fundamentals (15 min)

Think of electricity like data flow with physical constraints.

**Three things to internalize:**

- **Voltage (V)** = pressure. Your 18650 cells are 3.7V each, two in series = 7.4V. The "force" pushing electrons.
- **Current (A)** = flow rate. A BO motor draws ~200-300mA. The ESP32 draws ~240mA when WiFi is active. The L298N can supply up to 2A per channel. Current is what actually does work (spins motors, heats things up, kills components).
- **Resistance (ohms)** = restriction. Ohm's law: V = I * R. That's it. Everything else derives from this.

**Why this matters for your build:** Your ESP32 GPIO pins output 3.3V at ~12mA max. A BO motor needs 3-6V at 200-300mA. You literally cannot connect a motor to a GPIO pin -- it would fry the ESP32. This is why the L298N exists.

**Read:** Search "SparkFun voltage current resistance" -- their tutorial is the best concise primer. 10 minutes.

## Part 2: How DC Motors Actually Work (10 min)

A DC motor is a coil of wire sitting in a magnetic field. Push current through the coil, it creates an electromagnetic field that interacts with the permanent magnets, and the shaft spins. Reverse the current direction, the shaft spins the other way.

**Key concepts:**

- **Stall current** -- the current drawn when the motor is physically blocked from spinning. This is the maximum current the motor will ever draw. Relevant because it's what your driver must handle.
- **Back-EMF** -- when a motor spins, it generates voltage *back* into the circuit (it's also a generator). This is why motors draw less current at full speed than at startup.
- **PWM for speed control** -- instead of reducing voltage (wasteful), you rapidly switch the motor on and off. 50% duty cycle = motor gets power half the time = roughly half speed. The ESP32 does this natively with its LEDC peripheral.

Your BO motors are geared DC motors -- a small motor connected to a gearbox that trades speed for torque. That's why they spin slowly but can push the chassis.

**Read:** "How DC motors work - Learn Engineering" article or the Jared Owen video from the week 1 plan.

## Part 3: The H-Bridge -- Why TB6612FNG Exists (15 min)

This is the single most important concept for week 1.

**The problem:** To make a motor go forward, current flows A -> B. To reverse, current must flow B -> A. A simple on/off switch can't do this.

**The solution:** Four switches arranged in an H pattern:

```
+V ---- S1 ----+---- S3 ---- +V
                |
              MOTOR
                |
GND ---- S2 ----+---- S4 ---- GND
```

- Close S1 + S4: current flows left-to-right through motor. Forward.
- Close S3 + S2: current flows right-to-left. Reverse.
- Close S1 + S3: both sides connected to +V. **Short circuit. Bad.**
- Close S2 + S4: both sides connected to GND. Motor brakes (coasts to stop).

The TB6612FNG contains two H-bridges (one per motor channel) built from MOSFETs. It takes logic-level inputs from the ESP32 (3.3V, microamps) and switches the motor power (7.4V, hundreds of milliamps).

**TB6612FNG vs L298N:** The L298N uses BJT transistors which have a ~1.4V drop per bridge -- your 7.4V battery would only deliver ~6V to the motors. The TB6612FNG uses MOSFETs with near-zero voltage drop, so motors get almost the full battery voltage. It's also smaller, runs cooler, and doesn't need a heatsink.

**TB6612FNG pins you'll wire:**

| Pin | Purpose |
|-|-|
| AIN1, AIN2 | Logic inputs for Channel A / left motors (from ESP32 GPIOs) |
| BIN1, BIN2 | Logic inputs for Channel B / right motors |
| PWMA, PWMB | PWM speed control for each channel (from ESP32 PWM pins) |
| AO1, AO2 | Channel A motor outputs (left motors) |
| BO1, BO2 | Channel B motor outputs (right motors) |
| VM | Motor battery input (7.4V) |
| VCC | Logic power input (3.3V from ESP32) |
| STBY | Standby pin -- must be HIGH for driver to work |
| GND | Common ground (connect to battery GND AND ESP32 GND) |

**4WD with 2 channels:** You have 4 motors but only 2 channels. Wire the two left motors in parallel on Channel A (both red wires to AO1, both black to AO2) and the two right motors in parallel on Channel B. This gives tank/differential steering.

**Critical concept: common ground.** The ESP32 and TB6612FNG must share the same GND reference, otherwise the logic signals are meaningless. This is the #1 beginner wiring mistake.

**Read:** Search "TB6612FNG motor driver tutorial" -- the Sparkfun hookup guide is the best reference.

## Part 4: ESP32 -- The Microcontroller Mental Model (20 min)

Rethink what a "computer" is. The ESP32 is:

- 240MHz dual-core processor
- 520KB RAM
- 4MB flash (where your code lives)
- WiFi + Bluetooth built in
- 34 GPIO pins (General Purpose Input/Output)
- No OS (unless you put one on). Your code IS the entire runtime.

### GPIO -- the bridge between software and physical world

Each GPIO pin can be configured as:

- **Digital output** -- 3.3V (HIGH) or 0V (LOW). Like a boolean. This is how you tell the L298N "motor A forward" (IN1=HIGH, IN2=LOW).
- **Digital input** -- read whether something external is applying voltage. Week 2's ultrasonic sensor uses this.
- **PWM output** -- rapid HIGH/LOW switching at a configurable frequency and duty cycle. This is how you control motor speed via ENA/ENB.
- **ADC input** -- read analog voltage (0-3.3V mapped to 0-4095). For sensors that output variable voltage.

### The firmware loop

```
setup()    -- runs once at boot. Configure pins, start WiFi, etc.
loop()     -- runs forever after setup. Check for commands, drive motors.
```

There's no scheduler, no process manager, no kernel. If your loop() blocks, everything stops. WiFi handling runs on the second core automatically (in Arduino framework), but your logic is single-threaded unless you explicitly use FreeRTOS tasks.

### MicroPython vs Arduino (C++)

Both work. Arduino gives you more examples and better motor library support. MicroPython lets you iterate faster (no compile step, REPL over serial). For a WiFi-controlled car, either is fine. Pick based on what you want to learn.

**Read:**
- https://randomnerdtutorials.com/getting-started-with-esp32/ -- focus on pin reference, flashing code, WiFi station vs AP mode
- https://randomnerdtutorials.com/esp32-pinout-reference/ -- bookmark this, you'll use it every day

## Part 5: Power -- The Thing That Will Bite You (10 min)

### 18650 cells

- Nominal voltage: 3.7V (fully charged: 4.2V, dead: 3.0V)
- Two in series: 7.4V nominal, 8.4V fully charged
- They're lithium-ion. They can deliver serious current. A short circuit can cause fire. Not theoretical -- it happens.

### Power distribution in your robot

```
Battery (7.4V)
  |
  +--> TB6612FNG VM pin (powers motors directly, near-zero voltage drop)

ESP32 USB (from laptop)
  |
  +--> ESP32 onboard regulator --> 3.3V
                                     |
                                     +--> TB6612FNG VCC (logic power)
                                     +--> TB6612FNG STBY (driver enable)
```

The TB6612FNG does NOT have an onboard voltage regulator like the L298N. It needs separate logic power (VCC, 2.7-5.5V) and motor power (VM, up to 15V). During development, the ESP32 runs off USB and supplies 3.3V to VCC. For untethered operation later, you'd add a small buck converter from the battery to power the ESP32's VIN pin.

### Why TB6612FNG is better than L298N for this build

The L298N uses BJT transistors with a ~1.4V drop per bridge -- 7.4V battery delivers only ~6V to motors. The TB6612FNG uses MOSFETs with near-zero drop, so motors get almost the full 7.4V. More power to the wheels, less wasted as heat.

### Rules

- Never connect batteries in reverse polarity
- Never connect the ESP32 3.3V pin directly to a 5V source
- Always connect GND between all components first before connecting power
- If something smells hot, disconnect power immediately

## Part 6: WiFi Control Architecture (10 min)

Your week 1 goal is: browser on phone -> WiFi -> ESP32 -> motors.

The ESP32 runs in **Access Point (AP) mode** -- it creates its own WiFi network. Your phone connects to it. No router needed. The ESP32 runs a tiny HTTP server. You visit its IP (usually 192.168.4.1) in a browser. The page has buttons (forward, back, left, right). Each button hits an endpoint. The handler sets GPIO pins accordingly.

```
GET /forward  -> AIN1=HIGH, AIN2=LOW, BIN1=HIGH, BIN2=LOW, PWMA=255, PWMB=255
GET /left     -> AIN1=LOW,  AIN2=LOW, BIN1=HIGH, BIN2=LOW, PWMA=0,   PWMB=255
GET /stop     -> AIN1=LOW,  AIN2=LOW, BIN1=LOW,  BIN2=LOW, PWMA=0,   PWMB=0
```

That's it. It's a REST API that controls GPIO pins. You already know how HTTP servers work -- the only new part is that the "response" is a physical action in the real world.

**Alternative:** If you want smoother control (hold-to-move instead of click), use WebSocket for persistent connection. But HTTP GET is fine for week 1.

## Part 7: The Full Signal Chain (10 min)

From your finger to the wheel:

1. You tap "Forward" on your phone browser
2. HTTP GET request over WiFi to ESP32
3. ESP32 request handler sets GPIO 27=HIGH, GPIO 26=LOW (Channel A forward), GPIO 14=PWM 255 (full speed)
4. GPIO 27 outputs 3.3V, GPIO 26 outputs 0V, GPIO 14 outputs PWM signal
5. TB6612FNG AIN1 sees HIGH, AIN2 sees LOW, PWMA sees duty cycle
6. TB6612FNG H-bridge MOSFETs switch on, connecting battery power through the motor
7. Battery current flows through both left motors (wired in parallel) in the forward direction
8. Motor shafts spin, gearboxes reduce speed and increase torque
9. Left wheels turn. Same logic on Channel B for right wheels. Robot moves.

Every layer is a translation: HTTP -> GPIO -> H-bridge -> current -> mechanical rotation. Week 1 is about understanding and building this chain end to end.

## Reading List

Open these tabs before you go underground:

1. **SparkFun: Voltage, Current, Resistance** -- search "sparkfun voltage current resistance ohms law"
2. **RandomNerdTutorials ESP32 Getting Started** -- https://randomnerdtutorials.com/getting-started-with-esp32/
3. **RandomNerdTutorials ESP32 Pinout** -- https://randomnerdtutorials.com/esp32-pinout-reference/
4. **SparkFun TB6612FNG Hookup Guide** -- search "sparkfun tb6612fng hookup guide"
5. **ESP32 PWM tutorial** -- search "randomnerdtutorials esp32 pwm"

If you still have time, search "esp32 web server control motor" on RandomNerdTutorials -- they have a walkthrough of exactly the WiFi car you're building.

By the time you're done, the hardware will feel like APIs with physical side effects. Week 2 (ultrasonic sensor + obstacle avoidance) will just be adding an input to this same loop.
