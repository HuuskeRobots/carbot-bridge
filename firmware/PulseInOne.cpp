/*Copyright (c) 2013 Mike Almond - @mikedotalmond - https://github.com/mikedotalmond
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

/**
* @author Mike Almond - https://github.com/mikedotalmond
* 
* Use a hardware interrupt to emulate the Arduino::pulseIn functionality without pausing code execution while waiting for the pulse
* see http://arduino.cc/en/Reference/Interrupts
* */

#include "Arduino.h"
#include "PulseInOne.h"

// initialise static vars
bool PulseInOne::state = false;
bool PulseInOne::active = false;
int PulseInOne::pin = 1;

void (*PulseInOne::onComplete)(unsigned long) = NULL;

/**
* 
*
**/
void PulseInOne::setup(int pin, void (*pulseComplete)(unsigned long))
{
	onComplete = pulseComplete;
	active = false;
	PulseInOne::pin = pin;
}

/**
* @public
* Start listening on the desired interrupt for a pulse
*/
void PulseInOne::begin()
{
	state = false;
	active = true;
	attachInterrupt(pin, pinChange, RISING);
}

/**
* @public
* There's no timeout with this class: it will listen indefinitely for a change on the interrupt pin.
* Use abandon to stop waiting for a pulse.
*/
void PulseInOne::abandon()
{
	if (active)
	{
		state = active = false;
		detachInterrupt(pin);
	}
}

/**
* @static 
* interrupt handler - called whenever the interrupt pin state changes
*/
void PulseInOne::pinChange()
{

	static unsigned long pulseStart = 0;

	state = !state;

	if (state)
	{
		// interrupt pin has changed, a pulse has started
		pulseStart = micros();					// store the current microseconds
		attachInterrupt(pin, pinChange, FALLING); // now wait for the falling edge
	}
	else
	{

		// pulse complete, detach the interrupt...
		detachInterrupt(pin);

		// pin state changed again - pulse ended
		unsigned long duration = micros() - pulseStart; // get the pulse length

		active = false;
		onComplete(duration);
	}
}