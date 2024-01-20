#include<Wire.h>

void setup()
{
  Serial.begin(9600);
  Wire.begin(0x21);
}

void loop()
{
  
}