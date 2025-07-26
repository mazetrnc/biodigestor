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

void setup() {
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
  float rs = readRs();       // Resistencia actual del sensor
  float ratio = rs / R0;     // Relación Rs/R0
  float ppm_ch4 = calculateCH4(rs); // PPM de metano

  // Ajustar valor mínimo (no mostrar menos que aire limpio)
  ppm_ch4 = max(ppm_ch4, minPPM); 

  Serial.print("Rs: "); Serial.print(rs, 1); 
  Serial.print(" kΩ | Ratio: "); Serial.print(ratio, 2);
  Serial.print(" | CH4: "); Serial.print(ppm_ch4); 
  Serial.println(" ppm");

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