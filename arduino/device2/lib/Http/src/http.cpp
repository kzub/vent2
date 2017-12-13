#include "http.h"

namespace http {
// --------------------------------------------------------------------------
bool Text::operator==(const char *s) {
  if (position == nullptr) {
    return false;
  }
  for (int16_t i = 0; i < length; ++i) {
    if (*(position + i) != s[i]) {
      return false;
    }
  }
  return length > 0;
}

// --------------------------------------------------------------------------
int16_t Text::get(char *buf, int16_t size) {
  if (position == nullptr) {
    return -1;
  }
  if (size < length + 1) {
    return -2;
  }
  int16_t i;
  for (i = 0; i < length; ++i) {
    buf[i] = *(position + i);
  }
  buf[i] = 0;  // string terminator
  return i;
}

#define MAX_NUMERIC_LEN 10
// --------------------------------------------------------------------------
bool Text::copyInt16To(int16_t &val) {
  if (position == nullptr || length <= 0 || length >= MAX_NUMERIC_LEN) {
    return false;
  }
  char value[MAX_NUMERIC_LEN];
  memcpy(value, position, length);
  value[length] = 0;
  val = atoi(value);
  return true;
}

// --------------------------------------------------------------------------
bool Text::copyUInt16To(uint16_t &val) {
  if (position == nullptr || length <= 0 || length >= MAX_NUMERIC_LEN) {
    return false;
  }
  char value[MAX_NUMERIC_LEN];
  memcpy(value, position, length);
  value[length] = 0;
  val = atoi(value);
  return true;
}

// --------------------------------------------------------------------------
bool Text::copyUInt8To(uint8_t &val) {
  if (position == nullptr || length <= 0 || length >= MAX_NUMERIC_LEN) {
    return false;
  }
  char value[MAX_NUMERIC_LEN];
  memcpy(value, position, length);
  value[length] = 0;
  val = atoi(value);
  return true;
}

// --------------------------------------------------------------------------
bool Text::copyFloatTo(float &val) {
  if (position == nullptr || length <= 0 || length >= MAX_NUMERIC_LEN) {
    return false;
  }
  char value[MAX_NUMERIC_LEN];
  memcpy(value, position, length);
  value[length] = 0;
  val = atof(value);
  return true;
}


// combination of two words, parameter name and it value
// --------------------------------------------------------------------------
bool Parameter::exists() {
  return name.position != nullptr;
}

bool Parameter::operator==(const char *s) {
  return value == s;
}

Parameter::operator bool() {
  return name.position != nullptr;
}

// get paths and params from uri
// --------------------------------------------------------------------------
const Parameter &Request::parameter(const char *name) {
  for (int16_t i = 0; i < MAX_REQUEST_PARAMS; ++i) {
    if (p[i].name == name) {
      return p[i];
    }
  }
  return notFound;
}

// URL parser
// --------------------------------------------------------------------------
int16_t parseHTTP(char *buf, int16_t size, Request &request) {
  // only GET method supported
  if (strncmp(buf, "GET ", 4) != 0) {
    return -5;
  }

  char spaces = 0;
  char params = -1;

  // GET /path?params=1 HTTP/1.1\r\n
  for (int16_t i = 0; i < size; i++) {
    if (buf[i] == ' ') {
      spaces++;
      if (spaces == 1) {
        request.path.position = buf + i + 1;
        continue;
      }
      if (spaces == 2) {
        if (request.path.length == -1) {
          request.path.length = buf + i - request.path.position;
        }
        if (params >= 0 && request.p[params].value.length == -1) {
          request.p[params].value.length = buf + i - request.p[params].value.position;
        }
        if (params >= 0) {
          return params;
        }
        return 0;
      }
      return -2;
    }

    if (buf[i] == '?') {
      request.path.length = buf + i - request.path.position;
    }

    // parse uri params
    if (buf[i] == '?' || buf[i] == '&') {
      // set prev parameter value length
      if (params >= 0) {
        request.p[params].value.length = buf + i - request.p[params].value.position;
      }
      params++;
      if (params >= MAX_REQUEST_PARAMS) {
        return params;
      }
      // start writing new parameter
      request.p[params].name.position = buf + i + 1;
      continue;
    }

    // ?test=true&d
    if (buf[i] == '=') {
      request.p[params].name.length = buf + i - request.p[params].name.position;
      request.p[params].value.position = buf + i + 1;
      continue;
    }
  }

  return params;
}

// HTTP/1.0 200 OK\r\n
// --------------------------------------------------------------------------
size_t makeHTTPResponse(char *buf, uint16_t size, uint16_t code, const char *status) {
  uint16_t statuslen = strlen(status);
  if (statuslen + 18 > size) {
    return -1;
  }
  if (code < 100 || code > 999) {
    return -2;
  }

  char num[5];
  itoa(code, num, 10);
  char *p = buf;

  memcpy(p, "HTTP/1.0 ", 9);
  p += 9;

  memcpy(p, num, 3);
  p += 3;

  memcpy(p, " ", 1);
  p += 1;

  memcpy(p, status, statuslen);
  p += statuslen;

  memcpy(p, "\r\n\r\n\0", 5);

  return strlen(buf);
}
}  // namespace http
