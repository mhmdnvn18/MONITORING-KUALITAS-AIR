// === KONFIGURASI PIN DAN PARAMETER ===
#define TDS_PIN 34             // Pin sensor TDS
#define TURBIDITY_PIN 35       // Pin sensor Turbidity
#define VREF 3.3               // Tegangan referensi ADC (ESP32: 3.3V)
#define ADC_RESOLUTION 4095.0  // Resolusi ADC 12-bit

// === KONFIGURASI FILTERING TDS ===
#define SCOUNT 30              // Jumlah sampel untuk median filter
int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
float temperature = 25.0;      // Suhu default untuk kompensasi

// === SETUP ===
void setup() {
  Serial.begin(115200);
  pinMode(TDS_PIN, INPUT);
  pinMode(TURBIDITY_PIN, INPUT);
}

// === LOOP ===
void loop() {
  // === BACA TDS ===
  static unsigned long tdsSampleTime = millis();
  if (millis() - tdsSampleTime > 40U) {
    tdsSampleTime = millis();
    analogBuffer[analogBufferIndex] = analogRead(TDS_PIN);
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) analogBufferIndex = 0;
  }

  // === CETAK DATA SETIAP 1 DETIK ===
  static unsigned long printTime = millis();
  if (millis() - printTime > 1000U) {
    printTime = millis();

    // Salin buffer untuk filtering
    for (int i = 0; i < SCOUNT; i++) {
      analogBufferTemp[i] = analogBuffer[i];
    }

    // Hitung median dan tegangan rata-rata TDS
    float averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (VREF / ADC_RESOLUTION);
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
    float compensationVoltage = averageVoltage / compensationCoefficient;

    // Konversi ke ppm (menggunakan rumus DFRobot TDS)
    float tdsValue = (133.42 * pow(compensationVoltage, 3)
                    - 255.86 * pow(compensationVoltage, 2)
                    + 857.39 * compensationVoltage) * 0.5;

    // === BACA TURBIDITY ===
    int turbidityRaw = analogRead(TURBIDITY_PIN);
    float turbidityVoltage = turbidityRaw * (VREF / ADC_RESOLUTION);

    // Rumus konversi ke NTU dari datasheet SEN0189
    float ntu = -1120.4 * turbidityVoltage * turbidityVoltage + 5742.3 * turbidityVoltage - 4352.9;
    if (ntu < 0) ntu = 0;

    // === TAMPILKAN DATA KE SERIAL ===
    Serial.println("=== Data Sensor TDS ===");
    Serial.print(" Tegangan TDS     : "); Serial.print(averageVoltage, 3); Serial.println(" V");
    Serial.print(" Nilai TDS        : "); Serial.print(tdsValue, 0); Serial.println(" ppm");

    Serial.println("\n=== Data Sensor Turbidity ===");
    Serial.print(" Tegangan Turbidity : "); Serial.print(turbidityVoltage, 3); Serial.println(" V");
    Serial.print(" Nilai NTU          : "); Serial.print(ntu, 1); Serial.println(" NTU");

    Serial.println();
  }
}

// === FUNGSI MEDIAN FILTER ===
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

  if (iFilterLen % 2 == 0)
    return (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  else
    return bTab[iFilterLen / 2];
}