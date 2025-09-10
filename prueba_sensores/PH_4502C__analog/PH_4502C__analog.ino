const int po = 32;
void setup() {
  // put your setup code here, to run once:
pinMode(po,INPUT);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
int value;
float voltaje;
value = analogRead(po);
Serial.print(value);
voltaje = (value * 3.3) / 4095.0;
Serial.println(voltaje);
}
