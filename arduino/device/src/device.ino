#include <heater.h>
#include <http_server.h>
#include <pwm.h>
#include <relay.h>
#include <temperature.h>
#include <virtual_threads.h>

const uint8_t macaddr[] = {0x90, 0xA2, 0xda, 0x10, 0xe2, 0x04};
const uint8_t ipaddr[] = {192, 168, 55, 2};

#define TEMP_PIN 0
#define TEMP2_PIN 1
#define FAN1_PIN 2
#define FAN2_PIN 3
#define RELAY1_PIN 4
#define RELAY2_PIN 5
#define RELAY3_PIN 6
#define RELAY4_PIN 7
#define HEATER_PIN 8
#define LED_PIN 13

pwm::Controller led(LED_PIN);
pwm::Controller fan1(FAN1_PIN);
pwm::Controller fan2(FAN2_PIN);
class relay::Controller relays[] = {{RELAY1_PIN}, {RELAY2_PIN}, {RELAY3_PIN}, {RELAY4_PIN}};
#define relays_count sizeof(relays) / sizeof(relay::Controller)

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
  resp.end(200, "OK", "[\"fan1\", \"temp1\", \"temp2\", \"warmer\"]");
}

// ---
void handler_fan(http::Request &request, httpserver::ResponseWriter &resp) {
  if (request.parameter("on")) {
    request.parameter("on").value.copyUInt16To(led.on_delay);
  }
  if (request.parameter("off")) {
    request.parameter("off").value.copyUInt16To(led.off_delay);
  }
  resp.end(200, "OK", nullptr);
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
  resp.end(200, "OK", buffer);
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
  resp.end(200, "OK", buffer);
}

// ---
void handler_warmer(http::Request &request, httpserver::ResponseWriter &resp) {
  if (request.parameter("temp")) {
    request.parameter("temp").value.copyInt16To(warmer.target_temp);
  }

  char buffer[100];
  char buf[30];
  if (temp2.error) {
    sprintf(buf, "%s", temp2.error.getMessage());
  } else {
    temp2.getTextValue(buf, 20);
  }
  sprintf(buffer, "{\"current_temp\":%s, \"target_temp\":%d, \"level\":%d}\r\n", buf,
          warmer.target_temp, warmer.current_level);

  resp.end(200, "OK", buffer);
}

String getStatus(http::Request &request, relay::Controller &relay) {
  if (request.parameter("on") == "true") {
    relay.turnOn();
  } else if (request.parameter("on") == "false") {
    relay.turnOff();
  }
  if (relay.isOn()) {
    return "on";
  }
  return "off";
}

// ---
void handler_relays(http::Request &request, httpserver::ResponseWriter &resp) {
  int8_t id = 0;
  if (request.parameter("id").value.copyInt8To(id)) {
    for (uint8_t i = 0; i < relays_count; ++i) {
      if (i == id) {
        if (request.parameter("on") == "true") {
          relays[i].turnOn();
        }
        if (request.parameter("on") == "false") {
          relays[i].turnOff();
        }
        break;
      }
    }
  }

  char buffer[100];
  resp.writeHead(200, "OK");
  resp.write("[");
  for (uint8_t i = 0; i < relays_count; ++i) {
    auto state = relays[i].isOn() ? "true" : "false";
    auto last = (i == relays_count - 1) ? ' ' : ',';
    sprintf(buffer, "{\"id\":%d, \"on\":%s}%c", i, state, last);
    resp.write(buffer);
  }
  resp.write("]\r\n");
  resp.end();
}

// MAIN ---------------------------------------------------------------------
void loop() {
  httpserver::NetworkService srv(80);

  srv.add_handler("/fan1", handler_fan);
  srv.add_handler("/info", handler_info);
  srv.add_handler("/temp1", handler_temp1);
  srv.add_handler("/temp2", handler_temp2);
  srv.add_handler("/warmer", handler_warmer);
  srv.add_handler("/relays", handler_relays);

  auto engine = VT::Engine::getInstance();
  engine->start();
  // will never reach this point
}
