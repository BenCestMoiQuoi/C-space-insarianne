/*
Programme Arduino
Nom : Main.ino
Auteur : Roche Corentin
Version : 4
Date : 20/01/2023


*/
#include <I2C_Insarianne.h>
#include <Adafruit_BMP085.h>

#include <LoRa.h>
#include <Wire.h>
#include <SPI.h>
//#include <SD.h>

#define _BV(n) (1<<n)

#define PRESSION_MER 101325
#define ALTITUDE_BASE 85


#define Val_Timer 3000 // 3s ici


//File myFile;
//#define path "text.tsv"
//#define SD_pin 5
float packet[10];
#define tete "Acc_X\tAcc_Y\tAcc_Z\tRot_X\tRot_Y\tRot_Z\tTemp\tPre\tAlt\tTemps"
MPU6050 mpu;
//BMP085 bmp;
Adafruit_BMP085 bmp;

/*  Declare the variables  */


bool Etat_vol;

unsigned long timer_ms;
unsigned long timer_info;
unsigned long count_ms;
unsigned long count_s;

/*  Initialisation  */

void Init_Timer(){
  /*
  Initialisation du Timer, 
  le timmer est fait pour une intervalle de 1 seconde,
  une interruption pour l'update se fait toutes les 1ms.

  Pour comprendre, faut se référer à la 
    Datasheet ATMEGA48A
  l'init du timer se fait par les registres de ce micro-processeur
  (Celui de l'Arduino Nano) Clk = 16 MHz
  
  Nous allons utilisez le Timmer 0 pour nous faire notre horloge.
  Le Timmer 1 est utilisé pour les deux servo en pin PWM 9 et 10 par la librairie Servo.
  */
  cli();//stop interrupts

  //set timer0 interrupt at 1kHz
  TCCR0A = 0 | _BV(WGM01); // turn on CTC mode
  TCCR0B = 0 | _BV(CS01) | _BV(CS00); //Set the prescale 1/64 clock
  TIMSK0 |= _BV(OCIE0A); //Set the interrupt request
  // set compare match register for 1khz increments
  OCR0A = 249;// = (16*10^6) / (1000*64) - 1 (must be <256)
  

  sei(); //Enable interrupt
  // Init variable timer
  timer_ms = 0;
  timer_info = 0;
  count_ms = 0;
  count_s = 0;
}

void Init_Sensor(){
  /* 
  Initialisation de la partie capteur
  
  Avec 1 carte SD
       1 accelerometre 3 axes
       1 gyroscope 3 axes
       1 capteur de température
  */
  //SD.begin(SD_pin);
  //myFile = SD.open(file1, FILE_WRITE);
  //myFile.println("Acc_X\tAcc_Y\tAcc_Z\tRot_X\tRot_Y\tRot_Z\tTemp\tPre\tAlt\tTemps"); /*création d'un en-tête pour notre fichier de mesure*/
  //myFile.close();

  mpu.begin();
  bmp.begin();
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //bmp.Set_SLP(ALTITUDE_BASE);
}

void Verif_Sol(){
  /*
  Fonction qui permet de gérer entièrement l'état d'avancement de la fusée
  lorsquel est au sol (Etat_sol de 1 à 3).
  
  Etat_sol = 1 
      LED 1 --> Alumé
      LED 2 --> Eteinte
      Servo_haut --> 0
  Etat_sol = 2 (si prise Jack branché)
      LED 1 --> Eteinte
      LED 2 --> Alumé
      Servo_haut --> 180
  Etat_sol = 3 (dernière validation)
      LED 1 --> Alumé
      LED 2 --> Alumé
      Servo_haut --> 180
  Lorsque le Jack et débranché, Etat_vol = True
  */
  if (!digitalRead(Switch_pin_sol_1) && (Etat_sol == 2 || Etat_sol == 3)){
    Etat_sol = 1;
    write_LED_Sol();
    servo_haut.write(0);
  }
  if (digitalRead(Switch_pin_sol_1) && Etat_sol == 1) servo_haut.write(180);
  if (Etat_sol == 1 && digitalRead(Switch_pin_sol_1) && !digitalRead(Switch_pin_sol_2) 
        && !digitalRead(Jack_pin) && servo_haut.read()==180){
    Etat_sol = 2;
    write_LED_Sol();
  }
  if (Etat_sol == 2 && digitalRead(Switch_pin_sol_2)){
    Etat_sol = 3;
    write_LED_Sol();
  }
  if (Etat_sol == 3 && !digitalRead(Switch_pin_sol_2)){
    Etat_sol = 2;
    write_LED_Sol();
  }
  if (Etat_sol == 3 && digitalRead(Jack_pin)){
    Etat_vol = true;
  }
}

void Transfert_Info(unsigned long nb_ms){
  /*
  Fonction qui envoie l'ensemble des données des capteurs sur la Carte SD
  Permet également de gérer l'horloge personnalisé (secondes:milliseconde).

  L'écriture sur la carte se fait toutes les nb_ms

  Input : 
    unsigned long nb_ms -> Nombre de millis pour l'écriture
  */
  //bmp.read_sensor();
  mpu.read_sensor();
  if (count_ms >= 1000){
    count_ms-=1000;
    count_s++;
  }

  if (timer_info >= nb_ms){
    timer_info = 0;
    packet = {mpu.accX,mpu.accY,mpu.accZ,mpu.gyroX,mpu.gyroY,mpu.gyroZ,mpu.temperature,bmp.readPressure(),bmp.readAltitude(101500),timer_ms}; /*Regroupement de toutes les mesure dans un unique tableau*/
    //myFile = SD.open(path,FILE_WRITE);
    LoRa.beginPacket();
    LoRa.print("\n");/*espacement des données*/
    LoRa.println(tete); /*Impression de l'en tête pour une meilleure compréhension de l'utilisateur*/
    for(int i = 0;i<10;i++){
      //myFile.print(packet[i]);
      //myFile.print("\t");
      LoRa.print(packet[i]);
      LoRa.print("\t");      
    }
    //myFile.print("\n");
    LoRa.print("\n");
    LoRa.endPacket();
    //myFile.close();
  }
}

void Sol(){
  Etat_vol = false;
  timer_ms = 0;
  while (!Etat_vol){
    Verif_Sol();
  }
}

void Vol(){
  /*
  Fonction de la fusée lorsqu'elle est en vol.
  Toutes les 200ms on envoi des infos des capteurs
  Quand la carte reçoit un signal d'arrêt elle enregistre le nombre de mesures et arrête les mesures
  while(timer_ms<){
  }/
  */
  timer_ms = 0;
  while(1){ 
    Transfert_Info(200);
    }
  while(true) void; /*je sais pas quelle ligne mettre pour le faire patienter*/
}

void setup() {
  Serial.begin(9600);

  Init_Timer();
  Init_Sensor();

  while(digitalRead(Switch_pin_sol_1) || digitalRead(Switch_pin_sol_2));
  
  count_ms = 0;
  count_s = 0;
  
  /*  Main program  */
  //Sol();
  Vol();
}


void loop() {
  void;
}

ISR(TIMER0_COMPA_vect){    //This  is the interrupt request
  timer_ms++;
  count_ms++;
  timer_info++;
}
