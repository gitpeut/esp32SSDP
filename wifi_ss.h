#ifndef WIFI_SS_H
#define WIFI_SS_H

#include <WiFi.h>
// made a directory wificredentials in libraries and define ssid and password in wificredentials.h
#include <wificredentials.h>

void wifi_begin();
void wifi_event ( WiFiEvent_t event );


#endif
