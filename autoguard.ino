#include <LiquidCrystal_I2C.h>

// Motor 1 connections
int ENA = 5;    // PWM pin for Motor 1 speed control
int IN1 = 6;    // Motor 1 direction control pin
int IN2 = 7;    // Motor 1 direction control pin

// Motor 2 connections
int ENB = 9;    // PWM pin for Motor 2 speed control
int IN3 = 8;    // Motor 2 direction control pin
int IN4 = 10;   // Motor 2 direction control pin

// Ultrasonic sensor connections
int TRIG_PIN = 11; // Trigger pin
int ECHO_PIN = 12; // Echo pin

// Define motor direction constants
int FORWARD = 1;
int REVERSE = 0;

// Initialize LCD (I2C address 0x27, 16 columns x 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);


const float MAX_RPM = 3760.0;
const float WHEEL_CIRCUMFERENCE = 0.2;

void setup() {
  // Set up motor control pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Set up ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Initializing");
  delay(2000);
  lcd.clear();
}

// Function to calculate speed in km/h based on PWM value
float calculateSpeed(int pwmValue) {
  float rpm = (pwmValue / 255.0) * MAX_RPM; // Scale PWM to RPM
  float speed_mps = (rpm * WHEEL_CIRCUMFERENCE) / 60.0; // Speed in meters/second
  return speed_mps * 3.6; // Convert m/s to km/h
}

// Function to stop both motors
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

void loop() {
  long distance = measureDistance(); // Measure distance using ultrasonic sensor
  float speed_kmph = 0.0;

  if (distance <= 10) { // Very close
    stopMotors();
    lcd.setCursor(0, 0);
    lcd.print("STOP! Obstacle");
    lcd.setCursor(0, 1);
    lcd.print("Dist: ");
    lcd.print(distance);
    lcd.print(" cm      ");
  } else if (distance > 10 && distance <= 20) { // Medium distance
    int speed = map(distance, 100, 500, 100, 255); // Map distance to PWM speed
    speed_kmph = calculateSpeed(speed);
    moveMotors(FORWARD, speed);
    lcd.setCursor(0, 0);
    lcd.print("Speed Adjusting  ");
    lcd.setCursor(0, 1);
    lcd.print("Speed: ");
    lcd.print(speed_kmph, 1);
    lcd.print(" km/h ");
  } else { // Safe distance
    int speed = 255; // Full speed
    speed_kmph = calculateSpeed(speed);
    moveMotors(FORWARD, speed);
    lcd.setCursor(0, 0);
    lcd.print("Full Speed Ahead");
    lcd.setCursor(0, 1);
    lcd.print("Speed: ");
    lcd.print(speed_kmph, 1);
    lcd.print(" km/h ");
  }
  delay(100); // Small delay for stability
}

// Function to measure distance with the ultrasonic sensor
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2; // returns distance in cm
}

// Function to move both motors in the same direction
void moveMotors(int direction, int speed) {
  // Motor 1
  if (direction == FORWARD) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (direction == REVERSE) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}


