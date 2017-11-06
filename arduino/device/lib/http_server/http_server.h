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
void initialize(uint8_t *macaddr, uint8_t *ipaddr);
}  // namespace network

const int buffer_size = 200;
// --------------------------------------------------------------------------
class ResponseWriter {
 public:
  ResponseWriter(EthernetClient &client) : client(client){};
  ~ResponseWriter();
  void write(uint16_t code, const char *s, const char *text);

 private:
  EthernetClient &client;
  bool closed = false;
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
