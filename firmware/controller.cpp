#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "motor_control.h"
#include "controller.h"

int mode = modeStop;
int iteration = 0;

void setupController()
{
    Serial.println("Controller started");
}

void setControllerMode(int newMode) {
    mode = newMode;
}

bool isControllerMode(int x) {
    return (mode == x);
}

void loopController()
{
    if (iteration == 2)
    {
        if (mode == modeForward)
        {
            //detectObstacle();
        }
        iteration = 0;
    }
    else
    {
        iteration++;
    }
    
    switch (mode)
    {
    case modeStop:
        motorStop();
        break;
    case modeForward:
        motorForward();
        delay(3);
        motorStop();
        delay(2);
        break;
    case modeBackward:
        motorBackward();
        delay(3);
        motorStop();
        delay(2);
        break;
    case modeLeft:
        motorTurnLeft();
        delay(3);
        motorStop();
        delay(2);
        break;
    case modeRight:
        motorTurnRight();
        delay(3);
        motorStop();
        delay(2);
        break;
    }
}
