// DroneControlData.h

#pragma once

#include <string.h>

#define MotorReset 1000
#define MotorSpeedMax 2000

#define Motor1StopSpeed 1220
#define Motor2StopSpeed 1182
#define Motor3StopSpeed 1218
#define Motor4StopSpeed 1198

#define Motor1StartSpeed 1232
#define Motor2StartSpeed 1192
#define Motor3StartSpeed 1229
#define Motor4StartSpeed 1212

class DroneControlData {
	char		structMark[4];
	uint32_t	structSize;

	uint32_t	timeStamp;

	uint32_t	speedMotor1;
	uint32_t	speedMotor2;
	uint32_t	speedMotor3;
	uint32_t	speedMotor4;
	uint32_t	function;

	bool		validSpeed;
	bool		validFunction;

public:
	DroneControlData() :
		structMark ("DCD"), structSize(sizeof (DroneControlData)), timeStamp (0), 
		speedMotor1 (0), speedMotor2(0), speedMotor3(0), speedMotor4(0),
		function (0), validSpeed (false), validFunction (false)
	{}

	DroneControlData(uint32_t ts) :
		structMark("DCD"), structSize(sizeof(DroneControlData)), timeStamp(ts),
		speedMotor1(0), speedMotor2(0), speedMotor3(0), speedMotor4(0),
		function(0), validSpeed(false), validFunction(false)
	{}

	uint32_t GetTimeStamp() const { return timeStamp; }
	void UpdateTimetick();

	void SetSpeed(uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4) {
		if (s1 >= MotorReset && s1<= MotorSpeedMax)
			speedMotor1 = s1;
		if (s2 >= MotorReset && s2 <= MotorSpeedMax)
			speedMotor2 = s2;
		if (s3 >= MotorReset && s3 <= MotorSpeedMax)
			speedMotor3 = s3;
		if (s4 >= MotorReset && s4 <= MotorSpeedMax)
			speedMotor4 = s4;
		validSpeed = true;
	}

	void SetFunction(uint32_t f) {
		function = f;
		validFunction = true;
	}

	bool GetSpeed(uint32_t& s1, uint32_t& s2, uint32_t& s3, uint32_t& s4) {
		s1 = s2 = s3 = s4 = 0;
		if (validSpeed) {
			s1 = speedMotor1;
			s2 = speedMotor2;
			s3 = speedMotor3;
			s4 = speedMotor4;
			return true;
		}
		return false;
	}

	bool GetFunction(uint32_t& f) {
		f = 0;
		if (validFunction) {
			f = function;
			return true;
		}
		return false;
	}
};

extern DroneControlData currentControl;
extern DroneControlData temporaryControl;
