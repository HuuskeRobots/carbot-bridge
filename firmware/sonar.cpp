#include "PulseInOne.h"

#include "config.h"
#include "controller.h"
#include "motor_control.h"
#include "sonar.h"

static void triggerDetection()
{
    PulseInOne::abandon();

    // Trigger pulse
    digitalWrite(sonarTriggerPin, LOW);
    delayMicroseconds(2);

    digitalWrite(sonarTriggerPin, HIGH);
    delayMicroseconds(10);

    digitalWrite(sonarTriggerPin, LOW);
    //Serial.println("PulseInOneBegin");
    PulseInOne::begin();
}

/**
* Pulse complete callback hanlder for PulseInOne
* @param duration - pulse length in microseconds
*/
static void pulseInComplete(unsigned long duration)
{
    // note: if you're detecting a lot of pulses a second it's probably best to remove the serial prints...
    // (this function is triggered by an interrupt so the serial buffer can fill up and cause the program to hang)
    /*Serial.print("pulse complete - duration: ");
    Serial.print(duration);
    Serial.println(" us");*/

    long distance = (duration / 2) / 29.1;

    if (distance < 20)
    {
        if (isControllerMode(modeForward)) {
            setControllerMode(modeStop);
            motorStop();
        }
        Serial.print(distance);
        Serial.println(" cm");
    }
    //    delay(1000);
}

void setupSonar()
{
    pinMode(sonarTriggerPin, OUTPUT);
    pinMode(sonarEchoPin, INPUT);

    // set up PulseInOne, pass in the callback function to be triggered when a pulse completes.
    PulseInOne::setup(sonarEchoPin, pulseInComplete);
}

static unsigned long lastTrigger = 0;

void loopSonar()
{
    unsigned long now = millis();
    if ((now - lastTrigger) > 100)
    {
        lastTrigger = now;
        triggerDetection();
    }
}
