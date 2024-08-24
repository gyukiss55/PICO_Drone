// DeviceGPS.cpp




#include <Arduino.h>
#include <string>

#include "DeviceGPS.h"
#include "DroneSensorData.h"

#define UART1TXPIN 0
#define UART1RXPIN 1

void setupDeviceGPS()
{
	// Initialize the Serial Monitor
	Serial1.setRX(UART1RXPIN);
	Serial1.setTX(UART1TXPIN);
	Serial1.setFIFOSize(512);
	Serial1.begin(9600);
	delay(2000);
	//Serial1.print("\r\n\r\n\r\n");
	Serial.println("GPS Setup done!");
	//delay (2000);
}

void loopDeviceGPS()
{
	static std::string sentence;

	while (Serial1.available()) {
		char ch = Serial1.read();
		if (ch == '\r' || ch == '\n') {
			if (sentence.find("$GPGGA,") == 0) {
				if (sentence.length() >= 100)
					sentence = sentence.substr(0, 99);
				droneExportData.droneGPSData.SetGPGGA(sentence.c_str ());
				Serial.println(sentence.c_str());
			} else if (sentence.find("$GPGSA,") == 0) {
				if (sentence.length() >= 100)
					sentence = sentence.substr(0, 99);
				droneExportData.droneGPSData.SetGPGSA(sentence.c_str());
				Serial.println(sentence.c_str());
			} else if (sentence.find("$GPRMC,") == 0) {
				if (sentence.length() >= 100)
					sentence = sentence.substr(0, 99);
				droneExportData.droneGPSData.SetGPRMC(sentence.c_str());
				Serial.println(sentence.c_str());
			}
			sentence.clear();
		}
		else {
			if (sentence.length () < 200)
				sentence += ch;
		}
		//Serial.write(ch);
	}
}
