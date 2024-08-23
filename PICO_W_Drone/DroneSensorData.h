// DroneSensorData.h

#include <Arduino.h>


class DroneSensorData {
public:

	enum DroneSensorDataEnum{
		NotValid = 0,
		AccelValid = 1,
		GyroValid = 2,
		MagValid = 4,
		ElevBaroValid = 8,
		ElevUSValid = 16,
		BatteryMotorValid = 32,
		BatteryMPUValid = 64
	};
private:
	uint32_t	structSize;

	uint32_t	timeStamp;

	float		accelX;
	float		accelY;
	float		accelZ;

	float		gyroX;
	float		gyroY;
	float		gyroZ;

	float		magX;
	float		magY;
	float		magZ;

	float		elevationBaro;
	float		elevationUS;
	float		batteryMotor;
	float		batteryMPU;

	uint32_t	status;

public:
	DroneSensorData()
		: 
		structSize (sizeof(DroneSensorData)), timeStamp (0),
		accelX(0), accelY(0), accelZ(0),
		gyroX(0), gyroY(0), gyroZ(0),
		magX(0), magY(0), magZ(0),
		elevationBaro(0), elevationUS(0), batteryMotor(0), batteryMPU(0),
		status (NotValid)
	{
	}
	DroneSensorData(uint32_t ts)
		: 
		structSize (sizeof(DroneSensorData)), timeStamp (ts),
		accelX(0), accelY(0), accelZ(0),
		gyroX(0), gyroY(0), gyroZ(0),
		magX(0), magY(0), magZ(0),
		elevationBaro(0), elevationUS(0), batteryMotor(0), batteryMPU(0),
		status (NotValid)
	{
	}

	void SetTimeStamp(uint32_t	st) { timeStamp = st; }

	void SetAccel(float x, float y, float z) {
		accelX = x; accelY = y;	accelZ = z; status |= AccelValid;
	}
	void SetGyro (float x, float y, float z) { 
		gyroX = x; gyroY = y;	gyroZ = z;  status |= GyroValid;
	}
	void SetMag  (float x, float y, float z) { 
		magX = x; magY = y; magZ = z; status |= MagValid;
	}
	void SetElevationBaro(float v) { elevationBaro = v; status |= ElevBaroValid; }
	void SetElevationUS(float v) { elevationUS = v; status |= ElevUSValid; }
	void SetBatteryMotor(float v) { batteryMotor = v; status |= BatteryMotorValid; }
	void SetBatteryMPU(float v) { batteryMPU = v; status |= BatteryMPUValid; }

	uint32_t GetSize() const { return structSize; }
	uint32_t GetTimeStamp() const { return timeStamp; }
	bool GetAccel(float& x, float& y, float& z) const { x = accelX; y = accelY; z = accelZ; return ((status & AccelValid) != 0); }
	bool GetGyro (float &x, float &y, float &z) const { x = gyroX; y = gyroY; z = gyroZ; return ((status & GyroValid) != 0);	}
	bool GetMag  (float &x, float &y, float &z) const { x = magX; y = magY; z = magZ; return ((status & MagValid) != 0);	}
	bool GetElevationBaro(float& v) const { v = elevationBaro; return ((status & ElevBaroValid) != 0); }
	bool GetElevationUS(float& v) const { v = elevationUS; return ((status & ElevUSValid) != 0); }
	bool GetBatteryMotor(float& v) const { v = batteryMotor; return ((status & BatteryMotorValid) != 0); }
	bool GetBatteryMPU(float& v) const { v = batteryMPU; return ((status & BatteryMPUValid) != 0); }
};

extern DroneSensorData droneSensorDataCurrent;

#define DroneSensorDataVectorSize 10 

extern uint32_t droneSensorDataVectorIndex;
extern DroneSensorData droneSensorDataVector1[DroneSensorDataVectorSize];
extern DroneSensorData droneSensorDataVector2[DroneSensorDataVectorSize];

void AddDroneSensorData(const DroneSensorData& data);
void CloneDroneSensorDataVector();
