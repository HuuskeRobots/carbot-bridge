#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "controller.h"
#include "http_server.h"

static ESP8266WebServer server(80);
static String robotName;

void handleRoot()
{
    digitalWrite(led, 1);
    char temp[32];
    int sec = millis() / 1000;
    int min = sec / 60;
    int hr = min / 60;

    snprintf(temp, 32, "%02d:%02d:%02d", hr, min % 60, sec % 60);
    String content =
        "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>CarBot Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
	<script type='text/javascript'>\
	function motor(action) {\
		var xhr = new XMLHttpRequest();\
		xhr.open('GET', '/motor/' + action);\
		xhr.send();\
	}\
	</script>\
  </head>\
  <body>\
    <h1>Hallo vanaf " +
        robotName + "</h1>\
    <p>Uptime: " + temp + "</p>\
	<div>\
	  <a href='javascript:motor(\"stop\")'>Stop</a>\
	  <a href='javascript:motor(\"vooruit\")'>Vooruit</a>\
	  <a href='javascript:motor(\"achteruit\")'>Achteruit</a>\
	  <a href='javascript:motor(\"links\")'>Links</a>\
	  <a href='javascript:motor(\"rechts\")'>Rechts</a>\
	</div>\
  </body>\
</html>";

    server.send(200, "text/html", content);
    digitalWrite(led, 0);
}

void handleNotFound()
{
    digitalWrite(led, 1);
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, "text/plain", message);
    digitalWrite(led, 0);
}

void handleMotorStop()
{
    setControllerMode(modeStop);
    server.send(200, "text/plain", "ok");
}

void handleMotorForward()
{
    setControllerMode(modeForward);
    server.send(200, "text/plain", "ok");
}

void handleMotorBackward()
{
    setControllerMode(modeBackward);
    server.send(200, "text/plain", "ok");
}

void handleMotorTurnRight()
{
    setControllerMode(modeRight);
    server.send(200, "text/plain", "ok");
}

void handleMotorTurnLeft()
{
    setControllerMode(modeLeft);
    server.send(200, "text/plain", "ok");
}

void handlePoll()
{
    digitalWrite(activityLed, 1);
    server.send(200, "text/plain", "");
    digitalWrite(activityLed, 0);
}

void setupHttpServer(const String& name)
{
    server.on("/", handleRoot);
    server.on("/poll", handlePoll);
    server.on("/reset_all", handleMotorStop);
    server.on("/motor/stop", handleMotorStop);
    server.on("/motor/vooruit", handleMotorForward);
    server.on("/motor/achteruit", handleMotorBackward);
    server.on("/motor/links", handleMotorTurnLeft);
    server.on("/motor/rechts", handleMotorTurnRight);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");
}

void loopHttpServer() {
    server.handleClient();
}
