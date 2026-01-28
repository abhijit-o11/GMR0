/*
 * ESP32 Robot Control System
 * Controls 4 DC motors via motor driver, servo motor, and HC-SR04 ultrasonic sensor
 * 
 * Components:
 * - 4 DC Motors (via motor driver)
 * - Servo Motor
 * - HC-SR04 Ultrasonic Sensor
 * 
 * Motor Layout:
 * Front: Motor 1 (Left) | Motor 2 (Right)
 * Back:  Motor 3 (Left) | Motor 4 (Right)
 * Front is where servo and US sensor are mounted
 */

#include <Servo.h>

// HC-SR04 Ultrasonic Sensor Pins
const int trigPin = 18;  // GPIO 18 - Trigger pin
const int echoPin = 19;  // GPIO 19 - Echo pin

// Servo Motor Pin
const int servoPin = 5;  // GPIO 5

// Motor Driver Pins (Assuming L298N or similar dual H-bridge driver)
// Motor 1 (Front Left)
const int motor1Pin1 = 2;   // GPIO 2 - Motor 1 direction pin 1
const int motor1Pin2 = 4;   // GPIO 4 - Motor 1 direction pin 2
const int motor1PWM = 12;   // GPIO 12 - Motor 1 PWM pin (speed control)

// Motor 2 (Front Right)
const int motor2Pin1 = 16;  // GPIO 16 - Motor 2 direction pin 1
const int motor2Pin2 = 17;  // GPIO 17 - Motor 2 direction pin 2
const int motor2PWM = 13;   // GPIO 13 - Motor 2 PWM pin

// Motor 3 (Back Left)
const int motor3Pin1 = 14;  // GPIO 14 - Motor 3 direction pin 1
const int motor3Pin2 = 15;  // GPIO 15 - Motor 3 direction pin 2
const int motor3PWM = 25;   // GPIO 25 - Motor 3 PWM pin

// Motor 4 (Back Right)
const int motor4Pin1 = 26;  // GPIO 26 - Motor 4 direction pin 1
const int motor4Pin2 = 27;  // GPIO 27 - Motor 4 direction pin 2
const int motor4PWM = 32;   // GPIO 32 - Motor 4 PWM pin

// Constants
const int motorSpeed = 200;      // PWM speed (0-255) - adjust as needed
const int obstacleDistance = 15; // Distance threshold in cm (adjust as needed)
const int servoCenterPos = 90;   // Center position of servo (facing forward)
const int servoLeftPos = 0;      // 90 degrees left from center
const int servoRightPos = 180;   // 90 degrees right from center

// Servo object
Servo myServo;

// State variables
int servoCurrentPos = servoCenterPos; // Track current servo position
bool obstacleDetected = false;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(100);
  Serial.println("ESP32 Robot Control System Started");
  
  // Initialize Ultrasonic Sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Initialize Servo
  myServo.attach(servoPin);
  myServo.write(servoCenterPos);
  servoCurrentPos = servoCenterPos;
  delay(500);
  
  // Initialize Motor pins
  // Motor 1
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor1PWM, OUTPUT);
  
  // Motor 2
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(motor2PWM, OUTPUT);
  
  // Motor 3
  pinMode(motor3Pin1, OUTPUT);
  pinMode(motor3Pin2, OUTPUT);
  pinMode(motor3PWM, OUTPUT);
  
  // Motor 4
  pinMode(motor4Pin1, OUTPUT);
  pinMode(motor4Pin2, OUTPUT);
  pinMode(motor4PWM, OUTPUT);
  
  // Stop all motors initially
  stopAllMotors();
  
  Serial.println("Initialization Complete");
  delay(1000);
}

