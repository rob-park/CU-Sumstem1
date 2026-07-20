#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

const int trigPin = 2;
const int echoPin = 3;
const int potPin = A0;
const int servoPin = 10;

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo radarServo;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(potPin, INPUT);

  radarServo.attach(servoPin);

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  int potValue = analogRead(potPin);
  int angle = map(potValue, 0, 1023, 0, 180);

  radarServo.write(angle);

  long duration;
  int distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0343 / 2;

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Angle: ");
  lcd.print(angle);
  lcd.print((char)223);

  lcd.setCursor(0, 1);
  lcd.print("Dist: ");

  if (distance > 400 || distance <= 2) {
    lcd.print("Out of Range");
  } else {
    lcd.print(distance);
    lcd.print(" cm");
  }
}