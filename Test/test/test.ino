#include <Arduino.h>
#include <Wire.h>

#define VERSION_LIB "1.1.6"


class I2C {
  protected:
    I2C(TwoWire *theWire = &Wire) {
      _wire = theWire;
    }
    void begin(uint8_t address){
      _addr = address;
    }

    bool write(uint8_t *data, size_t len, bool stop = true, \
               uint8_t *reg = nullptr, size_t reg_len = 0) {
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

    uint8_t read8(uint8_t *reg) {
      uint8_t buffer;
      if (read_n(reg, buffer, 2)) 
        return false;
      
      return buffer;
    }

    uint16_t read16(uint8_t *reg) {
      uint8_t buffer[2];
      
      if (read_n(reg, buffer, 2)) 
        return false;
      
      buffer[0] = _wire->read();
      buffer[1] = _wire->read();

      return (buffer[0] << 8 | buffer[1]);
    }

    bool read_n(uint8_t *reg, uint8_t data[], int n) {
      if (_wire->requestFrom(_addr, (uint8_t) n, reg, 1, true) != n) 
        return false;

      for (int i=0; i<n; i++) 
        data[i] = _wire->read();
      
      return true;
    }

    bool write_bits(uint8_t data, uint8_t *reg, uint8_t bits, uint8_t shift) {
      uint8_t val = read8(reg);

      uint8_t mask = 0;
      for (int i=0; i<bits; i++) mask |= (1 << i);
      data &= mask;
      mask <<= shift;
      val &= ~mask;
      val |= data << shift;

      write(val, 1, true, reg, 1);
    }

    uint8_t read_bits(uint8_t *reg, uint8_t bits, uint8_t shift) {
      uint8_t val = read8(reg);

      uint8_t mask = 0;
      for (int i=0; i<bits; i++) mask |= (1 << i);

      val >>= shift;
      return val & mask;
    }

  
    uint8_t _addr;
    TwoWire *_wire;
};

class MPU6050 : private I2C
{
  public:
    MPU6050() : I2C() {
      _addr = 0x68;
    }

    bool begin() {
      write(0, 1, true, 0x19, 1);
      write_bits(0b10, 0x1B, 2, 3);
      write_bits(0b00, 0x1C, 2, 3);

      switch(read_bits(0x1B, 2, 3)) {
        case 0b00:
          gyro_scale = 131; break;
        case 0b01:
          gyro_scale = 65.5; break;
        case 0b10:
          gyro_scale = 32.8; break;
        case 0b11:
          gyro_scale = 16.4; break;
        default:
          return false;
      }

      switch(read_bits(0x1C, 2, 3)) {
        case 0b00:
          accel_scale = 16384; break;
        case 0b01:
          accel_scale = 8192; break;
        case 0b10:
          accel_scale = 4096; break;
        case 0b11:
          accel_scale = 2048; break;
        default:
          return false;
      }
      return true;
    }

    void read_sensor(void) {
      uint8_t buffer[14];

      MPU6050::read_n((uint8_t)0x3B, buffer, 14);

      rawAccX = buffer[0] << 8 | buffer[1];
      rawAccY = buffer[2] << 8 | buffer[3];
      rawAccZ = buffer[4] << 8 | buffer[5];

      rawTemp = buffer[6] << 8 | buffer[7];

      rawGyroX = buffer[8] << 8 | buffer[9];
      rawGyroY = buffer[10] << 8 | buffer[11];
      rawGyroY = buffer[12] << 8 | buffer[13];


      temperature = (rawTemp / 340.0) + 36.53;

      accX = ((float)rawAccX) / accel_scale;
      accY = ((float)rawAccY) / accel_scale;
      accZ = ((float)rawAccZ) / accel_scale;

      gyroX = ((float)rawGyroX) / gyro_scale;
      gyroY = ((float)rawGyroY) / gyro_scale;
      gyroZ = ((float)rawGyroZ) / gyro_scale;
    }

    float temperature, gyroX, gyroY, gyroZ, accX, accY, accZ;

  private:
    int _sensorID_tem = 0x650, _sensorID_acc = 0x651, _sensorID_gyr = 0x652;
    int16_t rawAccX, rawAccY, rawAccZ, rawTemp, rawGyroX, rawGyroY, rawGyroZ;
    float accel_scale, gyro_scale;
};


MPU6050 mpu;

void Transfert_Info(){
    mpu.read_sensor();

    Serial.print("Accéleration X : ");
    Serial.print(mpu.accX);
    Serial.println(" m/s^2,");
    Serial.print("Accéleration Y : ");
    Serial.print(mpu.accY);
    Serial.println(" m/s^2,");
    Serial.print("Accéleration Z : ");
    Serial.print(mpu.accZ);
    Serial.println(" m/s^2,");
    Serial.print("Rotation X : ");
    Serial.print(mpu.gyroX);
    Serial.println(" rad/s,");
    Serial.print("Rotation Y : ");
    Serial.print(mpu.gyroY);
    Serial.println(" rad/s,");
    Serial.print("Rotation Z : ");
    Serial.print(mpu.gyroZ);
    Serial.println(" rad/s,");
    Serial.print("Température : ");
    Serial.print(mpu.temperature);
    Serial.println(" °C.");
    Serial.println();
}

void setup() {
    Serial.begin(9600);
    Serial.print("Version Librairie : "); Serial.println(VERSION_LIB);
    Wire.begin();

    mpu.begin();
}

void loop() {
    Transfert_Info();
    delay(500);
}