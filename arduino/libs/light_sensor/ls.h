#ifndef _DHT22_H_
#define _DHT22_H_

namespace light {
const int MAX_VALUE = 1024;
const int MIN_VALUE = 0;

class Sensor {
 public:
  Sensor(int pin) : pin(pin) {
  }

  int get() {
    return MAX_VALUE - analogRead(pin);
  }

 private:
  int pin = 0;
};

}  // namespace ls

#endif