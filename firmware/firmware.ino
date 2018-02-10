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
const int activityLed = 13;
const int motorA_BW = 5; // D1 A-IA Backward
const int motorA_FW = 4; // D2 A-IB Forward
const int motorB_BW = 0; // D3 B-IA Backward
const int motorB_FW = D7; // D4 B-IB Forward

#define TRIGGER D5
#define ECHO    D6

const int modeStop = 0;
const int modeForward = 1;
const int modeBackward = 2;
const int modeLeft = 3;
const int modeRight = 4;

int mode = modeStop;

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
    <h1>Hallo vanaf " + robotName + "</h1>\
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

void motorStop() {
    digitalWrite(motorA_BW, 0);
    digitalWrite(motorA_FW, 0);
    digitalWrite(motorB_BW, 0);
    digitalWrite(motorB_FW, 0);	
}

void motorForward() {
    digitalWrite(motorA_BW, 0);
    digitalWrite(motorA_FW, 1);
    digitalWrite(motorB_BW, 0);
    digitalWrite(motorB_FW, 1);	
}

void motorBackward() {
    digitalWrite(motorA_BW, 1);
    digitalWrite(motorA_FW, 0);
    digitalWrite(motorB_BW, 1);
    digitalWrite(motorB_FW, 0);	
}

void motorTurnRight() {
    digitalWrite(motorA_BW, 1);
    digitalWrite(motorA_FW, 0);
    digitalWrite(motorB_BW, 0);
    digitalWrite(motorB_FW, 1);	
}

void motorTurnLeft() {
    digitalWrite(motorA_BW, 0);
    digitalWrite(motorA_FW, 1);
    digitalWrite(motorB_BW, 1);
    digitalWrite(motorB_FW, 0);	
}

void detectObstacle() {
    long duration, distance;
    digitalWrite(TRIGGER, LOW);  
    delayMicroseconds(2); 

    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(10); 

    digitalWrite(TRIGGER, LOW);
    duration = pulseIn(ECHO, HIGH);
    distance = (duration/2) / 29.1;

    if (distance < 20) {
        mode = modeStop;
        motorStop();
    }
    //Serial.print(distance);
    //Serial.println(" cm");
//    delay(1000);
}

void handleMotorStop() {
    mode = modeStop;
    server.send(200, "text/plain", "ok");
}

void handleMotorForward() {
    mode = modeForward;
    server.send(200, "text/plain", "ok");
}

void handleMotorBackward() {
    mode = modeBackward;
    server.send(200, "text/plain", "ok");
}

void handleMotorTurnRight() {
    mode = modeRight;
    server.send(200, "text/plain", "ok");
}

void handleMotorTurnLeft() {
    mode = modeLeft;
    server.send(200, "text/plain", "ok");
}

void handlePoll() {
    digitalWrite(activityLed, 1);
    server.send(200, "text/plain", "");
    digitalWrite(activityLed, 0);
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
    pinMode(activityLed, OUTPUT);
    pinMode(TRIGGER, OUTPUT);
    pinMode(ECHO, INPUT);
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

int iteration = 0;

void loop(void)
{
    if (iteration == 2) {
        if (mode == modeForward) {
            detectObstacle();
        }
        iteration = 0;
    } else {
        iteration++;
    }
    server.handleClient();
    switch (mode) {
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
