#ifndef LIB_HTTPSRV_HPP
#define LIB_HTTPSRV_HPP

#include <Arduino.h>
#include <Ethernet2.h>

#include <virtual_threads.h>
#include <http.hpp>

#define MAX_HANDLERS 10

namespace httpserver {
namespace network {
// --------------------------------------------------------------------------
void initialize(uint8_t *macaddr, uint8_t *ipaddr);
}  // namespace network

const int buffer_size = 200;
// --------------------------------------------------------------------------
class ResponseWriter {
  EthernetClient &client;
  bool closed = false;

 public:
  ResponseWriter(EthernetClient &client) : client(client){};
  ~ResponseWriter();
  void write(uint16_t code, const char *s, const char *text);
};

// --------------------------------------------------------------------------
using MethodHandler = void (*)(http::Request &request, ResponseWriter &w);
struct Handler {
  char *path;
  MethodHandler handler;
};

// --------------------------------------------------------------------------
class NetworkService : public VT::Routine {
  void body();

  EthernetServer server;
  Handler handlers[MAX_HANDLERS];
  uint8_t handlers_count = 0;

 public:
  NetworkService(uint16_t port);
  void add_handler(const char *name, MethodHandler);
};
}  // namespace server
#endif
