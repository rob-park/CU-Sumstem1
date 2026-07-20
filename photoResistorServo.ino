#include <Servo.h>

Servo myServo;

#define laserPin 10
#define photoresistorPin A0

int lightValue = 0;
int angle = 0;

void setup() {
  myServo.attach(9);
  pinMode(laserPin, OUTPUT);
  pinMode(photoresistorPin, INPUT);
  digitalWrite(laserPin, HIGH);
}

void loop() {
  lightValue = analogRead(photoresistorPin);
  
  if (lightValue > 1023) {
    lightValue = 1023;
  }
  if (lightValue < 0) {
    lightValue = 0;
  }
  
  angle = map(lightValue, 100, 400, 0, 180);
  
  myServo.write(angle);
  delay(15);
}