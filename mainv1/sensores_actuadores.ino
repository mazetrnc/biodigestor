#include <DHT.h>
#include "MAX6675.h"
#include <math.h>

// Definición de pines
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

#define SOIL_PIN 35        // Pin para el sensor de humedad del suelo (HW-103)

// Pines para MAX6675
const int dataPin = 19;
const int clockPin = 18;
const int selectPin = 5;
MAX6675 thermoCouple(selectPin, dataPin, clockPin);

// Pin para DHT22
DHT dht(26, DHT22);

// Variables de lecturas de sensores
float temperature = 0.0;
float humidity = 0.0;
float methane = 0.0;
float soilMoisture = 0.0;
float internalTemp = 0.0;

// Prototipos de funciones locales
float readRs(bool precisionMode = false);
float calculateCH4(float rs);
float calibrateR0();

void setupSensors() {
  dht.begin();
  thermoCouple.begin();
  thermoCouple.setSPIspeed(4000000);
  delay(2000);

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
float readRs(bool precisionMode) {
  int samples = precisionMode ? 50 : 10; // Muestras según modo
  float adcValue = 0;
  
  for(int i = 0; i < samples; i++) {
    adcValue += analogRead(MQ_PIN);
    delay(5);
  }
  adcValue /= samples;
  
  // Convertir a voltaje (ESP32 3.3V, resolución 12 bits)
  float voltage = adcValue * (3.3 / 4095.0);
  
  // Fórmula corregida para 3.3V (RL = 10kΩ optimizado)
  return (3.3 - voltage) * RL / voltage;
}

void updateSensors() {
  // Leer DHT22
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Leer MQ-5 (Metano)
  float rs = readRs();       // Resistencia actual del sensor
  methane = calculateCH4(rs); // PPM de metano
  methane = max(methane, minPPM); // Ajustar valor mínimo

  // Leer MAX6675 (Temperatura interna)
  int status = thermoCouple.read();
  if (status == 0) {
    internalTemp = thermoCouple.getTemperature();
  } else {
    // Manejo de errores
    if (status == 4) {
      Serial.println("Error: Revisar termopar");
    } else if (status == 129) {
      Serial.println("Error: Sin comunicacion con MAX6675");
    }
  }

  // Leer HW-103 (Humedad del suelo)
  int rawValue = analogRead(SOIL_PIN);
  soilMoisture = ((4095 - rawValue) / 4095.0) * 100.0;

  // Imprimir para depuración
  Serial.println("-----------------------------");
  Serial.print("Temp: "); Serial.print(temperature); Serial.println(" C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");
  Serial.print("CH4: "); Serial.print(methane); Serial.println(" ppm");
  Serial.print("Internal Temp: "); Serial.print(internalTemp); Serial.println(" C");
  Serial.print("Soil Moisture: "); Serial.print(soilMoisture); Serial.println("%");
}


// Calcular PPM de metano (fórmula datasheet)
float calculateCH4(float rs) {
  return A_CH4 * pow(rs / R0, B_CH4);
}

// Calibrar R0 en aire limpio
float calibrateR0() {
  Serial.println("Calibrando en aire limpio...");
  
  float sum = 0;
  for(int i = 0; i < 100; i++) { // 100 lecturas
    sum += readRs(true);      // Modo alta precisión
    delay(100);
  }
  return sum / 100.0 / RATIO_CLEAN_AIR;
}