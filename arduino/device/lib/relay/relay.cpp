#include "relay.h"

namespace relay {
//----------------------------------------------------------------------
Controller::Controller(uint8_t pin) : pin(pin) {
  turnOff();
  pinMode(pin, OUTPUT);
}

//----------------------------------------------------------------------
void Controller::turnOn() {
  is_on = true;
  digitalWrite(pin, HIGH);
}

//----------------------------------------------------------------------
void Controller::turnOff() {
  is_on = false;
  digitalWrite(pin, LOW);
}

//----------------------------------------------------------------------
bool Controller::isOn() {
  return is_on;
}

}  // namespace
