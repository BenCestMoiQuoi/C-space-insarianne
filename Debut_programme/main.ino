/*
Created on Wed Mar 29 11:24:36 2024

@author: croche
    
Programme lié à celui de mesure python
Permet de gérer le switch (manuel) en changeant les pins
en fonction de ce qui est donné par le programme python
*/

#define en_pin 3
#define an0_pin 4
#define an1_pin 5

#define SELECT_CHANNEL 100
#define INIT 50

char c;

void setup() {
// Setup des trois pins du switch
    pinMode(en_pin, OUTPUT);
    pinMode(an0_pin, OUTPUT);
    pinMode(an1_pin, OUTPUT);

    Serial.begin(500000);
    Serial.flush();

    while(Serial.available() == 0);
    c = Serial.read();
    if(c == INIT) Serial.write(INIT);
}

void loop() {
    if(Serial.available()>0){
        c = Serial.read();
        if (c == SELECT_CHANNEL) select_channel();
    }
}


void select_channel() {
    /*
Fonction qui gère le statut des pins de sortie vers le switch
    */
    while(Serial.available() < 3); // On attend trois infos
    digitalWrite(en_pin, Serial.read());
    digitalWrite(an0_pin, Serial.read());
    digitalWrite(an1_pin, Serial.read());
}