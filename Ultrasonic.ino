const int trigPin = 9;
const int echoPin = 10;
const int ledPin = 13;
const int redPin = 12;
const int yellowPin = 11;
const int greenPin = 8;

long duration;
float distance;

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  if (distance <= 5) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(redPin, LOW);
    }  else if (distance <= 15) {
    digitalWrite(redPin, HIGH);
    digitalWrite(ledPin, LOW);
  }
  delay(100);
}
