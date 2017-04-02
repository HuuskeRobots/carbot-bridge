#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// firmware-secure.h must contain:
// char ssid[] = "your-wifi-ssid";
// char wpa_passphrase[] = "your-wpa-password";
// char ap_wpa_passphrase[] = "your-wpa-password for the soft AP";
// #define OTA_PASSWORD "your-ota-password"
#include "firmware-secure.h"

String robotName;
ESP8266WebServer server(80);
const int led = 13;
const int motorA_BW = 5; // D1 A-IA Backward
const int motorA_FW = 4; // D2 A-IB Forward
const int motorB_BW = 0; // D3 B-IA Backward
const int motorB_FW = 2; // D4 B-IB Forward

void handleRoot()
{
    digitalWrite(led, 1);
    char temp[32];
    int sec = millis() / 1000;
    int min = sec / 60;
    int hr = min / 60;

    snprintf(temp, 32, "%02d:%02d:%02d",  hr, min % 60, sec % 60);
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
    <h1>Hello from " + robotName + "</h1>\
    <p>Uptime: " + temp + "</p>\
	<div>\
	  <a href='javascript:motor(\"stop\")'>Stop</a>\
	  <a href='javascript:motor(\"forward\")'>Forward</a>\
	  <a href='javascript:motor(\"backward\")'>Backward</a>\
	  <a href='javascript:motor(\"turnleft\")'>Turn left</a>\
	  <a href='javascript:motor(\"turnright\")'>Turn right</a>\
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

void handleMotorStop() {
    digitalWrite(motorA_BW, 0);
    digitalWrite(motorA_FW, 0);
    digitalWrite(motorB_BW, 0);
    digitalWrite(motorB_FW, 0);	
    server.send(200, "text/plain", "ok");
}

void handleMotorForward() {
    digitalWrite(motorA_BW, 0);
    digitalWrite(motorA_FW, 1);
    digitalWrite(motorB_BW, 0);
    digitalWrite(motorB_FW, 1);	
    server.send(200, "text/plain", "ok");
}

void handleMotorBackward() {
    digitalWrite(motorA_BW, 1);
    digitalWrite(motorA_FW, 0);
    digitalWrite(motorB_BW, 1);
    digitalWrite(motorB_FW, 0);	
    server.send(200, "text/plain", "ok");
}

void handleMotorTurnRight() {
    digitalWrite(motorA_BW, 1);
    digitalWrite(motorA_FW, 0);
    digitalWrite(motorB_BW, 0);
    digitalWrite(motorB_FW, 1);	
    server.send(200, "text/plain", "ok");
}

void handleMotorTurnLeft() {
    digitalWrite(motorA_BW, 0);
    digitalWrite(motorA_FW, 1);
    digitalWrite(motorB_BW, 1);
    digitalWrite(motorB_FW, 0);	
    server.send(200, "text/plain", "ok");
}

void setup(void)
{
    char tmp[8];
    sprintf(tmp, "%06x", ESP.getChipId());
    robotName = String("CarBot-") + String(tmp);

	// Setup pins 
    pinMode(led, OUTPUT);
    digitalWrite(led, 0);
    pinMode(motorA_BW, OUTPUT);
    pinMode(motorA_FW, OUTPUT);
    pinMode(motorB_BW, OUTPUT);
    pinMode(motorB_FW, OUTPUT);
    digitalWrite(motorA_BW, 0);
    digitalWrite(motorA_FW, 0);
    digitalWrite(motorB_BW, 0);
    digitalWrite(motorB_FW, 0);

	// Setup serial port 
    Serial.begin(115200);

	// Initialize WIFI
    WiFi.begin(ssid, wpa_passphrase);
    Serial.println("");

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

    server.on("/", handleRoot);
    server.on("/inline", []() {
	server.send(200, "text/plain", "this works as well");
    });
    server.on("/motor/stop", handleMotorStop);
    server.on("/motor/forward", handleMotorForward);
    server.on("/motor/backward", handleMotorBackward);
    server.on("/motor/turnleft", handleMotorTurnLeft);
    server.on("/motor/turnright", handleMotorTurnRight);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void)
{
    server.handleClient();
}
