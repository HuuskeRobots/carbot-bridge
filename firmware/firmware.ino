#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "config.h"
#include "controller.h"
#include "http_server.h"
#include "motor_control.h"
#include "ota.h"
#include "sonar.h"

// firmware-secure.h must contain:
// char ssid[] = "your-wifi-ssid";
// char wpa_passphrase[] = "your-wpa-password";
// char ap_wpa_passphrase[] = "your-wpa-password for the soft AP";
// #define OTA_PASSWORD "your-ota-password"
#include "firmware-secure.h"

void setup(void)
{
    // Setup serial port
    Serial.begin(9600);

    // Prepare our name
    char tmp[8];
    sprintf(tmp, "%06x", ESP.getChipId());
    String robotName = String("CarBot-") + String(tmp);
    Serial.println();
    Serial.print("Starting ");
    Serial.println(robotName);

    // Setup pins
    pinMode(led, OUTPUT);
    pinMode(activityLed, OUTPUT);
    digitalWrite(led, 0);
    setupMotors();
    setupSonar();

    // Initialize WIFI
    Serial.print("Connecting to WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, wpa_passphrase);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin(robotName.c_str()))
    {
        Serial.println("MDNS responder started");
        MDNS.addService("carbot", "tcp", 80);
    }

    setupOTA(robotName);
    setupController();
    setupHttpServer(robotName);
}

void loop(void)
{
    loopOTA();
    loopHttpServer();
    loopSonar();
    loopController();
}
