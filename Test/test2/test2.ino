#include <INSARIANNE.h>

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

    if(!mpu.begin()) {
        Serial.println("Non initialisé");
        While(1);
    }
}

void loop() {
    Transfert_Info();
    delay(500);
}