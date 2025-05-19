#define PH_PIN        34
#define TDS_PIN       35
#define TURBIDITY_PIN 32
#define VREF 3.3

#define TDS_SCOUNT 30
int tdsAnalogBuffer[TDS_SCOUNT];
int tdsAnalogBufferTemp[TDS_SCOUNT];
int tdsAnalogBufferIndex = 0;
float tdsAverageVoltage = 0;
float tdsValue = 0;
float tdsTemperature = 25.0; // default

int phBuf[10];
unsigned long phAvgValue = 0;

// Timing variables
unsigned long previousMillis_pH = 0;
unsigned long previousMillis_TDSsample = 0;
unsigned long previousMillis_TDScalc = 0;
unsigned long previousMillis_turbidity = 0;

void setup() {
  Serial.begin(115200);

  // Configure ADC attenuation to measure 0–3.3 V correctly
  analogSetPinAttenuation(PH_PIN, ADC_11db);          
  analogSetPinAttenuation(TDS_PIN, ADC_11db);         
  analogSetPinAttenuation(TURBIDITY_PIN, ADC_11db);   

  // Ensure 12-bit resolution (0–4095)
  analogReadResolution(12);
}

int getMedianNum(int bArray[], int iFilterLen) {
  // Use a fixed-size array instead of VLA
  int bTab[TDS_SCOUNT];
  for (int i = 0; i < iFilterLen; i++) {
    bTab[i] = bArray[i];
  }
  // Bubble sort (acceptable for small iFilterLen)
  for (int j = 0; j < iFilterLen - 1; j++) {
    for (int i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        int temp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = temp;
      }
    }
  }
  if (iFilterLen % 2 == 1) {
    return bTab[(iFilterLen - 1) / 2];
  } else {
    return (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
}

void readAndComputePH() {
  // Read 10 samples with minimal delay
  for (int i = 0; i < 10; i++) {
    phBuf[i] = analogRead(PH_PIN);
    delay(2); // ~20 ms total
  }
  // Sort the 10 readings
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (phBuf[i] > phBuf[j]) {
        int temp = phBuf[i];
        phBuf[i] = phBuf[j];
        phBuf[j] = temp;
      }
    }
  }
  phAvgValue = 0;
  for (int i = 2; i < 8; i++) {
    phAvgValue += phBuf[i];
  }
  float phVoltage = (phAvgValue / 6.0) * (VREF / 4095.0);
  float phValue = 3.5 * phVoltage;  // adjust calibration as needed
  Serial.print("pH: ");
  Serial.print(phValue, 2);
}

void computeAndPrintTDS() {
  // Copy buffer for median
  for (int i = 0; i < TDS_SCOUNT; i++) {
    tdsAnalogBufferTemp[i] = tdsAnalogBuffer[i];
  }
  int medianRaw = getMedianNum(tdsAnalogBufferTemp, TDS_SCOUNT);
  tdsAverageVoltage = (float)medianRaw * (VREF / 4095.0);
  float compensationCoefficient = 1.0 + 0.02 * (tdsTemperature - 25.0);
  float compensationVoltage = tdsAverageVoltage / compensationCoefficient;
  tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage
             - 255.86 * compensationVoltage * compensationVoltage
             + 857.39 * compensationVoltage) * 0.5;
  Serial.print(" | TDS: ");
  Serial.print(tdsValue, 0);
  Serial.print(" ppm");
}

void readAndPrintTurbidity() {
  int turbidityRaw = analogRead(TURBIDITY_PIN);
  float turbidityVoltage = (float)turbidityRaw * (VREF / 4095.0);
  // If you need NTU:
  // float turbidityNTU = mapOrComputeNTU(turbidityVoltage);
  Serial.print(" | Turbidity Voltage: ");
  Serial.print(turbidityVoltage, 2);
  Serial.print(" V");
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. pH every 1000 ms
  if (currentMillis - previousMillis_pH >= 1000UL) {
    previousMillis_pH = currentMillis;
    readAndComputePH();
  }

  // 2. TDS sample every 40 ms
  if (currentMillis - previousMillis_TDSsample >= 40UL) {
    previousMillis_TDSsample = currentMillis;
    tdsAnalogBuffer[tdsAnalogBufferIndex++] = analogRead(TDS_PIN);
    if (tdsAnalogBufferIndex >= TDS_SCOUNT) {
      tdsAnalogBufferIndex = 0;
    }
  }

  // 3. TDS compute & print every 800 ms
  if (currentMillis - previousMillis_TDScalc >= 800UL) {
    previousMillis_TDScalc = currentMillis;
    computeAndPrintTDS();
  }

  // 4. Turbidity every 1000 ms (or adjust as desired)
  if (currentMillis - previousMillis_turbidity >= 1000UL) {
    previousMillis_turbidity = currentMillis;
    readAndPrintTurbidity();
    Serial.println();
  }

  // No blocking delay() here—loop continues immediately
}
