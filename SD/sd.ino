#include <SPI.h>
#include <SD.h>

File dataFile;
const int chipSelect = 5; // numero pin
const char path[] = "TEST.txt";

void setup() {
  Serial.begin(9600);
  while(1){ // attente d'une carte
    if (!SD.begin(chipSelect)) {
      Serial.println("Carte SD non détectée.");
    }
    else{
      Serial.println("Carte SD prête à être utilisée.");
      break;
    }
  }
}

void loop() {
  dataFile = SD.open(path,O_APPEND | O_WRITE);

  if (dataFile) {
    dataFile.println("Bonjour, Arduino!"); // Écrire sur le fichier
    dataFile.close(); // Fermer le fichier
    Serial.println("Données écrites avec succès.");
  } else {
    Serial.println("Erreur d'ouverture du fichier.");
  }
    delay(1000); // Attendre 1 seconde
}