# Changelog – MyBME280

## [v0.1.3-alpha] – 2025-08-04

### Update 

- Fix: readRawData() add returns true
- Prevents undefined behavior and false-negative checks in user code

## [v0.1.2-alpha] – 2025-08-03

### Update 

-fix overflow, fake positif (dig_H4 & dig_H5)
-use float  instead shift bit integer for readRawData humanity
-update comment

## [v0.1.1-alpha] – 2025-08-02

### Update 

- Update file and folder structure
- Update header comment
- Update funtion comment

## [v0.1.0-alpha] – 2025-07-24

### Initial Alpha Release

- Implemented I2C communication with BME280 (default 0x76)
- Added temperature, humidity, and pressure reading
- Altitude estimation via `getAltitude()`
- Calibration read from sensor registers
- `update()` method handles all read & process
- `printAll()` method for easy serial output
- Temperature offset function (`setTempOffset()`)

### Notes

- Only supports I2C (no SPI yet)
- Limited error checking
- Tested on Arduino Nano & Uno
