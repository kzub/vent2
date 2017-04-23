#ifndef _DHT22_H_
#define _DHT22_H_

#define DIRECT_READ(base, mask) (((*(base)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask) ((*(base + 1)) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask) ((*(base + 1)) |= (mask))

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