#ifndef LIB_DS1820_HPP
#define LIB_DS1820_HPP

#include <OneWire.h>

namespace DS1820 {

#define DS1820_READ_INTERVAL 2000

enum class ErrorCode : uint8_t {
  NONE = 0,
  BUS_HUNG,
  NOT_PRESENT,
  ACK_TOO_LONG,
  SYNC_TIMEOUT,
  DATA_TIMEOUT,
  CHECKSUM,
  TOO_QUICK,
  UNKNOWN_DEVICE,
  BAD_RESULT
};

class Error {
 public:
  Error(ErrorCode code) : err(code) {
  }
  operator bool() {
    return err != ErrorCode::NONE;
  }
  const char *getMessage();

 private:
  ErrorCode err;
};

class Sensor {
 public:
  Sensor(uint8_t pin);
  bool init();
  void startConversion(void);
  void readData(void);
  void getTextValue(char *buf, int len);
  int16_t getTemperatureCInt();
  float getTemperatureC();
  Error error;
  const int8_t pin;

 private:
  void checkResult();
  OneWire ds;
  uint8_t addr[8];
  bool initialized = false;
  int16_t SignBit = 0;
  int16_t Tc_100 = 0;
  int16_t Whole = 0;
  int16_t Fract = 0;
};
}  // namespace

#endif