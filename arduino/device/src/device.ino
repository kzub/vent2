#include <heater.h>
#include <http_server.h>
#include <pwm.h>
#include <relay.h>
#include <temperature.h>
#include <virtual_threads.h>

const uint8_t macaddr[] = {0x90, 0xA2, 0xda, 0x10, 0xe2, 0x04};
const uint8_t ipaddr[] = {192, 168, 55, 1};

#define TEMP1_PIN 0
#define TEMP2_PIN 1
#define TEMP3_PIN 2
#define FAN1_PIN 3
#define FAN2_PIN 4
#define RELAY1_PIN 5
#define RELAY2_PIN 6
#define RELAY3_PIN 7
#define RELAY4_PIN 8
#define HEATER_PIN 9
#define LED_PIN 13

pwm::Controller fans[]{{FAN1_PIN}, {FAN2_PIN}};
#define fans_count sizeof(fans) / sizeof(pwm::Controller)

relay::Controller relays[] = {{RELAY1_PIN}, {RELAY2_PIN}, {RELAY3_PIN}, {RELAY4_PIN}};
#define relays_count sizeof(relays) / sizeof(relay::Controller)

temperature::DS1820Sensor t_sensors[] = {{TEMP1_PIN}, {TEMP2_PIN}, {TEMP3_PIN}};
#define t_sensors_count sizeof(t_sensors) / sizeof(temperature::DS1820Sensor)

// temperature::DHT22Sensor temp1(TEMP1_PIN);
heater::Controller warmer(HEATER_PIN, fans[0], t_sensors[0]);

char buffer[200];
char buf[30];

const char *HTTP_PUBLIC_OK = "OK\r\nAccess-Control-Allow-Origin: *";
void write_warmer_response(httpserver::ResponseWriter &resp);
void write_t_sensors_response(httpserver::ResponseWriter &resp);
void write_fans_response(httpserver::ResponseWriter &resp);
void write_relays_response(httpserver::ResponseWriter &resp);

// INIT ---------------------------------------------------------------------
void setup() {
  // Serial.begin(115200);
  // while (!Serial) {
  // }

  httpserver::network::initialize(macaddr, nullptr /*, ipaddr*/);
}

// REQEUST HANDLERS ---------------------------------------------------------
// ---
void handler_info(http::Request &request, httpserver::ResponseWriter &resp) {
  resp.writeHead(200, HTTP_PUBLIC_OK);
  resp.write("[");
  resp.write("\"fans\":");
  write_fans_response(resp);
  resp.write(",\r\n\"relays\":");
  write_relays_response(resp);
  resp.write(",\r\n\"t_sensors\":");
  write_t_sensors_response(resp);
  resp.write(",\r\n\"warmer\":");
  write_warmer_response(resp);
  resp.write("]");
  resp.end();
}

// ---
/*
void handler_temp1(http::Request &request, httpserver::ResponseWriter &resp) {
  if (temp1.error) {
    sprintf(buffer, "{\"error\":\"%s\"}\r\n", temp1.error.getMessage());
  } else {
    char c[20], h[20];
    dtostrf(temp1.getTemperatureC(), 6, 1, c);
    dtostrf(temp1.getHumidity(), 6, 1, h);
    sprintf(buffer, "{\"t\":%s,\"h\":%s, \"pin\":%d}\r\n", c, h, temp1.pin);
  }
  resp.end(200, HTTP_PUBLIC_OK, buffer);
}
*/

// ---
void write_warmer_response(httpserver::ResponseWriter &resp) {
  if (warmer.temp.error) {
    sprintf(buf, "%s", warmer.temp.error.getMessage());
  } else {
    warmer.temp.getTextValue(buf, sizeof(buf));
  }
  sprintf(buffer, "{\"current_t\":%s, \"target_t\":%d, \"level\":%d, \"pin\":%d}\r\n", buf,
          warmer.target_temp, warmer.current_level, warmer.pin);
  resp.write(buffer);
}

void handler_warmer(http::Request &request, httpserver::ResponseWriter &resp) {
  request.parameter("temp").value.copyInt16To(warmer.target_temp);

  resp.writeHead(200, HTTP_PUBLIC_OK);
  write_warmer_response(resp);
  resp.end();
}

// ---
void write_relays_response(httpserver::ResponseWriter &resp) {
  resp.write("[");
  for (uint8_t i = 0; i < relays_count; ++i) {
    auto state = relays[i].isOn() ? "true" : "false";
    auto last = (i == relays_count - 1) ? ' ' : ',';
    sprintf(buffer, "{\"id\":%d, \"on\":%s, \"pin\":%d}%c", i, state, relays[i].pin, last);
    resp.write(buffer);
  }
  resp.write("]\r\n");
}

void handler_relays(http::Request &request, httpserver::ResponseWriter &resp) {
  int8_t id = 0;
  if (request.parameter("id").value.copyInt8To(id) && id < relays_count) {
    if (request.parameter("on") == "true") {
      relays[id].turnOn();
    }
    if (request.parameter("on") == "false") {
      relays[id].turnOff();
    }
  }

  resp.writeHead(200, HTTP_PUBLIC_OK);
  write_relays_response(resp);
  resp.end();
}

// ---
void write_fans_response(httpserver::ResponseWriter &resp) {
  resp.write("[");
  for (uint8_t i = 0; i < fans_count; ++i) {
    auto last = (i == fans_count - 1) ? ' ' : ',';
    sprintf(buffer, "{\"id\":%d, \"on\":%d, \"off\":%d, \"pin\":%d}%c", i, fans[i].on_delay,
            fans[i].off_delay, fans[i].pin, last);
    resp.write(buffer);
  }
  resp.write("]\r\n");
}

void handler_fans(http::Request &request, httpserver::ResponseWriter &resp) {
  int8_t id = 0;
  if (request.parameter("id").value.copyInt8To(id) && id < fans_count) {
    request.parameter("on").value.copyUInt16To(fans[id].on_delay);
    request.parameter("off").value.copyUInt16To(fans[id].off_delay);
  }

  resp.writeHead(200, HTTP_PUBLIC_OK);
  write_fans_response(resp);
  resp.end();
}

// ---
void write_t_sensors_response(httpserver::ResponseWriter &resp) {
  resp.write("[");
  for (uint8_t i = 0; i < t_sensors_count; ++i) {
    auto last = (i == t_sensors_count - 1) ? ' ' : ',';
    if (t_sensors[i].error) {
      sprintf(buf, "%s", t_sensors[i].error.getMessage());
    } else {
      t_sensors[i].getTextValue(buf, sizeof(buf));
    }
    sprintf(buffer, "{\"id\":%d, \"t\":%s, \"pin\":%d}%c", i, buf, t_sensors[i].pin, last);
    resp.write(buffer);
  }
  resp.write("]\r\n");
}

void handler_t_sensors(http::Request &request, httpserver::ResponseWriter &resp) {
  resp.writeHead(200, HTTP_PUBLIC_OK);
  write_t_sensors_response(resp);
  resp.end();
}

// MAIN ---------------------------------------------------------------------
void loop() {
  httpserver::NetworkService srv(80);

  srv.add_handler("/info", handler_info);
  // srv.add_handler("/temp", handler_temp1);
  srv.add_handler("/warmer", handler_warmer);
  srv.add_handler("/relays", handler_relays);
  srv.add_handler("/fans", handler_fans);
  srv.add_handler("/t_sensors", handler_t_sensors);

  auto engine = VT::Engine::getInstance();
  engine->start();
  // will never reach this point
}