void loop() {
  // Check for obstacle in front
  float distance = getDistance();
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // Condition 1: If obstacle detected in front, stop all motors
  if (distance < obstacleDistance) {
    Serial.println("Obstacle detected in front!");
    stopAllMotors();
    
    // Reset servo to center position first
    myServo.write(servoCenterPos);
    servoCurrentPos = servoCenterPos;
    delay(500);
    
    // Condition 2: Rotate servo 90 degrees to left and check
    Serial.println("Checking left side...");
    myServo.write(servoLeftPos);
    servoCurrentPos = servoLeftPos;
    delay(500); // Wait for servo to reach position
    delay(200); // Additional delay for sensor stabilization
    
    float leftDistance = getDistance();
    Serial.print("Left distance: ");
    Serial.print(leftDistance);
    Serial.println(" cm");
    
    if (leftDistance >= obstacleDistance) {
      // No obstacle on left - move motors 1 and 3 forward
      Serial.println("No obstacle on left. Moving motors 1 and 3 forward");
      moveMotor1Forward();
      moveMotor3Forward();
      stopMotor2();
      stopMotor4();
    } else {
      // Obstacle detected on left
      Serial.println("Obstacle detected on left. Checking right side...");
      
      // Condition 3: Rotate servo 90 degrees to right (2 times = 180 degrees total)
      myServo.write(servoRightPos);
      servoCurrentPos = servoRightPos;
      delay(500);
      delay(200); // Additional delay for sensor stabilization
      
      float rightDistance = getDistance();
      Serial.print("Right distance: ");
      Serial.print(rightDistance);
      Serial.println(" cm");
      
      if (rightDistance >= obstacleDistance) {
        // No obstacle on right - move motors 2 and 4 forward
        Serial.println("No obstacle on right. Moving motors 2 and 4 forward");
        stopMotor1();
        moveMotor2Forward();
        stopMotor3();
        moveMotor4Forward();
      } else {
        // Obstacle detected on both sides
        Serial.println("Obstacle detected on both sides. Stopping all motors");
        stopAllMotors();
        // Reset servo to center
        myServo.write(servoCenterPos);
        servoCurrentPos = servoCenterPos;
      }
    }
  } else {
    // No obstacle in front - move all motors forward
    Serial.println("No obstacle. Moving all motors forward");
    moveAllMotorsForward();
    // Reset servo to center position if not already
    if (servoCurrentPos != servoCenterPos) {
      myServo.write(servoCenterPos);
      servoCurrentPos = servoCenterPos;
      delay(500);
    }
  }
  
  delay(100); // Small delay before next iteration
}

// Function to get distance from HC-SR04 ultrasonic sensor
float getDistance() {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Set the trigPin HIGH for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout (max range ~500cm)
  
  // Calculate the distance
  float distance = duration * 0.034 / 2;
  
  // Return 999 if no echo received (obstacle too far)
  if (distance == 0 || distance > 500) {
    return 999;
  }
  
  return distance;
}

// Motor control functions
void moveMotor1Forward() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  analogWrite(motor1PWM, motorSpeed);
}

void moveMotor2Forward() {
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
  analogWrite(motor2PWM, motorSpeed);
}

void moveMotor3Forward() {
  digitalWrite(motor3Pin1, HIGH);
  digitalWrite(motor3Pin2, LOW);
  analogWrite(motor3PWM, motorSpeed);
}

void moveMotor4Forward() {
  digitalWrite(motor4Pin1, HIGH);
  digitalWrite(motor4Pin2, LOW);
  analogWrite(motor4PWM, motorSpeed);
}

void stopMotor1() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  analogWrite(motor1PWM, 0);
}

void stopMotor2() {
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
  analogWrite(motor2PWM, 0);
}

void stopMotor3() {
  digitalWrite(motor3Pin1, LOW);
  digitalWrite(motor3Pin2, LOW);
  analogWrite(motor3PWM, 0);
}

void stopMotor4() {
  digitalWrite(motor4Pin1, LOW);
  digitalWrite(motor4Pin2, LOW);
  analogWrite(motor4PWM, 0);
}

void moveAllMotorsForward() {
  moveMotor1Forward();
  moveMotor2Forward();
  moveMotor3Forward();
  moveMotor4Forward();
}

void stopAllMotors() {
  stopMotor1();
  stopMotor2();
  stopMotor3();
  stopMotor4();
}

