#include <Arduino_FreeRTOS.h>
#include <Ethernet2.h>

#include <heater.hpp>
#include <http.hpp>

byte mac[] = {0x90, 0xA2, 0xda, 0x10, 0xe2, 0x04};

IPAddress ip(192, 168, 55, 2);
EthernetServer server(80);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  //  while(!Ethernet.begin(mac)){
  //    Serial.println("Failed to configure Ethernet using DHCP");
  //    delay(5000);
  //  }

  Ethernet.begin(mac, ip);

  Serial.print("My IP address: ");
  auto ip = Ethernet.localIP();

  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(ip[thisByte], DEC);
    Serial.print(".");
  }

  Serial.println();

  xTaskCreate(TaskHeater, (const portCHAR *)"Heater", 128, NULL, 1, NULL);

  server.begin();
}

// --------------------------------------------------------------------------
void writeAndClose(char *buf, int size, EthernetClient &client, int code, const char *s) {
  int res = makeHTTPResponse(buf, size, code, s);
  if (res > 0) {
    client.println(buf);
    client.stop();
    return;
  }

  res = makeHTTPResponse(buf, size, 500, "MR error 1");
  if (res > 0) {
    client.println(buf);
    client.stop();
    return;
  }

  // cannot answer anything =(
  client.stop();
}

void loop() {
  EthernetClient client = server.available();

  if (client.available()) {
    // Serial.println("We have a new client");

    size_t size = 200;
    char buf[size];

    int res = client.read((uint8_t *)&buf, size);
    if (res <= 0) {
      // Serial.println("ERROR 1");
      writeAndClose(buf, size, client, 400, "Bad request [1]");
      return;
    }

    Request request;
    res = parseHTTP(buf, size, &request);
    if (res < 0) {
      Serial.print("ERROR 2: ");
      Serial.println(res);
      writeAndClose(buf, size, client, 400, "Bad request [2]");
      return;
    }
    // Serial.println(res);

    if (request.parameter("test") == "true") {
      // Serial.println("BINGO1");
    }

    if (request.parameter("on").exists()) {
      // Serial.println("BINGO2");
      char value[10];
      request.parameter("on").value.get(value, 10);
      // Serial.print("on:");
      // Serial.println(value);
    }

    if (request.path == "/info") {
      // Serial.println("BINGO3");
    }

    char value[50];
    res = request.path.get(value, 50);
    // Serial.println(res);
    // Serial.println(value);

    writeAndClose(buf, size, client, 200, value);
    // Serial.println("exit");
  }

  // maintain dhcp connection
  Ethernet.maintain();

}
