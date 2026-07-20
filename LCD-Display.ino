#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int joystickX = A0;
const int joystickY = A1;

int raw = 0;
int raw_1 = 0;

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  int xValue = analogRead(joystickX);
  int yValue = analogRead(joystickY);


  raw = map(xValue, 0, 670, 0, 14);
  raw_1 = map(yValue, 0, 670, 0, 2);

  lcd.clear();
  lcd.setCursor(raw, raw_1);
  lcd.print("robert");
}