#include <Servo.h>

Servo myServo;

const int trigPin = 2;
const int echoPin = 3;

long duration;
int distance;
int angle;

void setup() {
  myServo.attach(9);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  if (distance > 50) {
    distance = 50;
  }
  if (distance < 2) {
    distance = 2;
  }

  angle = map(distance, 2, 50, 0, 180);

  myServo.write(angle);
}