// DroneControlData.h

#pragma once

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
	void SetSpeed(uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4) {
		speedMotor1 = s1;
		speedMotor2 = s2;
		speedMotor3 = s3;
		speedMotor4 = s4;
		validSpeed = true;
	}
	void SetFunction(uint32_t f) {
		function = f;
		validFunction = true;
	}

	bool GetSpeed(uint32_t& s1, uint32_t& s2, uint32_t& s3, uint32_t& s4) {
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
		if (validFunction) {
			f = function;
			return true;
		}
		return false;
	}
};

extern DroneControlData currentControl;
extern DroneControlData previousControl;
