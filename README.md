/*
 * ██████╗  ██████╗ ██████╗  ██████╗     ███████╗ ██████╗  ██████╗ ██████╗███████╗██████╗
 * ██╔══██╗██╔═══██╗██╔══██╗██╔═══██╗    ██╔════╝██╔═══██╗██╔════╝██╔════╝██╔════╝██╔══██╗
 * ██████╔╝██║   ██║██████╔╝██║   ██║    ███████╗██║   ██║██║     ██║     █████╗  ██████╔╝
 * ██╔══██╗██║   ██║██╔══██╗██║   ██║    ╚════██║██║   ██║██║     ██║     ██╔══╝  ██╔══██╗
 * ██║  ██║╚██████╔╝██████╔╝╚██████╔╝    ███████║╚██████╔╝╚██████╗╚██████╗███████╗██║  ██║
 * ╚═╝  ╚═╝ ╚═════╝ ╚═════╝  ╚═════╝    ╚══════╝ ╚═════╝  ╚═════╝ ╚═════╝╚══════╝╚═╝  ╚═╝
 *
 * Arduino · FlySky FS-iA6B · BTS7960 43A · Tank Drive · 3S LiPo 11.1V
 * Built by Yashwanth R — github.com/yashwanthR1207
 *
 * POWER SUPPLY — 3S LiPo (11.1V nominal, 12.6V full, 9.9V cutoff)
 * ---------------------------------------------------------------
 * LiPo (+) ──► BTS7960 VCC (motor power)
 * LiPo (+) ──► Voltage Divider ──► A0 (battery monitor)
 *              [ R1=30kΩ top, R2=10kΩ bottom ]
 *              Divider ratio = 10k/(30k+10k) = 0.25
 *              11.1V × 0.25 = 2.775V → safe for Arduino analog pin
 * LiPo (–) ──► BTS7960 GND + Arduino GND (common ground)
 * Arduino powered via USB or 5V regulator from LiPo
 *
 * LOW BATTERY WARNING  : 10.5V  (LED blinks fast)
 * LOW BATTERY CUTOFF   : 9.9V   (motors disabled, LED solid)
 */

// ─────────────────────────────────────────────
//  RC RECEIVER PINS
// ─────────────────────────────────────────────
const int CH1_PIN = 8;   // Steering  (Left / Right)
const int CH2_PIN = 9;   // Throttle  (Forward / Backward)

// ─────────────────────────────────────────────
//  BTS7960 — LEFT MOTORS (Driver 1)
// ─────────────────────────────────────────────
const int L_RPWM = 3;
const int L_LPWM = 4;
const int L_REN  = 5;
const int L_LEN  = 6;

// ─────────────────────────────────────────────
//  BTS7960 — RIGHT MOTORS (Driver 2)
// ─────────────────────────────────────────────
const int R_RPWM = 7;
const int R_LPWM = 2;
const int R_REN  = 11;
const int R_LEN  = 12;

// ─────────────────────────────────────────────
//  BATTERY MONITOR
//  Voltage divider: R1=30kΩ, R2=10kΩ → ratio 0.25
//  Arduino ADC ref = 5.0V, resolution = 1023
// ─────────────────────────────────────────────
const int   BATT_PIN          = A0;
const float DIVIDER_RATIO     = 0.25;       // R2 / (R1 + R2)
const float ADC_REF_VOLTAGE   = 5.0;
const float WARN_VOLTAGE      = 10.5;       // blink LED warning
const float CUTOFF_VOLTAGE    = 9.9;        // disable motors
const int   BATT_SAMPLES      = 10;         // rolling average samples
const int   LED_PIN           = 13;         // built-in LED for alerts

// ─────────────────────────────────────────────
//  TUNABLE PARAMETERS
// ─────────────────────────────────────────────
const int DEAD_ZONE           = 15;         // joystick dead zone (PWM units)
const int PPM_CENTER          = 1500;       // neutral PPM value (µs)
const int PPM_MIN             = 1000;       // min PPM value (µs)
const int PPM_MAX             = 2000;       // max PPM value (µs)
const unsigned long PPM_TIMEOUT = 25000;    // pulseIn timeout (µs)

// ─────────────────────────────────────────────
//  GLOBALS
// ─────────────────────────────────────────────
bool  batteryOK   = true;
float battVoltage = 12.6;

// ─────────────────────────────────────────────
//  READ A SINGLE PPM CHANNEL
//  Returns 1500 (stopped) on signal loss — failsafe
// ─────────────────────────────────────────────
int readChannel(int pin) {
  unsigned long pulse = pulseIn(pin, HIGH, PPM_TIMEOUT);
  if (pulse == 0) return PPM_CENTER;                    // failsafe
  return constrain((int)pulse, PPM_MIN, PPM_MAX);
}

// ─────────────────────────────────────────────
//  MAP PPM → MOTOR SPEED  [-255 .. 255]
// ─────────────────────────────────────────────
int ppmToSpeed(int ppm) {
  // Map 1000–2000 µs to -255..+255
  return map(ppm, PPM_MIN, PPM_MAX, -255, 255);
}

