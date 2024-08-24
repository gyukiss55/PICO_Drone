// MeasureDistance_HC_SR04.cpp

/*
 * HC-SR04 example sketch
 *
 * https://create.arduino.cc/projecthub/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-036380
 *
 * by Isaac100
 */
#include <Arduino.h>
#include <string>

#include "MeasureDistance_HC_SR04.h"

#include "DeviceUART2.h"
#include "DroneSensorData.h"
#include "DebugLog.h"

const int trigPin = 27;
const int echoPin = 26;
const int DistanceVectorSize = 10;

float distanceVector[DistanceVectorSize];
float minDist, maxDist, avrDist;

void setupHC_SR04()
{
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
}

float MeasureDistance()
{
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);

	float duration = pulseIn(echoPin, HIGH, 12000L);
	float distance = (duration * .0343) / 2;

#if defined SERIAL2_DEBUG_LOG_LEVEL
	if (CheckLogStateSerial2() > 0) {
		Serial2.print(millis());
		Serial2.print(" duration: ");
		Serial2.print(duration);
		Serial2.print(", distance: ");
		Serial2.println(distance);
	}
#endif // defined SERIAL2_DEBUG_LOG_LEVEL

	return distance;
}

bool Measure_HC_SR04()
{
	/*
	static uint32_t lastTimeTick = 0;

	uint32_t currentTimeTick = millis();
	if (currentTimeTick - lastTimeTick < 250) {
		return;
	}
	lastTimeTick = currentTimeTick;
	*/
	int nr = 0;
	for (int i = 0; i < DistanceVectorSize; i++) {
		float distance = MeasureDistance();
		if (distance < 1.0)
			continue;
		distanceVector[nr++] = distance;
		if (nr == 0) {
			avrDist = maxDist = minDist = distance;
		}
		else {
			if (distance < minDist) {
				minDist = distance;
			}
			if (distance > maxDist) {
				maxDist = distance;
			}
			avrDist += distance;
		}
	}
	if (nr == 0) {

		static int cnt = 0;

		cnt++;
		if (cnt > 100) {
			cnt = 0;
			Serial.println("US distance measure failed!");
			Serial2.println("US distance measure failed!");
		}
		return false;
	}

	avrDist = avrDist / nr;

	droneSensorDataCurrent.SetElevationUS(avrDist);

	std::string log = std::to_string(millis ()) + " -Distance: " + std::to_string(avrDist);
	log += ", min:" + std::to_string(minDist);
	log += ", max:" + std::to_string(maxDist) + "[";
	for (int i = 0; i < nr; i++) {
		if (i > 0)
			log += ", ";
		log += std::to_string(distanceVector[i]);
	}
	log += "]\r\n\r\n";
#if defined SERIAL_DEBUG_LOG_LEVEL
	if (CheckLogStateSerial(DEBUG_ID_2) > 0) {
		Serial.print(log.c_str());
	}
#endif // SERIAL_DEBUG_LOG_LEVEL
#if defined SERIAL_DEBUG_LOG_LEVEL
	if (CheckLogStateSerial2(true) > 0) {
		Serial2.println(log.c_str());
		Serial2.print(millis());
		Serial2.print(" -Distance: ");
		Serial2.println(avrDist);
	}
#endif // SERIAL_DEBUG_LOG_LEVEL

	return true;
}