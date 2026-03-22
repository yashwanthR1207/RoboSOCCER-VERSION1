// ============================================================
//  Robo Soccer — FlySky + BTS7960 + Arduino
//  Author : Yashwanth R (github.com/yashwanthR1207)
//
//  Tank-drive robot built for robot soccer competitions.
//  Controlled via FlySky RC transmitter with dual BTS7960
//  motor drivers for left and right side drive wheels.
// ============================================================

// --- FlySky receiver input pins ---
#define CH1_PIN 8   // Left/Right steering
#define CH2_PIN 9   // Forward/Backward throttle

// --- BTS7960 Motor Driver 1 — LEFT side motors ---
#define RPWM1 3
#define LPWM1 4
#define REN1  5
#define LEN1  6

// --- BTS7960 Motor Driver 2 — RIGHT side motors ---
#define RPWM2 7
#define LPWM2 2
#define REN2  11
#define LEN2  12

// Dead zone threshold — prevents motor drift near center
#define DEAD_ZONE 15

// ============================================================
//  Read PPM signal from receiver (1000–2000 µs)
//  Returns 1500 (center/stopped) as failsafe if no signal
// ============================================================
int readChannel(int pin) {
  unsigned long pulse = pulseIn(pin, HIGH, 25000);
  if (pulse == 0) return 1500; // Failsafe: stop
  return constrain(pulse, 1000, 2000);
}

// ============================================================
//  Map receiver value (1000–2000) to motor range (-255–+255)
// ============================================================
int mapToMotor(int rcVal) {
  return map(rcVal, 1000, 2000, -255, 255);
}

// ============================================================
//  Drive a BTS7960 motor driver
//  speed: -255 (full reverse) to +255 (full forward), 0 = stop
// ============================================================
void driveMotor(int rpwm, int lpwm, int ren, int len, int speed) {
  digitalWrite(ren, HIGH);
  digitalWrite(len, HIGH);

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

// ============================================================
//  SETUP
// ============================================================
void setup() {
  // Motor enable pins
  pinMode(REN1, OUTPUT); pinMode(LEN1, OUTPUT);
  pinMode(REN2, OUTPUT); pinMode(LEN2, OUTPUT);

  // Motor PWM pins
  pinMode(RPWM1, OUTPUT); pinMode(LPWM1, OUTPUT);
  pinMode(RPWM2, OUTPUT); pinMode(LPWM2, OUTPUT);

  // RC receiver input pins
  pinMode(CH1_PIN, INPUT);
  pinMode(CH2_PIN, INPUT);

  Serial.begin(9600);
  Serial.println("Robo Soccer Ready!");
}

// ============================================================
//  LOOP
// ============================================================
void loop() {
  // Read receiver channels
  int ch1 = readChannel(CH1_PIN); // Steering : 1000=Left  | 2000=Right
  int ch2 = readChannel(CH2_PIN); // Throttle : 1000=Back  | 2000=Fwd

  // Convert to motor values
  int throttle = mapToMotor(ch2);
  int steering = mapToMotor(ch1);

  // Tank-style mixing — independent left/right wheel control
  int leftSpeed  = constrain(throttle + steering, -255, 255);
  int rightSpeed = constrain(throttle - steering, -255, 255);

  // Apply dead zone to prevent drift during gameplay
  if (abs(leftSpeed)  < DEAD_ZONE) leftSpeed  = 0;
  if (abs(rightSpeed) < DEAD_ZONE) rightSpeed = 0;

  // Drive motors
  driveMotor(RPWM1, LPWM1, REN1, LEN1, leftSpeed);
  driveMotor(RPWM2, LPWM2, REN2, LEN2, rightSpeed);

  // Serial debug (open Serial Monitor at 9600 baud)
  Serial.print("L:"); Serial.print(leftSpeed);
  Serial.print("  R:"); Serial.println(rightSpeed);

  delay(20);
}
