#ifndef LIB_HEATER_HPP
#define LIB_HEATER_HPP

#include <Arduino.h>
#include <pwm.h>
#include <relay.h>
#include <temperature.h>
#include <virtual_threads.h>

namespace heater {
class Controller : public VT::Routine {
 public:
  Controller(uint8_t pin, pwm::Controller& fan, temperature::DS1820Sensor& temp);
  float target_temp = 0.0;
  const uint8_t pin;

  temperature::DS1820Sensor& temp;
  relay::Controller coil;
  pwm::Controller& fan;

 private:
  void body();
};
}  // namespace pow

#endif