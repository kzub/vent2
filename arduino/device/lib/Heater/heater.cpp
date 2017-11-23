#include "heater.h"

namespace heater {
//----------------------------------------------------------------------
Controller::Controller(uint8_t pin, pwm::Controller& fan, temperature::DS1820Sensor& temp)
    : pin(pin), coil(pin), fan(fan), temp(temp) {
}

#define HEATER_SLEEP_INTERVAL 1000
#define HEATER_CHECK_INTERVAL 1000
#define HEATER_MAX_TEMPERATURE 50
//----------------------------------------------------------------------
void Controller::body() {
  if (fan.on_delay == 0 || target_temp == 0.0 || temp.error) {
    turnOff();
    first_step(HEATER_SLEEP_INTERVAL);
    return;
  }

  auto out_temp = temp.getTemperatureC();
  if (out_temp > HEATER_MAX_TEMPERATURE) {
    turnOff();
    first_step(HEATER_SLEEP_INTERVAL);
    return;
  }

  // always on manual mode
  if (target_temp == 200.0) {
    coil.on_delay = 200;
    coil.off_delay = 0;
    current_level = 200;
    first_step(HEATER_SLEEP_INTERVAL);
    return;
  }

  auto diff = target_temp - out_temp;
  if (diff < -5.0) { // temperature has rised to fast
    turnOff();     // this may happend on cold start
    return;
  }

  if (diff > 0.0) {
    if (current_level < (total_levels*10) - 1) {
      current_level++;
      updateCurrentLevel();
    }
  } else if (diff < 0.0) {
    if (current_level > 0) {
      current_level--;
      updateCurrentLevel();
    }
  }

  first_step(HEATER_CHECK_INTERVAL);
}

//----------------------------------------------------------------------
void Controller::updateCurrentLevel() {
  auto& level = levels[current_level/10];
  coil.on_delay = level.on;
  coil.off_delay = level.off;
}

//---------------------------------------------------
void Controller::turnOff() {
  coil.turnOff();
  coil.on_delay = 0;
  coil.off_delay = 0;
  current_level = 0;
}

}  // namespace
