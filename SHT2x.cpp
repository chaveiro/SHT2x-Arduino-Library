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

#include <Wire.h>
#include "SHT2x.h"

SHT2x::SHT2x()
{
  //Set initial values for private vars
}

/******************************************************************************
 * Global Functions
 ******************************************************************************/
void SHT2x::begin(void)
{
  Wire.begin();
}
 
/**********************************************************
 * GetHumidity
 *  Gets the current humidity from the sensor.
 *
 * @return float - The relative humidity in %RH
 **********************************************************/
float SHT2x::GetHumidity(void)
{
    return (-6.0 + 125.0 / 65536.0 * (float)(readSensor(TRIGGER_HUMD_MEASURE_HOLD)));
}

/**********************************************************
 * GetTemperature
 *  Gets the current temperature from the sensor.
 *
 * @return float - The temperature in Deg C
 **********************************************************/
float SHT2x::GetTemperature(void)
{
    return (-46.85 + 175.72 / 65536.0 * (float)(readSensor(TRIGGER_TEMP_MEASURE_HOLD)));
}

/**********************************************************
 * Sets the sensor resolution to one of four levels:
 *  0/0 = 12bit RH, 14bit Temp
 *  0/1 = 8bit RH, 12bit Temp
 *  1/0 = 10bit RH, 13bit Temp
 *  1/1 = 11bit RH, 11bit Temp
 *  Power on default is 0/0
 **********************************************************/
 void SHT2x::setResolution(uint8_t resolution)
{
  uint8_t userRegister = read_user_register(); //Go get the current register state
  userRegister &= B01111110; //Turn off the resolution bits
  resolution &= B10000001; //Turn off all other bits but resolution bits
  userRegister |= resolution; //Mask in the requested resolution bits
  
  //Request a write to user register
  Wire.beginTransmission(HTDU21D_ADDRESS);
  Wire.write(WRITE_USER_REG); //Write to the user register
  Wire.write(userRegister); //Write the new resolution bits
  Wire.endTransmission();
}

/**********************************************************
 * Read the user register
 **********************************************************/
uint8_t SHT2x::read_user_register(void)
{
  uint8_t userRegister;
  
  //Request the user register
  Wire.beginTransmission(HTDU21D_ADDRESS);
  Wire.write(READ_USER_REG); //Read the user register
  Wire.endTransmission();
  
  //Read result
  Wire.requestFrom(HTDU21D_ADDRESS, 1);
  
  userRegister = Wire.read();

  return(userRegister);  
}


/******************************************************************************
 * Private Functions
 ******************************************************************************/

uint16_t SHT2x::readSensor(uint8_t command)
{
    uint16_t result;

    Wire.beginTransmission(HTDU21D_ADDRESS); //begin
    Wire.write(command);                     //send the pointer location
    Wire.endTransmission();                  //end
    
    //Hang out while measurement is taken. 50mS max, page 4 of datasheet.
    delay(55);

    //Comes back in three bytes, data(MSB) / data(LSB) / Checksum
    Wire.requestFrom(HTDU21D_ADDRESS, 3);
    
    //Wait for data to become available
    int counter = 0;
    while(Wire.available() < 3)
    {
        counter++;
        delay(1);
        if(counter > 100) return ERROR_TIMEOUT; //Error timout
    }

    //Store the result
    result = ((Wire.read()) << 8);
    result |= Wire.read();
    //Check validity
    uint8_t checksum = Wire.read();
    if(check_crc(result, checksum) != 0) return(ERROR_CRC); //Error checksum
    //sensorStatus = rawTemperature & 0x0003; //get status bits
    result &= ~0x0003;   // clear two low bits (status bits)
    return result;
}


//Give this function the 2 byte message (measurement) and the check_value byte from the HTU21D
//If it returns 0, then the transmission was good
//If it returns something other than 0, then the communication was corrupted
//From: http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html
//POLYNOMIAL = 0x0131 = x^8 + x^5 + x^4 + 1 : http://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
#define SHIFTED_DIVISOR 0x988000 //This is the 0x0131 polynomial shifted to farthest left of three bytes

uint8_t SHT2x::check_crc(uint16_t message_from_sensor, uint8_t check_value_from_sensor)
{
  uint32_t remainder = (uint32_t)message_from_sensor << 8; // Pad with 8 bits because we have to add in the check value
  remainder |= check_value_from_sensor;                    // Add on the check value
  uint32_t divsor = (uint32_t)SHIFTED_DIVISOR;

  for (int i = 0 ; i < 16 ; i++) //Operate on only 16 positions of max 24. The remaining 8 are our remainder and should be zero when we're done.
  {
    if( remainder & (uint32_t)1<<(23 - i) ) //Check if there is a one in the left position
      remainder ^= divsor;
    divsor >>= 1; //Rotate the divsor max 16 times so that we have 8 bits left of a remainder
  }
  return (uint8_t)remainder;
}


