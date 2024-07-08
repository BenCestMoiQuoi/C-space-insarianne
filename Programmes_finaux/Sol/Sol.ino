/*
Programme Arduino
Nom : Sol.ino
Auteur : Roche Corentin (@BenCestMoiQuoi)
         Léo-Paul You (@Lyouu)
Version : 1
Date : 07/07/2024
*/

#include <I2C_Insarianne.h>

#define frequence 868E6
#define LORA_pin 5

#define Vitesse_Serial 9600

LoRa lora;


void setup() {
  Serial.begin(Vitesse_Serial);
  while(!Serial);

  if(!lora.begin(frequence, LORA_pin)){
    Serial.println("Init uncompled, please retry !");  
    while(1);
  }
}

void loop() {
  int packetSize = lora.parsePacket();
  if(packetSize) {
    while(lora.available()) {
      Serial.write((char)lora.read());
    }
  }
}