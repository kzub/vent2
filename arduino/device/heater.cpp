#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include "heater.hpp"

void TaskHeater(void *pvParameters)  // This is a task.
{
  (void)pvParameters;

  for (;;)  // A Task shall never return or exit.
  {
    // if (SystemSettings.x > 0) {
    //   Serial.print("X");
    // }
//    Serial.println("TaskHeater");
    vTaskDelay(5000 / portTICK_PERIOD_MS);  // wait for one second
  }
}
