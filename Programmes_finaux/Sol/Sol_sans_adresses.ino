#include <SPI.h>
#include <LoRa.h>
#define frequence 915E6 //fréquence séléctionnée

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa au sol");

  if (!LoRa.begin(frequence)) { 
    Serial.println("Lancement compromis, vérifier les branchements du LoRa");
    while (1);
  }
}

void loop() {
  // recherche d'un signal
  int paquets = LoRa.parsePacket();
  if (paquets) {
    // signal de données reçu
    Serial.print("Données reçues :");

    // lecture des données
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("Fin du signal");
  }
}