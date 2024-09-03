// MotorControl

#include <Arduino.h>
#include "MotorControl.h"

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

const int blme1 = 9; // GPIO 9 on Raspberry Pi Pico
const int blme2 = 8; // GPIO 8 on Raspberry Pi Pico
const int blme3 = 7; // GPIO 7 on Raspberry Pi Pico
const int blme4 = 6; // GPIO 6 on Raspberry Pi Pico

int motorPrev = 4;
int speedPrev = 0;

#define EnVectorSize     10

uint32_t encoderTSvector[4][EnVectorSize];
uint32_t encoderTSindex[4] = { 0,0,0,0 };


void ReadEncoder(uint32_t ei)
{
    encoderTSvector[ei][encoderTSindex[ei]] = micros ();
    encoderTSindex[ei]++;
    if (encoderTSindex[ei] >= EnVectorSize)
        encoderTSindex[ei] = 0;

}
void ReadEncoder4()
{
    ReadEncoder(3);
}
void ReadEncoder3()
{
    ReadEncoder(2);
}
void ReadEncoder2()
{
    ReadEncoder(1);
}
void ReadEncoder1()
{
    ReadEncoder(0);
}


void setupMotorControl()
{
    // Set up the ESC pin as a PWM output
    pinMode(escPin1, OUTPUT);
    pinMode(escPin2, OUTPUT);
    pinMode(escPin3, OUTPUT);
    pinMode(escPin4, OUTPUT);

    pinMode(escPin1, INPUT);
    pinMode(escPin2, INPUT);
    pinMode(escPin3, INPUT);
    pinMode(escPin4, INPUT);

    // Initialize the PWM signal
    analogWriteFreq(PWMFREQ); // Set frequency to 50 Hz (20 ms period)
    analogWriteRange(65535);
    analogWriteResolution(16);

    // Arm the ESC (send the minimum throttle signal)
    analogWrite(escPin1, map(1000, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
    analogWrite(escPin2, map(1000, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
    analogWrite(escPin3, map(1000, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)
    analogWrite(escPin4, map(1000, 1000, 2000, MAPMIN, MAPMAX)); // 1 ms pulse (min throttle)

    attachInterrupt(blme4, ReadEncoder4, RISING);
    attachInterrupt(blme3, ReadEncoder3, RISING);
    attachInterrupt(blme2, ReadEncoder2, RISING);
    attachInterrupt(blme1, ReadEncoder1, RISING);

    for (uint32_t i = 0; i < 4; ++i) {
        encoderTSindex[i] = 0;
        for (uint32_t j = 0; j < EnVectorSize; ++j) {
            encoderTSvector[i][j] = 0;
        }
    }

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

void loopMotorControl1()
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
        speed += step;
        if (speed >= maxSpeed) {
            state = 1;
        }
    }
    else {

        analogWrite(escPin1, map(speed, 1000, 2000, MAPMIN, MAPMAX));
        speed -= step;
        if (speed <= minSpeed) {
            state = 0;
        }
    }
}

void SetSpeedAndPrint(int motor, int speed)
{
    SetSpeedMotor(motor, speed);
    Serial.print("speed:");
    Serial.println(speed);

    motorPrev = motor;
    speedPrev = speed;
}

void loopMotorControl()
{
    static uint32_t lastTS = 0;
    uint32_t v[EnVectorSize];
    uint32_t currTS = millis();
    if (currTS - lastTS > 1000) {
        lastTS = currTS;
        for (uint32_t i = 0; i < 4; ++i) {
            uint32_t k1 = encoderTSindex[i];
            uint32_t k2 = k1 + 1;
            if (k2 >=  EnVectorSize) {
              k2 = 0;
            } 
            if (encoderTSvector[i][k2] > encoderTSvector[i][k1]) {
              for (uint32_t j = 0; j < EnVectorSize - 1; ++j) {
                v[j] = encoderTSvector[i][k2] - encoderTSvector[i][k1];
                k1 = k2;
                k2 = k1 + 1;
                if (k2 >=  EnVectorSize) {
                  k2 = 0;
                } 
              }
              for (uint32_t j = 0; j < EnVectorSize - 1; ++j) {
                if (j > 0)
                  Serial.print(", ");
                Serial.print(v[j]);

              }
              Serial.println("");
            }
        }
    }

    std::string command;
    while (Serial.available()) {
        char ch = Serial.read();
        command += ch;
    }
    if (command.length() > 0) {
        if (command[0] == '1') {
            int speed = std::stoi(command);
            if (speed >= 1000 && speed <= 2000) {
                int motor = 4;
                if (command.find('a') < command.length())
                    motor = 1;
                else if (command.find('b') < command.length())
                    motor = 2;
                else if (command.find('c') < command.length())
                    motor = 3;
                else if (command.find('d') < command.length())
                    motor = 4;
                SetSpeedAndPrint(motor, speed);
            }
        } else if (command[0] == '+') {
            SetSpeedAndPrint(motorPrev, speedPrev + 1);
        } else if (command[0] == '-') {
            SetSpeedAndPrint(motorPrev, speedPrev - 1);
        } else if (command[0] == '*') {
            SetSpeedAndPrint(motorPrev, speedPrev + 10);
        } else if (command[0] == '/') {
            SetSpeedAndPrint(motorPrev, speedPrev - 10);
        }
        else
            Serial.println("Invalid speed!");
    }


}
