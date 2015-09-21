#include Nerduino_BME280_Library.h"


BME280::BME280() : BME280(SHIELD_SDA)
{
}

BME280::BME280(uint8_t ioPin) :
    I2C_Shield(ioPin, 0x77)
{
    // verify that the device is present
    if (isDetected())
    {
        // initialize the device
        
    }
}

bool BME280::isDetected()
{
    return (read8(BME280_CHIPID) == 0x60);
}

void BME280::initialize()
{
    dig_T1 = readU16(BME280_T1);
    dig_T2 = read16(BME280_T2);
    dig_T3 = read16(BME280_T3);
    
    dig_P1 = readU16(BME280_P1);
    dig_P2 = read16(BME280_P2);
    dig_P3 = read16(BME280_P3);
    dig_P4 = read16(BME280_P4);
    dig_P5 = read16(BME280_P5);
    dig_P6 = read16(BME280_P6);
    dig_P7 = read16(BME280_P7);
    dig_P8 = read16(BME280_P8);
    dig_P9 = read16(BME280_P9);

    dig_H1 = readU8(BME280_H1);
    dig_H2 = read16(BME280_H2);
    dig_H3 = readU8(BME280_H3);
    dig_H4 = read16(BME280_H4);
    dig_H5 = read16(BME280_H5);
    dig_H6 = read8(BME280_H6);

    // enable control
    write(BME280_CONTROL, 0x3F);
    
    // enable humidity
    write(BME280_CONTROL_HUMIDITY, 0x03);
}

float BME280::getTemperature()
{
    int32_t adc_T = (read16(BME280_TEMP) << 8 | read8(BME280_TEMP + 2)) >> 4;
    
    
    int32_t var1, var2;
    var1 = ((((adc_T>>3) – ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T>>4) – ((int32_t)dig_T1)) * ((adc_T>>4) – ((int32_t)dig_T1))) >> 12) *
            ((int32_t)dig_T3)) >> 14;
    int32_t t_fine = var1 + var2;
    
    float fT =(t_fine*5+128)>>8;
    
    fT = /= 100.0f;
    
    return fT;
}

float BME280::getPressure()
{
    int32_t adc_P = (read16(BME280_PRESSURE) << 8 | read8(BME280_PRESSURE + 2)) >> 4;

    
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) – 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
    var2 = var2 + (((int64_t)dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12); var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
    
    if (var1 == 0)
    {
        return 0.0f; // avoid exception caused by division by zero
    }
    
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25; var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
    
    float fP = ((float) p) / 256.0f;
    
    return fP;
}

float BME280::getHumidity()
{
    int32_t adc_H = read16(BME280_HUMIDITY);
    
    uint32_t v_x1_u32r;
    v_x1_u32r = (t_fine – ((uint32_t)76800));
    v_x1_u32r = (((((adc_H << 14) – (((uint32_t)dig_H4) << 20) – (((uint32_t)dig_H5) * v_x1_u32r)) +
                   ((uint32_t)16384)) >> 15) * (((((((v_x1_u32r * ((uint32_t)dig_H6)) >> 10) * (((v_x1_u32r *
                                                                                                          ((uint32_t)dig_H3)) >> 11) + ((uint32_t)32768))) >> 10) + ((uint32_t)2097152)) *
                                                     ((uint32_t)dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r – (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)dig_H1)) >> 4)); v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    
    float fH = (float) (v_x1_u32r >> 12) / 1024.0f;
    
    return fH;
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