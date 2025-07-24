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
