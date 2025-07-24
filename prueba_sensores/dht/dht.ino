#include "DHT.h"

#define DHTPIN 7

#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT22 // DHT 22 (AM2302)
//#define DHTTYPE DHT21 // DHT 21 (AM2301)

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

void setup() {
Serial.begin(11520);
dht.begin();
}

void loop() {
delay(2000);
float h = dht.readHumidity();
float t = dht.readTemperature();
float f = dht.readTemperature(true);

if (isnan(h) || isnan(t) || isnan(f)) {
Serial.println("Failed to read from DHT sensor");
return;
}

// Compute heat index
// Must send in temp in Fahrenheit
float hi = dht.computeHeatIndex(f, h);

Serial.print("Humidity: ");
Serial.print(h);
Serial.print(" %\t");
Serial.print("Temperature: ");
Serial.print(t);
Serial.print(" *C ");
Serial.print(f);
Serial.print(" *F\t");
Serial.print("Heat index: ");
Serial.print(hi);
Serial.println(" *F");
}