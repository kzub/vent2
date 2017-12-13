#ifndef LIB_HTTPSRV_HPP
#define LIB_HTTPSRV_HPP

#include <Arduino.h>
#include <Ethernet2.h>

#include <http.h>
#include <virtual_threads.h>

#define MAX_HANDLERS 10

namespace httpserver {
namespace network {
// --------------------------------------------------------------------------
void initialize(const uint8_t *macaddr, const uint8_t *ipaddr);
}  // namespace network

const int buffer_size = 200;
// --------------------------------------------------------------------------
class ResponseWriter {
 public:
  ResponseWriter(EthernetClient &client) : client(client){};
  ~ResponseWriter();
  void end();
  size_t end(uint16_t code, const char *s, const char *text);
  size_t writeHead(uint16_t code, const char *s);
  size_t write(const char *text);

 private:
  void writeInternalError(uint8_t code);
  EthernetClient &client;
  bool closed = false;
  bool head_written = false;
};

// --------------------------------------------------------------------------
using MethodHandler = void (*)(http::Request &request, ResponseWriter &w);
struct Handler {
  char *path;
  MethodHandler handler;
};

// --------------------------------------------------------------------------
class NetworkService : public VT::Routine {
 public:
  NetworkService(uint16_t port);
  void add_handler(const char *name, MethodHandler);

 private:
  void body();

  EthernetServer server;
  Handler handlers[MAX_HANDLERS];
  uint8_t handlers_count = 0;
};
}  // namespace server
#endif
