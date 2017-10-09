#ifndef FAN_H_
#define FAN_H_

namespace fun {

#define FUN_ON_OFF_DELAY_MS 100

class Controller {
 public:
  Controller(uint8_t pin) : pin(pin) {
    pinMode(pin, OUTPUT);
  }

  void change(uint16_t set_on_delay, uint16_t set_off_delay) {
    cli();
    on_delay = set_on_delay;
    off_delay = set_off_delay;
    sei();
  }

  void off() {
    cli();
    on_delay = 0;
    off_delay = 1;
    sei();
  }

  void on() {
    cli();
    on_delay = 1;
    off_delay = 0;
    sei();
  }

  void loop() {
    while (1) {
      if (on_delay == 0) {
        digitalWrite(pin, LOW);
        delay(FUN_ON_OFF_DELAY_MS);
        continue;
      }
      if (off_delay == 0) {
        digitalWrite(pin, HIGH);
        delay(FUN_ON_OFF_DELAY_MS);
        continue;
      }

      digitalWrite(pin, HIGH);
      delay(on_delay);
      digitalWrite(pin, LOW);
      delay(off_delay);
    }
  }

 private:
  uint8_t pin;
  uint16_t on_delay = 0;
  uint16_t off_delay = 0;
};
}  // namespace fun
#endif  // FAN_H_