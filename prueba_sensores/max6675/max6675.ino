#include "MAX6675.h"
const int dataPin = 7;
const int clockPin = 6;
const int selectPin = 5;
MAX6675 thermoCouple(selectPin, dataPin, clockPin);
uint32_t start, stop;
void setup() {
  Serial.begin(115200);
  SPI.begin();
  thermoCouple.begin();
  thermoCouple.setSPIspeed(4000000);
}


void loop() {
  int status = thermoCouple.read();
  float temp = thermoCouple.getTemperature();
  //thermoCouple.setOffset(-10.0);
  if (status == 0) {
  Serial.print("temp: ");
  Serial.println(temp);
  }
  else if (status == 4) {
    Serial.println("Revisar termopar");
  }

  else if (status == 129) {
    Serial.println("Sin comunicacion");
  }

  delay(500);
}