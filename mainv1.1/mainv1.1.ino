#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Maze";
const char* password = "00000000";

WebServer server(80);

// Declaraciones de las funciones en sensors.ino
void setupSensors();
void updateSensors();

// Declaraciones externas de las variables de los sensores
extern float temperature;
extern float humidity;
extern float methane;
extern float soilMoisture;
extern float internalTemp;

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
  server.on("/data", handleData);  // Ruta para obtener todos los datos en JSON
  
  server.begin();
  
  // Inicializar sensores
  setupSensors();
}

void loop() {
  server.handleClient();
  updateSensors();  // Actualizar lecturas de sensores
  delay(500);
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
      async function updateData() {
        try {
          const response = await fetch('/data');
          const data = await response.json();
          
          document.getElementById('temp').textContent = data.temperature.toFixed(2) + ' °C';
          document.getElementById('humidity').textContent = data.humidity.toFixed(2) + ' %';
          document.getElementById('methane').textContent = data.methane.toFixed(2) + ' ppm';
          document.getElementById('soil').textContent = data.soilMoisture.toFixed(2) + ' %';
          document.getElementById('internalTemp').textContent = data.internalTemp.toFixed(2) + ' °C';
        } catch (error) {
          console.error('Error al actualizar datos:', error);
        }
      }
      
      setInterval(updateData, 2000); // Actualizar cada 2 segundos
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
        <h2>Metano (CH4)</h2>
        <div id="methane" class="value">--</div>
      </div>
      
      <div class="sensor">
        <h2>Humedad del Suelo</h2>
        <div id="soil" class="value">--</div>
      </div>
      
      <div class="sensor">
        <h2>Temperatura Interna</h2>
        <div id="internalTemp" class="value">--</div>
      </div>
    </div>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleData() {
  String json = "{";
  json += "\"temperature\":" + String(temperature) + ",";
  json += "\"humidity\":" + String(humidity) + ",";
  json += "\"methane\":" + String(methane) + ",";
  json += "\"soilMoisture\":" + String(soilMoisture) + ",";
  json += "\"internalTemp\":" + String(internalTemp);
  json += "}";
  
  server.send(200, "application/json", json);
}