#define ledPin 9    // defines LED to digital pin 9
#define ledPin2 10  // defines 2nd LED to digital pin 10

void setup() {  // sets up Arduino for the rest of the code
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);           // Configures the first LED pin
  pinMode(ledPin2, OUTPUT);          // Configures the second LED pin // tells Arduino to send power to the pin (that was set above)
  Serial.begin(9600);                // tells Arduino to open a data connection at 9600
  Serial.println("Start of Blink");  // tells Arduino to print on a screen the text
}

void loop() {  // tells Arduino to repeat this code specifically indefinitely
  // put your main code here, to run repeatedly:
  blink(ledPin, 5, 1);   // tells Arduino to send power to ledPin for 300ms for 9 times
  delay(1);              // stops all function on arduino for 500ms
  blink(ledPin2, 5, 1);  // same thing but for 600ms
  delay(.1);             // same thing
}

void blink(int pin, int wait, int rep) {  // makes a function with 3 integer perameters, pin, wait time, and times repeated
  for (int i = 0; i < rep; ++i) {         // creates a variable i temporarily set to 0 and makes the code stop at the number of repetitions given, and after every runthrough, it increases the i value by 1
    digitalWrite(pin, HIGH);              // sends power to whatever pin value was set
    delay(wait);                          // tells arduino to stop function for however amount of time you set
    digitalWrite(pin, LOW);               // stops power to whatever pin value was set
    delay(wait);                          // same
  }
}
