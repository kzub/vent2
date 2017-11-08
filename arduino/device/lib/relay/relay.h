#ifndef LIB_RELAY_HPP
#define LIB_RELAY_HPP

#include <Arduino.h>

namespace relay {
class Controller {
 public:
  Controller(uint8_t pin);
  void turnOff();
  void turnOn();
  bool isOn();
  const uint8_t pin;

 private:
  bool is_on = false;
};
}  // namespace pow

#endif