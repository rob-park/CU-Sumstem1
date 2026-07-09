#define greenPin 9
#define bluePin 10
#define redPin 11

const int buzzerPin = 6;
const int buttonPin = 7;

void setup() {
  // put your setup code here, to run once:
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, INPUT_PULLUP);
}



void loop() {
  // put your main code here, to run repeatedly:
  int buttonState = digitalRead(buzzerPin);
  if (buzzerPin == LOW) {
  }
}
