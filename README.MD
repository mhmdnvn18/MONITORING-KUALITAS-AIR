# IoT Monitoring Kualitas Air (ESP32)

Proyek ini bertujuan untuk memonitor kualitas air secara real-time menggunakan sensor pH, turbidity, dan TDS yang terhubung ke mikrokontroler ESP32 dan mengirimkan data ke platform IoT.

## Langkah-langkah Instalasi dan Penggunaan

### 1. Instalasi Arduino IDE
- Unduh dan instal Arduino IDE dari [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software).
- Jalankan Arduino IDE setelah instalasi selesai.

### 2. Menambahkan Board ESP32 ke Arduino IDE
- Buka menu **File > Preferences**.
- Pada kolom **Additional Boards Manager URLs**, tambahkan:
  ```
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```
- Klik **OK**.
- Buka menu **Tools > Board > Boards Manager**.
- Cari "ESP32" lalu klik **Install** pada "esp32 by Espressif Systems".

### 3. Pilih Board ESP32
- Buka menu **Tools > Board** dan pilih board ESP32 yang sesuai (misal: "ESP32 Dev Module").

### 4. Hubungkan ESP32 ke Komputer
- Gunakan kabel USB untuk menghubungkan ESP32 ke komputer.
- Pilih port yang sesuai di **Tools > Port**.

### 5. Persiapan Hardware
- Hubungkan sensor pH ke GPIO 34, TDS ke GPIO 35, dan Turbidity ke GPIO 32 (atau sesuaikan dengan kebutuhan).
- Pastikan semua sensor terhubung ke pin ADC ESP32.

### 6. Upload Program
- Buka kode program gabungan pada folder `ESP32`.
- Klik tombol **Upload** di Arduino IDE untuk mengunggah program ke ESP32.

### 7. Monitoring Data
- Buka **Serial Monitor** di Arduino IDE (baudrate 115200).
- Amati data pH, turbidity, dan TDS yang terbaca.

### 8. Integrasi IoT (Opsional)
- Modifikasi kode untuk mengirim data ke platform IoT seperti ThingSpeak, Blynk, atau MQTT sesuai kebutuhan.

## Sensor yang Digunakan
- **Sensor pH**: Untuk mengukur tingkat keasaman air.
- **Sensor Turbidity**: Untuk mengukur tingkat kekeruhan air.
- **Sensor TDS**: Untuk mengukur total padatan terlarut dalam air.

## Contoh Pin ESP32
- pH Sensor: GPIO 34 (ADC1_CH6)
- TDS Sensor: GPIO 35 (ADC1_CH7)
- Turbidity Sensor: GPIO 32 (ADC1_CH4)

> **Catatan:** Pastikan sensor terhubung ke pin ADC yang sesuai pada ESP32.

## Langkah Umum Pembuatan
1. **Persiapan Hardware**
   - Siapkan ESP32.
   - Hubungkan sensor pH, turbidity, dan TDS ke pin ADC ESP32 sesuai kebutuhan.
2. **Instalasi Library**
   - Tidak memerlukan library khusus, cukup gunakan fungsi `analogRead()` pada Arduino IDE.
3. **Pemrograman**
   - Tulis kode untuk membaca data dari ketiga sensor secara bersamaan dan kirimkan ke platform IoT (misal: ThingSpeak, Blynk, MQTT, dsb).
   - Contoh kode gabungan tersedia pada folder `ESP32`.
4. **Pengujian**
   - Uji sensor dengan sampel air berbeda dan pastikan data terbaca dengan benar di Serial Monitor.
5. **Integrasi IoT**
   - Kirim data ke cloud untuk monitoring jarak jauh sesuai kebutuhan aplikasi Anda.

## Contoh Output Serial
```
pH: 7.12 | Turbidity Voltage: 2.45 V | TDS: 350 ppm
```

## Topologi Wiring

Berikut adalah ilustrasi sederhana topologi wiring antara ESP32 dan sensor-sensor air:

```
+-------------------+         +-------------------+
|                   |         |                   |
|     Sensor pH     |---------|    ESP32 Board    |
|   (VCC, GND, S)   |         |                   |
+-------------------+         |   3V3/5V   (VCC)  |
                              |   GND      (GND)  |
+-------------------+         |   GPIO 34  (pH S) |
|                   |         |   GPIO 35  (TDS S)|
|   Sensor TDS      |---------|   GPIO 32  (Turb) |
|   (VCC, GND, S)   |         +-------------------+
+-------------------+
                              Legend:
+-------------------+         S = Signal pin sensor
|                   |
| Sensor Turbidity  |
|   (VCC, GND, S)   |
+-------------------+
```

### Tabel Wiring

| Sensor         | Pin Sinyal ke ESP32 | VCC      | GND      |
|----------------|---------------------|----------|----------|
| pH             | GPIO 34 (ADC1_CH6)  | 3.3V/5V* | GND      |
| TDS            | GPIO 35 (ADC1_CH7)  | 3.3V/5V* | GND      |
| Turbidity      | GPIO 32 (ADC1_CH4)  | 3.3V/5V* | GND      |

> *Catatan:*
> - Beberapa sensor dapat menggunakan 3.3V atau 5V, cek datasheet sensor Anda.
> - Pastikan pin sinyal sensor dihubungkan ke pin ADC (analog input) pada ESP32.

## Referensi
- [Grove PH Sensor Kit](https://wiki.seeedstudio.com/Grove-PH-Sensor-kit/)
- [PH Sensor E-201-C Datasheet](https://www.e-gizmo.net/oc/kits%20documents/PH%20Sensor%20E-201-C/PH%20Sensor%20E-201-C.pdf)
- [DFRobot Turbidity Sensor](https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189)
- [Arduino TDS Water Quality Sensor](https://randomnerdtutorials.com/arduino-tds-water-quality-sensor/)