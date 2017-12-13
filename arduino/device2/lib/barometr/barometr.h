#ifndef LIB_BAROMETR_HPP
#define LIB_BAROMETR_HPP

#include <virtual_threads.h>
#include "LPS331.h"

namespace barometr {

class Sensor : public VT::Routine, public LPS331 {
 public:
  Sensor();

 private:
  void body();
  float pressure = 0;
  float temperature = 0;
};
}  // namespace barometr

#endif