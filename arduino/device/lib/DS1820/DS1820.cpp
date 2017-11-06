#include "DS1820.h"

namespace DS1820 {
//---------------------------------------------------------------
Sensor::Sensor(uint8_t pin) : ds(pin), error(ErrorCode::NOT_PRESENT) {
  pinMode(pin, INPUT);
}

//---------------------------------------------------------------
bool Sensor::init() {
  if (initialized) {
    return true;
  }

  if (!ds.search(addr)) {
    ds.reset_search();
    error = Error(ErrorCode::NOT_PRESENT);
    // Serial.println("[] cant search");
    return false;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    error = Error(ErrorCode::CHECKSUM);
    // Serial.println("[] CRC is not valid");
    return false;
  }

  if (addr[0] != 0x28) {
    error = Error(ErrorCode::UNKNOWN_DEVICE);
    // Serial.println("[] Device is not a DS18B20 family device.");
    // Serial.println(addr[0], HEX);
    return false;
  }

  // Serial.println("[] init ok");
  error = Error(ErrorCode::NONE);

  initialized = true;
  return true;
}

//---------------------------------------------------------------
void Sensor::startConversion(void) {
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);  // start conversion, with parasite power on at the end
  // delay(1000);  // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
}

//---------------------------------------------------------------
void Sensor::readData(void) {
  uint8_t present = ds.reset();
  if (!present) {
    Serial.println("NOTPRESENT");
    error = Error(ErrorCode::NOT_PRESENT);
    return;
  }

  ds.select(addr);
  ds.write(0xBE);  // Read Scratchpad

  uint8_t data[12];
  for (uint8_t i = 0; i < 9; i++) {  // we need 9 bytes
    data[i] = ds.read();
  }

  uint8_t LowByte = data[0];
  uint8_t HighByte = data[1];
  int16_t TReading = (HighByte << 8) + LowByte;

  // calc temperature:
  SignBit = TReading & 0x8000;           // test most sig bit
  if (SignBit) {                         // negative
    TReading = (TReading ^ 0xffff) + 1;  // 2's comp
  }
  // Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25
  Tc_100 = (TReading * 10 / 2);  // DS 1820 with 0.5 Degree Resolution
  Whole = Tc_100 / 100;          // separate off the whole and fractional portions
  Fract = Tc_100 % 100;
  checkResult();
}

//---------------------------------------------------------------
int16_t Sensor::getTemperatureCInt() {
  if (SignBit) {
    return -Whole;
  } else {
    return Whole;
  }
}

//---------------------------------------------------------------
float Sensor::getTemperatureC() {
  if (SignBit) {
    return -Tc_100 / 100;
  } else {
    return Tc_100 / 100;
  }
}

//---------------------------------------------------------------
void Sensor::checkResult() {
  if (Whole > 125 || Fract >= 100) {
    Serial.print("Bad result:");
    Serial.print(SignBit);
    Serial.print(" ");
    Serial.print(Tc_100);
    Serial.print(" ");
    Serial.print(Fract);
    Serial.print(" ");
    Serial.println(Whole);
    SignBit = 0;
    Tc_100 = 0;
    Fract = 0;
    Whole = 0;
    error = Error(ErrorCode::BAD_RESULT);
  }
}

//---------------------------------------------------------------
void Sensor::getTextValue(char *buf, int len) {
  // +125.0625  - 10 chars
  if (buf == nullptr || len < 20) {
    return;
  }

  sprintf(buf, "%c%d.%d", SignBit ? '-' : '+', Whole, Fract);
}

//---------------------------------------------------------------
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
    case ErrorCode::UNKNOWN_DEVICE:
      return "Unknown device";
    case ErrorCode::BAD_RESULT:
      return "Bad conversion result";
    default:
      return "Unknown error code";
  }
}

}  // namespace