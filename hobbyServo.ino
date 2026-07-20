// trig = 2, echo =3, potentio = a0, lcd with l2c 2x16 display servo= 10

#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Servo myServo;

const int potPin = A0;
int potValue = 0;
int angle = 0; 

void setup() {
  myServo.attach(10);
}

void loop() {
  potValue = analogRead(potPin);
  angle = map(potValue, 0, 1023, 0, 180);
  myServo.write(angle);
  delay(15);
}