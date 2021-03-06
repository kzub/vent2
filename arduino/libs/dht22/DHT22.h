#ifndef _DHT22_H_
#define _DHT22_H_

#include <inttypes.h>
namespace DHT22 {

#define DHT22_ERROR_VALUE -995
#define DHT22_CHECK_STEP_US 4

enum class ErrorCode : uint8_t {
  NONE = 0,
  BUS_HUNG,
  NOT_PRESENT,
  ACK_TOO_LONG,
  SYNC_TIMEOUT,
  DATA_TIMEOUT,
  CHECKSUM,
  TOO_QUICK
};

class Error {
 public:
  Error(ErrorCode code) : err(code) {
  }
  operator bool() {
    return err != ErrorCode::NONE;
  }
  const char *getMessage();

 private:
  ErrorCode err;
};

class Sensor {
 private:
  uint8_t bitmask;
  volatile uint8_t *reg;
  unsigned long lastReadTime;
  short int lastHumidity;
  short int lastTemperature;

 public:
  Sensor(uint8_t pin);
  Error readData();
  Error waitForLow(uint8_t us, Error stage);
  Error waitForHigh(uint8_t us, Error stage);
  Error measureHigh(uint8_t us, Error stage, uint8_t &retryCount);
  short int getHumidityInt();
  short int getTemperatureCInt();
  float getHumidity();
  float getTemperatureC();
  float getTemperatureF();
};

// Report the humidity in .1 percent increments, such that 635 means 63.5% relative humidity
// Converts from the internal integer format on demand, so you might want
// to cache the result.
inline short int Sensor::getHumidityInt() {
  return lastHumidity;
}

// Get the temperature in decidegrees C, such that 326 means 32.6 degrees C.
// The temperature may be negative, so be careful when handling the fractional part.
inline short int Sensor::getTemperatureCInt() {
  return lastTemperature;
}

// Return the percentage relative humidity in decimal form
inline float Sensor::getHumidity() {
  return float(lastHumidity) / 10;
}

// Return the percentage relative humidity in decimal form
// Converts from the internal integer format on demand, so you might want
// to cache the result.
inline float Sensor::getTemperatureC() {
  return float(lastTemperature) / 10;
}

inline float Sensor::getTemperatureF() {
  return float(lastTemperature) / 10 * 9 / 5 + 32;
}

}  // namespace dht22
#endif /*_DHT22_H_*/