#include "temperature.h"

namespace temperature {
//----------------------------------------------------------------------
DHT22Sensor::DHT22Sensor(uint8_t pin) : DHT22::Sensor(pin), error(DHT22::ErrorCode::TOO_QUICK) {
}

//----------------------------------------------------------------------
void DHT22Sensor::body() {
  if (is_current_step()) {
    // 2s warm-up after power-on.
    next_step(DHT22_READ_INTERVAL);
    return;
  }

  if (is_current_step()) {
    // Serial.print("Requesting data...");
    error = readData();
    /*
    if (error) {
      Serial.println(error.getMessage());
    } else {
      Serial.println("OK");
      char c[20], h[20];
      dtostrf(getTemperatureC(), 6, 1, c);
      dtostrf(getHumidity(), 6, 1, h);
      Serial.print("T:");
      Serial.println(getTemperatureC());
      Serial.print("H:");
      Serial.println(getHumidity());
    }*/
    first_step();
    return;
  }
}

//----------------------------------------------------------------------
DS1820Sensor::DS1820Sensor(uint8_t pin) : DS1820::Sensor(pin) {
}

//----------------------------------------------------------------------
void DS1820Sensor::body() {
  if (is_current_step()) {
    if (init() == false) {
      repeat_step(1000);
      return;
    }

    startConversion();
    next_step(DS1820_READ_INTERVAL);
    return;
  }
  // Serial.print(pin);
  // Serial.print(" temp: ");
  // Serial.println(getTemperatureCInt());

  if (is_current_step()) {
    readData();
    first_step();
    return;
  }
}
}  // namespace temperature
