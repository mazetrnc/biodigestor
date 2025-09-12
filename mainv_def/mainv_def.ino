#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Maze";
const char* password = "01010101";

WebServer server(80);

// Declaraciones de las funciones en sensors.ino
void setupSensors();
void updateSensors();

// Declaraciones externas de las variables de los sensores
extern float temperature;
extern float humidity;
extern float soilMoisture;
extern float internalTemp;
extern float lpgPPM;
extern int mq2AnalogValue;
extern float ph;

void setup() {
  Serial.begin(115200);
  
  // Conectar a WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado: IP = " + WiFi.localIP().toString());
  
  // Configurar rutas del servidor
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/temp", handleTemp);
  server.on("/humidity", handleHumidity);
  server.on("/soil", handleSoil);
  server.on("/internalTemp", handleInternalTemp);
  server.on("/lpg", handleLPG);
  server.on("/mq2analog", handleMQ2Analog);
  server.on("/ph", handlePh);

  
  server.begin();
  
  // Inicializar sensores
  setupSensors();
}

void loop() {
  server.handleClient();
  updateSensors();
  delay(500);
}

// Handlers individuales para cada sensor
void handleTemp() {
  server.send(200, "text/plain", String(temperature));
}

void handleHumidity() {
  server.send(200, "text/plain", String(humidity));
}

void handleSoil() {
  server.send(200, "text/plain", String(soilMoisture));
}

void handleInternalTemp() {
  server.send(200, "text/plain", String(internalTemp));
}

void handleLPG() {
  server.send(200, "text/plain", String(lpgPPM));
}

void handleMQ2Analog() {
  server.send(200, "text/plain", String(mq2AnalogValue));
}

void handlePh() {
  server.send(200, "text/plain", String(ph));
}

void handleData() {
  String json = "{";
  json += "\"temperature\":" + String(temperature) + ",";
  json += "\"humidity\":" + String(humidity) + ",";
  json += "\"soilMoisture\":" + String(soilMoisture) + ",";
  json += "\"internalTemp\":" + String(internalTemp) + ",";
  json += "\"lpgPPM\":" + String(lpgPPM) + ",";
  json += "\"mq2AnalogValue\":" + String(mq2AnalogValue);
  json += "\"ph\":" + String(ph);
  json += "}";
  
  server.send(200, "application/json", json);
}

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="es">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Sensores Biodigestor</title>
    <style>
      body {
        font-family: 'Arial', sans-serif;
        background-color: #f0f0f0;
        margin: 0;
        padding: 20px;
      }
      .container {
        max-width: 800px;
        margin: 0 auto;
        background-color: white;
        padding: 20px;
        border-radius: 10px;
        box-shadow: 0 0 10px rgba(0,0,0,0.1);
      }
      .sensor {
        margin-bottom: 15px;
        padding: 10px;
        border: 1px solid #ddd;
        border-radius: 5px;
      }
      .sensor h2 {
        margin-top: 0;
        color: #333;
      }
      .value {
        font-size: 24px;
        font-weight: bold;
        color: #2c3e50;
      }
    </style>
    <script>
      function updateSensor(id, endpoint, unit = '') {
        fetch(endpoint)
          .then(res => res.text())
          .then(data => {
            document.getElementById(id).textContent = data + unit;
          })
          .catch(err => {
            console.error('Error updating ' + id + ':', err);
          });
      }

      function updateAllSensors() {
        updateSensor('temp', '/temp', ' °C');
        updateSensor('humidity', '/humidity', ' %');
        updateSensor('soil', '/soil', ' %');
        updateSensor('internalTemp', '/internalTemp', ' °C');
        updateSensor('lpg', '/lpg', ' ppm');
        updateSensor('mq2Analog', '/mq2analog');
        updateSensor('ph', '/ph');
      }

      // Actualizar cada 500ms (medio segundo)
      setInterval(updateAllSensors, 500);
      document.addEventListener('DOMContentLoaded', updateAllSensors);
    </script>
  </head>
  <body>
    <div class="container">
      <h1>Monitoreo Biodigestor</h1>
      
      <div class="sensor">
        <h2>Temperatura Ambiente</h2>
        <div id="temp" class="value">--</div>
      </div>
      
      <div class="sensor">
        <h2>Humedad Ambiente</h2>
        <div id="humidity" class="value">--</div>
      </div>
      
      <div class="sensor">
        <h2>Humedad del Suelo</h2>
        <div id="soil" class="value">--</div>
      </div>
      
      <div class="sensor">
        <h2>Temperatura Interna</h2>
        <div id="internalTemp" class="value">--</div>
      </div>
      
      <div class="sensor">
        <h2>Gas LPG</h2>
        <div id="lpg" class="value">--</div>
      </div>
      
      <div class="sensor">
        <h2>Valor Analógico MQ-2</h2>
        <div id="mq2Analog" class="value">--</div>
      </div>
      
      <div class="sensor">
        <h2>pH</h2>
        <div id="ph" class="value">--</div>
      </div>
    </div>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}
