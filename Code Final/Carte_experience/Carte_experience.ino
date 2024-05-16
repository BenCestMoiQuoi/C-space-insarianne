/*
Programme Arduino
Nom : Main.ino
Auteur : Roche Corentin
Version : 4
Date : 20/01/2023


*/


#include <Adafruit_MPU6050.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#define _BV(n) (1<<n)

#define PRESSION_MER 101325



#define Val_Timer 3000 // 3s ici


File myFile;
#define file1 "text.tsv"
#define file2 "number.txt"
#define SD_pin 5
double loop;

Adafruit_MPU6050 mpu;
Adafruit_BMP085 bmp;

/*  Declare the variables  */

int Etat_sol;
bool Etat_vol;

unsigned long timer_ms;
unsigned long timer_info;
unsigned long count_ms;
unsigned long count_s;

sensors_event_t acc, gyr, temp;
float pre, alt;

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
  SD.begin(SD_pin);
  myFile = SD.open(file1, FILE_WRITE);
  myFile.println("Acc_X\tAcc_Y\tAcc_Z\tRot_X\tRot_Y\tRot_Z\tTemp\tPre\tAlt\tTemps"); /*création d'un en-tête pour notre fichier de mesure*/
  loop = 1;
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  bmp.begin();
}

void Verif_Sol(){
  /*
  fonction où la carte attend un signal de la carte connetée à oa Jack pour commencer les mesures
  */
  if (Serial.available() >= sizeof(int)) {
    int i;
    Serial.readBytes((char*)&i, sizeof(nombre));
    if(i==1){
      Etat_vol = 1
    }
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
  if (count_ms >= 1000){
    count_ms-=1000;
    count_s++;
  }

  if (timer_info >= nb_ms){
    timer_info = 0;
    
    mpu.getEvent(&acc, &gyr, &temp);
    pre = bmp.readPressure();
    alt = bmp.readAltitude(PRESSION_MER);

    myFile.print(acc.acceleration.x);myFile.print("\t");
    myFile.print(acc.acceleration.y);myFile.print("\t");
    myFile.print(acc.acceleration.z);myFile.print("\t");

    myFile.print(gyr.gyro.x);myFile.print("\t");
    myFile.print(gyr.gyro.y);myFile.print("\t");
    myFile.print(gyr.gyro.z);myFile.print("\t");

    myFile.print(temp.temperature);myFile.print("\t");
    myFile.print(pre);myFile.print("\t");
    myFile.print(alt);myFile.print("\t");

    myFile.print(timer_ms);myFile.print("\t");

    myFile.println();
    loop++
  }
}

void Sol(){
  Etat_vol = false;
  timer_ms = 0;
  while (!Etat_vol){
    Verif_Sol();
  }
}

int stop(){
  if (Serial.available() >= sizeof(int)) {
    int i;
    Serial.readBytes((char*)&i, sizeof(nombre));
    if(i==2){
      return 0;
    }
  }
  return 1;
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
  int continue = 1;
  while(continue){ 
    Transfert_Info(200);
    continue = stop();
    }
  /*Autre version si on connait le temps pour lequel la fusée est au sol pour un arrêt automatique
  while(timer_ms<Val_Timer){ 
    Transfert_Info(200);
  }
  où Val_Timer sera le moment où la fusée est au sol
  */
    myFile.close();
    myFile = SD.open(file2,FILE_WRITE);
    myFile.print(loop) /*ici on enregistre le nombre de ligne du fichier*/
    myFile.close();
  while(true)int i = 1 /*je sais pas quelle ligne mettre pour le faire patienter*/;
}

void setup() {
  Serial.begin(9600);

  Init_Timer();
  Init_Sensor();

  while(digitalRead(Switch_pin_sol_1) || digitalRead(Switch_pin_sol_2));
  
  Etat_sol = 1;
  count_ms = 0;
  count_s = 0;
  
  /*  Main program  */
  Sol();
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
