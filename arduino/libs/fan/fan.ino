#include "TimerOne.h"
#include "fan.h"

fun::Controller fan(7);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
  Serial.println("Initialized");

  Timer1.initialize(3000000);        // initialize timer1, and set a  period
  Timer1.pwm(9, 512);                // setup pwm on pin 9, 50% duty cycle
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt
}

int cur = 1;
void callback() {
  digitalWrite(10, digitalRead(10) ^ 1);
  if (cur == 0) {
    cur = 1;
    fan.change(0, 0);
  } else {
    cur = 0;
    fan.change(100, 50);
  }
}

void loop() {
  fan.loop();
}
