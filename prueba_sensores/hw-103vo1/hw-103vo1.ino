const int soil = 4;
void setup(){
  Serial.begin(115200);
  pinMode(soil, INPUT);
}
void loop() {
int rawValue = analogRead(soil);
float humedadper = ((4095 - rawValue) / 4095.0)*100.0;
Serial.print("  Suelo: ");
Serial.print(humedadper);
Serial.println("% ");
delay(2000);
}
