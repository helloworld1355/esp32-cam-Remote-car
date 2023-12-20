#pragma once
#include<string.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>


extern WiFiClient client;

const int freq = 5000;
const int whatbit = 8;
const int Left_Channel_1 = 0;
const int Left_Channel_2 = 1;
const int Right_Channel_1 = 2;
const int Right_Channel_2 = 3;
const int leftPin_1 = 14;
const int leftPin_2 = 15;
const int rightPin_1 = 13;
const int rightPin_2 = 12;

void initPwm();
void handlePwm(void *pvParameters);

void handleForward();
void handleForwardStop();
void handleLeft();
void handleLeftStop();
void handleRight();
void handleRightStop();
void handleBack();
void handleBackStop();
