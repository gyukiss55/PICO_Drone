/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-bme280-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  Adapted from the Adafruit BME280 examples: https://github.com/adafruit/Adafruit_BME280_Library
*********/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include  <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; // I2C Interface

void setup()  {
  Serial.begin(115200);
  Serial.println(F("BMP280 test"));

  if  (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor,  check wiring!"));
    while (1);
  }

  /* Default settings from datasheet.  */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500);  /* Standby time. */
}

void loop() {
    Serial.print(F("Temperature  = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure()/100);  //displaying the Pressure in hPa, you can change the unit
    Serial.println("  hPa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1019.66));  //The "1019.66" is the pressure(hPa) at sea level in day in your region
    Serial.println("  m");                    //If you don't know it, modify it until you get your current  altitude

    Serial.println();
    delay(2000);
}
