#include <DHT.h>
#include "MAX6675.h"
#include <math.h>

#define MQ_PIN 34          // Pin ADC (GPIO34)
#define RL 10.0            // Resistencia de carga en kΩ (ajustada para 3.3V)
#define RATIO_CLEAN_AIR 9.83 // Rs/R0 en aire limpio (datasheet)
#define PREHEAT_MIN 0.5     // Minutos mínimo de precalentamiento (recomendado 48h)
// Constantes para CH4 (Metano) - Datasheet MQ-5
const float A_CH4 = 1000.0;   // Rango biodigestor: 500-5000 ppm
const float B_CH4 = -2.3;     // Exponente para metano
// Parámetros de calibración en aire
float R0 = 10.0;             // Valor base (se calibrará)
float minPPM = 0;            // Límite inferior biodigestor

const int soil = 4;

const int dataPin = 19;
const int clockPin = 18;
const int selectPin = 5;
MAX6675 thermoCouple(selectPin, dataPin, clockPin);
uint32_t start, stop;

DHT dht(26, DHT22);
void setup() {
  // put your setup code here, to run once:
  dht.begin();
  SPI.begin();
  thermoCouple.begin();
  thermoCouple.setSPIspeed(4000000);
  delay(2000);
  Serial.begin(115200);

    // Precalentamiento inicial
  Serial.print("Precalentando sensor (min "); 
  Serial.print(PREHEAT_MIN); 
  Serial.println(" minutos)...");
  delay(PREHEAT_MIN * 60000); // Convertir minutos a ms
  // Calibración en aire limpio
  R0 = calibrateR0();
  minPPM = calculateCH4(R0 * RATIO_CLEAN_AIR); // Ppm en aire limpio
  
  Serial.println("==== Calibracion Completa ====");
  Serial.print("R0 = "); Serial.println(R0, 4);
  Serial.print("Aire limpio: "); Serial.print(minPPM); Serial.println(" ppm CH4");
  Serial.println("==============================");
}

// Leer resistencia del sensor (kΩ)
float readRs(bool precisionMode = false) {
  int samples = precisionMode ? 50 : 10; // Muestras según modo
  float adcValue = 0;
  
  for(int i=0; i<samples; i++) {
    adcValue += analogRead(MQ_PIN);
    delay(5);
  }
  adcValue /= samples;
  
  // Convertir a voltaje (ESP32 3.3V)
  float voltage = adcValue * (3.3 / 4095.0);
  
  // Fórmula corregida para 3.3V (RL = 10kΩ optimizado)
  return (3.3 - voltage) * RL / voltage;
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println("-----------------------------");
float temp = dht.readTemperature();
float humidity = dht.readHumidity();
Serial.print("Temp: ");
Serial.print(temp);
Serial.print(" C ");
Serial.print("Humidity: ");
Serial.print(humidity);
Serial.println("% ");

  float rs = readRs();       // Resistencia actual del sensor
  float ratio = rs / R0;     // Relación Rs/R0
  float ppm_ch4 = calculateCH4(rs); // PPM de metano

  // Ajustar valor mínimo (no mostrar menos que aire limpio)
  ppm_ch4 = max(ppm_ch4, minPPM); 

  Serial.print("Rs: "); Serial.print(rs, 1); 
  Serial.print(" kΩ | Ratio: "); Serial.print(ratio, 2);
  Serial.print(" | CH4: "); Serial.print(ppm_ch4); 
  Serial.println(" ppm");

int status = thermoCouple.read();
float tempx = thermoCouple.getTemperature();
    //thermoCouple.setOffset(-10.0);
if (status == 0) {
  Serial.print("temp: ");
  Serial.print(tempx);
  }
else if (status == 4) {
    Serial.println("Revisar termopar");
  }

else if (status == 129) {
    Serial.println("Sin comunicacion");
  }

int rawValue = analogRead(soil);
float humedadper = ((4095 - rawValue) / 4095.0)*100.0;
Serial.print("  Suelo: ");
Serial.print(humedadper);
Serial.println("% ");
delay(2000);
}

// Calcular PPM de metano (fórmula datasheet)
float calculateCH4(float rs) {
  return A_CH4 * pow(rs / R0, B_CH4);
}
// Calibrar R0 en aire limpio
float calibrateR0() {
  Serial.println("Calibrando en aire limpio...");
  
  float sum = 0;
  for(int i=0; i<100; i++) { // 100 lecturas
    sum += readRs(true);      // Modo alta precisión
    delay(100);
  }
  return sum / 100.0 / RATIO_CLEAN_AIR;
}