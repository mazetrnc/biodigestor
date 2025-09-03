#include <DHT.h>
#include "MAX6675.h"
#include <math.h>

// Definición de pines
#define SOIL_PIN 35        // Pin para el sensor de humedad del suelo (HW-103)
#define MQ_PIN 34          // Pin para el sensor MQ-2

// Pines para MAX6675
const int dataPin = 19;
const int clockPin = 18;
const int selectPin = 5;
MAX6675 thermoCouple(selectPin, dataPin, clockPin);

// Pin para DHT22
DHT dht(26, DHT22);

// Variables de lecturas de sensores (DEBEN estar definidas aquí)
float temperature = 0.0;
float humidity = 0.0;
float soilMoisture = 0.0;
float internalTemp = 0.0;
float lpgPPM = 0.0;
int mq2AnalogValue = 0;

/*********************** Configuración MQ-2 ************************************/
#define RL_VALUE (5)                    // Resistencia de carga en kilo ohms
#define RO_CLEAN_AIR_FACTOR (9.83)      // Factor para aire limpio

#define CALIBARAION_SAMPLE_TIMES (50)   // Muestras para calibración
#define CALIBRATION_SAMPLE_INTERVAL (500) // Intervalo entre muestras de calibración
#define READ_SAMPLE_INTERVAL (50)       // Intervalo entre lecturas normales
#define READ_SAMPLE_TIMES (5)           // Muestras para lectura normal

#define GAS_LPG (0)
#define GAS_CO (1)
#define GAS_SMOKE (2)

float LPGCurve[3] = {2.3, 0.21, -0.47};
float COCurve[3] = {2.3, 0.72, -0.34};
float SmokeCurve[3] = {2.3, 0.53, -0.44};
float Ro = 10;  // Valor inicial de Ro

/****************** MQResistanceCalculation ****************************************/
float MQResistanceCalculation(int raw_adc) {
  if (raw_adc == 0) raw_adc = 1; // Evitar división por cero
  return ((float)RL_VALUE * (4095.0 - (float)raw_adc) / (float)raw_adc);
}

/***************************** MQCalibration ****************************************/
float MQCalibration(int mq_pin) {
  int i;
  float val = 0;
  
  for (i = 0; i < CALIBARAION_SAMPLE_TIMES; i++) {
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val / CALIBARAION_SAMPLE_TIMES;
  val = val / RO_CLEAN_AIR_FACTOR;
  return val;
}

/*****************************  MQRead *********************************************/
float MQRead(int mq_pin) {
  int i;
  float rs = 0;

  for (i = 0; i < READ_SAMPLE_TIMES; i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
  
  rs = rs / READ_SAMPLE_TIMES;
  return rs;
}

/*****************************  MQGetPercentage **********************************/
int MQGetPercentage(float rs_ro_ratio, float *pcurve) {
  if (rs_ro_ratio <= 0) rs_ro_ratio = 0.001; // Evitar log(0)
  return (pow(10, (((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0])));
}

/*****************************  MQGetGasPercentage **********************************/
int MQGetGasPercentage(float rs_ro_ratio, int gas_id) {
  if (gas_id == GAS_LPG) {
    return MQGetPercentage(rs_ro_ratio, LPGCurve);
  } else if (gas_id == GAS_CO) {
    return MQGetPercentage(rs_ro_ratio, COCurve);
  } else if (gas_id == GAS_SMOKE) {
    return MQGetPercentage(rs_ro_ratio, SmokeCurve);
  }
  return 0;
}

void setupSensors() {
  dht.begin();
  thermoCouple.begin();
  thermoCouple.setSPIspeed(4000000);
  delay(2000);

  // Calibrar sensor MQ-2
  Serial.print("Calibrando MQ-2...\n");
  Ro = MQCalibration(MQ_PIN);
  Serial.print("Calibracion MQ-2 completada...\n");
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm\n");
}

void updateSensors() {
  // Leer DHT22
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

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

  // Leer MQ-2 (Gas LPG y valor analógico)
  mq2AnalogValue = analogRead(MQ_PIN); // Lectura directa del valor analógico
  float rs = MQRead(MQ_PIN);
  lpgPPM = MQGetGasPercentage(rs / Ro, GAS_LPG);

  // Imprimir para depuración
  Serial.println("-----------------------------");
  Serial.print("Temp: "); Serial.print(temperature); Serial.println(" C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");
  Serial.print("Internal Temp: "); Serial.print(internalTemp); Serial.println(" C");
  Serial.print("Soil Moisture: "); Serial.print(soilMoisture); Serial.println("%");
  Serial.print("LPG: "); Serial.print(lpgPPM); Serial.println(" ppm");
  Serial.print("MQ-2 Analog: "); Serial.println(mq2AnalogValue);
}