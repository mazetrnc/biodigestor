#include <OneWire.h>
#include <DallasTemperature.h>

// Pines de los LEDs
const uint8_t ledR = 2;
const uint8_t ledG = 4;
const uint8_t ledB = 5;

// Sensor de temperatura
#define ONE_WIRE_BUS_temp 26
OneWire oneWire(ONE_WIRE_BUS_temp);
DallasTemperature sensors(&oneWire);

// Variable compartida
float tempC = 0.0;

void setupSensorsActuators() {
  sensors.begin();
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  
  ledcAttach(ledR, 12000, 8);
  ledcAttach(ledG, 12000, 8);
  ledcAttach(ledB, 12000, 8);
}

void updateSensorsActuators() {
  // Leer sensor
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);

  // Controlar LEDs según temperatura
  if (tempC < 32) {
    ledcWrite(ledR, 0);
    ledcWrite(ledG, 0);
    ledcWrite(ledB, 255);
  }
  else if (tempC >= 32 && tempC <= 38) {
    ledcWrite(ledR, 0);
    ledcWrite(ledG, 255);
    ledcWrite(ledB, 0);
  }
  else if (tempC > 38) {
    ledcWrite(ledR, 255);
    ledcWrite(ledG, 0);
    ledcWrite(ledB, 0);
  }
}

// Función para manejar la página principal (definida aquí para mantener todo junto)
void handleRoot() {
  String html = R"rawliteral(
  <html lang="es">
  <head>
    <meta charset="utf-8">
    <title>Sensores ws</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      @import url('https://fonts.googleapis.com/css2?family=Inter:wght@400;600&display=swap');

      body {
        margin: 0;
        padding: 0;
        font-family: 'Inter', 'Segoe UI', sans-serif;
        background-color: #0d1117;
        color: #c9d1d9;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        height: 100vh;
      }

      h1 {
        font-size: 2rem;
        font-weight: 600;
        margin-bottom: 1rem;
        color: #58a6ff;
      }

      p {
        font-size: 1.25rem;
        background: #161b22;
        padding: 1rem 2rem;
        border-radius: 12px;
        box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
        border: 1px solid #30363d;
      }

      span#temp {
        font-weight: bold;
        color: #f0f6fc;
      }
    </style>
    <script>
      setInterval(() => {
        fetch("/temp")
          .then(res => res.text())
          .then(data => {
            document.getElementById("temp").textContent = data + " °C";
          });
      }, 1000);
    </script>
  </head>
  <body>
    <h1>Temperatura</h1>
    <p>Actual: <span id="temp">Cargando...</span></p>
  </body>
  </html>
)rawliteral";

  server.send(200, "text/html", html);
}

// Función para manejar la solicitud de temperatura
void handleTemp() {
  server.send(200, "text/plain", String(tempC, 2));
}