const int laser1 = 7;
const int laser2 = 3;

void setup() {
  pinMode(laser1,OUTPUT);
  pinMode(laser2,OUTPUT);
}

void loop() {
  digitalWrite(laser1, HIGH);
  digitalWrite(laser2, HIGH);
}