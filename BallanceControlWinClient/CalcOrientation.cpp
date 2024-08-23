/*
CalcOrientation.cpp
*/

#define PI 3.1415926535

#include <math.h>
#include "CalcOrientation.h"


bool CalcOrientation(GearAccMagItem& gam)
{
	double aRoll = atan2(gam.acc[1], gam.acc[2]);
	double aPitch = atan2(-gam.acc[0], sqrt (gam.acc[1]* gam.acc[1] + gam.acc[2] * gam.acc[2]));
	double mX = gam.mag[0] * cos(aPitch) + gam.mag[2] * sin(aPitch);
	double mY = gam.mag[0] * sin(aRoll) * sin(aPitch) + gam.mag[1] + cos(aRoll) - gam.mag[2]* sin(aRoll)*cos(aPitch);
	double mYaw = atan2(mY, mX);
	gam.pitch = aPitch * 180. / PI;
	gam.roll = aRoll * 180. / PI;
	gam.north = mYaw * 180. / PI;
	return true;
}
