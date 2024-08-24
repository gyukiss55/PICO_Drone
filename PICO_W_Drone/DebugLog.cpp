// DebugLog.cpp

#include <Arduino.h>

#include "DebugLog.h"

int logSerial2Request = 0;
int logSerialRequest = 0;
int serialDebugID = 0;
int serial2DebugID = 0;

int CheckLogStateSerial(int id)
{
	if (id != 0) {
		if (id == serialDebugID) {
			serialDebugID = 0;
			if (logSerialRequest == 1)
				logSerialRequest = 0;
		} else {
			serialDebugID = id;
		}
	}
	if (Serial.available()) {
		char ch = Serial.read();
		if (ch >= '0' && ch <= '9')
			logSerialRequest = ch - '0';
	}
#if (SERIAL_DEBUG_LOG_LEVEL == 3)
	logSerialRequest = 3;
#endif
	return logSerialRequest;
}

int CheckLogStateSerial2(int id)
{
	if (id != 0) {
		if (id == serial2DebugID) {
			serial2DebugID = 0;
			if (logSerial2Request == 1)
				logSerial2Request = 0;
		}
		else {
			serial2DebugID = id;
		}
	}
	if (Serial2.available()) {
		char ch = Serial2.read();
		if (ch >= '0' && ch <= '9')
			logSerial2Request = ch - '0';
	}
#if (SERIAL_DEBUG_LOG_LEVEL == 3)
	logSerial2Request = 3;
#endif
	return logSerial2Request;
}
