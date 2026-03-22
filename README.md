# ⚽ Robo Soccer — Arduino + FlySky + BTS7960

A competitive **robot soccer bot** built with an Arduino, controlled via a **FlySky RC transmitter**, powered by dual **BTS7960 motor drivers** for fast, responsive tank-drive movement on the field.

---

## 📦 Hardware

| Component | Details |
|---|---|
| Microcontroller | Arduino Uno / Mega |
| RC Receiver | FlySky FS-iA6B (or compatible PPM) |
| Motor Driver ×2 | BTS7960 43A H-Bridge |
| Motors | DC gear motors (left & right side) |
| Power | 7.4V–12V LiPo battery |

---

## 🔌 Pin Wiring

### FlySky Receiver → Arduino

| Receiver Channel | Arduino Pin | Function |
|---|---|---|
| CH1 | D8 | Steering (Left / Right) |
| CH2 | D9 | Throttle (Forward / Backward) |

### BTS7960 Driver 1 — LEFT Motors

| BTS7960 Pin | Arduino Pin |
|---|---|
| RPWM | D3 |
| LPWM | D4 |
| REN  | D5 |
| LEN  | D6 |

### BTS7960 Driver 2 — RIGHT Motors

| BTS7960 Pin | Arduino Pin |
|---|---|
| RPWM | D7 |
| LPWM | D2 |
| REN  | D11 |
| LEN  | D12 |

---

## 🧠 How It Works

### PPM Signal Reading
The FlySky receiver outputs a **PPM signal (1000–2000 µs)**. `pulseIn()` reads each channel:
- **1000 µs** → full left / full reverse
- **1500 µs** → center / stopped
- **2000 µs** → full right / full forward

If no signal is received within 25 ms, the bot safely stops (failsafe returns 1500).

### Tank-Style Mixing
Throttle and steering channels are combined for independent left/right wheel control:

```
leftSpeed  = throttle + steering
rightSpeed = throttle - steering
```

This allows the bot to drive forward, reverse, pivot in place, and arc-turn — ideal for chasing a ball on the soccer field.

### Dead Zone
A ±15 dead zone on both channels prevents the bot from drifting when joysticks are at center, keeping it stationary during gameplay pauses.

---

## 🚀 Getting Started

### 1. Clone the repo
```bash
git clone https://github.com/yashwanthR1207/robo-soccer.git
cd robo-soccer
```

### 2. Open in Arduino IDE
- Open `robo_soccer/robo_soccer.ino`
- Go to **Tools → Board** and select your Arduino model
- Select the correct **Port**
- Click **Upload**

### 3. Serial Monitor
Open Serial Monitor at **9600 baud** to see live motor output:
```
Robo Soccer Ready!
L:200  R:180
L:255  R:100
L:0    R:0
```

---

## 📁 Project Structure

```
robo-soccer/
├── robo_soccer/
│   └── robo_soccer.ino     # Main Arduino sketch
└── README.md
```

---

## ⚙️ Tunable Parameters

| Parameter | Location | Default | Description |
|---|---|---|---|
| `DEAD_ZONE` | top of sketch | `15` | Minimum speed before motor activates |
| Failsafe value | `readChannel()` | `1500` | PPM value on signal loss (stop) |
| PWM timeout | `readChannel()` | `25000 µs` | Max wait time for pulse |

---

## 🛠️ Dependencies

- Arduino core only — **no external libraries needed**

---

## 📄 License

MIT License — free to use, modify, and distribute.

---

## 👤 Author

**Yashwanth R** — [@yashwanthR1207](https://github.com/yashwanthR1207)
