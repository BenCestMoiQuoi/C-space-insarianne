#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>


#define _BV(n) (1<<n)
#define PRESSION_MER 100810

int counter = 0;

Adafruit_BMP085 bmp; // Adress 0x77
Adafruit_MPU6050 mpu;// Adress 0x68

unsigned long timer_ms;
unsigned long timer_info;
unsigned long count_ms;
unsigned long count_s;

float pre, alt;
sensors_event_t acc, gyr, temp;

void setup() {
  Serial.begin(9600);
  
  Init_Timer();
  Init_Sensor();
  
  Serial.println("Initialisation Finie");
  count_ms = 0;
  count_s = 0;

  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

}

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

  mpu.begin();
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
    Serial.print("Donnée n°");
    Serial.println(counter);
    
    mpu.getEvent(&acc, &gyr, &temp);
    pre = bmp.readPressure();
    alt = bmp.readAltitude(PRESSION_MER);

    Serial.print("Acceleration X: ");
    Serial.print(acc.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(acc.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(acc.acceleration.z - 9.81);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(gyr.gyro.x);
    Serial.print(", Y: ");
    Serial.print(gyr.gyro.y);
    Serial.print(", Z: ");
    Serial.print(gyr.gyro.z);
    Serial.println(" rad/s");

    Serial.println();

    Serial.print("Température : ");
    Serial.print(temp.temperature);
    Serial.println(" Deg °C");

    Serial.println();

    Serial.print("Pression : ");
    Serial.print(pre);
    Serial.println(" Pa");
    Serial.print("Altitude : ");
    Serial.print(alt);
    Serial.println(" m");

    Serial.println();
//LoRa

    LoRa.beginPacket();

    LoRa.print(count_s);
    LoRa.print(":");
    LoRa.println(count_ms);
    LoRa.print("Donnée n°");
    LoRa.println(counter);

    LoRa.print("Acceleration X: ");
    LoRa.print(acc.acceleration.x);
    LoRa.print(", Y: ");
    LoRa.print(acc.acceleration.y);
    LoRa.print(", Z: ");
    LoRa.print(acc.acceleration.z - 9.81);
    LoRa.println(" m/s^2");

    LoRa.print("Rotation X: ");
    LoRa.print(gyr.gyro.x);
    LoRa.print(", Y: ");
    LoRa.print(gyr.gyro.y);
    LoRa.print(", Z: ");
    LoRa.print(gyr.gyro.z);
    LoRa.println(" rad/s");

    LoRa.println();

    LoRa.print("Température : ");
    LoRa.print(temp.temperature);
    LoRa.println(" Deg °C");

    LoRa.println();

    LoRa.print("Pression : ");
    LoRa.print(pre);
    LoRa.println(" Pa");
    LoRa.print("Altitude : ");
    LoRa.print(alt);
    LoRa.println(" m");

    LoRa.endPacket();

    counter++;
  }
  
}

void loop() {

  Transfert_Info(1000);

}

ISR(TIMER0_COMPA_vect){    //This  is the interrupt request
  timer_ms++;
  count_ms++;
  timer_info++;
}