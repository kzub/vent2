#include "DHT22.h"

// Connect a 4.7K resistor between VCC and the data pin if absent
#define DHT22_PIN 2

DHT22::Sensor sensor(DHT22_PIN);

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
  }
  Serial.println("Initialized");
}

void loop(void) {
  // 2s warm-up after power-on.
  delay(2000);

  Serial.print("Requesting data...");
  auto error = sensor.readData();
  if (error) {
    Serial.println(error.getMessage());
    return;
  }

  Serial.print("Got Data ");
  Serial.print(sensor.getTemperatureC());
  Serial.print("C ");
  Serial.print(sensor.getHumidity());
  Serial.println("%");

  //     char buf[128];
  //     sprintf(buf, "Integer-only reading: Temperature %hi.%01hi C, Humidity %i.%01i %% RH",
  //             sensor.getTemperatureCInt() / 10, abs(sensor.getTemperatureCInt() % 10),
  //             sensor.getHumidityInt() / 10, sensor.getHumidityInt() % 10);
  //     Serial.println(buf);
  //     break;
}
