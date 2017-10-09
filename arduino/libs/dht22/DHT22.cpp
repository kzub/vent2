#include "DHT22.h"
#include <Arduino.h>
#include <pins_arduino.h>

extern "C" {
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
}

namespace DHT22 {
#define DIRECT_READ(base, mask) (((*(base)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask) ((*(base + 1)) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask) ((*(base + 1)) |= (mask))
// #define DIRECT_WRITE_HIGH(base, mask) ((*(base + 2)) |= (mask))

#define DHT22_DATA_BIT_COUNT 40

Sensor::Sensor(uint8_t pin) {
  bitmask = digitalPinToBitMask(pin);
  reg = portInputRegister(digitalPinToPort(pin));
  lastReadTime = millis();
  lastHumidity = DHT22_ERROR_VALUE;
  lastTemperature = DHT22_ERROR_VALUE;
}

//
// Read the 40 bit data stream from the DHT 22
// Store the results in private member data to be read by public member functions
Error Sensor::readData() {
  unsigned long currentTime = millis();
  if (abs(currentTime - lastReadTime) < 2000) {
    return Error(ErrorCode::TOO_QUICK);
  }
  lastReadTime = currentTime;

  // Pin needs to start HIGH, wait until it is HIGH with a timeout
  cli();
  DIRECT_MODE_INPUT(reg, bitmask);
  sei();

  // wait for bus be ready
  auto res = waitForHigh(250, ErrorCode::BUS_HUNG);
  if (res) {
    return res;
  }

  // Send the activate pulse
  cli();
  DIRECT_MODE_OUTPUT(reg, bitmask);  // Output Low
  sei();
  delayMicroseconds(1100);  // 1.1 ms
  cli();
  DIRECT_MODE_INPUT(reg, bitmask);  // Switch back to input so pin can float
  sei();

  // bus release
  res = waitForHigh(200, ErrorCode::NOT_PRESENT);
  if (res) {
    return res;
  }

  // ACK start
  res = waitForLow(200, ErrorCode::NOT_PRESENT);
  if (res) {
    return res;
  }
  // ACK finish
  res = waitForHigh(150, ErrorCode::ACK_TOO_LONG);
  if (res) {
    return res;
  }

  // first data byte
  res = waitForLow(150, ErrorCode::ACK_TOO_LONG);
  if (res) {
    return res;
  }

  // Read the 40 bit data stream
  uint8_t bitTimes[DHT22_DATA_BIT_COUNT];
  memset(bitTimes, 0, DHT22_DATA_BIT_COUNT);

  for (int i = 0; i < DHT22_DATA_BIT_COUNT; i++) {
    res = waitForLow(100, ErrorCode::SYNC_TIMEOUT);
    if (res) {
      return res;
    }
    res = waitForHigh(100, ErrorCode::SYNC_TIMEOUT);
    if (res) {
      return res;
    }
    res = measureHigh(100, ErrorCode::DATA_TIMEOUT, bitTimes[i]);
    if (res) {
      return res;
    }
  }

  int currentHumidity = 0;
  int currentTemperature = 0;
  uint8_t checkSum = 0;
  uint8_t measurePoint = 30 / DHT22_CHECK_STEP_US;
  // Now bitTimes have the number of retries (us *2)
  // that were needed to find the end of each data bit
  // Spec: 0 is 22 to 30 us
  // Spec: 1 is 75 us
  // bitTimes[x] <= 30 is a 0
  // bitTimes[x] >  30 is a 1
  // Note: the bits are offset by one from the data sheet, not sure why
  for (int i = 0; i < 16; i++) {
    if (bitTimes[i] > measurePoint) {
      currentHumidity |= (1 << (15 - i));
    }
  }
  for (int i = 0; i < 16; i++) {
    if (bitTimes[i + 16] > measurePoint) {
      currentTemperature |= (1 << (15 - i));
    }
  }
  for (int i = 0; i < 8; i++) {
    if (bitTimes[i + 32] > measurePoint) {
      checkSum |= (1 << (7 - i));
    }
  }

  lastHumidity = currentHumidity & 0x7FFF;
  if (currentTemperature & 0x8000) {
    // Below zero, non standard way of encoding negative numbers!
    // Convert to native negative format.
    lastTemperature = -(currentTemperature & 0x7FFF);
  } else {
    lastTemperature = currentTemperature;
  }

  uint8_t csPart1 = currentHumidity >> 8;
  uint8_t csPart2 = currentHumidity & 0xFF;
  uint8_t csPart3 = currentTemperature >> 8;
  uint8_t csPart4 = currentTemperature & 0xFF;
  if (checkSum == ((csPart1 + csPart2 + csPart3 + csPart4) & 0xFF)) {
    return ErrorCode::NONE;
  }
  return ErrorCode::CHECKSUM;
}

Error Sensor::waitForHigh(uint8_t us, Error stage) {
  uint8_t retryCount = 0;
  uint8_t maxRetry = us / DHT22_CHECK_STEP_US;
  do {
    if (retryCount > maxRetry) {
      return Error(stage);
    }
    retryCount++;
    delayMicroseconds(DHT22_CHECK_STEP_US);
  } while (!DIRECT_READ(reg, bitmask));
  return Error(ErrorCode::NONE);
}

Error Sensor::waitForLow(uint8_t us, Error stage) {
  uint8_t retryCount = 0;
  uint8_t maxRetry = us / DHT22_CHECK_STEP_US;
  do {
    if (retryCount > maxRetry) {
      return Error(stage);
    }
    retryCount++;
    delayMicroseconds(DHT22_CHECK_STEP_US);
  } while (DIRECT_READ(reg, bitmask));
  return Error(ErrorCode::NONE);
}

Error Sensor::measureHigh(uint8_t us, Error stage, uint8_t &retryCount) {
  retryCount = 0;
  uint8_t maxRetry = us / DHT22_CHECK_STEP_US;
  do {
    if (retryCount > maxRetry) {
      return Error(stage);
    }
    retryCount++;
    delayMicroseconds(DHT22_CHECK_STEP_US);
  } while (DIRECT_READ(reg, bitmask));
  return Error(ErrorCode::NONE);
}

const char *Error::getMessage() {
  switch (err) {
    case ErrorCode::BUS_HUNG:
      return "BUS Hung";
    case ErrorCode::NOT_PRESENT:
      return "Not Present";
    case ErrorCode::ACK_TOO_LONG:
      return "ACK time out";
    case ErrorCode::SYNC_TIMEOUT:
      return "Sync Timeout";
    case ErrorCode::DATA_TIMEOUT:
      return "Data Timeout";
    case ErrorCode::TOO_QUICK:
      return "Polled to quick";
    case ErrorCode::CHECKSUM:
      return "Wrong checksum";
    default:
      return "Unknown error code";
  }
}
}  // namespace dht22