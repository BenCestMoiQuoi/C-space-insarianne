/*!
 * @file _SPI.cpp
 *
 * @mainpage INSARIANNE Sensors
 * 
 * @section Intoduction
 * 
 * This is a library for the BMP085 Barometric Pressure + Temp
 * sensor and the MUP6050
 *
 * These displays use I2C to communicate, 2 pins are required to
 * interface
 * 
 * @section Author
 * 
 * Written by Corentin Roche
 * 
 */

#include "INSARIANNE.h"

#define _BV(n) (1<<n)

#define SPI_MISO_PIN 12
#define SPI_MOSI_PIN 13
#define SPI_CLK_PIN 14

SPI::SPI(){
  pinMode(SPI_MISO_PIN, INPUT);
  pinMode(SPI_MOSI_PIN, OUTPUT);
  pinMode(SPI_CLK_PIN, OUTPUT);
}

void SPI::SendSPI(uint8_t buff) {
  SPDR = buff;
  while (!(SPSR & _BV(SPIF)));
}

uint8_t SPI::RecSPI(void) {
  SendSPI(0xFF);
  return SPDR;
}

void SPI::begin(uint8_t cs_pin) {
  _CS_pin = cs_pin;
  pinMode(_CS_pin, OUTPUT);
  digitalWrite(_CS_pin, HIGH);

  // Enable SPI, Master, clock rate f_osc/128
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0);
  // clear double speed
  SPSR &= ~_BV(SPI2X);

  for (uint8_t i = 0; i < 10; i++) {
    RecSPI();
  }

  digitalWrite(_CS_pin, LOW);
}
