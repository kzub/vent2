#ifndef LIB_HEATER_HPP
#define LIB_HEATER_HPP

#include <Arduino.h>
#include <pwm.h>
#include <temperature.h>
#include <virtual_threads.h>

namespace heater {
struct Speed {
  uint16_t on;
  uint16_t off;
};

constexpr uint16_t cycle = 1000;

constexpr struct Speed levels[]{
    {cycle * 0.0, cycle * 1.0},  // off
    {cycle * 0.1, cycle * 0.9},  // 10%
    {cycle * 0.2, cycle * 0.8},  // 20%
    {cycle * 0.3, cycle * 0.7},  // 30%
    {cycle * 0.4, cycle * 0.6},  // 40%
    {cycle * 0.5, cycle * 0.5},  // 50%
    {cycle * 0.6, cycle * 0.4},  // 60%
    {cycle * 0.7, cycle * 0.3},  // 70%
    {cycle * 0.8, cycle * 0.2},  // 80%
    {cycle * 0.9, cycle * 0.1},  // 90%
    {cycle * 1.0, cycle * 0.0}   // 100%
};

#define total_levels ((sizeof(levels) / sizeof(struct Speed)))

class Controller : public VT::Routine {
 public:
  Controller(uint8_t pin, pwm::Controller& fan, temperature::DS1820Sensor& temp);
  int16_t target_temp = 0;
  uint8_t current_level = 0;
  void turnOff();

 private:
  pwm::Controller coil;
  void body();
  bool is_on = false;
  pwm::Controller& fan;
  temperature::DS1820Sensor& temp;
};
}  // namespace pow

#endif