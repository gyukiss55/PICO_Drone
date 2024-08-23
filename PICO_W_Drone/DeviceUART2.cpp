// DeviceUART2.cpp


/*
void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
  Serial2.begin(115200);
  while (!Serial)
    ; // Serial is via USB; wait for enumeration
}

void loop() {
  Serial.println("This is port \"Serial\"");
  Serial1.println("1111111111111\r\nThis is port \"Serial1\"");
  Serial2.println("2222222222222\r\nThis is port \"Serial2\"");

  if (Serial1.read() > 0) {  // read and discard data from UART0
    Serial.println("Input detected on UART0");
  }
  if (Serial2.read() > 0) {  // read and discard data from UART1
    Serial.println("Input on UART1");
  }
  delay(2000);
}
*/

#include <Arduino.h>

#include "DeviceUART2.h"

#define UART2RXPIN 9
#define UART2TXPIN 8

void setupDeviceUART2(uint32_t baudrate, const String& msg)
{
	// Initialize the Serial Monitor
	Serial2.setRX(UART2RXPIN);
	Serial2.setTX(UART2TXPIN);
	Serial2.setFIFOSize(256);
	Serial2.begin(baudrate);
	delay(2000);
	Serial1.println(msg);
	Serial.println (msg);
	delay (2000);
}

void loopDeviceUART2()
{
}
