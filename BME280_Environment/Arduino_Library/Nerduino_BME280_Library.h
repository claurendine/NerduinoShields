/**************************************
 Nerduino BME280 Shield
 Library for Arduino

 Bosch Combined humidity and pressure sensor
 http://www.nerduino.com/Shields/BME280/bme280.html
 
 Sensor Capabilities:
 
 Summary of the sensor's capability
 
 Sensor Configuration:
 
 Summary of the shield's configuration of the chip
 
 Notes for alternate platforms
 
 Nerduino plug
 
 Licensing info  BSD?  check adafruint and sparkfun to see if they are standardized on this
**************************************/

#include "Nerduino_I2C_Shield.h"

#define BME280_T1 0x88
#define BME280_T2 0x8A
#define BME280_T3 0x8C

#define BME280_P1 0x8E
#define BME280_P2 0x90
#define BME280_P3 0x92

#define BME280_P4 0x94
#define BME280_P5 0x96
#define BME280_P6 0x98

#define BME280_P7 0x9A
#define BME280_P8 0x9C
#define BME280_P9 0x9E

#define BME280_H1 0xA1
#define BME280_H2 0xE1
#define BME280_H3 0xE3
#define BME280_H4 0xE4
#define BME280_H5 0xE5
#define BME280_H6 0xE7

#define BME280_CHIPID 0xD0
#define BME280_VERSION 0xD1
#define BME280_RESET 0xE0

#define BME280_CONTROL_HUMIDITY 0xF2
#define BME280_CONTROL 0xF4
#define BME280_CONFIG 0xF5

#define BME280_TEMP 0xFA
#define BME280_PRESSURE 0xF7
#define BME280_HUMIDITY 0xFD





class BME280
{
public:
    BME280(); // using shield default io pin for sda
    BME280(uint8_t scl, uint8_t sda);
    
    float readTemperature();
    float readPressure();
    float readHumidity();
    
    void configure(uint8_t tempOversampling, uint8_t pressureOversampling, uint8_t humidityOversampling, uint8_t mode);
    // Oversampling settings
    // 0 - off
    // 1 - 1x
    // 2 - 2x
    // 3 - 4x
    // 4 - 8x
    // 5 - 16x

    // Mode settings
    // 0 - Sleep
    // 1 - Forced
    // 2 - Forced
    // 3 - Normal
    

    bool isDetected();
    
private:
    void initialize();
    void applySampling();
    
    int8_t humidityOversampling;
    int8_t temperatureOversampling;
    int8_t pressureOversampling;
    
    I2C_Shield i2c;
    
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    
    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
    
    int32_t t_fine;
};

