#include <I2C_Insarianne.h>

#define frequence 868E6
#define LORA_pin 5

#define Vitesse_Serial 9600

void setup() {
  Serial.begin(Vitesse_Serial);
  while(!Serial);


  LoRa.begin(frequence, LORA_pin);
  Serial.write("Init completed")
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if(packetSize) {
    while(LoRa.available()) {
      Serial.write((char)LoRa.read());
    }
  }
}