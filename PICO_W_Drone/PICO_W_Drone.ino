#include <Wire.h>
//#include <MPU6050.h>  // Use MPU9250.h if you have MPU-9250
#include <MPU9250_asukiaaa.h>
#include <string>

#include "MeasureDistance_HC_SR04.h" 
#include "MPU_6500_Drive.h" 
#include "DeviceUART2.h" 
#include "DeviceGPS.h" 
#include "AsyncUDPServer.h"
#include "DroneSensorData.h"
#include "MotorControl.h"


//MPU6050 mpu;  // Create an object for the MPU6050/9250
MPU9250_asukiaaa  mpu;  // Create an object for the MPU6050/9250

bool setupDone = false;

void setup()
{
	// Initialize the Serial Monitor
	Serial.begin(115200);
	delay(2000);
	setupMotorControl ();
	setupDeviceUART2 (115200, String ("Setup Serial 2 baud:115200"));
	setupMPU_6500_Drive();
	setupHC_SR04();
	setupDone = true;
}

void loop()
{
	std::string command;
	while(Serial.available ()) {
		command += Serial.read ();
	}
	if (command.length () > 0) {
		int v = std::stoi( command );
		Serial.print("command:");
		Serial.print(command.c_str());
		if (v >=1000 && v <= 2000) {
			SetSpeedMotor (1, v);
		} else {
			Serial.println(" INVALID!");
		}
	}
	//loopMotorControl ();
	if (Measure_MPU_6500_Drive()) {
		Measure_HC_SR04();
		AddDroneSensorData (droneSensorDataCurrent);
	}
}

void setup1()
{
	setupAsyncUDPServer ();
	setupDeviceGPS ();
}
void loop1()
{
	loopDeviceGPS ();
	if (!setupDone)
		return;
	loopAsyncUDPServer ();
}