#include "http_server.hpp"

namespace httpserver {
// --------------------------------------------------------------------------
namespace network {
void initialize(uint8_t *macaddr, uint8_t *ipaddr) {
  IPAddress ip(ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
  Ethernet.begin(macaddr, ip);

  //  while(!Ethernet.begin(mac)){
  //    Serial.println("Failed to configure Ethernet using DHCP");
  //    delay(5000);
  //  }
}
}  // namespace network

char buffer[buffer_size];
// --------------------------------------------------------------------------
NetworkService::NetworkService(uint16_t port) : server(port) {
  Serial.print("My IP address: ");
  auto ip = Ethernet.localIP();

  for (uint8_t thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(ip[thisByte], DEC);
    Serial.print(".");
  }

  server.begin();
}

NetworkService::~NetworkService() {
  server.end();
}

// --------------------------------------------------------------------------
void NetworkService::add_handler(const char *name, MethodHandler handler) {
  if (handlers_count > MAX_HANDLERS) {
    return;
  }
  handlers[handlers_count].path = name;
  handlers[handlers_count].handler = handler;
  handlers_count++;
}

// --------------------------------------------------------------------------
void ResponseWriter::write(uint16_t code, const char *s, const char *text) {
  closed = true;
  int res = http::makeHTTPResponse(buffer, buffer_size, code, s);
  if (res > 0) {
    client.println(buffer);
    if (text != nullptr) {
      client.println(text);
    }
    client.stop();
    return;
  }

  res = http::makeHTTPResponse(buffer, buffer_size, 500, "Internal Error 1");
  if (res > 0) {
    client.println(buffer);
    client.stop();
    return;
  }
}

ResponseWriter::~ResponseWriter() {
  if (!closed) {
    client.stop();
  }
}

// --------------------------------------------------------------------------
void NetworkService::body() {
  EthernetClient client = server.available();

  if (!client.available()) {
    Ethernet.maintain();  // maintain dhcp connection
    first_step(500);
    return;
  }

  ResponseWriter resp(client);
  int res = client.read((uint8_t *)&buffer, buffer_size);
  if (res <= 0) {
    first_step();
    return;
  }

  http::Request request;
  res = http::parseHTTP(buffer, buffer_size, request);
  if (res < 0) {
    resp.write(400, "Bad request [2]", nullptr);
    first_step();
    return;
  }

  for (uint8_t i = 0; i < handlers_count; ++i) {
    const auto &c = handlers[i];
    if (request.path == c.path) {
      c.handler(request, resp);
      first_step();
      return;
    }
  }

  resp.write(404, "Not found", "Not found");
  first_step();
}
}  // namespace server{
