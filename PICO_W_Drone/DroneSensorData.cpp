// DroneSensorData.cpp
#include <Arduino.h>

#include "DroneSensorData.h"

DroneSensorData droneSensorDataCurrent;

uint32_t droneSensorDataVectorIndex;

DroneSensorData droneSensorDataVector1[DroneSensorDataVectorSize];

DroneExportData droneExportData;


volatile bool lockDroneSensorDataVector = false;

void AddDroneSensorData(const DroneSensorData& data)
{
	while (lockDroneSensorDataVector) { delay(0); }
	lockDroneSensorDataVector = true;
	droneSensorDataVector1[droneSensorDataVectorIndex] = data;
//	Serial.println("ts1 " + String(droneSensorDataVector1[droneSensorDataVectorIndex].GetTimeStamp()) + " ms"); // debug log
	droneSensorDataVectorIndex++;
	if (droneSensorDataVectorIndex >= DroneSensorDataVectorSize)
		droneSensorDataVectorIndex = 0;
	lockDroneSensorDataVector = false;
}


void CloneDroneSensorDataVector()
{
	while (lockDroneSensorDataVector) { delay(0); }
	lockDroneSensorDataVector = true;
	uint32_t j = droneSensorDataVectorIndex;
	for (uint32_t i = 0; i < DroneSensorDataVectorSize; i++, j++) {
		if (j >= DroneSensorDataVectorSize)
			j = 0;
		droneExportData.droneSensorDataVector[i] = droneSensorDataVector1[j];
//		Serial.println("ts2 " + String(droneSensorDataVector2[i].GetTimeStamp()) + " ms"); // debug log
	}
	lockDroneSensorDataVector = false;
}

