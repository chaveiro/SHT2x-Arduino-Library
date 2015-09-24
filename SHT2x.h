/*
 SHT2x - Humidity Library for Arduino.
 Get humidity and temperature from the HTU21D/SHT2x sensors.
  
 Hardware Setup: 
 Attach the SDA pin to A4, SCL to A5. 
 
 Software:
 Call SHT2x.Begin() in setup.
 SHT2x.ReadHumidity() will return a float containing the humidity. Ex: 54.7
 SHT2x.ReadTemperature() will return a float containing the temperature in Celsius. Ex: 24.1
 SHT2x.SetResolution(byte: 0b.76543210) sets the resolution of the readings.
 
  Copyright (C) 2015  Nuno Chaveiro  nchaveiro[at]gmail.com  Lisbon, Portugal
*/

#if defined(ARDUINO) && ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#ifndef SHT2X_H
#define SHT2X_H

#define HTDU21D_ADDRESS             0x40  //Unshifted 7-bit I2C address for the sensor

#define TRIGGER_TEMP_MEASURE_HOLD   0xE3
#define TRIGGER_HUMD_MEASURE_HOLD   0xE5
#define TRIGGER_TEMP_MEASURE_NOHOLD 0xF3
#define TRIGGER_HUMD_MEASURE_NOHOLD 0xF5

#define WRITE_USER_REG              0xE6
#define READ_USER_REG               0xE7
#define SOFT_RESET                  0xFE

#define ERROR_CRC                   998
#define ERROR_TIMEOUT               999

class SHT2x
{
  public:
  //Public Functions
    SHT2x();
    void     begin();
    float    GetHumidity(void);
    float    GetTemperature(void);
    void     setResolution(uint8_t resBits);
    uint8_t  read_user_register(void);
	uint16_t readSensor(uint8_t command);
  private:
  //Private Functions
    
    uint8_t  check_crc(uint16_t message_from_sensor, uint8_t check_value_from_sensor);
};

#endif
