#include "barometr.h"

int ledPin = 13;  // select the pin for the LED

barometr::Sensor barometer;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {
  }
  Serial.println("Initialized");
  barometer.begin();
}

void loop() {
  float pressure = barometer.readPressureMillibars();
  float pressure2 = barometer.readPressureMillimetersHg();
  float altitude = barometer.pressureToAltitudeMeters(pressure);
  float temperature = barometer.readTemperatureC();

  // Вывод данных в Serial порт
  Serial.print("p: ");
  Serial.print(pressure);
  Serial.print(" mbar \t");
  Serial.print("p: ");
  Serial.print(pressure2);
  Serial.print(" mm \t");
  Serial.print("h: ");
  Serial.print(altitude);
  Serial.print(" m \t");
  Serial.print("t: ");
  Serial.print(temperature);
  Serial.println(" C");
  delay(5000);
}
