#pragma once
#include "esp_http_server.h"
#include "string.h"
#include "WiFi.h"
#include "iostream"

extern IPAddress serverIp;
extern uint16_t serverPort;

void wifiServer();
int WiFiStart(const char * ssid,const char* password);
static esp_err_t connect_handler(httpd_req_t *req);
static esp_err_t submit_handler(httpd_req_t *req);




