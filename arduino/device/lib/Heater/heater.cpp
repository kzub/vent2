#include "heater.h"

namespace heater {
//----------------------------------------------------------------------
Controller::Controller(uint8_t pin, pwm::Controller& fan, temperature::DS1820Sensor& temp)
    : pin(pin), coil(pin), fan(fan), temp(temp) {
}

#define HEATER_SLEEP_INTERVAL 1000
#define HEATER_MAX_TEMPERATURE 50.0
//----------------------------------------------------------------------
void Controller::body() {
  if (fan.on_delay == 0 || target_temp == 0.0 || temp.error) {
    coil.turnOff();
    first_step(HEATER_SLEEP_INTERVAL);
    return;
  }

  float out_temp = temp.getTemperatureC();
  if (out_temp > HEATER_MAX_TEMPERATURE) {
    coil.turnOff();
    first_step(HEATER_SLEEP_INTERVAL);
    return;
  }

  // always on manual mode
  if (target_temp == 200.0) {
    coil.turnOn();
    first_step(HEATER_SLEEP_INTERVAL);
    return;
  }

  float diff = target_temp - out_temp;
  if (diff <= 0.0) {
    coil.turnOff();
  }else{
    coil.turnOn();
  }

  first_step(HEATER_SLEEP_INTERVAL);
}
}  // namespace
