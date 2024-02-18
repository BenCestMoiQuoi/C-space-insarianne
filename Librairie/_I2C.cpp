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


I2C::I2C(TwoWire *theWire) {
    _wire = theWire;
}

void I2C::begin(uint8_t address) {
    _addr = address;
}

bool I2C::write(uint8_t *data, size_t len, bool stop, 
                   uint8_t *reg, size_t reg_len) {
    _wire->beginTransmission(_addr);
    if ((reg_len != 0) && (reg != nullptr)) {
        if (_wire->write(reg, reg_len) != reg_len) return false;
    }
    if((len != 0) && (data != nullptr)) {
        if (_wire->write(data, len) != len) return false;
    }
    if (_wire->endTransmission(stop) == 0) return true;
    return false;    
}

uint8_t I2C::read8(uint8_t reg) {
    uint8_t buffer;
    if (read_n(reg, buffer, 1)) 
        return false;
    
    return buffer;
}

uint16_t I2C::read16(uint8_t reg) {
    uint8_t buffer[2];
    
    if (read_n(reg, buffer, 2)) 
        return false;
    
    buffer[0] = _wire->read();
    buffer[1] = _wire->read();

    return (buffer[0] << 8 | buffer[1]);
}

bool I2C::read_n(uint8_t reg, uint8_t data[], int n) {
    if (_wire->requestFrom(_addr, (uint8_t) n, reg, 1, true) != n) 
        return false;

    for (int i=0; i<n; i++) 
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

    return write(val, 1, true, reg, 1);
}

uint8_t I2C::read_bits(uint8_t reg, uint8_t bits, uint8_t shift) {
    uint8_t val = read8(reg);

    uint8_t mask = 0;
    for (int i=0; i<bits; i++) mask |= (1 << i);

    val >>= shift;
    return val & mask;
}
