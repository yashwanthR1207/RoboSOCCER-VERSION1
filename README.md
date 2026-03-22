# 🤖 RC Tank — Arduino + FlySky + BTS7960

A tank-drive RC robot controlled via a **FlySky receiver** with dual **BTS7960 motor drivers** and an **Arduino**. Supports full tank-style mixing for smooth forward, backward, and pivot turns.

---

## 📦 Hardware

| Component | Details |
|---|---|
| Microcontroller | Arduino Uno / Mega |
| RC Receiver | FlySky FS-iA6B (or compatible PPM) |
| Motor Driver ×2 | BTS7960 43A H-Bridge |
| Motors | DC gear motors (left & right side) |
| Power | 7.4V–12V LiPo / battery pack |

---

## 🔌 Pin Wiring

### FlySky Receiver → Arduino

| Receiver Channel | Arduino Pin | Function |
|---|---|---|
| CH1 | D8 | Steering (Left/Right) |
| CH2 | D9 | Throttle (Forward/Backward) |

### BTS7960 Driver 1 — LEFT Motors

| BTS7960 Pin | Arduino Pin |
|---|---|
| RPWM | D3 |
| LPWM | D4 |
| REN | D5 |
| LEN | D6 |

### BTS7960 Driver 2 — RIGHT Motors

| BTS7960 Pin | Arduino Pin |
|---|---|
| RPWM | D7 |
| LPWM | D2 |
| REN | D11 |
| LEN | D12 |

---

## 🧠 How It Works

### PPM Signal Reading
The FlySky receiver outputs a **PPM signal (1000–2000 µs)**. `pulseIn()` reads each channel:
- **1000 µs** → full left / full reverse
- **1500 µs** → center / stopped
- **2000 µs** → full right / full forward

A failsafe returns `1500` (stopped) if no signal is detected within 25 ms.

### Tank-Style Mixing
The throttle and steering channels are combined to produce independent left/right wheel speeds:

```
leftSpeed  = throttle + steering
rightSpeed = throttle - steering
```

Both values are clamped to `[-255, 255]` for PWM output.

### Dead Zone
A ±15 dead zone filters out joystick drift near center — preventing the robot from creeping when sticks are released.

---

## 🚀 Getting Started

### 1. Clone the repo
```bash
git clone https://github.com/yashwanthR1207/<repo-name>.git
cd <repo-name>
```

### 2. Open in Arduino IDE or PlatformIO

**Arduino IDE:**
- Open `src/main.cpp` (rename to `main.ino` if needed)
- Select your board under **Tools → Board**
- Upload

**PlatformIO:**
```bash
pio run --target upload
```

### 3. Serial Monitor
Open Serial Monitor at **9600 baud** to watch live motor speed output:
```
L:200 R:180
L:0 R:0
L:-150 R:-150
```

---

## 📁 Project Structure

```
rc-tank/
├── src/
│   └── main.cpp        # Main Arduino sketch
├── README.md           # This file
└── platformio.ini      # (optional) PlatformIO config
```

---

##  Configuration

You can tune these values in `main.cpp`:

| Parameter | Location | Default | Description |
|---|---|---|---|
| Dead zone threshold | `loop()` | `15` | Minimum speed before motor activates |
| Failsafe value | `readChannel()` | `1500` | PPM value returned on signal loss |
| PWM timeout | `readChannel()` | `25000 µs` | Max wait time for pulse |

---

## Dependencies

- Arduino core (`Arduino.h`) — no external libraries required

---

## License

MIT License — free to use, modify, and distribute.

---

##  Author

**Yashwanth R** — [@yashwanthR1207](https://github.com/yashwanthR1207)
