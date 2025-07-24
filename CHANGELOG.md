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
