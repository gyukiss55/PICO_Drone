// DroneControlData.cpp

#include <windows.h>
#include <stdint.h>

#include "DroneControlData.h"

void DroneControlData::UpdateTimetick() {
	timeStamp = GetTickCount();
}



DroneControlData currentControl;
DroneControlData temporaryControl;
