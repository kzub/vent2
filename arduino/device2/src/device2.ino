#include <Ethernet2.h>
#include <LPS331.h>
#include <Wire.h>
#include <http_server.h>
#include <temperature.h>
#include <virtual_threads.h>

const uint8_t macaddr[] = {0x90, 0xA2, 0xda, 0x10, 0xe2, 0x24};
const uint8_t ipaddr[] = {192, 168, 55, 2};

#define TEMP1_PIN 1
#define LED_PIN 13

const temperature::DHT22Sensor t_sensors[] = {{TEMP1_PIN}};
const char *HTTP_PUBLIC_OK = "OK\r\nAccess-Control-Allow-Origin: *";
constexpr int t_sensors_count = sizeof(t_sensors) / sizeof(temperature::DHT22Sensor);

LPS331 barometr;

char buffer[200];
char buf[30];

// --- INIT ---------------------------------------------------------------------
void setup() {
  barometr.begin();
  httpserver::network::initialize(macaddr, nullptr /*ipaddr*/);
}

// --- REQEUST HANDLERS ---------------------------------------------------------
using specific_handler = void (*)(httpserver::ResponseWriter &resp);
void handle_with_specific(httpserver::ResponseWriter &resp, const specific_handler &func) {
  resp.writeHead(200, HTTP_PUBLIC_OK);
  resp.write("[");
  func(resp);
  resp.write("]");
  resp.end();
}

// --- INFO ------------------------------------------------------
void write_t_sensors_response(httpserver::ResponseWriter &resp);
void write_barometr_response(httpserver::ResponseWriter &resp);
void write_info_response(httpserver::ResponseWriter &resp) {
  write_t_sensors_response(resp);
  resp.write(",\r\n");
  write_barometr_response(resp);
}

void handler_info(http::Request &request, httpserver::ResponseWriter &resp) {
  handle_with_specific(resp, write_info_response);
}

// --- T_SENSORS ------------------------------------------------------
void write_t_sensors_response(httpserver::ResponseWriter &resp) {
  for (uint8_t i = 0; i < t_sensors_count; ++i) {
    auto last = (i == t_sensors_count - 1) ? ' ' : ',';
    if (t_sensors[i].error) {
      sprintf(buf, "\"%s\"", t_sensors[i].error.getMessage());
      sprintf(buffer, "{\"type\":\"t_sensor22\", \"id\":%d, \"t\":%s, \"h\":%s, \"pin\":%d}%c", i,
              buf, buf, t_sensors[i].pin, last);
      resp.write(buffer);
      continue;
    }

    char c[20], h[20];
    dtostrf(t_sensors[i].getTemperatureC(), 6, 1, c);
    dtostrf(t_sensors[i].getHumidity(), 6, 1, h);
    sprintf(buffer, "{\"type\":\"t_sensor22\", \"t\":%s,\"h\":%s, \"pin\":%d}%c", c, h,
            t_sensors[i].pin, last);
    resp.write(buffer);
  }
}

// --- BAROMETR ------------------------------------------------------
void write_barometr_response(httpserver::ResponseWriter &resp) {
  float pressure = barometr.readPressureMillimetersHg();
  float temperature = barometr.readTemperatureC();
  char p[20], t[20];
  dtostrf(pressure, 6, 1, p);
  dtostrf(temperature, 6, 1, t);
  sprintf(buffer, "{\"type\":\"barometr\", \"p\":%s,\"t\":%s}", p, t);
  resp.write(buffer);
}

void handler_t_sensors(http::Request &request, httpserver::ResponseWriter &resp) {
  handle_with_specific(resp, write_t_sensors_response);
}
void handler_barometr(http::Request &request, httpserver::ResponseWriter &resp) {
  handle_with_specific(resp, write_barometr_response);
}

// MAIN ---------------------------------------------------------------------
void loop() {
  httpserver::NetworkService srv(80);

  srv.add_handler("/info", handler_info);
  srv.add_handler("/t_sensors", handler_t_sensors);
  srv.add_handler("/barometr", handler_barometr);
  auto engine = VT::Engine::getInstance();
  engine->start();
  // will never reach this point
}
