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
