// === Library ===
#include "DFRobot_PH.h"
#include <EEPROM.h>

// === Konfigurasi Pin ===
#define TDS_PIN       32    // Ganti pin TDS (jangan pakai yang sama dengan PH!)
#define TURBIDITY_PIN 35
#define PH_PIN        34

// === Konstanta ADC dan Tegangan ===
#define VREF 3.3
#define ADC_RESOLUTION 4095.0

// === Objek dan Variabel Global ===
DFRobot_PH ph;
float voltagePH, phValue;
float temperature = 25.0; // Gunakan default suhu, atau sesuaikan dengan sensor

#define SCOUNT 30
int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);   // EEPROM untuk pH sensor
  ph.begin();

  pinMode(TDS_PIN, INPUT);
  pinMode(TURBIDITY_PIN, INPUT);
  pinMode(PH_PIN, INPUT);
}

void loop() {
  static unsigned long sampleTime = millis();

  // === Sampling TDS setiap 40ms ===
  if (millis() - sampleTime > 40U) {
    sampleTime = millis();
    analogBuffer[analogBufferIndex] = analogRead(TDS_PIN);
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) analogBufferIndex = 0;
  }

  // === Cetak data setiap 1 detik ===
  static unsigned long printTime = millis();
  if (millis() - printTime > 1000U) {
    printTime = millis();

    // === Baca TDS ===
    for (int i = 0; i < SCOUNT; i++) analogBufferTemp[i] = analogBuffer[i];
    float averageVoltageTDS = getMedianNum(analogBufferTemp, SCOUNT) * (VREF / ADC_RESOLUTION);
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
    float compensationVoltage = averageVoltageTDS / compensationCoefficient;
    float tdsValue = (133.42 * pow(compensationVoltage, 3) -
                      255.86 * pow(compensationVoltage, 2) +
                      857.39 * compensationVoltage) * 0.5;

    // === Baca Turbidity ===
    int turbidityRaw = analogRead(TURBIDITY_PIN);
    float turbidityVoltage = turbidityRaw * (VREF / ADC_RESOLUTION);
    float ntu = -1120.4 * turbidityVoltage * turbidityVoltage + 5742.3 * turbidityVoltage - 4352.9;
    if (ntu < 0) ntu = 0;

    // === Baca pH ===
    int analogPH = analogRead(PH_PIN);
    voltagePH = analogPH / 4095.0 * 3300; // Konversi ke mV
    phValue = ph.readPH(voltagePH, temperature);

    // === Tampilkan Semua Data ===
    Serial.println("=== Data Sensor ===");

    Serial.print("TDS Tegangan : "); Serial.print(averageVoltageTDS, 3); Serial.println(" V");
    Serial.print("TDS (ppm)    : "); Serial.println(tdsValue, 0);

    Serial.print("Turbidity Tegangan : "); Serial.print(turbidityVoltage, 3); Serial.println(" V");
    Serial.print("Turbidity (NTU)    : "); Serial.println(ntu, 1);

    Serial.print("pH Tegangan : "); Serial.print(voltagePH, 0); Serial.println(" mV");
    Serial.print("pH Value    : "); Serial.println(phValue, 2);

    Serial.println();
  }

  // === Kalibrasi pH jika diperlukan ===
  ph.calibration(voltagePH, temperature);
}

// === Median Filter untuk TDS ===
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (int i = 0; i < iFilterLen; i++) bTab[i] = bArray[i];

  for (int j = 0; j < iFilterLen - 1; j++) {
    for (int i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        int temp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = temp;
      }
    }
  }

  return bTab[iFilterLen / 2];
}
