# MyBME280

Lightweight Arduino library for the **BME280** environmental sensor   
Supports temperature (°C), humidity (%RH), pressure (hPa), and altitude (m) readings via I²C.

---

## Features

- Read **temperature**, **humidity**, and **pressure**
- Calculate **altitude** 
- Supports **I²C** (default address `0x76`)
- Simple, minimal, and educational
- Designed for **embedded projects** and **students**

---

## Getting Started

### Wiring (I2C)

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

---
```cpp
Example

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



```
## API  Reference



Function Description

begin() 
Initializes the BME280 sensor and prepares it for reading.

update() 
Reads and processes the latest temperature, pressure, and humidity data.

getTemperature() 
Returns the current temperature in degrees Celsius (°C).

getHumidity()
Returns the current relative humidity in percent (%RH).

getPressure()
Returns the current atmospheric pressure in hectopascals (hPa).

getAltitude(seaLevel_hPa)
Estimates the current altitude (in meters) based on the pressure and provided sea-level pressure.

printAll()
Prints all sensor readings (temperature, pressure, humidity) to the Serial Monitor.

setTempOffset(offset)
Adjusts the temperature reading by applying a manual offset (in °C).



📄 License

MIT License.
Made in Malaysia 🇲🇾 by fadhil-1911


