// MotorControl

#include <Arduino.h>
#include "MotorControl.h"

#include <string>

#define RESOLUTION 65535
#define PWMFREQ     100
#define MAPMIN   (65535 / 10)
#define MAPMAX   ((65535 * 3) / 20)

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

#define EnVectorSize     100

uint32_t encoderTSvector[4][EnVectorSize];
int32_t encoderDTSvector[4][EnVectorSize];
uint32_t encoderTSindex[4] = { 0,0,0,0 };

int32_t encoderDTSvectorTmp[EnVectorSize];

void ReadEncoder(uint32_t ei, int level)
{
    encoderTSvector[ei][encoderTSindex[ei]] = micros ();

    uint32_t prev = EnVectorSize - 1;
    if (encoderTSindex[ei] > 0)
        prev = encoderTSindex[ei] - 1;

    int32_t sign = 1;
    if (!level)
        sign = -1;

    encoderDTSvector[ei][encoderTSindex[ei]] = (encoderTSvector[ei][encoderTSindex[ei]] - encoderTSvector[ei][prev]) * sign;

    encoderTSindex[ei]++;
    if (encoderTSindex[ei] >= EnVectorSize)
        encoderTSindex[ei] = 0;

}

void ReadEncoder4()
{

    ReadEncoder(3, digitalRead (blme4));
}

void ReadEncoder3()
{
    ReadEncoder(2, digitalRead(blme3));
}

void ReadEncoder2()
{
    ReadEncoder(1, digitalRead(blme2));
}

void ReadEncoder1()
{
    ReadEncoder(0, digitalRead(blme1));
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

    attachInterrupt(blme4, ReadEncoder4, FALLING);
    attachInterrupt(blme3, ReadEncoder3, FALLING);
    attachInterrupt(blme2, ReadEncoder2, FALLING);
    attachInterrupt(blme1, ReadEncoder1, FALLING);

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

uint32_t Previous(uint32_t i) {
    if (i > 0)
        i--;
    else
        i = EnVectorSize - 1;
    return i;
}

uint32_t EvaluateEncoder(int channel)
{
    const uint32_t NEGMAXMIN = 100;
    const uint32_t NEGMAXMAX = 1000;

    if (channel < 0 || channel > 3)
        return 0;
    //uint32_t nowTs = micros();
    uint32_t prev = EnVectorSize - 1;
    uint32_t next = encoderTSindex[channel];
    if (next > 0)
        prev = next - 1;

    uint32_t tsCurrent = micros();
    uint32_t tsEnd = encoderTSvector[channel][prev];
    if (tsCurrent < tsEnd || (tsCurrent - tsEnd) > 1000000) // 1sec
        return 0;

    //uint32_t posPeriod = 0;
    //uint32_t negPeriod = 0;
    uint32_t posPeriodSum = 0;
    uint32_t negPeriodSum = 0;
    uint32_t posPeriod1 = 0;
    uint32_t negPeriod1 = 0;
    uint32_t negMax = 0;


    uint32_t prev1 = prev;
    uint32_t prev2 = prev;
    int num = 0;
    for (int i = 0; i < EnVectorSize / 2; i++) {
        int32_t cdts = encoderDTSvector[channel][prev];
        uint32_t cdtsAbs = 0;
        if (cdts < 0) {
            cdtsAbs = -cdts;
        }
        else
            cdtsAbs = cdts;
        if (negMax < cdtsAbs) {
            negMax = cdtsAbs;
            prev2 = prev;
        }

        num++;

        prev1 = prev;
        prev = Previous(prev);

        if (num > 20 && negMax >= NEGMAXMIN)
            break;
    }
    if (negMax > NEGMAXMAX)
        return 0;

    if (negMax < NEGMAXMIN)
        return 0;

    negPeriod1 = negMax;
    negPeriodSum += negMax;

    uint32_t result = 0;

    prev = Previous(prev2);

    for (int i = 0; i < EnVectorSize / 2; i++) {
        int32_t cdts = encoderDTSvector[channel][prev];
        uint32_t cdtsAbs = 0;
        if (cdts < 0) {
            cdtsAbs = -cdts;
        }
        else {
            cdtsAbs = cdts;
        }
        posPeriod1 = cdtsAbs;
        posPeriodSum += cdtsAbs;
        if (negPeriodSum * 9 < posPeriod1 * 10) {
            result = negPeriodSum * 2;
            break;
        } else if (negPeriodSum * 9 < posPeriodSum * 10) {
            result = negPeriodSum * 2;
            break;
        }
        prev = Previous(prev);
    }
    {
        for (uint32_t i = 0; i < EnVectorSize; i++) {
            encoderDTSvectorTmp[i] = encoderDTSvector[channel][i];
        }
        Serial.print("EvaluateEncoder:");
        Serial.print(channel);
        Serial.print(", i1:");
        Serial.print(prev1);
        Serial.print(", i2:");
        Serial.print(prev);
        for (uint32_t i = prev1; i != prev; ) {
            Serial.print(".");
            Serial.print(encoderDTSvectorTmp[i]);

            i = Previous(i);
        }
        Serial.println("");
    }
    return result;
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
    if (currTS - lastTS > 2000) {
        lastTS = currTS;

        for (uint32_t i = 0; i < 4; ++i) {
            uint32_t period = EvaluateEncoder(i);
            if (period > 0) {
                Serial.print("ts:");
                Serial.print(currTS);
                Serial.print("ch:");
                Serial.print(i + 1);
                Serial.print("period:");
                Serial.println(period);

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
