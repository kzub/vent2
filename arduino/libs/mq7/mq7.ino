// библиотека для работы с датчиками MQ (Troyka-модуль)
#include "MQ7.h"

// имя для пина, к которому подключен датчик
#define PIN_MQ7 A0
// имя для пина, к которому подключен нагреватель датчика
#define PIN_MQ7_HEATER 11

// создаём объект для работы с датчиком
// и передаём ему номер пина выходного сигнала и нагревателя
MQ7 mq7(PIN_MQ7, PIN_MQ7_HEATER);

void setup() {
  // открываем последовательный порт
  Serial.begin(115200);
  while (!Serial) {
  }
  // запускаем термоцикл
  // в течении 60 секунд на нагревательный элемент подаётся 5 вольт
  // в течении 90 секунд — 1,5 вольта
  Serial.print("Init");
  mq7.begin();
  Serial.print("Ro = ");
  Serial.println(mq7.getRo());
  mq7.cycleHeat();
}

// надо отколибровать на открытом воздухе и тогда не прийдется калибровать каждый раз

void loop() {
  if (!mq7.atHeatCycleEnd()) {
  	Serial.print(".");
  	delay(1000);
    return;
  }
  Serial.println("");

  Serial.print("Ratio: ");
  Serial.print(mq7.readRatio());
  Serial.print(" CarbonMonoxide: ");
  Serial.print(mq7.readCarbonMonoxide());
  Serial.println(" ppm ");

  mq7.cycleHeat();
}