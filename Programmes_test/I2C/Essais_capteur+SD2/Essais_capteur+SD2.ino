#include <Adafruit_BMP085.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#define _BV(n) (1<<n)

File myFile;
#define SD_pin 5

Adafruit_BMP085 bmp;

unsigned long timer_ms;
unsigned long timer_info;
unsigned long count_ms;
unsigned long count_s;

float pre, alt;

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
  myFile = SD.open("test.txt", FILE_WRITE);

  bmp.begin();
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

    Serial.print(count_s);
    Serial.print(":");
    Serial.println(count_ms);
    
    pre = bmp.readPressure();
    alt = bmp.readAltitude(PRESSION_MER);

    Serial.print("Pressure = ");myFile.print("Pressure = ");
    Serial.print(pre);myFile.print(pre);
    Serial.println(" Pa");myFile.println(" Pa");
    Serial.print("Altitude = ");myFile.print("Altitude = ");
    Serial.print(alt);myFile.print(alt);
    Serial.println(" m");myFile.println(" m");

    Serial.println();myFile.println();
  }
}

void setup() {
  Serial.begin(9600);

  Init_Timer();
  Init_Sensor();
  
  Serial.println("Initialisation Finis");
  count_ms = 0;
  count_s = 0;
}

void loop() {
    Transfert_Info(500);
}

ISR(TIMER0_COMPA_vect){    //This  is the interrupt request
  timer_ms++;
  count_ms++;
  timer_info++;
}
