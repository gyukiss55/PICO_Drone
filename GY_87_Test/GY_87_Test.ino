
#include <Wire.h>
#include "gy87_test.h"
#include "I2CScenner.h"

void setup ()
{
	Serial.begin (115200);
	setupGY_87();
	//setupI2CScenner();
}

void loop ()
{
	loopGY_87();
	//loopI2CScenner();
}