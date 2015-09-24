/****************************************************************
 ReadSHT2x
 An example sketch that reads the sensor and prints the
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
  Serial.print("Humidity(%RH): ");
  Serial.print(SHT2x.GetHumidity(),1);
  Serial.print("\tTemperature(C): ");
  Serial.print(SHT2x.GetTemperature(),1);

  uint32_t stop = micros();
  Serial.print("\tRead Time: ");
  Serial.println(stop - start);
  delay(1000);
}
