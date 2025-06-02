// === Library ===
#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <WiFi.h>
#include <HTTPClient.h>

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

// === Konfigurasi WiFi ===
const char* ssid = "iphone";         // Ganti dengan SSID WiFi Anda
const char* password = "12345678"; // Ganti dengan password WiFi Anda

// === Supabase Config ===
const char* supabase_url = "https://jaxoedhomiloedbddcuh.supabase.co";
const char* supabase_api_key = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImpheG9lZGhvbWlsb2VkYmRkY3VoIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDg4NTUyODcsImV4cCI6MjA2NDQzMTI4N30.rv1Ie4wfUaRtz-BIV8AYMocObV_ZZbYoR2VbZxShcIo";
const char* supabase_table = "water_quality"; // Nama tabel di Supabase

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);   // EEPROM untuk pH sensor
  ph.begin();

  pinMode(TDS_PIN, INPUT);
  pinMode(TURBIDITY_PIN, INPUT);
  pinMode(PH_PIN, INPUT);

  // === Koneksi ke WiFi ===
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void loop() {
  static unsigned long printTime = millis();
  if (millis() - printTime > 1000U) {
    printTime = millis();

    // === DATA DUMMY ===
    float phValue = 6.5 + (rand() % 300) / 100.0;         // 6.5 - 9.5
    float tdsValue = 100 + (rand() % 900);                // 100 - 999 ppm
    float turbidityVoltage = 0.5 + (rand() % 400) / 100.0; // 0.5 - 4.5 V

    // Kirim data dummy ke Supabase
    sendToSupabase(phValue, tdsValue, turbidityVoltage);

    // Tampilkan data dummy ke Serial
    Serial.println("=== Data Sensor (DUMMY) ===");
    Serial.print("TDS Tegangan : "); Serial.print(turbidityVoltage, 3); Serial.println(" V");
    Serial.print("TDS (ppm)    : "); Serial.println(tdsValue, 0);
    Serial.print("Turbidity Tegangan : "); Serial.print(turbidityVoltage, 3); Serial.println(" V");
    Serial.print("Turbidity (NTU)    : "); Serial.println((turbidityVoltage * 100), 1);
    Serial.print("pH Tegangan : "); Serial.print(phValue * 100, 0); Serial.println(" mV");
    Serial.print("pH Value    : "); Serial.println(phValue, 2);
    Serial.println();

    // --- KOMENTAR BAGIAN SENSOR ASLI ---
    /*
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

    // === Kirim data ke Supabase ===
    sendToSupabase(phValue, tdsValue, turbidityVoltage);

    // === Tampilkan Semua Data ===
    Serial.println("=== Data Sensor ===");
    Serial.print("TDS Tegangan : "); Serial.print(averageVoltageTDS, 3); Serial.println(" V");
    Serial.print("TDS (ppm)    : "); Serial.println(tdsValue, 0);
    Serial.print("Turbidity Tegangan : "); Serial.print(turbidityVoltage, 3); Serial.println(" V");
    Serial.print("Turbidity (NTU)    : "); Serial.println(ntu, 1);
    Serial.print("pH Tegangan : "); Serial.print(voltagePH, 0); Serial.println(" mV");
    Serial.print("pH Value    : "); Serial.println(phValue, 2);
    Serial.println();
    */
  }

  // === Kalibrasi pH jika diperlukan ===
  // ph.calibration(voltagePH, temperature);
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

// === Fungsi Kirim Data ke Supabase ===
void sendToSupabase(float ph, float tds, float turbidity) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(supabase_url) + "/rest/v1/" + supabase_table;
    http.begin(url);
    http.addHeader("apikey", supabase_api_key);
    http.addHeader("Authorization", "Bearer " + String(supabase_api_key));
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Prefer", "return=minimal");

    // Format JSON sesuai kolom tabel Supabase Anda
    String payload = "{\"ph\":" + String(ph, 2) +
                     ",\"tds\":" + String(tds, 0) +
                     ",\"turbidity\":" + String(turbidity, 2) + "}";

    int httpResponseCode = http.POST(payload);
    http.end();
    Serial.print("Supabase POST: ");
    Serial.println(httpResponseCode);
  }
}
