#ifndef LIB_VREG_SRC_VREG_HPP
#define LIB_VREG_SRC_VREG_HPP

#include <Arduino.h>
#include <virtual_threads.h>

namespace pwm {
class Controller : public VT::Routine {
 public:
  Controller(uint8_t pin);
  uint16_t on_delay = 0;
  uint16_t off_delay = 0;
  void turnOn();
  void turnOff();

 private:
  void body();
  const uint8_t pin;
  bool is_on = false;
};
}  // namespace pow

#endif