#include "heater.h"

namespace heater {
//----------------------------------------------------------------------
Controller::Controller(uint8_t pin, pwm::Controller& fan, temperature::DS1820Sensor& temp)
    : pin(pin), coil(pin), fan(fan), temp(temp) {
}

#define HEATER_SLEEP_INTERVAL 1000
#define HEATER_CHECK_INTERVAL 5000
#define HEATER_MAX_TEMPERATURE 60
//----------------------------------------------------------------------
void Controller::body() {
  if (fan.on_delay == 0 || target_temp == 0 || temp.error) {
    turnOff();
    first_step(HEATER_SLEEP_INTERVAL);
    return;
  }

  auto out_temp = temp.getTemperatureCInt();

  if (out_temp > HEATER_MAX_TEMPERATURE) {
    turnOff();
    first_step(HEATER_SLEEP_INTERVAL);
    return;
  }

  int16_t diff = target_temp - out_temp;
  // Serial.print("diff:");
  // Serial.print(diff);
  // Serial.print(" target_temp:");
  // Serial.println(target_temp);

  if (diff > 1) {
    if (current_level < total_levels - 1) {
      current_level++;
      coil.on_delay = levels[current_level].on;
      coil.off_delay = levels[current_level].off;
      // Serial.print("increase:");
      // Serial.println(current_level);
      // Serial.print("total:");
      // Serial.println(total_levels);
    }
  } else if (diff < -1) {
    if (current_level > 0) {
      current_level--;
      coil.on_delay = levels[current_level].on;
      coil.off_delay = levels[current_level].off;
      // Serial.print("decrease");
      // Serial.println(current_level);
    }
  }

  // Serial.print("coil on:");
  // Serial.print(coil.on_delay);
  // Serial.print(" coil off:");
  // Serial.println(coil.off_delay);

  first_step(HEATER_CHECK_INTERVAL);
}

//---------------------------------------------------
void Controller::turnOff() {
  coil.turnOff();
  coil.on_delay = 0;
  coil.off_delay = 0;
  current_level = 0;
}

}  // namespace
