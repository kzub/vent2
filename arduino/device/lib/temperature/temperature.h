#ifndef LIB_TEMPERATURE_HPP
#define LIB_TEMPERATURE_HPP

#include <Arduino.h>
#include <DHT22.h>
#include <DS1820.h>
#include <virtual_threads.h>

namespace temperature {
class DHT22Sensor : public VT::Routine, public DHT22::Sensor {
 public:
  DHT22Sensor(uint8_t pin);
  DHT22::Error error;

 private:
  void body();
};

class DS1820Sensor : public VT::Routine, public DS1820::Sensor {
 public:
  DS1820Sensor(uint8_t pin);

 private:
  void body();
};

}  // namespace
#endif