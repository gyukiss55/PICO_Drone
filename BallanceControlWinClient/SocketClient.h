#pragma once

#include <windows.h>
#include <Winhttp.h>
#include <string>


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


struct SharedDataServer {
	long			sizeOfData;
	char			nameOfData[16];

	GearAccMag		gearAccMag;
	MotorStatus		motorLeft;
	MotorStatus		motorRight;
	PIDStatus		pidTunning;

	long			sec;
	long			microSec;
	unsigned long	cycle;
	bool			stop;
};



int WinSocketClient (const char* ipAddressStr, SharedDataServer& sharedData);
int WinWebSocketClient (const std::wstring& ipAddressStr, SharedDataServer& sharedData);
int SendAndReceivePacket(const std::string& ipAddress, WORD port, const std::string& message, std::string& messageRet);
int SendAndReceivePacket(const std::string& ipAddress, WORD port, SharedDataServer& sharedData);

