//MotorControl.h
void setupMotorControl();

void SetSpeedMotor(uint32_t speed1, uint32_t speed2, uint32_t speed3, uint32_t speed4);

void SetSpeedMotor(uint32_t motorIndex, uint32_t speed);

void loopMotorControl();