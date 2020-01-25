// OTA.h


#pragma once

#include "arduino.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void OtaSetup(const char* ssid, const char* password);

void OtaHandle();




