#ifndef LIB_HTTP_SRC_HTTP_HPP
#define LIB_HTTP_SRC_HTTP_HPP

#include <Arduino.h>

#define MAX_REQUEST_PARAMS 3

namespace http {
class Parameter;
class Request;

// pointer to one word in any given buffer
// --------------------------------------------------------------------------
class Text {
 public:
  bool operator==(const char *s);
  int16_t get(char *buf, int16_t size);
  bool copyUInt16To(uint16_t &val);
  bool copyInt16To(int16_t &val);
  bool copyUInt8To(uint8_t &val);
  bool copyFloatTo(float &val);

 private:
  char *position = nullptr;
  int16_t length = -1;

  friend class Parameter;
  friend int16_t parseHTTP(char *buf, int16_t size, Request &request);
};

// combination of two words, parameter name and it value
// --------------------------------------------------------------------------
class Parameter {
 public:
  Text name;
  Text value;

  bool exists();
  bool operator==(const char *s);
  operator bool();
};

// Empty parameter
static const Parameter notFound;

// Request. Contains paths and params
// --------------------------------------------------------------------------
class Request {
 public:
  Text path;
  Parameter p[MAX_REQUEST_PARAMS];

  const Parameter &parameter(const char *name);
};

// --------------------------------------------------------------------------
int16_t parseHTTP(char *buf, int16_t size, Request &request);
size_t makeHTTPResponse(char *buf, uint16_t size, uint16_t code, const char *status);
}  // namespace http
#endif