#include <Wire.h>//libreria requrida para usar SDA y SCL

//Declaracion de variables
int Humedad=0;//Variable utilizada para calcular la humedad
const int AO=0;//variable para leer el valor del pin analogico
void setup(){
  Serial.begin(11520)
  pinMode(AO, INPUT)
}
void loop() 
{
  char buffer[10];//Variable para crear una cadena
  AO = analogRead(A0);//Leer el valor del potenciometro 
  Humedad = map(AO, 0, 1023, 100, 0);//Escala
  Serial.write(Humedad)
  delay(250);//Esperar 250 ms
}