// ─────────────────────────────────────────────
//  DRIVE A BTS7960 CHANNEL
//  speed : -255 (full reverse) … 0 (stop) … 255 (full forward)
// ─────────────────────────────────────────────
void driveMotor(int rpwm, int lpwm, int speed) {
  if (abs(speed) < DEAD_ZONE) speed = 0;
  if (speed > 0) {
    analogWrite(rpwm, speed);
    analogWrite(lpwm, 0);
  } else if (speed < 0) {
    analogWrite(rpwm, 0);
    analogWrite(lpwm, -speed);
  } else {
    analogWrite(rpwm, 0);
    analogWrite(lpwm, 0);
  }
}

// ─────────────────────────────────────────────
//  STOP ALL MOTORS IMMEDIATELY
// ─────────────────────────────────────────────
void stopAll() {
  analogWrite(L_RPWM, 0); analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, 0); analogWrite(R_LPWM, 0);
}

// ─────────────────────────────────────────────
//  READ BATTERY VOLTAGE (rolling average)
//  Formula: V_batt = (ADC / 1023) × Vref / divider_ratio
// ─────────────────────────────────────────────
float readBatteryVoltage() {
  long sum = 0;
  for (int i = 0; i < BATT_SAMPLES; i++) {
    sum += analogRead(BATT_PIN);
    delay(2);
  }
  float avg     = (float)sum / BATT_SAMPLES;
  float vAtPin  = (avg / 1023.0) * ADC_REF_VOLTAGE;
  return vAtPin / DIVIDER_RATIO;
}

// ─────────────────────────────────────────────
//  BATTERY STATUS — call every loop
//  Returns true  → safe to run motors
//          false → voltage too low, motors locked
// ─────────────────────────────────────────────
bool checkBattery() {
  battVoltage = readBatteryVoltage();

  if (battVoltage <= CUTOFF_VOLTAGE) {
    // Hard cutoff — solid LED, motors disabled
    digitalWrite(LED_PIN, HIGH);
    Serial.print(F("!! LOW BATTERY CUTOFF !! "));
    Serial.print(battVoltage, 2);
    Serial.println(F("V — MOTORS DISABLED"));
    return false;
  }

  if (battVoltage <= WARN_VOLTAGE) {
    // Warning — fast blink LED, motors still run
    digitalWrite(LED_PIN, (millis() / 150) % 2);
    Serial.print(F("! LOW BATT WARNING: "));
    Serial.print(battVoltage, 2);
    Serial.println(F("V"));
    return true;
  }

  // All good — LED off
  digitalWrite(LED_PIN, LOW);
  return true;
}

// ─────────────────────────────────────────────
//  SETUP
// ─────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  // Motor driver enable pins
  pinMode(L_REN, OUTPUT); pinMode(L_LEN, OUTPUT);
  pinMode(R_REN, OUTPUT); pinMode(R_LEN, OUTPUT);
  digitalWrite(L_REN, HIGH); digitalWrite(L_LEN, HIGH);
  digitalWrite(R_REN, HIGH); digitalWrite(R_LEN, HIGH);

  // PWM pins
  pinMode(L_RPWM, OUTPUT); pinMode(L_LPWM, OUTPUT);
  pinMode(R_RPWM, OUTPUT); pinMode(R_LPWM, OUTPUT);

  // RC input pins
  pinMode(CH1_PIN, INPUT);
  pinMode(CH2_PIN, INPUT);

  // Battery & LED
  pinMode(BATT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  stopAll();

  Serial.println(F("============================="));
  Serial.println(F("  Robo Soccer Ready!"));
  Serial.print  (F("  Battery: "));
  Serial.print  (readBatteryVoltage(), 2);
  Serial.println(F("V"));
  Serial.println(F("  Warn  : 10.5V | Cutoff: 9.9V"));
  Serial.println(F("============================="));
}

// ─────────────────────────────────────────────
//  MAIN LOOP
// ─────────────────────────────────────────────
void loop() {

  // 1. Battery check first — lock motors if too low
  batteryOK = checkBattery();
  if (!batteryOK) {
    stopAll();
    delay(500);
    return;
  }

  // 2. Read RC channels
  int ch1 = readChannel(CH1_PIN);   // Steering
  int ch2 = readChannel(CH2_PIN);   // Throttle

  // 3. Convert PPM → speed values
  int steering = ppmToSpeed(ch1);   // -255 (left) … +255 (right)
  int throttle = ppmToSpeed(ch2);   // -255 (back) … +255 (forward)

  // 4. Tank mixing
  int leftSpeed  = constrain(throttle + steering, -255, 255);
  int rightSpeed = constrain(throttle - steering, -255, 255);

  // 5. Drive motors
  driveMotor(L_RPWM, L_LPWM, leftSpeed);
  driveMotor(R_RPWM, R_LPWM, rightSpeed);

  // 6. Serial telemetry
  Serial.print(F("Batt:"));  Serial.print(battVoltage, 1); Serial.print(F("V  "));
  Serial.print(F("L:"));     Serial.print(leftSpeed);
  Serial.print(F("  R:"));   Serial.println(rightSpeed);
}
```

---

Here's what was added and why:

**Voltage Divider Circuit (hardware you need to wire)**
Since 11.1V will fry the Arduino's analog pin (max 5V), you need a resistor divider on A0:
```
LiPo (+) ──[R1: 30kΩ]──┬──[R2: 10kΩ]── GND
                        │
                       A0
