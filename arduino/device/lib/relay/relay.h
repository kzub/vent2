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

 private:
  bool is_on = false;
  uint8_t pin;
};
}  // namespace pow

#endif