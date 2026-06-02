#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WebServer.h>
#endif
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#endif
#include "sensor_hub.h"
#include "WebServerWrapper.h"
#include "HallSensor.h"
#include "wifi_config.h"