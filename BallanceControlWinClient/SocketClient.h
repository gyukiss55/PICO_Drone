#pragma once

#include <windows.h>
#include <Winhttp.h>
#include <string>

#include "DroneSensorData.h"
#include "DroneControlData.h"

#define IP_ADDRESS_WEB_SERVER "192.168.50.173"


struct GearAccMag {
	long			sizeOfData;

	double			pitch;
	double			roll;
	double			north;

	double			gear[3];
	double			acc[3];
	double			mag[3];
	double			quat[4];
};


struct MotorStatus {
	long			sizeOfData;

	double			speed;
};


struct PIDStatus {
	long			sizeOfData;

	double			setPoint;
	double			propMul;
	double			inteMul;
	double			diffMul;

	double			measuredPoint;
	double			measuredPointPrev;
	double			delta;
	double			intePrev;
	double			control;

	bool			run;
	bool			execute;

};

#define DroneSensorDataVectorSize 10

struct SharedDataServer {
	uint32_t			cycle;
	uint32_t			lastTimestamp;
	uint32_t			droneSensorDataVectorNumber;
	DroneSensorData		droneSensorDataVector[DroneSensorDataVectorSize];
	DroneControlData	droneControlData;
	SharedDataServer() : cycle(0), lastTimestamp(0), droneSensorDataVectorNumber(0) { ; }
};



int WinSocketClient (const char* ipAddressStr, SharedDataServer& sharedData);
int WinWebSocketClient (const std::wstring& ipAddressStr, SharedDataServer& sharedData);
int SendAndReceivePacket(const std::string& ipAddress, WORD port, const std::string& message, std::string& messageRet);
int SendAndReceivePacket(const std::string& ipAddress, WORD port, SharedDataServer& sharedData);

