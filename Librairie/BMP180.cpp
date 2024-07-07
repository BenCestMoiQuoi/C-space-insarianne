/*!
 * @file BMP180.cpp
 *
 * @mainpage BMP180 Sensors
 * 
 * @section Intoduction
 * 
 * This is a library for the BMP180 Barometric Pressure + Temp sensor
 *
 * These displays use I2C to communicate, 2 pins are required to
 * interface
 * 
 * @section Author
 * 
 * Written by Corentin Roche
 * 
 */

#include "I2C_Insarianne.h"


#define BMP_ADDR 0x77

#define BMP180_ULTRALOWPOWER 0 //!< Ultra low power mode
#define BMP180_STANDARD 1      //!< Standard mode
#define BMP180_HIGHRES 2       //!< High-res mode
#define BMP180_ULTRAHIGHRES 3  //!< Ultra high-res mode
#define BMP180_CAL_AC1 0xAA    //!< R   Calibration data (16 bits)
#define BMP180_CAL_AC2 0xAC    //!< R   Calibration data (16 bits)
#define BMP180_CAL_AC3 0xAE    //!< R   Calibration data (16 bits)
#define BMP180_CAL_AC4 0xB0    //!< R   Calibration data (16 bits)
#define BMP180_CAL_AC5 0xB2    //!< R   Calibration data (16 bits)
#define BMP180_CAL_AC6 0xB4    //!< R   Calibration data (16 bits)
#define BMP180_CAL_B1 0xB6     //!< R   Calibration data (16 bits)
#define BMP180_CAL_B2 0xB8     //!< R   Calibration data (16 bits)
#define BMP180_CAL_MB 0xBA     //!< R   Calibration data (16 bits)
#define BMP180_CAL_MC 0xBC     //!< R   Calibration data (16 bits)
#define BMP180_CAL_MD 0xBE     //!< R   Calibration data (16 bits)

#define BMP180_CONTROL 0xF4         //!< Control register
#define BMP180_TEMPDATA 0xF6        //!< Temperature data register
#define BMP180_PRESSUREDATA 0xF6    //!< Pressure data register
#define BMP180_READTEMPCMD 0x2E     //!< Read temperature control register value
#define BMP180_READPRESSURECMD 0x34 //!< Read pressure control register value
#define BMP_OK_REG 0xD0             //!< Read if the sensor is ready
#define BMP_OK 0x55                 //!< Sensor ready


BMP180::BMP180() : I2C() {
    _addr = BMP_ADDR;
}

bool BMP180::begin() {
    oversampling = BMP180_ULTRAHIGHRES;
    
    if (read8(BMP_OK_REG) != BMP_OK)
        return false;

    /* read calibration data */
    ac1 = read16(BMP180_CAL_AC1);
    ac2 = read16(BMP180_CAL_AC2);
    ac3 = read16(BMP180_CAL_AC3);
    ac4 = read16(BMP180_CAL_AC4);
    ac5 = read16(BMP180_CAL_AC5);
    ac6 = read16(BMP180_CAL_AC6);

    b1 = read16(BMP180_CAL_B1);
    b2 = read16(BMP180_CAL_B2);

    mb = read16(BMP180_CAL_MB);
    mc = read16(BMP180_CAL_MC);
    md = read16(BMP180_CAL_MD);

    return true;
}

void BMP180::read_sensor(void) {
    Temperature();
    Pressure();
    Altitude();
}

void BMP180::Set_SLP(float altitude_base) {
    Temperature();
    Pressure();

    sealevelpressure = pressure / pow(1.0 - altitude_base / 44330, 5.255);
}

void BMP180::Temperature(void) {
    //write(BMP180_READTEMPCMD, 1, true, BMP180_CONTROL, 1);
    UT = read16(BMP180_TEMPDATA);
    
    x1 = (UT - (int32_t)ac6) * ((int32_t)ac5) >> 15;
    x2 = ((int32_t)mc << 11) / (x1 + (int32_t)md);
    
    temperature = (x1 + x2 + 8) >> 4;
    temperature /= 10;
}

void BMP180::Pressure(void) {
    write(BMP180_READPRESSURECMD + (oversampling << 6), BMP180_CONTROL);
    
    if (oversampling == BMP180_ULTRALOWPOWER)
        delay(5);
    else if (oversampling == BMP180_STANDARD)
        delay(8);
    else if (oversampling == BMP180_HIGHRES)
        delay(14);
    else
        delay(26);

    c1 = read16(BMP180_PRESSUREDATA);

    c1 <<= 8;
    c1 |= read8(BMP180_PRESSUREDATA + 2);
    c1 >>= (8 - oversampling); 


    c6 = c5 - 4000;
    x1 = ((int32_t)b2 * ((c6 * c6) >> 12)) >> 11;
    x2 = ((int32_t)ac2 * c6) >> 11;
    x3 = x1 + x2;
    c3 = ((((int32_t)ac1 * 4 + x3) << oversampling) + 2) / 4;

    x1 = ((int32_t)ac3 * c6) >> 13;
    x2 = ((int32_t)b1 * ((c6 * c6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    c4 = ((uint32_t)ac4 * (uint32_t)(x3 + 32768)) >> 15;
    c7 = ((uint32_t)UP - c3) * (uint32_t)(50000UL >> oversampling);

    if (c7 < 0x80000000) {
        x3 = (c7 * 2) / c4;
    } else {
        x3 = (c7 / c4) * 2;
    }
    x1 = (x3 >> 8) * (x3 >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * x3) >> 16;

    pressure = x3 + ((x1 + x2 + (int32_t)3791) >> 4);
}

void BMP180::Altitude(void) {
    altitude = 44330 * (1.0 - pow(pressure / sealevelpressure, 0.1903));
}
