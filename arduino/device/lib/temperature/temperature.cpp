#include "temperature.hpp"

namespace temperature {
//----------------------------------------------------------------------
Sensor::Sensor(uint8_t pin) : DHT22::Sensor(pin), error(DHT22::ErrorCode::TOO_QUICK) {
}

//----------------------------------------------------------------------
void Sensor::body() {
  if (is_current_step()) {
    // 2s warm-up after power-on.
    next_step(DHT22_READ_INTERVAL);
    return;
  }

  if (is_current_step()) {
    // Serial.print("Requesting data...");
    error = readData();
    /*if (error) {
      Serial.println(error.getMessage());
    } else {
      Serial.println("OK");
    }*/
    repeat_step(DHT22_READ_INTERVAL);
    return;
  }
}
}  // namespace temperature
