/***************************************************************
*  File Name    : MyBME280.cpp
*  Description  : Driver for the TM1637 4-digit 7-segment display
*                 with extended features such as decimal point,
*                 text, time display, and brightness control.
*
*  Version      : 0.1.0
*  Date         : 01 August 2025
*  Platform     : Arduino (Uno/Nano/Mega, etc.)
*  Author       : [fadhil1911]
*
*  This library supports:
*   - Number display with or without leading zeros
*   - Text and symbol display
*   - Time display (HH:MM)
*   - Decimal point control (manual & automatic)
*   - Support for uppercase/lowercase letters and special characters
*
*  Notes:
*   - 
*   - 
****************************************************************/

#include "MyBME280.h"
//==============================
// Constructor
//==============================
MyBME280::MyBME280(uint8_t address)
: _address(address), _tempOffset(0.0),
_temperature(0), _pressure(0), _humidity(0) {}

//==============================
// begin()
//==============================
bool MyBME280::begin() {
Wire.begin();

if (!isConnected()) return false;
    // Reset device (optional)
    write8(0xE0, 0xB6);
    delay(100);
    // Set oversampling & mode
    write8(0xF2, 0x01); // Humidity oversampling x1
    write8(0xF4, 0x27); // Temp & Pressure oversampling x1, mode normal
    write8(0xF5, 0xA0); // Standby 1000ms, filter off
    return readCalibration();
}
//==============================
// isConnected()
//==============================
bool MyBME280::isConnected() {
    Wire.beginTransmission(_address);
    return (Wire.endTransmission() == 0);
}

//==============================
// update()
//==============================
void MyBME280::update() {
    if (readRawData()) {
    compensateAll();
    }
  }

//==============================
// getTemperature()
//==============================
float MyBME280::getTemperature() {
    return _temperature + _tempOffset;
}

//==============================
// getPressure()
//==============================
float MyBME280::getPressure() {
    return _pressure;
}

//==============================
// getHumidity()
//==============================
float MyBME280::getHumidity() {
    return _humidity;
}

//==============================
// getAltitude()
//==============================
float MyBME280::getAltitude(float seaLevel_hPa) {
    return 44330.0 * (1.0 - pow(_pressure / seaLevel_hPa, 0.1903));
}

//==============================
// printAll()
//==============================
void MyBME280::printAll() {
    Serial.print("T=");
    Serial.print(getTemperature(), 2);
    Serial.print("°C  P=");
    Serial.print(getPressure(), 2);
    Serial.print("hPa  H=");
    Serial.print(getHumidity(), 2);
    Serial.println("%");
}

//==============================
// setTempOffset()
//==============================
void MyBME280::setTempOffset(float offset) {
    _tempOffset = offset;
}

//==============================
// readCalibration()
//==============================
bool MyBME280::readCalibration() {
    dig_T1 = read16(0x88);
    dig_T2 = readS16(0x8A);
    dig_T3 = readS16(0x8C);
    dig_P1 = read16(0x8E);
    dig_P2 = readS16(0x90);
    dig_P3 = readS16(0x92);
    dig_P4 = readS16(0x94);
    dig_P5 = readS16(0x96);
    dig_P6 = readS16(0x98);
    dig_P7 = readS16(0x9A);
    dig_P8 = readS16(0x9C);
    dig_P9 = readS16(0x9E);
    dig_H1 = read8(0xA1);
    dig_H2 = readS16(0xE1);
    dig_H3 = read8(0xE3);
    dig_H4 = (read8(0xE4) << 4) | (read8(0xE5) & 0x0F);
    dig_H5 = (read8(0xE6) << 4) | (read8(0xE5) >> 4);
    dig_H6 = (int8_t)read8(0xE7);
    return true;
}

//==============================
// readRawData()
//==============================
bool MyBME280::readRawData() {
    Wire.beginTransmission(_address);
    Wire.write(0xF7); // Start from pressure MSB
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)8);

if (Wire.available() < 8) return false;
    uint32_t adc_P = (Wire.read() << 12) | (Wire.read() << 4) | (Wire.read() >> 4);
    uint32_t adc_T = (Wire.read() << 12) | (Wire.read() << 4) | (Wire.read() >> 4);
    uint16_t adc_H = (Wire.read() << 8) | Wire.read();
    // Compensation below
    // Temperature
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) *
    ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    _temperature = (t_fine * 5 + 128) >> 8;
    _temperature /= 100.0;
    // Pressure
    int64_t p1 = (int64_t)t_fine - 128000;
    int64_t p2 = p1 * p1 * (int64_t)dig_P6;
    p2 = p2 + ((p1 * (int64_t)dig_P5) << 17);
    p2 = p2 + (((int64_t)dig_P4) << 35);
    p1 = ((p1 * p1 * (int64_t)dig_P3) >> 8) + ((p1 * (int64_t)dig_P2) << 12);
    p1 = (((((int64_t)1) << 47) + p1)) * ((int64_t)dig_P1) >> 33;

if (p1 == 0) return false;
    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - p2) * 3125) / p1;
    p1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    p2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + p1 + p2) >> 8) + (((int64_t)dig_P7) << 4);
    _pressure = p / 256.0 / 100.0; // Convert to hPa
    // Humidity
    int32_t h = (t_fine - ((int32_t)76800));
    h = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * h)) + 16384) >> 15) *
    (((((((h * ((int32_t)dig_H6)) >> 10) * (((h * ((int32_t)dig_H3)) >> 11) + 32768)) >> 10) + 2097152) *
    ((int32_t)dig_H2) + 8192) >> 14));
    h = h - (((((h >> 15) * (h >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4);
    h = (h < 0 ? 0 : h);
    h = (h > 419430400 ? 419430400 : h);
    _humidity = h >> 12;
    _humidity /= 1024.0;
    return true;
}

//==============================
// compensateAll()
//==============================
void MyBME280::compensateAll() {
    // Tidak diperlukan jika formula terus dalam readRawData()
}

//==============================
// Helper: read8()
//==============================
uint8_t MyBME280::read8(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)1);
    return Wire.read();
}

//==============================
// Helper: read16()
//==============================
uint16_t MyBME280::read16(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)2);
    uint16_t val = Wire.read();
    val |= (Wire.read() << 8);
    return val;
}

//==============================
// Helper: readS16()
//==============================
int16_t MyBME280::readS16(uint8_t reg) {
    return (int16_t)read16(reg);
}

//==============================
// Helper: write8()
//==============================
void MyBME280::write8(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(_address); 
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}
