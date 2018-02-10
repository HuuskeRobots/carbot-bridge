#include <Arduino.h>
#include "config.h"
#include "motor_control.h"

void setupMotors() {
    pinMode(motorA_BW, OUTPUT);
    pinMode(motorA_FW, OUTPUT);
    pinMode(motorB_BW, OUTPUT);
    pinMode(motorB_FW, OUTPUT);

    digitalWrite(motorA_BW, 0);
    digitalWrite(motorA_FW, 0);
    digitalWrite(motorB_BW, 0);
    digitalWrite(motorB_FW, 0);

    Serial.println("Motor control started");
}

void motorStop()
{
    digitalWrite(motorA_BW, 0);
    digitalWrite(motorA_FW, 0);
    digitalWrite(motorB_BW, 0);
    digitalWrite(motorB_FW, 0);
}

void motorForward()
{
    digitalWrite(motorA_BW, 0);
    digitalWrite(motorA_FW, 1);
    digitalWrite(motorB_BW, 0);
    digitalWrite(motorB_FW, 1);
}

void motorBackward()
{
    digitalWrite(motorA_BW, 1);
    digitalWrite(motorA_FW, 0);
    digitalWrite(motorB_BW, 1);
    digitalWrite(motorB_FW, 0);
}

void motorTurnRight()
{
    digitalWrite(motorA_BW, 1);
    digitalWrite(motorA_FW, 0);
    digitalWrite(motorB_BW, 0);
    digitalWrite(motorB_FW, 1);
}

void motorTurnLeft()
{
    digitalWrite(motorA_BW, 0);
    digitalWrite(motorA_FW, 1);
    digitalWrite(motorB_BW, 1);
    digitalWrite(motorB_FW, 0);
}
