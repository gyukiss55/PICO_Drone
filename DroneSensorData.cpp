// DroneSensorData.cpp
#include <Arduino.h>

#include "DroneSensorData.h"

DroneSensorData droneSensorDataCurrent;

uint32_t droneSensorDataVectorIndex;

DroneSensorData droneSensorDataVector1[DroneSensorDataVectorSize];
DroneSensorData droneSensorDataVector2[DroneSensorDataVectorSize];

volatile bool lockDroneSensorDataVector = false;

void AddDroneSensorData(const DroneSensorData& data)
{
	while (lockDroneSensorDataVector) { delay(0); }
	lockDroneSensorDataVector = true;
	droneSensorDataVector1[droneSensorDataVectorIndex] = data;
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
		droneSensorDataVector2[i] = droneSensorDataVector1[j];
	}
	lockDroneSensorDataVector = false;
}

