#define interval 500  // defines a varialbe, interval, as 500
#define ledPin 9      // defines the pin number of LED to digital pin #9

int ledState = LOW;  // turns the ledState off

unsigned long time = millis();  // sets a variable to return the number of milliseconds that have passed since the board started running the program
unsigned long previous = 0; // sets a baseline time to 0

void setup() {
  pinMode(ledPin, OUTPUT); (turns the ledPin on)
}

void loop() {
  time = millis(); 
  if (time - previous >= interval) {
    previous = time;

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    digitalWrite(ledPin, ledState);
  }
}
