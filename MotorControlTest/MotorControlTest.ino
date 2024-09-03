
#include <string>

#include "MotorControl.h"


bool setupDone = false;

void setup()
{
	// Initialize the Serial Monitor
	Serial.begin(115200);
	delay(2000);
	setupMotorControl ();
	setupDone = true;
}

void loop()
{
	loopMotorControl ();
}

