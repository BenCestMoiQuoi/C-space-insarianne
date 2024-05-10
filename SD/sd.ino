#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

#define _BV(n) (1<<n)
#define PRESSION_MER 101350
#define w 1

Adafruit_BMP085 bmp; // Adress 0x77
Adafruit_MPU6050 mpu;// Adress 0x68

unsigned long timer_ms;
unsigned long timer_info;
unsigned long count_ms;
unsigned long count_s;

float pre, alt;
sensors_event_t acc, gyr, temp;

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
  */

  mpu.begin();
  bmp.begin();
}


//initialisation de la partie SD
#include <SPI.h>
#include <SD.h>

File dataFile;
const int chipSelect = 5; // numero pin
const char path[] = "TEST.tsv";
const char path2[] = "nb_loop.txt";
unsigned long nb_ms = 200;
long nb_loop = 0;
int writing = 0;

int stop(){
  return 0;
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
    
  mpu.getEvent(&acc, &gyr, &temp);
  pre = bmp.readPressure();
  alt = bmp.readAltitude(PRESSION_MER);
    
    
  //envoi des données sur la carte
  if(writing){
    float packet[] = {acc.acceleration.x,acc.acceleration.y,acc.acceleration.z,gyr.gyro.x,gyr.gyro.y,gyr.gyro.z,temp.temperature,pre,alt};
    //ecriture carte sd
    dataFile = SD.open(path,O_APPEND | O_WRITE);
    if (dataFile) {
      for(int i =0;i<9;i++){
            dataFile.print(packet[i]);
            dataFile.print("\t");
      }
      dataFile.print("\n");
      dataFile.close();
      nb_loop ++;
      }
    if(stop()){
      writing = 0;
      dataFile = SD.open(path2);
      dataFile.print(nb_loop);
      dataFile.close();
    }
  }
  }
}

void setup() {
  while(1){ // attente d'une carte SD
    if (!SD.begin(chipSelect)) {
      Serial.println("Carte SD non détectée.");
    }
    else{
      Serial.println("Carte SD prête à être utilisée.");
      break;
    }
  dataFile = SD.open(path);
  dataFile.println(nb_ms);
  dataFile.println("acceleration x\tacceleration y\t acceleration z\trotation x\t rotation y\t rotation z\ttemperature\tpression\taltitude");
  }

  Serial.begin(38400);

  Init_Timer();
  Init_Sensor();
  
  Serial.println("Initialisation Finie");
  count_ms = 0;
  count_s = 0;
}

void loop() {
  Transfert_Info(nb_ms);  

}

ISR(TIMER0_COMPA_vect){    //This  is the interrupt request
  timer_ms++;
  count_ms++;
  timer_info++;
}