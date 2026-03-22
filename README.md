<div align="center">

```
██████╗  ██████╗ ██████╗  ██████╗     ███████╗ ██████╗  ██████╗ ██████╗███████╗██████╗
██╔══██╗██╔═══██╗██╔══██╗██╔═══██╗    ██╔════╝██╔═══██╗██╔════╝██╔════╝██╔════╝██╔══██╗
██████╔╝██║   ██║██████╔╝██║   ██║    ███████╗██║   ██║██║     ██║     █████╗  ██████╔╝
██╔══██╗██║   ██║██╔══██╗██║   ██║    ╚════██║██║   ██║██║     ██║     ██╔══╝  ██╔══██╗
██║  ██║╚██████╔╝██████╔╝╚██████╔╝    ███████║╚██████╔╝╚██████╗╚██████╗███████╗██║  ██║
╚═╝  ╚═╝ ╚═════╝ ╚═════╝  ╚═════╝    ╚══════╝ ╚═════╝  ╚═════╝ ╚═════╝╚══════╝╚═╝  ╚═╝
```

**Arduino · FlySky FS-iA6B · BTS7960 43A · Tank Drive**

![Arduino](https://img.shields.io/badge/Arduino-00979D?style=flat-square&logo=arduino&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Uno%20%2F%20Mega-blue?style=flat-square)
![Libraries](https://img.shields.io/badge/Libraries-None-orange?style=flat-square)

*A competitive robot soccer bot — fast, responsive, and built to win.*

</div>

---

## TABLE OF CONTENTS

- [Hardware](#hardware)
- [Pin Wiring](#pin-wiring)
- [How It Works](#how-it-works)
- [Getting Started](#getting-started)
- [Project Structure](#project-structure)
- [Tunable Parameters](#tunable-parameters)
- [Dependencies](#dependencies)
- [License](#license)

---

## HARDWARE

| Component | Details |
|:---|:---|
| Microcontroller | Arduino Uno / Mega |
| RC Receiver | FlySky FS-iA6B (or compatible PPM) |
| Motor Driver x2 | BTS7960 43A H-Bridge |
| Motors | DC gear motors (left & right side) |
| Power | 7.4V – 12V LiPo battery |

---

## PIN WIRING

### FlySky Receiver → Arduino

| Receiver Channel | Arduino Pin | Function |
|:---:|:---:|:---|
| CH1 | **D8** | Steering — Left / Right |
| CH2 | **D9** | Throttle — Forward / Backward |

### BTS7960 Driver 1 — LEFT Motors

| BTS7960 Pin | Arduino Pin | Type |
|:---:|:---:|:---:|
| RPWM | **D3** | PWM |
| LPWM | **D4** | PWM |
| REN  | D5 | Enable |
| LEN  | D6 | Enable |

### BTS7960 Driver 2 — RIGHT Motors

| BTS7960 Pin | Arduino Pin | Type |
|:---:|:---:|:---:|
| RPWM | **D7** | PWM |
| LPWM | **D2** | PWM |
| REN  | D11 | Enable |
| LEN  | D12 | Enable |

---

## HOW IT WORKS

### 1 — PPM Signal Reading

The FlySky receiver outputs a **PPM signal (1000 – 2000 µs)**.  
`pulseIn()` reads each channel with a 25 ms timeout:

| Value | Meaning |
|:---:|:---|
| `1000 µs` | Full left / Full reverse |
| `1500 µs` | Center / Stopped |
| `2000 µs` | Full right / Full forward |

> If no signal is detected within 25 ms, the bot safely returns `1500` (stopped) as a failsafe.

---

### 2 — Tank-Style Mixing

Throttle and steering channels are combined for independent left/right wheel control:

```
leftSpeed  = throttle + steering
rightSpeed = throttle - steering
```

Both values are clamped to `[-255, 255]` for PWM output.  
This enables forward, reverse, on-the-spot pivots, and arc turns — essential for tracking a ball.

---

### 3 — Dead Zone

A **±15 dead zone** filters joystick drift near center.  
The bot stays completely still when sticks are released — no creeping during gameplay pauses.

---

## GETTING STARTED

**Step 1 — Clone the repository**

```bash
git clone https://github.com/yashwanthR1207/robo-soccer.git
cd robo-soccer
```

**Step 2 — Open in Arduino IDE**

- Open `robo_soccer/robo_soccer.ino`
- Go to **Tools → Board** → select your Arduino model
- Select the correct **Port**
- Click **Upload**

**Step 3 — Serial Monitor**

Open Serial Monitor at **9600 baud** to watch live motor output:

```
Robo Soccer Ready!
L:200  R:180
L:255  R:100
L:0    R:0
```

---

## PROJECT STRUCTURE

```
robo-soccer/
├── robo_soccer/
│   └── robo_soccer.ino       # Main Arduino sketch
└── README.md
```

---

## TUNABLE PARAMETERS

All adjustable values are defined at the top of `robo_soccer.ino`:

| Parameter | Location | Default | Description |
|:---|:---|:---:|:---|
| `DEAD_ZONE` | top of sketch | `15` | Minimum speed before motor activates |
| Failsafe value | `readChannel()` | `1500` | PPM value returned on signal loss |
| PWM timeout | `readChannel()` | `25000 µs` | Max wait time for incoming pulse |

---

## DEPENDENCIES

No external libraries required.  
Uses **Arduino core only** — works out of the box with any standard Arduino IDE installation.

---


---

<div align="center">

Built by **Yashwanth R** — [@yashwanthR1207](https://github.com/yashwanthR1207)

</div>
