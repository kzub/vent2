#include "barometr.h"

namespace barometr {

Sensor::Sensor() : LPS331() {
  Serial.print("const begin");
  begin();
}

void Sensor::body() {
  Serial.print("body begin");
  pressure = readPressureMillimetersHg();
  temperature = readTemperatureC();
  Serial.print("pressure:");
  Serial.println(pressure);
  Serial.print("temperature:");
  Serial.println(temperature);
  first_step(1000);
}
}  // namespace barometr