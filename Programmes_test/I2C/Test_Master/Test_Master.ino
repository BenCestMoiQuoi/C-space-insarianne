#include<Wire.h>

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  Wire.beginTransmission(0x21); //roll calling for the Slave
  byte busStatus = Wire.endTransmission();
  if (busStatus != 0x00)
  {
    Serial.println("I2C Bus communication problem...!");
    while (1);  //wait for ever
  }
  Serial.println("Slave found!");
}

void loop()
{
 
} 