#pragma once
#include <ESP8266WiFi.h>

const int led = 13;
const int activityLed = 13;
const int motorA_BW = 5;  // D1 A-IA Backward
const int motorA_FW = 4;  // D2 A-IB Forward
const int motorB_BW = 0;  // D3 B-IA Backward
const int motorB_FW = D7; // D4 B-IB Forward

const int sonarTriggerPin = D5;
const int sonarEchoPin = D6;

