// Pin assignment (ubah sesuai wiring Anda)
#define PH_PIN        34
#define TDS_PIN       35
#define TURBIDITY_PIN 32

#define VREF 3.3

// --- TDS Sensor ---
#define TDS_SCOUNT 30
int tdsAnalogBuffer[TDS_SCOUNT];
int tdsAnalogBufferTemp[TDS_SCOUNT];
int tdsAnalogBufferIndex = 0;
int tdsCopyIndex = 0;
float tdsAverageVoltage = 0;
float tdsValue = 0;
float tdsTemperature = 25; // suhu default untuk kompensasi

int getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

// --- pH Sensor ---
int phBuf[10], phTemp;
unsigned long int phAvgValue;

// --- Turbidity Sensor ---
// Tidak perlu buffer, cukup baca langsung

void setup() {
  Serial.begin(115200);
}

void loop() {
  // --- pH Sensor ---
  for(int i=0;i<10;i++) {
    phBuf[i]=analogRead(PH_PIN);
    delay(2);
  }
  for(int i=0;i<9;i++) {
    for(int j=i+1;j<10;j++) {
      if(phBuf[i]>phBuf[j]) {
        phTemp=phBuf[i];
        phBuf[i]=phBuf[j];
        phBuf[j]=phTemp;
      }
    }
  }
  phAvgValue=0;
  for(int i=2;i<8;i++)
    phAvgValue+=phBuf[i];
  float phVoltage=(float)phAvgValue*VREF/4095/6;
  float phValue=3.5*phVoltage; // kalibrasi bisa disesuaikan

  // --- TDS Sensor ---
  static unsigned long tdsAnalogSampleTimepoint = millis();
  if(millis()-tdsAnalogSampleTimepoint > 40U){
    tdsAnalogSampleTimepoint = millis();
    tdsAnalogBuffer[tdsAnalogBufferIndex] = analogRead(TDS_PIN);
    tdsAnalogBufferIndex++;
    if(tdsAnalogBufferIndex == TDS_SCOUNT){
      tdsAnalogBufferIndex = 0;
    }
  }
  static unsigned long tdsPrintTimepoint = millis();
  bool tdsReady = false;
  if(millis()-tdsPrintTimepoint > 800U){
    tdsPrintTimepoint = millis();
    for(tdsCopyIndex=0; tdsCopyIndex<TDS_SCOUNT; tdsCopyIndex++){
      tdsAnalogBufferTemp[tdsCopyIndex] = tdsAnalogBuffer[tdsCopyIndex];
    }
    tdsAverageVoltage = getMedianNum(tdsAnalogBufferTemp,TDS_SCOUNT) * (float)VREF / 4095.0;
    float compensationCoefficient = 1.0+0.02*(tdsTemperature-25.0);
    float compensationVoltage=tdsAverageVoltage/compensationCoefficient;
    tdsValue=(133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;
    tdsReady = true;
  }

  // --- Turbidity Sensor ---
  int turbidityRaw = analogRead(TURBIDITY_PIN);
  float turbidityVoltage = turbidityRaw * (VREF / 4095.0);

  // --- Output Serial ---
  Serial.print("pH: ");
  Serial.print(phValue,2);
  Serial.print(" | Turbidity Voltage: ");
  Serial.print(turbidityVoltage,2);
  Serial.print(" V");
  if (tdsReady) {
    Serial.print(" | TDS: ");
    Serial.print(tdsValue,0);
    Serial.print(" ppm");
  }
  Serial.println();

  delay(1000);
}