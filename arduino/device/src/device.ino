#include <heater.h>
#include <http_server.h>
#include <pwm.h>
#include <temperature.h>
#include <virtual_threads.h>

const uint8_t macaddr[] = {0x90, 0xA2, 0xda, 0x10, 0xe2, 0x04};
const uint8_t ipaddr[] = {192, 168, 55, 2};

#define LED_PIN 13
#define TEMP_PIN 4
#define TEMP2_PIN 6
#define HEATER_PIN 7

pwm::Controller led(LED_PIN);
temperature::DHT22Sensor temp1(TEMP_PIN);
temperature::DS1820Sensor temp2(TEMP2_PIN);
heater::Controller warmer(HEATER_PIN, led, temp2);

// INIT ---------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // while (!Serial) {
  // }

  httpserver::network::initialize(macaddr, ipaddr);
}

// REQEUST HANDLERS ---------------------------------------------------------
// ---
void handler_info(http::Request &request, httpserver::ResponseWriter &resp) {
  resp.write(200, "OK", "[\"fan1\", \"temp1\", \"temp2\", \"warmer\"]");
}

// ---
void handler_fun(http::Request &request, httpserver::ResponseWriter &resp) {
  if(request.parameter("on")){
    request.parameter("on").value.copyUInt16To(led.on_delay);
  }
  if(request.parameter("off")){
    request.parameter("off").value.copyUInt16To(led.off_delay);
  }
  resp.write(200, "OK", nullptr);
}

// ---
void handler_temp1(http::Request &request, httpserver::ResponseWriter &resp) {
  char buffer[100];
  if (temp1.error) {
    sprintf(buffer, "{\"error\":\"%s\"}\r\n", temp1.error.getMessage());
  } else {
    char c[20], h[20];
    dtostrf(temp1.getTemperatureC(), 6, 1, c);
    dtostrf(temp1.getHumidity(), 6, 1, h);
    sprintf(buffer, "{\"temperature\":%s,\"humidity\":%s}\r\n", c, h);
  }
  resp.write(200, "OK", buffer);
}

// ---
void handler_temp2(http::Request &request, httpserver::ResponseWriter &resp) {
  char buffer[100];
  if (temp2.error) {
    sprintf(buffer, "{\"error\":\"%s\"}\r\n", temp2.error.getMessage());
  } else {
    char buf[20];
    temp2.getTextValue(buf, 20);
    sprintf(buffer, "{\"temperature\":%s}\r\n", buf);
  }
  resp.write(200, "OK", buffer);
}

// ---
void handler_warmer(http::Request &request, httpserver::ResponseWriter &resp) {
  if(request.parameter("temp")){
    request.parameter("temp").value.copyInt16To(warmer.target_temp);
  }

  char buffer[100];
  char buf[30];
  if (temp2.error) {
    sprintf(buf, "%s", temp2.error.getMessage());
  }else{
    temp2.getTextValue(buf, 20);
  }
  sprintf(buffer,
          "{\"current_temp\":%s, \"target_temp\":%d, \"level\":%d}\r\n",
          buf, warmer.target_temp, warmer.current_level);

  resp.write(200, "OK", buffer);
}

// MAIN ---------------------------------------------------------------------
void loop() {
  httpserver::NetworkService srv(80);

  srv.add_handler("/fan1", handler_fun);
  srv.add_handler("/info", handler_info);
  srv.add_handler("/temp1", handler_temp1);
  srv.add_handler("/temp2", handler_temp2);
  srv.add_handler("/warmer", handler_warmer);

  // manual "multithreading" starts here:
  // engine->add_routine(srv);
  // engine->add_routine(led);
  // engine->add_routine(temp1);
  // engine->add_routine(temp2);
  // engine->add_routine(warmer);
  auto engine = VT::Engine::getInstance();
  engine->start();
  // will never reach this point
}
