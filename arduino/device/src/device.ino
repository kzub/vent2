#include <virtual_threads.h>
#include <http_server.hpp>
#include <pwm.hpp>

const uint8_t macaddr[] = {0x90, 0xA2, 0xda, 0x10, 0xe2, 0x04};
const uint8_t ipaddr[] = {192, 168, 55, 2};
#define LED_PIN 13
pwm::Controller led(LED_PIN);

// INIT ---------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  httpserver::network::initialize(macaddr, ipaddr);
}

// REQEUST HANDLERS ---------------------------------------------------------
void handler_fun(http::Request &request, httpserver::ResponseWriter &resp) {
  request.parameter("on").value.copyInt16To(led.on_delay);
  request.parameter("off").value.copyInt16To(led.off_delay);
  resp.write(200, "OK", nullptr);
}

void handler_info(http::Request &request, httpserver::ResponseWriter &resp) {
  resp.write(200, "OK", "[\"fan1\"]");
}

// MAIN ---------------------------------------------------------------------
void loop() {
  auto engine = VT::Engine::getInstance();
  httpserver::NetworkService srv(80);

  srv.add_handler("/fan1", handler_fun);
  srv.add_handler("/info", handler_info);

  engine->add_routine(srv);
  engine->add_routine(led);
  engine->start();
  // will never reach this point
}
