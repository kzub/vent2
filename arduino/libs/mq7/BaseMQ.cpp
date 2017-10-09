#include "BaseMQ.h"

BaseMQ::BaseMQ(uint8_t pin) : _pin(pin) {
}

BaseMQ::BaseMQ(uint8_t pin, uint8_t pinHeater) : _pin(pin), _pinHeater(pinHeater) {
  pinMode(_pinHeater, OUTPUT);
}

/* Measure cycle
 cleaning
 _______
| 60s  | sensor accamulate co2
| 5V   |_________
|      |   90s  | measure R
|      |  1.4V  |__________>
                   0V
*/

void BaseMQ::heaterPwrHigh() {
  digitalWrite(_pinHeater, HIGH);
  _heater = true;
  _prMillis = millis();
}

void BaseMQ::heaterPwrLow() {
  analogWrite(_pinHeater, 72);
  _heater = true;
  _cooler = true;
  _prMillis = millis();
}

void BaseMQ::heaterPwrOff() {
  digitalWrite(_pinHeater, LOW);
  _heater = false;
}

// сопротивление датчика
float BaseMQ::calculateResistance(int rawAdc) const {
  float vrl = rawAdc * (5.0 / 1023);
  float rs = (5.0 - vrl) / vrl * getRL();
  return rs;
}

// считывание датчика
float BaseMQ::readRs() const {
  float rs = 0;
  for (int i = 0; i < MQ_SAMPLE_TIMES; i++) {
    rs += calculateResistance(analogRead(_pin));
    delay(MQ_SAMPLE_INTERVAL);
  }
  rs = rs / MQ_SAMPLE_TIMES;
  return rs;
}

float BaseMQ::readRatio() const {
  return readRs() / getRoInCleanAir();
}

unsigned long BaseMQ::readScaled(float a, float b) const {
  float ratio = readRatio();
  return exp((log(ratio) - b) / a);
}

bool BaseMQ::heatingCompleted() const {
  if ((_heater) && (!_cooler) && (millis() - _prMillis > 60000)) {
    return true;
  }
  return false;
}

bool BaseMQ::coolanceCompleted() const {
  if ((_heater) && (_cooler) && (millis() - _prMillis > 90000)) {
    return true;
  }
  return false;
}

void BaseMQ::cycleHeat() {
  _heater = false;
  _cooler = false;
  heaterPwrHigh();
}

bool BaseMQ::atHeatCycleEnd() {
  if (heatingCompleted()) {
    heaterPwrLow();
    return false;
  }

  if (coolanceCompleted()) {
    heaterPwrOff();
    return true;
  } 
  
  return false;
}