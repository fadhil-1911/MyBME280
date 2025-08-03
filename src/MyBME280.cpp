/***************************************************************
*  File Name    : MyBME280.h
*  Description  : Lightweight Arduino library for the BME280 
                   environmental sensor
                  Supports temperature (°C), humidity (%RH), 
                   pressure (hPa), and altitude (m) readings via I²C.
*
*  Version      : 0.1.2
*  Date         : 03 August 2025
*  Platform     : Arduino (Uno/Nano/Mega, etc.)
*  Author       : [fadhil1911/https://github.com/fadhil-1911]
*  Maintainer    :Fadhil <fadhil.hashim755@yahoo.com>
*
*  This library supports:
*   - Read temperature, humidity, and pressure
*   - Calculate altitude
*   - Supports I²C (default address 0x76)
*   - Simple and minimal
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
  write8(0xF2, 0x01);  // Humidity oversampling x1
  write8(0xF4, 0x27);  // Temp & Pressure oversampling x1, mode normal
  write8(0xF5, 0xA0);  // Standby 1000ms, filter off
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
  Serial.print(getTemperature(), 3);
  Serial.print("°C P=");
  Serial.print(getPressure(), 2);
  Serial.print("hPa H=");
  Serial.print(getHumidity(), 2);
  Serial.println("%");


  /*
  Serial.print("dig_H1 = ");
  Serial.println(dig_H1);
  Serial.print("dig_H2 = ");
  Serial.println(dig_H2);
  Serial.print("dig_H3 = ");
  Serial.println(dig_H3);
  Serial.print("dig_H4 = ");
  Serial.println(dig_H4);
  Serial.print("dig_H5 = ");
  Serial.println(dig_H5);
  Serial.print("dig_H6 = ");
  Serial.println(dig_H6);
*/
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
  dig_T1 = read16(0x88); // Penentukuran faktor skala untuk suhu
  dig_T2 = readS16(0x8A); // Penentukuran offset suhu
  dig_T3 = readS16(0x8C); // Penentukuran bukan linear suhu
  dig_P1 = read16(0x8E); // Faktor skala asas (selalu positif)
  dig_P2 = readS16(0x90); // Pembetulan linear tekanan
  dig_P3 = readS16(0x92); // Pembetulan bukan linear (kuasa dua t_fine)
  dig_P4 = readS16(0x94); // Offset tekanan (bergantung suhu)
  dig_P5 = readS16(0x96); // Faktor pembetulan tekanan tambahan
  dig_P6 = readS16(0x98); // Skala lanjut untuk bukan linear
  dig_P7 = readS16(0x9A); // Term faktor tambahan untuk tekanan
  dig_P8 = readS16(0x9C); // Pembetulan tekanan tinggi
  dig_P9 = readS16(0x9E); // Skala akhir tekanan bukan linear
  dig_H1 = read8(0xA1); // Faktor linear pertama dalam pembetulan kelembapan
  dig_H2 = readS16(0xE1); // Faktor utama pembetulan kelembapan (signed), skala besar
  dig_H3 = read8(0xE3); // // Faktor linear kedua (unsigned), untuk pembetulan kecil
  //dig_H4 = (read8(0xE4) << 4) | (read8(0xE5) & 0x0F); // bug (overflow, fake positif)
  //dig_H5 = (read8(0xE6) << 4) | (read8(0xE5) >> 4); // bug (overflow, fake positif)
  dig_H4 = ((int16_t)read8(0xE4) << 4) | (read8(0xE5) & 0x0F); // bug fixed | 	Offset kelembapan bergantung suhu
  dig_H5 = ((int16_t)read8(0xE6) << 4) | (read8(0xE5) >> 4); // bug fixed | Pembetulan tambahan bergantung suhu (termasuk nonlinear)
  dig_H6 = (int8_t)read8(0xE7); // Nilai pembetulan kecil terakhir untuk kelembapan (signed 8-bit)
  return true;
}

//==============================
// readRawData()
//==============================
bool MyBME280::readRawData() {
  // Baca 8 byte sekaligus: temp(3) + pres(3) + hum(2)
  Wire.beginTransmission(_address);
  Wire.write(0xF7);  // Register tekanan MSB
  if (Wire.endTransmission() != 0) return false;
  Wire.requestFrom(_address, (uint8_t)8);
  if (Wire.available() < 8) return false;

  // Baca data mentah
  uint32_t adc_P = ((uint32_t)Wire.read() << 12) | ((uint32_t)Wire.read() << 4) | (Wire.read() >> 4);
  uint32_t adc_T = ((uint32_t)Wire.read() << 12) | ((uint32_t)Wire.read() << 4) | (Wire.read() >> 4);
  uint16_t adc_H = ((uint16_t)Wire.read() << 8) | Wire.read();
  //Serial.print("adc_H = ");
  //Serial.println(adc_H);


  // Pastikan nilai tidak 0x80000 (not valid)
  if (adc_T == 0x80000 || adc_P == 0x80000 || adc_H == 0x8000) return false;

  // ===== SUHU (°C) =====
  int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
  int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  _temperature = (t_fine * 5 + 128) >> 8;  // in 0.01 °C
  _temperature /= 100.0;                   // convert to °C

  // ===== TEKANAN (hPa) =====
  int64_t var1_p = ((int64_t)t_fine) - 128000;
  int64_t var2_p = var1_p * var1_p * (int64_t)dig_P6;
  var2_p = var2_p + ((var1_p * (int64_t)dig_P5) << 17);
  var2_p = var2_p + (((int64_t)dig_P4) << 35);
  var1_p = ((var1_p * var1_p * (int64_t)dig_P3) >> 8) + ((var1_p * (int64_t)dig_P2) << 12);
  var1_p = (((((int64_t)1) << 47) + var1_p)) * ((int64_t)dig_P1) >> 33;

  if (var1_p == 0) {
    _pressure = 0;  // avoid divide by zero
  } else {
    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2_p) * 3125) / var1_p;
    var1_p = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2_p = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1_p + var2_p) >> 8) + (((int64_t)dig_P7) << 4);
    _pressure = (float)p / 256.0 / 100.0;  // convert to hPa
  }

  // ===== KELEMBAPAN (%RH) ===== (rebuilt)
  // Formula dari datasheet Bosch (ringkas & tepat)
  float var_H = (((float)t_fine) - 76800.0);
  var_H = (adc_H - (((float)dig_H4) * 64.0 + ((float)dig_H5) / 16384.0 * var_H)) * (((float)dig_H2) / 65536.0 * (1.0 + ((float)dig_H6) / 67108864.0 * var_H * (1.0 + ((float)dig_H3) / 67108864.0 * var_H)));
  var_H = var_H * (1.0 - ((float)dig_H1) * var_H / 524288.0);

  if (var_H > 100.0)
    var_H = 100.0;
  else if (var_H < 0.0)
    var_H = 0.0;

  _humidity = var_H;
}

/*
  // Humidity (Asal)
  int32_t h = (t_fine - ((int32_t)76800));
  h = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * h)) + 16384) >> 15) * (((((((h * ((int32_t)dig_H6)) >> 10) * (((h * ((int32_t)dig_H3)) >> 11) + 32768)) >> 10) + 2097152) * ((int32_t)dig_H2) + 8192) >> 14));
  h = h - (((((h >> 15) * (h >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4);
  h = (h < 0 ? 0 : h);
  h = (h > 419430400 ? 419430400 : h);
  _humidity = h >> 12;
  _humidity /= 1024.0;
  return true;
} */

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
