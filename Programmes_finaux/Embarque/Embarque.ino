/*
Programme Arduino
Nom : Embarque.ino
Auteur : Roche Corentin (@BenCestMoiQuoi)
         Léo-Paul You (@Lyouu)
Version : 3
Date : 07/07/2024
Der_modif : 13/08/2024
*/

/*  Includes  */

#include <INSARIANNE_BMP085.h>
#include <INSARIANNE_MPU6050.h>
#include <INSARIANNE_LORA.h>
#include <SD.h>


/*  Define  */

#define _BV(n) (1<<n)

#define ALTITUDE_BASE 430
#define Freq_LORA 866.3E6 

#define DELAY_INFO_SOL 400
#define DELAY_INFO_VOL 200

#define path "MESURE.TSV"
#define tete "Num_packet\tAcc_X\tAcc_Y\tAcc_Z\tRot_X\tRot_Y\tRot_Z\tTemp\tPre\tAlt\tTemps_s\tTemps_ms"
#define len_packet 9

#define SD_pin 6
#define LORA_pin 5
#define Optocoupleur_Pin 4

#define If_Octocoupleur false



/*  Declare the variables  */


bool Etat_vol;

unsigned long timer_info;
unsigned long count_ms;
unsigned long count_s;
unsigned long num_packet;

unsigned int pression_sea;
long pressure;
double altitude;
double Altitude_base;

File myFile;
MPU6050 mpu;
BMP180 bmp;
LoRa lora;


/*  Initialisation  */

void Init_Timer(){
  /*
  Initialisation du Timer, 
  le timer est fait pour une intervalle de 1 seconde,
  une interruption pour l'update se fait toutes les 1ms.

  Pour comprendre, faut se référer à la 
    Datasheet ATMEGA48A
  l'init du timer se fait par les registres de ce micro-processeur
  (Celui de l'Arduino Nano) Clk = 16 MHz
  
  Nous allons utilisez le Timer 0 pour nous faire notre horloge.
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

  // Serial.begin(9600);

  mpu.begin();
  bmp.begin();

  lora.begin(Freq_LORA, LORA_pin);
  lora.beginPacket();
  lora.println(tete); /*Impression de l'en tête pour une meilleure compréhension de l'utilisateur*/
  lora.endPacket();

  SD.begin(SD_pin);
  myFile = SD.open(path, FILE_WRITE);
  myFile.println(tete);
  myFile.close();

  num_packet = 0;
}

void Verif_Sol(){
  /*
  Fonction qui permet de gérer entièrement l'état d'avancement de la fusée
  lorsqu'elle est au sol (Etat_sol de 1 à 3). 
  */
  if(digitalRead(Optocoupleur_Pin)==HIGH) Etat_vol = true;
}

void Transfert_Info(int nb_ms){
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
    num_packet ++;

    bmp.read_sensor();
    mpu.read_sensor();
    
    lora.beginPacket();
    lora.print(num_packet); lora.print('\t');
    lora.print(mpu.accZ); lora.print('\t');
    lora.print(mpu.accY); lora.print('\t');
    lora.print(mpu.accX); lora.print('\t');
    lora.print(mpu.gyroZ); lora.print('\t');
    lora.print(mpu.gyroY); lora.print('\t');
    lora.print(mpu.gyroX); lora.print('\t');
    lora.print(mpu.temperature); lora.print('\t');
    lora.print(bmp.Pressure); lora.print('\t');
    lora.print(bmp.Altitude); lora.print('\t');
    lora.print(count_s); lora.print('\t');
    lora.print(count_ms); lora.print('\n');
    lora.endPacket();

    myFile = SD.open(path,FILE_WRITE);
    myFile.print(num_packet); myFile.print('\t');
    myFile.print(mpu.accZ); myFile.print('\t');
    myFile.print(mpu.accY); myFile.print('\t');
    myFile.print(mpu.accX); myFile.print('\t');
    myFile.print(mpu.gyroZ); myFile.print('\t');
    myFile.print(mpu.gyroY); myFile.print('\t');
    myFile.print(mpu.gyroX); myFile.print('\t');
    myFile.print(mpu.temperature); myFile.print('\t');
    myFile.print(bmp.Pressure); myFile.print('\t');
    myFile.print(bmp.Altitude); myFile.print('\t');
    myFile.print(count_s); myFile.print('\t');
    myFile.print(count_ms); myFile.print('\n');
    myFile.close();
    /*
    Serial.print(num_packet); Serial.print('\t');
    Serial.print(mpu.accZ); Serial.print('\t');
    Serial.print(mpu.accY); Serial.print('\t');
    Serial.print(mpu.accX); Serial.print('\t');
    Serial.print(mpu.gyroZ); Serial.print('\t');
    Serial.print(mpu.gyroY); Serial.print('\t');
    Serial.print(mpu.gyroX); Serial.print('\t');
    Serial.print(mpu.temperature); Serial.print('\t');
    Serial.print(bmp.Pressure); Serial.print('\t');
    Serial.print(bmp.Altitude); Serial.print('\t');
    Serial.print(count_s); Serial.print('\t');
    Serial.print(count_ms); Serial.print('\n');
    */
  }
}

void Sol(){
  /*
  Si l'octocoupleur est utiliser, on se fit à lui
  pour passer en mode vol, sinon on y est de base
  */
  if (If_Octocoupleur = true) Etat_vol = false;
  else Etat_vol = true;
  while (!Etat_vol){
    Verif_Sol();
    Transfert_Info(DELAY_INFO_SOL);
  }
}

void Vol(){
  /*
  Fonction de la fusée lorsqu'elle est en vol.
  Toutes les 300ms on envoi des infos des capteurs
  Quand la carte reçoit un signal d'arrêt elle enregistre le nombre de mesures et arrête les mesures
  while(timer_ms<){
  }/
  */
  while(1) Transfert_Info(DELAY_INFO_VOL);
}

void setup() {
  Init_Timer();
  Init_Sensor();
  
  count_ms = 0;
  count_s = 0;
  
  /*  Main program  */
  Sol();
  Vol();
}


void loop() {}

ISR(TIMER0_COMPA_vect){    //This  is the interrupt request
  count_ms++;
  timer_info++;
}
