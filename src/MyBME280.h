#ifndef MYBME280_H

#define MYBME280_H

#include <Arduino.h>

#include <Wire.h>

class MyBME280 {

public:

//==============================

// Constructor

//==============================

MyBME280(uint8_t address = 0x76);

//==============================

// Setup & Status

//==============================

bool begin();

bool isConnected();

//==============================

// Update & Read

//==============================

void update();

float getTemperature();              // °C

float getPressure();                 // hPa

float getHumidity();                 // %RH

float getAltitude(float seaLevel_hPa = 1013.25); // m

//==============================

// Utility

//==============================

void printAll();

void setTempOffset(float offset);

private:

uint8_t _address;

float _temperature, _pressure, _humidity;

float _tempOffset;

//==============================

// Calibration Data

//==============================

uint16_t dig_T1;

int16_t  dig_T2, dig_T3;

uint16_t dig_P1;

int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

uint8_t  dig_H1, dig_H3;

int16_t  dig_H2, dig_H4, dig_H5, dig_H6;

int32_t t_fine;

//==============================

// Internal Use

//==============================

bool readCalibration();

bool readRawData();

void compensateAll();

uint16_t read16(uint8_t reg);

int16_t  readS16(uint8_t reg);

uint8_t  read8(uint8_t reg);

void     write8(uint8_t reg, uint8_t value);

};

#endif
