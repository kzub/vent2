#ifndef LIB_TEMPERATURE_HPP
#define LIB_TEMPERATURE_HPP

#include <Arduino.h>
#include <DHT22.h>
#include <virtual_threads.h>

namespace temperature {
class Sensor : public VT::Routine, public DHT22::Sensor {
  void body();

 public:
  DHT22::Error error;
  Sensor(uint8_t pin);
};
}  // namespace
#endif