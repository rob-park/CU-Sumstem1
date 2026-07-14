const int X_PIN = A0;
const int Y_PIN = A1;
#define greenPin 5
#define whitePin 11
#define redPin 10
#define bluePin 9

int raw = 0;
int raw_1 = 0;
int xlevel = 0;
int ylevel = 0;

void setup() {
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(whitePin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void loop() {
  raw = analogRead(X_PIN);
  raw_1 = analogRead(Y_PIN);

  if (raw < 402) {
    xlevel = map(raw, 402, 0, 0, 255);
    analogWrite(greenPin, xlevel);
    analogWrite(bluePin, 0);
  } 
  else if (raw > 622) {
    xlevel = map(raw, 622, 1023, 0, 255);
    analogWrite(bluePin, xlevel);
    analogWrite(greenPin, 0);
  } 
  else {
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
  }

  if (raw_1 < 402) {
    ylevel = map(raw_1, 402, 0, 0, 255);
    analogWrite(redPin, ylevel);
    analogWrite(whitePin, 0);
  } 
  else if (raw_1 > 622) {
    ylevel = map(raw_1, 622, 1023, 0, 255);
    analogWrite(whitePin, ylevel);
    analogWrite(redPin, 0);
  } 
  else {
    analogWrite(redPin, 0);
    analogWrite(whitePin, 0);
  }
}