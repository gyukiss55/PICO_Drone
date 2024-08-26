
// MotorControl

#include <Arduino.h>

#include <string>

#define RESOLUTION 65535
#define PWMFREQ     100
#define MAPMIN   (65535 / 10)
#define MAPMAX   ((65535 * 2) / 10)

// Define the ESC signal pin
const int escPin1 = 10; // GPIO 10 on Raspberry Pi Pico
const int escPin2 = 11; // GPIO 11 on Raspberry Pi Pico
const int escPin3 = 12; // GPIO 12 on Raspberry Pi Pico
const int escPin4 = 13; // GPIO 13 on Raspberry Pi Pico

void setupMotorControl()
{
    // Set up the ESC pin as a PWM output
    pinMode(escPin1, OUTPUT);
    pinMode(escPin2, OUTPUT);
    pinMode(escPin3, OUTPUT);
    pinMode(escPin4, OUTPUT);

    // Initialize the PWM signal
    analogWriteFreq(PWMFREQ); // Set frequency to 50 Hz (20 ms period)
    analogWriteRange(65535);
    analogWriteResolution(16);

    // Arm the ESC (send the minimum throttle signal)
    analogWrite(escPin1, map(1000, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
    analogWrite(escPin2, map(1000, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
    analogWrite(escPin3, map(1000, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
    analogWrite(escPin4, map(1000, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
    delay(1000); // Wait 1 second
}

void SetSpeedMotor(uint32_t speed1, uint32_t speed2, uint32_t speed3, uint32_t speed4)
{
    analogWrite(escPin1, map(speed1, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
    analogWrite(escPin2, map(speed2, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
    analogWrite(escPin3, map(speed3, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
    analogWrite(escPin4, map(speed4, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
}

void SetSpeedMotor(uint32_t motorIndex, uint32_t speed)
{
    if (motorIndex >= 1 && motorIndex <= 4)
        analogWrite(escPin1 + motorIndex - 1, map(speed, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
}

void loopMotorControl() 
{
    const uint32_t minSpeed = 1000;
    const uint32_t maxSpeed = 1300;
    const uint32_t step = 10;
    static uint32_t lastTS = 0;
    static uint32_t speed = 1000;
    static uint32_t state = 0;

    uint32_t currentTS = millis();

    if (currentTS - lastTS < 20)
        return;

    lastTS = currentTS;
    // Increase throttle
    if (state == 0) {

        analogWrite(escPin1, map(speed, 1000, 2000, MAPMIN, MAPMAX));
        speed+=step;
        if (speed >= maxSpeed) {
            state = 1;
        }
    }
    else {

        analogWrite(escPin1, map(speed, 1000, 2000, MAPMIN, MAPMAX));
        speed-=step;
        if (speed <= minSpeed) {
            state = 0;
        }
    }
}

void setup() {
  Serial.begin(115200);
  setupMotorControl ();
}

void loop()
{
  // put your main code here, to run repeatedly:
    std::string command;
    while (Serial.available ()) {
        char ch = Serial.read();
        command += ch;
    }
    if (command.length () > 0) {    
        int speed = std::stoi(command);
        if (speed >= 1000 && speed <=2000) {
            Serial.print ("speed:");
            Serial.println (speed);
            int motor = 1;
            if (command.find('b') < command.length ())
              motor=2;
            else if (command.find('c') < command.length ())
              motor=3;
            else if (command.find('d') < command.length ())
              motor=4;
            SetSpeedMotor(motor, speed);
        } else
            Serial.println ("Invalid speed!");
    }
}
