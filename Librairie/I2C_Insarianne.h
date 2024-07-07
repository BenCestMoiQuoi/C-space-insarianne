/*!
 * @file INSARIANNE.h
 *
 * This is a library for the BMP180 Barometric Pressure + Temp
 * sensor and the MUP6050 accelerometer, gyroscopique and temperature sensor
 *
 * These displays use I2C to communicate, 2 pins are required to
 * interface
 * 
 */

#ifndef INSARIANNE_H
#define INSARIANNE_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>


#define VERSION_LIB "1.1"


class I2C {
  protected:
    I2C(TwoWire *theWire = &Wire, HardwareSerial *theSer = &Serial);
    virtual void begin(uint8_t address);

    bool write(uint8_t data, uint8_t reg_data);
    uint8_t read8(uint8_t reg);
    uint16_t read16(uint8_t reg);
    bool read_n(uint8_t reg, uint8_t data[], int n);
    bool write_bits(uint8_t data, uint8_t reg, uint8_t bits, uint8_t shift);
    uint8_t read_bits(uint8_t reg, uint8_t bits, uint8_t shift);

    uint8_t _addr;
    TwoWire *_wire;
    HardwareSerial *_ser;
};


class BMP180 : private I2C
{
  public:
    BMP180();

    bool begin(void);

    void read_sensor(void);
    void Set_SLP(float altitude_base);

    float pressure, sealevelpressure, temperature, altitude;

  private:
    void Temperature(void);
    void Pressure(void);
    void Altitude(void);

    uint8_t oversampling;
    int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
    uint16_t ac4, ac5, ac6;
    int32_t UP, UT;
    int32_t c3, c5, c6, x1, x2, x3;
    uint32_t c4, c7, c1;
};


class MPU6050 : private I2C
{
  public:
    MPU6050();

    bool begin();
    bool begin(uint8_t para_gyr, uint8_t para_acc);
    bool read_sensor(void);

    bool read_acce(void);
    bool read_gyro(void);
    bool read_temp(void);

    void Set_gyro_scale(float new_scale);
    void Set_accel_scale(float new_scale);
    bool Set_param_register(uint8_t val, uint8_t reg);
    bool Set_param_register_bits(uint8_t val, uint8_t reg, uint8_t bits, uint8_t shift);
    float Get_gyro_scale(void);
    float Get_accel_scale(void);
    uint8_t Get_param_register(uint8_t reg);
    uint8_t Get_param_register(uint8_t reg, uint8_t bits, uint8_t shift);

    void SetOffset_zero(void);
    void CalcOffset(void);

    uint8_t test(void);

    float temperature, gyroX, gyroY, gyroZ, accX, accY, accZ;
    float accXoffset, accYoffset, accZoffset;
    float gyroXoffset, gyroYoffset, gyroZoffset;

  private:
    int _sensorID_tem = 0x650, _sensorID_acc = 0x651, _sensorID_gyr = 0x652;
    int16_t rawAccX, rawAccY, rawAccZ, rawTemp, rawGyroX, rawGyroY, rawGyroZ;
    float accel_scale, gyro_scale;
};

#define LORA_DEFAULT_SPI           SPI
#define LORA_DEFAULT_SPI_FREQUENCY 8E6 
#define LORA_DEFAULT_SS_PIN        10
#define LORA_DEFAULT_RESET_PIN     9
#define LORA_DEFAULT_DIO0_PIN      2

#define PA_OUTPUT_RFO_PIN          0
#define PA_OUTPUT_PA_BOOST_PIN     1

class LoRa : public Stream {
  public:
    LoRa();
    int begin(long frequency, int our_ss);
    void end();

  int beginPacket(int implicitHeader = false);
  int endPacket(bool async = false);

  int parsePacket(int size = 0);
  int packetRssi();
  float packetSnr();
  long packetFrequencyError();

  int rssi();

  // from Print
  virtual size_t write(uint8_t byte);
  virtual size_t write(const uint8_t *buffer, size_t size);

  // from Stream
  virtual int available();
  virtual int read();
  virtual int peek();
  virtual void flush();

  void receive(int size = 0);

  void idle();
  void sleep();

  void setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN);
  void setFrequency(long frequency);
  void setSpreadingFactor(int sf);
  void setSignalBandwidth(long sbw);
  void setCodingRate4(int denominator);
  void setPreambleLength(long length);
  void setSyncWord(int sw);
  void enableCrc();
  void disableCrc();
  void enableInvertIQ();
  void disableInvertIQ();
  
  void setOCP(uint8_t mA); // Over Current Protection control
  
  void setGain(uint8_t gain); // Set LNA gain

  // deprecated
  void crc() { enableCrc(); }
  void noCrc() { disableCrc(); }

  byte random();

  void setPins(int ss = LORA_DEFAULT_SS_PIN, int reset = LORA_DEFAULT_RESET_PIN, int dio0 = LORA_DEFAULT_DIO0_PIN);
  void setSPI(SPIClass& spi);
  void setSPIFrequency(uint32_t frequency);

  void dumpRegisters(Stream& out);

private:
  void explicitHeaderMode();
  void implicitHeaderMode();

  void handleDio0Rise();
  bool isTransmitting();

  int getSpreadingFactor();
  long getSignalBandwidth();

  void setLdoFlag();

  uint8_t readRegister(uint8_t address);
  void writeRegister(uint8_t address, uint8_t value);
  uint8_t singleTransfer(uint8_t address, uint8_t value);

private:
  SPISettings _spiSettings;
  SPIClass* _spi;
  int _ss;
  int _reset;
  int _dio0;
  long _frequency;
  int _packetIndex;
  int _implicitHeaderMode;
  void (*_onReceive)(int);
  void (*_onTxDone)();
};

#endif 
