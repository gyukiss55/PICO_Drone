/* MPU_6500_Drive.cpp */
#include <Arduino.h>
#include <Wire.h>
//#include <MPU6050.h>  // Use MPU9250.h if you have MPU-9250
#include <MPU9250_asukiaaa.h>

#include "MPU_6500_Drive.h" 
#include "DroneSensorData.h"
#include "DebugLog.h"

//MPU6050 mpu;  // Create an object for the MPU6050/9250
MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;
uint8_t sensorId;

// pico
#define   I2C0SDA_PIN 20
#define   I2C0SCL_PIN 21

//#define _CALIBRATE_MAG_SENSOR_

#if defined (_CALIBRATE_MAG_SENSOR_)
#define CALIB_SEC 20
#endif

#if defined (_CALIBRATE_MAG_SENSOR_)
void setMagMinMaxAndSetOffset(MPU9250_asukiaaa* sensor, int seconds)
{
    unsigned long calibStartAt = millis();
    float magX, magXMin, magXMax, magY, magYMin, magYMax, magZ, magZMin, magZMax;

    sensor->magUpdate();
    magXMin = magXMax = sensor->magX();
    magYMin = magYMax = sensor->magY();
    magZMin = magZMax = sensor->magZ();

    while (millis() - calibStartAt < (unsigned long)seconds * 1000) {
        delay(100);
        sensor->magUpdate();
        magX = sensor->magX();
        magY = sensor->magY();
        magZ = sensor->magZ();
        if (magX > magXMax) magXMax = magX;
        if (magY > magYMax) magYMax = magY;
        if (magZ > magZMax) magZMax = magZ;
        if (magX < magXMin) magXMin = magX;
        if (magY < magYMin) magYMin = magY;
        if (magZ < magZMin) magZMin = magZ;
    }


    sensor->magXOffset = -(magXMax + magXMin) / 2;
    sensor->magYOffset = -(magYMax + magYMin) / 2;
    sensor->magZOffset = -(magZMax + magZMin) / 2;
}
#endif


void setupMPU_6500_Drive() 
{
    // Start the I2C communication
    Wire.setSDA(I2C0SDA_PIN);
    Wire.setSCL(I2C0SCL_PIN);
    Wire.begin();

    mySensor.setWire(&Wire);

    mySensor.beginAccel();
    mySensor.beginGyro();
    mySensor.beginMag();

#if defined (_CALIBRATE_MAG_SENSOR_)
    Serial.println("Start scanning values of magnetometer to get offset values.");
    Serial.println("Rotate your device for " + String(CALIB_SEC) + " seconds.");
    setMagMinMaxAndSetOffset(&mySensor, CALIB_SEC);
    Serial.println("Finished setting offset values.");
#else
    // You can set your own offset for mag values
    mySensor.magXOffset = -23;
    mySensor.magYOffset = -60;
    mySensor.magZOffset = 167;
#endif


    Serial.println("MPU6050/9250 ready!");
}

bool Measure_MPU_6500_Drive()
{
    static uint32_t lastTimeTick = 0;

    uint8_t sensorId;
    int result;

    uint32_t currentTimeTick = millis();
    if (currentTimeTick - lastTimeTick < 100) {
        return false;
    }
    lastTimeTick = currentTimeTick;

    droneSensorDataCurrent = DroneSensorData(currentTimeTick);
//  Serial.println("ts0 " + String(droneSensorDataCurrent.GetTimeStamp ()) + " ms"); // debug log

#if defined SERIAL_DEBUG_LOG_LEVEL
    if (CheckLogStateSerial(DEBUG_ID_3) > 0) {
        Serial.println("at " + String(currentTimeTick) + "ms");
    }
#endif // SERIAL_DEBUG_LOG_LEVEL

    result = mySensor.readId(&sensorId);
    if (result == 0) {
        //Serial.println("sensorId: " + String(sensorId));
    } else {
        Serial.println("Cannot read sensorId " + String(result));
    }

    result = mySensor.accelUpdate();
    if (result == 0) {
        droneSensorDataCurrent.SetAccel(mySensor.accelX(), mySensor.accelY(), mySensor.accelZ());
        aX = mySensor.accelX();
        aY = mySensor.accelY();
        aZ = mySensor.accelZ();
        aSqrt = mySensor.accelSqrt();

#if defined SERIAL_DEBUG_LOG_LEVEL
        if (CheckLogStateSerial(DEBUG_ID_4) > 0) {
            Serial.print("accel: " + String(aX));
            Serial.print(", " + String(aY));
            Serial.println(", " + String(aZ));
        //Serial.println("accelSqrt: " + String(aSqrt));
        }
#endif // SERIAL_DEBUG_LOG_LEVEL
    } else {
        Serial.println("Cannod read accel values " + String(result));
    }

    result = mySensor.gyroUpdate();
    if (result == 0) {
        droneSensorDataCurrent.SetGyro(mySensor.gyroX(), mySensor.gyroY(), mySensor.gyroZ());
        gX = mySensor.gyroX();
        gY = mySensor.gyroY();
        gZ = mySensor.gyroZ();
#if defined SERIAL_DEBUG_LOG_LEVEL
        if (CheckLogStateSerial(DEBUG_ID_5) > 0) {
            Serial.print("gyro: " + String(gX));
            Serial.print(", " + String(gY));
            Serial.println(", " + String(gZ));
        }
#endif // SERIAL_DEBUG_LOG_LEVEL
    } else {
        Serial.println("Cannot read gyro values " + String(result));
    }

    result = mySensor.magUpdate();
    if (result != 0) {
        Serial.println("cannot read mag so call begin again");
        mySensor.beginMag();
        result = mySensor.magUpdate();
    }
    if (result == 0) {
        droneSensorDataCurrent.SetMag(mySensor.magX(), mySensor.magY(), mySensor.magZ());
        droneSensorDataCurrent.SetHorizontalDir(mySensor.magHorizDirection());
        mX = mySensor.magX();
        mY = mySensor.magY();
        mZ = mySensor.magZ();
        mDirection = mySensor.magHorizDirection();
#if defined SERIAL_DEBUG_LOG_LEVEL
        if (CheckLogStateSerial(DEBUG_ID_6) > 0) {
#if defined (_CALIBRATE_MAG_SENSOR_)
            Serial.println("mySensor.magXOffset = " + String(mySensor.magXOffset) + ";");
            Serial.println("mySensor.maxYOffset = " + String(mySensor.magYOffset) + ";");
            Serial.println("mySensor.magZOffset = " + String(mySensor.magZOffset) + ";");
#endif

            Serial.print("mag: " + String(mX));
            Serial.print(", " + String(mY));
            Serial.println(", " + String(mZ));
            Serial.println("horizontal direction: " + String(mDirection));
        }
#endif // SERIAL_DEBUG_LOG_LEVEL

    } else {
        Serial.println("Cannot read mag values " + String(result));
    }

#if defined SERIAL_DEBUG_LOG_LEVEL
    if (CheckLogStateSerial(DEBUG_ID_7) > 0) {
        Serial.println(""); // Add an empty line
    }
#endif // SERIAL_DEBUG_LOG_LEVEL

    return true;
}
