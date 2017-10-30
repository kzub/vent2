#include <virtual_threads.h>
#include <http_server.hpp>
#include <pwm.hpp>
#include <temperature.hpp>

const uint8_t macaddr[] = {0x90, 0xA2, 0xda, 0x10, 0xe2, 0x04};
const uint8_t ipaddr[] = {192, 168, 55, 2};

#define LED_PIN 13
#define TEMP_PIN 6

pwm::Controller led(LED_PIN);
temperature::Sensor temp1(TEMP_PIN);

// INIT ---------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  httpserver::network::initialize(macaddr, ipaddr);
}

// REQEUST HANDLERS ---------------------------------------------------------
// ---
void handler_info(http::Request &request, httpserver::ResponseWriter &resp) {
  resp.write(200, "OK", "[\"fan1\", \"temp\"]");
}

// ---
void handler_fun(http::Request &request, httpserver::ResponseWriter &resp) {
  request.parameter("on").value.copyInt16To(led.on_delay);
  request.parameter("off").value.copyInt16To(led.off_delay);
  resp.write(200, "OK", nullptr);
}

// ---
void handler_temp(http::Request &request, httpserver::ResponseWriter &resp) {
  char buffer[100];
  if (temp1.error) {
    sprintf(buffer, "{\"error\":\"%s\"}\r\n", temp1.error.getMessage());
  } else {
    char c[10], h[10];
    dtostrf(temp1.getTemperatureC(), 6, 1, c);
    dtostrf(temp1.getHumidity(), 6, 1, h);
    sprintf(buffer, "{\"temperature\":%s,\"humidity\":%s}\r\n", c, h);
  }
  resp.write(200, "OK", buffer);
}

// MAIN ---------------------------------------------------------------------
void loop() {
  auto engine = VT::Engine::getInstance();
  httpserver::NetworkService srv(80);

  srv.add_handler("/fan1", handler_fun);
  srv.add_handler("/info", handler_info);
  srv.add_handler("/temp", handler_temp);

  engine->add_routine(srv);
  engine->add_routine(led);
  engine->add_routine(temp1);
  engine->start();
  // will never reach this point
}
