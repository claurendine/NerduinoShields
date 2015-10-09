#include "Nerduino_BME280_Library.h"


BME280::BME280() : i2c(0x77)
{
    // verify that the device is present
    if (isDetected())
    {
        // initialize the device
        initialize();
    }
}

BME280::BME280(uint8_t scl, uint8_t sda) :
    i2c(scl, sda, 0x77)
{
    // verify that the device is present
    if (isDetected())
    {
        // initialize the device
        initialize();
    }
}

bool BME280::isDetected()
{
    return (i2c.read8(BME280_CHIPID) == 0x60);
}

void BME280::initialize()
{
    dig_T1 = i2c.readU16LE(BME280_T1);
    dig_T2 = i2c.read16LE(BME280_T2);
    dig_T3 = i2c.read16LE(BME280_T3);
    
    dig_P1 = i2c.readU16LE(BME280_P1);
    dig_P2 = i2c.read16LE(BME280_P2);
    dig_P3 = i2c.read16LE(BME280_P3);
    dig_P4 = i2c.read16LE(BME280_P4);
    dig_P5 = i2c.read16LE(BME280_P5);
    dig_P6 = i2c.read16LE(BME280_P6);
    dig_P7 = i2c.read16LE(BME280_P7);
    dig_P8 = i2c.read16LE(BME280_P8);
    dig_P9 = i2c.read16LE(BME280_P9);

    dig_H1 = i2c.readU8(BME280_H1);
    dig_H2 = i2c.read16LE(BME280_H2);
    dig_H3 = i2c.readU8(BME280_H3);
    dig_H4 = (i2c.readU8(BME280_H4) << 4) | (i2c.readU8(BME280_H4+1) & 0xF);
    dig_H5 = (i2c.readU8(BME280_H5+1) << 4) | (i2c.readU8(BME280_H5) >> 4);
    dig_H6 = i2c.read8(BME280_H6);
    
    
    // enable control
    i2c.writeU8(BME280_CONTROL, 0x3F);
    
    // enable humidity
    i2c.writeU8(BME280_CONTROL_HUMIDITY, 0x03);
}

float BME280::readTemperature()
{
    int32_t adc_T = (((int32_t) i2c.readU16(BME280_TEMP)) << 8 | ((int32_t)i2c.readU8(BME280_TEMP + 2))) >> 4;
    
    int32_t var1, var2;
    
    var1 = ((((adc_T >> 3) - ((int32_t) dig_T1 << 1))) * ((int32_t) dig_T2)) >> 11;
    
    var2 = (((((adc_T >> 4) - ((int32_t) dig_T1)) * ((adc_T >> 4) - ((int32_t) dig_T1))) >> 12) * ((int32_t) dig_T3)) >> 14;
    
    t_fine = var1 + var2;
    
    float fT =(t_fine*5+128)>>8;
    
    fT /= 100.0f;
    
    return fT;
}

float BME280::readPressure()
{
    int32_t adc_P = ( ((int32_t)i2c.readU16(BME280_PRESSURE)) << 8 | ((int32_t) i2c.readU8(BME280_PRESSURE + 2))) >> 4;
    
    int64_t var1, var2, p;

    var1 = ((int64_t) t_fine) - 128000;
 
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
    var2 = var2 + (((int64_t)dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
    
    if (var1 == 0)
    {
        return 0.0f; // avoid exception caused by division by zero
    }
    
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
    
    float fP = ((uint32_t) p) / 256.0f;
    
    return fP;
}

float BME280::readHumidity()
{
    int32_t adc_H = i2c.readU16(BME280_HUMIDITY);
    
    int32_t v_x1_u32r;
    
    v_x1_u32r = t_fine - ((int32_t) 76800);
    
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) +
                   ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * (((v_x1_u32r *
                                                                            ((int32_t)dig_H3)) >> 11) +
                                                                            ((int32_t)32768))) >> 10) +
                                                                            ((int32_t)2097152)) *
                                                 ((int32_t)dig_H2) + 8192) >>14));
    
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t) dig_H1)) >> 4));
    
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    
    float fH = (float) (v_x1_u32r >> 12) / 1024.0f;
    
    return fH;
}

void BME280::configure(uint8_t temp, uint8_t pressure, uint8_t humidity, uint8_t mode)

{
    if (temp > 5)
        temp = 5;
    
    if (pressure > 5)
        pressure = 5;
    
    if (humidity > 5)
        humidity = 5;
    
    if (mode > 3)
        mode = 3;
    
    
    i2c.writeU8(BME280_CONTROL_HUMIDITY, humidity);
    
    uint8_t control = mode | (pressure << 2) | (temp << 5);
    
    Serial.print("Control = ");
    Serial.println(control);
    
    i2c.writeU8(BME280_CONTROL, control);
}

/*
 
 //From Bosch data sheet
 
 
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC. // t_fine carries fine temperature as global value
BME280_S32_t t_fine;
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T)
{
    BME280_S32_t var1, var2, T;
    var1 = ((((adc_T>>3) – ((BME280_S32_t)dig_T1<<1))) * ((BME280_S32_t)dig_T2)) >> 11;
    var2 = (((((adc_T>>4) – ((BME280_S32_t)dig_T1)) * ((adc_T>>4) – ((BME280_S32_t)dig_T1))) >> 12) *
            ((BME280_S32_t)dig_T3)) >> 14; t_fine = var1 + var2;
    T =(t_fine*5+128)>>8; return T;
}
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits). // Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P)
{
    BME280_S64_t var1, var2, p;
    var1 = ((BME280_S64_t)t_fine) – 128000;
    var2 = var1 * var1 * (BME280_S64_t)dig_P6;
    var2 = var2 + ((var1*(BME280_S64_t)dig_P5)<<17);
    var2 = var2 + (((BME280_S64_t)dig_P4)<<35);
    var1 = ((var1 * var1 * (BME280_S64_t)dig_P3)>>8) + ((var1 * (BME280_S64_t)dig_P2)<<12); var1 = (((((BME280_S64_t)1)<<47)+var1))*((BME280_S64_t)dig_P1)>>33;
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero }
        p = 1048576-adc_P;
        p = (((p<<31)-var2)*3125)/var1;
        var1 = (((BME280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25; var2 = (((BME280_S64_t)dig_P8) * p) >> 19;
        p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)dig_P7)<<4); return (BME280_U32_t)p;
    }
    // Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits). // Output value of “47445” represents 47445/1024 = 46.333 %RH
    BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H)
    {
        BME280_S32_t v_x1_u32r;
        v_x1_u32r = (t_fine – ((BME280_S32_t)76800));
        v_x1_u32r = (((((adc_H << 14) – (((BME280_S32_t)dig_H4) << 20) – (((BME280_S32_t)dig_H5) * v_x1_u32r)) +
                       ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)dig_H6)) >> 10) * (((v_x1_u32r *
                                                                                                              ((BME280_S32_t)dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) *
                                                         ((BME280_S32_t)dig_H2) + 8192) >> 14));
        v_x1_u32r = (v_x1_u32r – (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)dig_H1)) >> 4)); v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
        v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
        return (BME280_U32_t)(v_x1_u32r>>12);
    }
*/