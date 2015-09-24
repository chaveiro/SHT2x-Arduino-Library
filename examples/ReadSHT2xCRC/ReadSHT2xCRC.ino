/****************************************************************
 ReadSHT2x
 An example sketch that reads the sensor, validates CRC and prints the
 relative humidity to the PC's serial port
  
 Hardware Setup: 
 Attach the SDA pin to A4, SCL to A5. 
 
    
 Copyright (C) 2015  Nuno Chaveiro  nchaveiro[at]gmail.com  Lisbon, Portugal
***************************************************************/

#include <Wire.h>
#include "SHT2x.h"

SHT2x SHT2x;

void setup()
{
  SHT2x.begin();
  Serial.begin(9600);
}

void loop()
{
  uint32_t start = micros();
  
  uint16_t rawh = SHT2x.readSensor(TRIGGER_HUMD_MEASURE_HOLD);
  uint16_t rawt = SHT2x.readSensor(TRIGGER_TEMP_MEASURE_HOLD);

  if (rawh <= ERROR_TIMEOUT) {
        shtCheckOk(rawh);
  } else if (rawt <= ERROR_TIMEOUT) {
        shtCheckOk(rawt);
  } else { 
        float lastHumidity = (-6.0 + 125.0 / 65536.0 * (float)(rawh));       // Humidity
        float lastTemperature = (-46.85 + 175.72 / 65536.0 * (float)(rawt)); // Temperature
        Serial.print("Humidity(%RH): ");
        Serial.print(lastHumidity);
        Serial.print("\tTemperature(C): ");
        Serial.print(lastTemperature);
  }
  uint32_t stop = micros();
  Serial.print("\tRead Time: ");
  Serial.println(stop - start);
  delay(1000);
}

void shtCheckOk(int val) {
    switch (val)
    {
      case ERROR_CRC:
        Serial.println(PSTR("SHT: CRC error"));
        break;
      case ERROR_TIMEOUT:
        Serial.println(PSTR("SHT: Timeout"));
        break;
    }
}