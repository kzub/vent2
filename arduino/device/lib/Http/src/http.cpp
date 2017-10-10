#include "http.hpp"

// --------------------------------------------------------------------------
bool Text::operator==(const char *s) {
  if (positon == nullptr) {
    return false;
  }
  if (length != (int)strlen(s)) {
    return false;
  }
  for (int i = 0; i < length; ++i) {
    if (*(positon + i) != s[i]) {
      return false;
    }
  }
  return true;
}

// --------------------------------------------------------------------------
int Text::get(char *buf, int size) {
  if (positon == nullptr) {
    return -1;
  }
  if (size < length + 1) {
    return -2;
  }
  int i;
  for (i = 0; i < length; ++i) {
    buf[i] = *(positon + i);
  }
  buf[i] = 0;  // string terminator
  return i;
}

// combination of two words, parameter name and it value
// --------------------------------------------------------------------------
bool Parameter::exists() {
  return name.positon != nullptr;
}

bool Parameter::operator==(const char *s) {
  return value == s;
}

// get paths and params from uri
// --------------------------------------------------------------------------
const Parameter &Request::parameter(const char *name) {
  for (int i = 0; i < MAX_REQUEST_PARAMS; ++i) {
    if (p[i].name == name) {
      return p[i];
    }
  }
  return notFound;
}

// URL parser
// --------------------------------------------------------------------------
int parseHTTP(char *buf, int size, Request *request) {
  // only GET method supported
  if (strncmp(buf, "GET ", 4) != 0) {
    return -5;
  }

  char spaces = 0;
  char params = -1;

  // GET /path?params=1 HTTP/1.1\r\n
  for (int i = 0; i < size; i++) {
    if (buf[i] == ' ') {
      spaces++;
      if (spaces == 1) {
        request->path.positon = buf + i + 1;
        continue;
      }
      if (spaces == 2) {
        if (request->path.length == -1) {
          request->path.length = buf + i - request->path.positon;
        }
        if (params >= 0 && request->p[params].value.length == -1) {
          request->p[params].value.length = buf + i - request->p[params].value.positon;
        }
        if (params >= 0) {
          return params;
        }
        return 0;
      }
      return -2;
    }

    if (buf[i] == '?') {
      request->path.length = buf + i - request->path.positon;
    }

    // parse uri params
    if (buf[i] == '?' || buf[i] == '&') {
      // set prev parameter value length
      if (params >= 0) {
        request->p[params].value.length = buf + i - request->p[params].value.positon;
      }
      params++;
      if (params >= MAX_REQUEST_PARAMS) {
        return params;
      }
      // start writing new parameter
      request->p[params].name.positon = buf + i + 1;
      continue;
    }

    // ?test=true&d
    if (buf[i] == '=') {
      request->p[params].name.length = buf + i - request->p[params].name.positon;
      request->p[params].value.positon = buf + i + 1;
      continue;
    }
  }

  return params;
}

// HTTP/1.0 200 OK\r\n
// --------------------------------------------------------------------------
size_t makeHTTPResponse(char *buf, unsigned int size, int code, const char *status) {
  int statuslen = strlen(status);
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
