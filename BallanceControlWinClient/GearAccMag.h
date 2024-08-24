#pragma once

struct GearAccMagItem {

	unsigned long	microsec;
	unsigned long	sec;

	float			pitch;
	float			roll;
	float			north;
	float			gear[3];
	float			acc[3];
	float			mag[3];
	float			horizontalDir;

	float			elevationUS;
	float			elevationBaro;
	float			batteryMotor;
	float			batteryMPU;
	//	double			quat[4];

	//	double			speed[2];

	GearAccMagItem() : pitch(0.), roll(0.), north(0.), elevationUS(0.), elevationBaro(0.), batteryMotor(0.), batteryMPU(0.)
	{ }
};

