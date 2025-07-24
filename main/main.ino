#include <WiFi.h>
#include <WebServer.h>

// Configuración WiFi
const char* ssid = "JJMM-TISSU_ROSE";
const char* password = "trb12345678";

// Variable compartida para la temperatura
extern float tempC;

WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Configurar WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado: IP = " + WiFi.localIP().toString());

  // Configurar rutas del servidor
  server.on("/", handleRoot);
  server.on("/temp", handleTemp);
  server.begin();

  // Inicializar sensores y actuadores
  setupSensorsActuators();
}

void loop() {
  server.handleClient();
  updateSensorsActuators();
  delay(100);
}