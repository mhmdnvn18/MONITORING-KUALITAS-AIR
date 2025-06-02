void setup() {
  Serial.begin(9600); // Baud rate: 9600
}

void loop() {
  int sensorValue = analogRead(34); // Gunakan pin GPIO 34 sebagai input analog
  float voltage = sensorValue * (3.3 / 4095.0); // Konversi dari nilai ADC ke tegangan (3.3V referensi)
  Serial.println(voltage); // Tampilkan tegangan
  delay(500); // Delay 500 ms
}