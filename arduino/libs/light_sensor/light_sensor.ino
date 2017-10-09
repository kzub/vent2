#include "ls.h"

int sensorPin = A0;   // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor

light::Sensor sensor(sensorPin);

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {
  }
  Serial.println("Initialized");
}

void loop() {
  sensorValue = sensor.get();

  digitalWrite(ledPin, HIGH);
  delay(sensorValue);

  Serial.println(sensorValue);

  digitalWrite(ledPin, LOW);
  delay(light::MAX_VALUE - sensorValue);
}
