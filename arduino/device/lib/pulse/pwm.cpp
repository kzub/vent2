#include "pwm.h"

namespace pwm {
//----------------------------------------------------------------------
Controller::Controller(uint8_t pin) : pin(pin) {
  turnOff();
}

//----------------------------------------------------------------------
void Controller::body() {
  if (on_delay == 0) {
    if (is_on) {
      turnOff();
    }
    first_step(1000);
    return;
  }

  if (is_on) {
    if (off_delay == 0) {
      first_step(1000);
      return;
    }

    turnOff();
    first_step(off_delay);
  } else {
    turnOn();
    first_step(on_delay);
  }
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
}  // namespace pulse
