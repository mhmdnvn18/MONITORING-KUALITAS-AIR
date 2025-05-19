#define TURBIDITY_PIN 34 // Gunakan GPIO 34 sebagai pin ADC ESP32

void setup() {
  Serial.begin(9600); //Baud rate: 9600
}

void loop() {
  int sensorValue = analogRead(TURBIDITY_PIN); // baca input pada pin analog ESP32
  float voltage = sensorValue * (3.3 / 4095.0); // Konversi ADC ESP32 (0-4095) ke tegangan (0-3.3V)
  Serial.println(voltage); // print out the value you read:
  delay(500);
}

