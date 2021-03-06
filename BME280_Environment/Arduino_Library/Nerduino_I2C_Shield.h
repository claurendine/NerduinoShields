/**************************************
 Nerduino I2C Shield
 Library for Arduino
 
 http://www.nerduino.com/Shields/I2C/i2c.html
 
 Class Capabilities:
 
 Summary of the sensor's capability
 
 Notes for alternate platforms
 
 Nerduino plug
 
 Licensing info  BSD?  check adafruint and sparkfun to see if they are standardized on this
 **************************************/

#include "Arduino.h"

#define SHIELD_SDA 11

class I2C_Shield
{
public:
    I2C_Shield(uint8_t address);
    I2C_Shield(uint8_t scl, uint8_t sda, uint8_t address);

    int8_t read8(uint8_t reg);
    uint8_t readU8(uint8_t reg);
    int16_t read16(uint8_t reg);
    uint16_t readU16(uint8_t reg);
    int16_t read16LE(uint8_t reg);
    uint16_t readU16LE(uint8_t reg);

    
    void write8(uint8_t reg, int8_t value);
    void writeU8(uint8_t reg, uint8_t value);
    void write16(uint8_t reg, int16_t value);
    void writeU16(uint8_t reg, uint16_t value);
    
protected:
    
    uint8_t m_ioPin;
    int8_t m_address;
};