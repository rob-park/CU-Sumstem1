#include <DHT.h>

#define DHTPin 2
#define DHTTYPE DHT22
DHT dht(DHTPin, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  Serial.println("DHT11 Sensor Test");
}

void loop() {
  delay(2000);
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from sensor!");
    return;
  }
  Serial.println(temp);
  Serial.println(hum);
}