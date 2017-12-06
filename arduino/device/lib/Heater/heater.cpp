#include "heater.h"

namespace heater {
//----------------------------------------------------------------------
Controller::Controller(uint8_t pin, pwm::Controller& fan, temperature::DS1820Sensor& temp,
                       temperature::DS1820Sensor& ssr1, temperature::DS1820Sensor& ssr2)
    : pin(pin), coil(pin), fan(fan), temp(temp), ssr1(ssr1), ssr2(ssr2) {
}

#define HEATER_SLEEP_INTERVAL 1000
#define HEATER_MAX_TEMPERATURE 50.0
#define SSR_MAX_TEMPERATURE 65.0
//----------------------------------------------------------------------
void Controller::body() {
  // protections:
  if (fan.on_delay == 0 || target_temp == 0.0 || temp.error || ssr1.error || ssr2.error) {
    coil.turnOff();
    first_step(HEATER_SLEEP_INTERVAL);
    return;
  }

  if (ssr1.getTemperatureC() > SSR_MAX_TEMPERATURE ||
      ssr2.getTemperatureC() > SSR_MAX_TEMPERATURE) {
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
  if (target_temp == 222.0) {
    coil.turnOn();
    first_step(HEATER_SLEEP_INTERVAL);
    return;
  }

  // work math:
  float diff = target_temp - out_temp;
  if (diff <= 0.0) {
    coil.turnOff();
  } else {
    coil.turnOn();
  }

  first_step(HEATER_SLEEP_INTERVAL);
}
}  // namespace
