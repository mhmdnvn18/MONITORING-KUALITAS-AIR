# IoT Monitoring Kualitas Air

Proyek ini bertujuan untuk memonitor kualitas air secara real-time menggunakan sensor pH, turbidity, dan TDS yang terhubung ke mikrokontroler (misal: Arduino) dan mengirimkan data ke platform IoT.

## Sensor yang Digunakan
- **Sensor pH**: Untuk mengukur tingkat keasaman air.
- **Sensor Turbidity**: Untuk mengukur tingkat kekeruhan air.
- **Sensor TDS**: Untuk mengukur total padatan terlarut dalam air.

## Langkah Umum Pembuatan
1. **Persiapan Hardware**
   - Siapkan Arduino/ESP32/ESP8266.
   - Hubungkan sensor pH, turbidity, dan TDS ke board sesuai pinout masing-masing.
2. **Instalasi Library**
   - Instal library sensor yang diperlukan pada Arduino IDE.
3. **Pemrograman**
   - Tulis kode untuk membaca data dari sensor dan kirimkan ke platform IoT (misal: ThingSpeak, Blynk, MQTT, dsb).
4. **Pengujian**
   - Uji sensor dengan sampel air berbeda dan pastikan data terbaca dengan benar.
5. **Integrasi IoT**
   - Kirim data ke cloud untuk monitoring jarak jauh.

## Referensi
- [Grove PH Sensor Kit](https://wiki.seeedstudio.com/Grove-PH-Sensor-kit/)
- [PH Sensor E-201-C Datasheet](https://www.e-gizmo.net/oc/kits%20documents/PH%20Sensor%20E-201-C/PH%20Sensor%20E-201-C.pdf)
- [DFRobot Turbidity Sensor](https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189)
- [Arduino TDS Water Quality Sensor](https://randomnerdtutorials.com/arduino-tds-water-quality-sensor/)