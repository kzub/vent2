#include "http_server.h"

namespace httpserver {
// --------------------------------------------------------------------------
namespace network {
void initialize(uint8_t *macaddr, uint8_t *ipaddr) {
  if (ipaddr == nullptr) {
    while (!Ethernet.begin(macaddr)) {
      // Serial.println("Failed to configure Ethernet using DHCP");
      delay(1000);
    }
  } else {
    IPAddress ip(ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    Ethernet.begin(macaddr, ip);
  }
}
}  // namespace network

char buffer[buffer_size];
// --------------------------------------------------------------------------
NetworkService::NetworkService(uint16_t port) : server(port) {
  // Serial.print("My IP address: ");
  auto ip = Ethernet.localIP();

  for (uint8_t thisByte = 0; thisByte < 4; thisByte++) {
    // Serial.print(ip[thisByte], DEC);
    // Serial.print(".");
  }
  // Serial.println("done");

  server.begin();
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
void ResponseWriter::writeInternalError(uint8_t code) {
  auto res = http::makeHTTPResponse(buffer, buffer_size, 500, "Internal Error " + code);
  if (res > 0) {
    client.println(buffer);
  }
  closed = true;
  client.stop();
  return;
}

// --------------------------------------------------------------------------
size_t ResponseWriter::writeHead(uint16_t code, const char *s) {
  if (closed) {
    return 0;
  }
  if (head_written) {
    writeInternalError(1);
    return 0;
  }
  head_written = true;

  auto res = http::makeHTTPResponse(buffer, buffer_size, code, s);
  if (res > 0) {
    client.print(buffer);
    return res;
  }

  writeInternalError(2);
  return 0;
}

// --------------------------------------------------------------------------
size_t ResponseWriter::write(const char *text) {
  if (closed) {
    return 0;
  }
  if (!head_written) {
    writeInternalError(3);
    return 0;
  }
  client.print(text);
}

// --------------------------------------------------------------------------
size_t ResponseWriter::end(uint16_t code, const char *s, const char *text) {
  if (closed) {
    return 0;
  }

  auto res = writeHead(code, s);
  if (res > 0) {
    if (text != nullptr) {
      client.print(text);
    }
    client.stop();
    closed = true;
  }

  return res;
}

// --------------------------------------------------------------------------
void ResponseWriter::end() {
  if (closed) {
    return;
  }

  client.stop();
  closed = true;
}

// --------------------------------------------------------------------------
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
    resp.end(400, "Bad request [2]", nullptr);
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

  resp.end(404, "Not found", "Not found");
  first_step();
}
}  // namespace server{
