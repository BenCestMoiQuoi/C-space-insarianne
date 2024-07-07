/*!
 * @file _I2C.cpp
 *
 * @mainpage I2C Sensors
 * 
 * @section Intoduction
 * 
 * This is a library for mother class I2C sensor
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


I2C::I2C(TwoWire *theWire, HardwareSerial *theSer) {
    _wire = theWire;
    _ser = theSer;
}

void I2C::begin(uint8_t address) {
    _addr = address;
}

bool I2C::write(uint8_t data, uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->write(data);
    _wire->endTransmission();
    return true;
}

uint8_t I2C::read8(uint8_t reg) {

    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(true);
    _wire->requestFrom((uint8_t)_addr, (uint8_t)1);
    
    if (_wire->available())
        return _wire->read();

    return false;
}

uint16_t I2C::read16(uint8_t reg) {
    uint8_t buffer[2];
    
    if (read_n(reg, buffer, 2))
        return false;


    return (buffer[0] << 8 | buffer[1]);
}

bool I2C::read_n(uint8_t reg, uint8_t *data, int n) {
    
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(true);
    _wire->beginTransmission(_addr);
    _wire->requestFrom((uint8_t)_addr, (uint8_t)n);

    
    for (int i=0; _wire->available(); i++) 
        data[i] = _wire->read();
    
    return true;
}

bool I2C::write_bits(uint8_t data, uint8_t reg, uint8_t bits, uint8_t shift) {
    uint8_t val = read8(reg);

    uint8_t mask = 0;
    for (int i=0; i<bits; i++) mask |= (1 << i);
    data &= mask;
    mask <<= shift;
    val &= ~mask;
    val |= data << shift;

    return write(val, reg);
}

uint8_t I2C::read_bits(uint8_t reg, uint8_t bits, uint8_t shift) {
    uint8_t val = read8(reg);

    uint8_t mask = 0;
    for (int i=0; i<bits; i++) mask |= (1 << i);

    val >>= shift;
    return val & mask;
}
