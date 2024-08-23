#pragma once

struct GearAccMagItem {

	unsigned long	microsec;
	unsigned long	sec;

	double			pitch;
	double			roll;
	double			north;
	double			gear[3];
	double			acc[3];
	double			mag[3];

	//	double			quat[4];

	//	double			speed[2];

	GearAccMagItem() : pitch(0.), roll(0.), north(0.)
	{ }
};

