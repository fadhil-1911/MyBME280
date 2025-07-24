# MyBME280

Lightweight Arduino library for the **BME280** environmental sensor   
Supports temperature (°C), humidity (%RH), pressure (hPa), and altitude (m) readings via I²C.

---

## Features

- Read **temperature**, **humidity**, and **pressure**
- Calculate **altitude** (based on sea-level pressure)
- Supports **I²C** (default address `0x76`)
- Simple, minimal, and educational
- Designed for **embedded projects** and **students**

---

## 🔧 Getting Started

### 🔌 Wiring (I2C)

| BME280 Pin | Arduino |
|------------|---------|
| VCC        | 3.3V or 5V |
| GND        | GND     |
| SDA        | A4 (Uno/Nano) |
| SCL        | A5 (Uno/Nano) |

### Installation

1. Clone or download ZIP:
   ```bash
   https://github.com/fadhil-1911/MyBME280

	2.	Extract to your Arduino/libraries folder
	3.	Restart Arduino IDE

⸻

🚀 Example

#include <Wire.h>
#include <MyBME280.h>

MyBME280 bme;

void setup() {
  Serial.begin(9600);
  if (!bme.begin()) {
    Serial.println("Sensor not detected!");
    while (1);
  }
}

void loop() {
  bme.update();

  Serial.print("Temp: ");
  Serial.print(bme.getTemperature());
  Serial.print(" °C,  Hum: ");
  Serial.print(bme.getHumidity());
  Serial.print(" %,  Press: ");
  Serial.print(bme.getPressure());
  Serial.println(" hPa");

  delay(1000);
}


⸻

📘 API Reference

Function	Description
begin()	Inisialisasi sensor
update()	Baca dan proses data semasa
getTemperature()	Dapatkan suhu (°C)
getHumidity()	Dapatkan kelembapan (%RH)
getPressure()	Dapatkan tekanan udara (hPa)
getAltitude(seaLevel_hPa)	Anggaran altitud (meter)
printAll()	Cetak semua data ke Serial
setTempOffset(offset)	Laras suhu secara manual


⸻

🛣️ Roadmap
	•	SPI support
	•	Oversampling config
	•	Error handling improvement
	•	Unit test integration

⸻

📄 License

MIT License.
Made in Malaysia 🇲🇾 by fadhil-1911

---

## ✅ 2. Contoh `examples/SimpleRead/SimpleRead.ino`

```cpp
#include <Wire.h>
#include <MyBME280.h>

MyBME280 bme;

void setup() {
  Serial.begin(9600);
  if (!bme.begin()) {
    Serial.println("❌ BME280 not found. Check wiring!");
    while (1); // Halt
  }
}

void loop() {
  bme.update();
  bme.printAll(); // Print Temp, Pressure, Humidity
  delay(1000);
}


⸻

✅ 3. CHANGELOG.md – v0.1.0-alpha

# Changelog – MyBME280

## [v0.1.0-alpha] – 2025-07-24

### 🚀 Initial Alpha Release

- Implemented I2C communication with BME280 (default 0x76)
- Added temperature, humidity, and pressure reading
- Altitude estimation via `getAltitude()`
- Calibration read from sensor registers
- `update()` method handles all read & process
- `printAll()` method for easy serial output
- Temperature offset function (`setTempOffset()`)

### ⚠️ Notes

- Only supports I2C (no SPI yet)
- Limited error checking
- Tested on Arduino Nano & Uno


⸻

Seterusnya?

📁 Folder repo kamu sepatutnya begini:

MyBME280/
├── src/
│   ├── MyBME280.h
│   └── MyBME280.cpp
├── examples/
│   └── SimpleRead/
│       └── SimpleRead.ino
├── README.md
├── CHANGELOG.md
└── library.properties

Nak saya bantu generate library.properties juga?
Atau tolong semak gaya penamaan dan dokumentasi fungsi MyBME280.h untuk Doxygen?
