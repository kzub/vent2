#ifndef LIB_HTTP_SRC_HTTP_HPP
#define LIB_HTTP_SRC_HTTP_HPP

#include <Arduino.h>

#define MAX_REQUEST_PARAMS 3

class Parameter;
class Request;

// pointer to one word in any given buffer
// --------------------------------------------------------------------------
class Text {
 private:
  char *positon = nullptr;
  int length = -1;

 public:
  bool operator==(const char *s);
  int get(char *buf, int size);

  friend class Parameter;
  friend int parseHTTP(char *buf, int size, Request *request);
};

// combination of two words, parameter name and it value
// --------------------------------------------------------------------------
class Parameter {
 public:
  Text name;
  Text value;

  bool exists();
  bool operator==(const char *s);
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
int parseHTTP(char *buf, int size, Request *request);
size_t makeHTTPResponse(char *buf, unsigned int size, int code, const char *status);

#endif