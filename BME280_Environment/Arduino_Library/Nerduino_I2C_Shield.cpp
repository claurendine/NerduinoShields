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


#include "Nerduino_I2C_Shield.h"
#include <Wire.h>

I2C_Shield::I2C_Shield(int8_t ioPin, uint8_t address)
{
    m_ioPin = ioPin;
    m_address = address;
    begin();
}

bool I2C_Shield::begin()
{
    Wire.begin();
}

int8_t I2C_Shield::read8(uint8_t reg)
{
    return (int8_t) readU8(reg);
}

uint8_t I2C_Shield::readU8(uint8_t reg)
{
    Wire.beginTransmission(m_address);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom(m_address, (int8_t) 1);
    uint8_t value = Wire.read();
    Wire.endTransmission();
    
    return value;
}

int16_t I2C_Shield::read16(uint8_t reg)
{
    return (int16_t) readU16(reg);
}

uint16_t I2C_Shield::readU16(uint8_t reg)
{
    Wire.beginTransmission(m_address);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom(m_address, (int8_t) 2);
    uint16_t value = (Wire.read() << 8) | Wire.read();
    Wire.endTransmission();
    
    return value;
}


int16_t I2C_Shield::read16LE(uint8_t reg)
{
    return (int16_t) readU16LE(reg);
}

uint16_t I2C_Shield::readU16LE(uint8_t reg)
{
    Wire.beginTransmission(m_address);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom(m_address, (int8_t) 2);
    uint16_t value = Wire.read() | (Wire.read() << 8);
    Wire.endTransmission();
    
    return value;
}


void I2C_Shield::write(uint8_t reg, int8_t value)
{
    write(reg, (uint8_t) value);
}

void I2C_Shield::write(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(m_address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

void I2C_Shield::write(uint8_t reg, int16_t value)
{
    write(reg, (uint16_t) value);
}

void I2C_Shield::write(uint8_t reg, uint16_t value)
{
    Wire.beginTransmission(m_address);
    Wire.write(reg);
    Wire.write((value >> 8) & 0xFF);
    Wire.endTransmission();
    
    Wire.beginTransmission(m_address);
    Wire.write(reg + 1);
    Wire.write(value & 0xFF);
    Wire.endTransmission();

}

