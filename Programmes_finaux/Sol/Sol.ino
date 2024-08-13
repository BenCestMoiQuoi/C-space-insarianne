/*
Programme Arduino
Nom : Sol.ino
Auteur : Roche Corentin (@BenCestMoiQuoi)
         Léo-Paul You (@Lyouu)
Version : 1
Date : 07/07/2024
Der_modif : 13/08/2024

Fonction : Programme qui permet de récupérer les données du module Lora de la fusée 
  pour les transmettre sur le port COM via le câble USB sur l'ordinateur
*/

#include <INSARIANNE_LORA.h>

#define frequence 866.3E6
#define LORA_pin 5

#define Vitesse_Serial 9600

LoRa lora;


void setup() {
  Serial.begin(Vitesse_Serial);
  while(!Serial);

  if(!lora.begin(frequence, LORA_pin)){
    Serial.println("Module LORA non initialis, merci de redémarrer la carte !");  
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