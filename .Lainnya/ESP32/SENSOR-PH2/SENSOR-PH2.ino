#include "DFRobot_PH.h"
#include <EEPROM.h>

#define PH_PIN 34  // Gunakan GPIO34 sebagai input analog untuk pH sensor (ubah jika berbeda)
float voltage, phValue, temperature = 25.0;
DFRobot_PH ph;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);  // Inisialisasi EEPROM untuk ESP32
  ph.begin();
}

void loop() {
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) {  // interval 1 detik
    timepoint = millis();

    // temperature = readTemperature(); // Gunakan jika ada sensor suhu
    int analogValue = analogRead(PH_PIN);
    voltage = analogValue / 4095.0 * 3300;  // Konversi ADC 12-bit ESP32 ke mV (asumsi 3.3V referensi)
    phValue = ph.readPH(voltage, temperature);  // Hitung nilai pH

    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print(" °C\tpH: ");
    Serial.println(phValue, 2);
  }

  ph.calibration(voltage, temperature);  // Kalibrasi melalui Serial Command
}

float readTemperature() {
  // Tambahkan kode sensor suhu (DS18B20 atau lainnya) di sini jika diperlukan
  return 25.0; // Placeholder suhu default
}
