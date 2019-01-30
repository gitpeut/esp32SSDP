#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "update_server.h"
#include "wifi_ss.h"
#include "SSDPDevice.h"

const char* host = "esesdepe";

WebServer server(80);


/*-----------------------------------------------------------------------------------*/
/*
 * setup function
 */

char  *nothing = "<html><body>Hello, SSDP test. updater test is <a href=\"/update\">here</a></body></html>";

void setup(void) {
  Serial.begin(115200);

  wifi_begin();

  update_begin( &server);

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", nothing);
  });

   server.on("/reset", HTTP_GET, []() {
    server.send(200, "text/plain", "Resetting...");
    delay(3000);
    ESP.restart(); 
  });
  
  server.on("/description.xml", HTTP_GET, [](){
    SSDPDevice.schema( server.client());     
  });

SSDPDevice.setName( host );
  SSDPDevice.setDeviceType("urn:schemas-upnp-org:device:BinaryLight:1");
  SSDPDevice.setSchemaURL("description.xml");
  SSDPDevice.setSerialNumber(ESP.getEfuseMac());
  SSDPDevice.setURL("/");
  SSDPDevice.setModelName(host);
  SSDPDevice.setModelNumber("1");
  SSDPDevice.setManufacturer("Peut");
  SSDPDevice.setManufacturerURL("http://www.peut.org/");
  
  
  
  server.begin();
}

void loop(void) {
  server.handleClient();
  SSDPDevice.handleClient(); 
  
  delay(1);
}
