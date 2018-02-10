#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "controller.h"
#include "motor_control.h"
#include "ota.h"

void setupOTA(const String& name)
{
    ArduinoOTA.setHostname(name.c_str());
    ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
        Serial.println("OTA starting...");
        motorStop();
    });

    ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
        Serial.println("OTA ending...");
        motorForward();
        delay(500);
        motorBackward();
        delay(500);
        motorStop();
        Serial.println("OTA ended. rebooting");
    });

    ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });

    /* setup the OTA server */
    ArduinoOTA.begin();
    Serial.println("OTA started");
}

void loopOTA()
{
    ArduinoOTA.handle();
}
