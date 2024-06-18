/*
Programme Arduino
Nom : Main.ino
Auteur : Roche Corentin
Version : 4
Date : 20/01/2023


*/
#include <Servo.h>
#include <Wire.h>
#include <SPI.h>

#define _BV(n) (1<<n)



/*  Define pins  */

#define Jack_pin 4

#define Led_pin_sol_1 6
#define Led_pin_sol_2 7
#define Switch_pin_sol_1 2
#define Switch_pin_sol_2 3
#define Opto_pin 1

#define Val_Timer 3000 // 3s ici

Servo servo;
#define Pin_Servo 10


/*  Declare the variables  */

int Etat_sol;
bool Etat_vol;

unsigned long timer_ms;
unsigned long timer_info;
unsigned long count_ms;
unsigned long count_s;

/*  Initialisation  */


void Init_Sol(){
  /* 
  Initialisation de la phase au sol
  
  Avec 2 LEDs, 2 Switchs et la prise Jack
  Configure les LEDs en état "avec initialisation" 
  (Etat_sol = 0)
  */
  pinMode(Led_pin_sol_1, OUTPUT);
  pinMode(Led_pin_sol_2, OUTPUT);
  pinMode(Opto_pin, OUTPUT);
  pinMode(Switch_pin_sol_1, INPUT_PULLUP);
  pinMode(Switch_pin_sol_2, INPUT_PULLUP);
  
  pinMode(Jack_pin, INPUT_PULLUP);

  Etat_sol = 0;
  write_LED_Sol();
}

void Init_Vol(){
  /* 
  Initialisation de la phase de vol
  
  Avec 2 Servomoteur
  (Etat_vol = 0)
  */

  servo.attach(Pin_Servo);
  servo.write(0);

  Etat_vol = false;
}

int read_pin(){
  if(!digitalRead(Switch_pin_sol_1)&&!digitalRead(Switch_pin_sol_2)){
    return 1;
  }
  if(digitalRead(Switch_pin_sol_1)&&!digitalRead(Switch_pin_sol_2)){
    return 2;
  }
  if(digitalRead(Switch_pin_sol_1)&&digitalRead(Switch_pin_sol_2)){
    return 3;
  }
  if(!digitalRead(Switch_pin_sol_1)&&digitalRead(Switch_pin_sol_2)){
    return 0;
  }
}

void bouger_servo(int states){
  if (states >=2){
    servo.write(90);
  }
  else{
    servo.write(0);
  }
}

void Verif_Sol(){
  /*
  Fonction qui permet de gérer entièrement l'état d'avancement de la fusée
  lorsquel est au sol (Etat_sol de 1 à 3).
  
  Etat_sol = 1 
      LED 1 --> Alumé
      LED 2 --> Eteinte
      Servo --> 0
  Etat_sol = 2 (si prise Jack branché)
      LED 1 --> Eteinte
      LED 2 --> Alumé
      Servo --> 90
  Etat_sol = 3 (dernière validation)
      LED 1 --> Alumé
      LED 2 --> Alumé
      Servo --> 90
  Lorsque le Jack et débranché, Etat_vol = True
  */
  Etat_sol = read_pin();
  if (Etat_sol == 3 && digitalRead(Jack_pin)){Etat_vol = true;Serial.println("decollage");}
  write_LED_Sol();
  bouger_servo(Etat_sol);
}

void Ouverture_porte(){
  /*
  Fonction qui permet d'ouvrir la trappe tout en transmettant 
  les données des capteurs

  Ouverture du servo (locket)
  Puis 
  */
  timer_ms = 0;
  while (timer_ms < 500) {
    servo.write(0);
  }
}

void Emission(){
  digitalWrite(Opto_pin, HIGH);
}

void Sol(){
  /*
  Fonction de la fusée lorsqu'elle est au sol.
  Et lorsque que l'initialisation est faite.
  S'arrete lorsque la prise jack (ici le 3e switch) est arraché
    Etat_vol devient True

    Plusieurs phase sont considéré : 
      - Phase 1 : 
        LED 1 -> Allumée
        En attante de pose de la porte
        Une fois la porte monté, appuyer sur le switch 1
      Entre P1 et P2, Servo se ferme

      - Phase 2 :
        LED 2 -> Allumée
        En attente d'une dernière validation (porte bien fermé)
        Appuyer sur le switch 2
      - Phase 3 :
        LED 1 et LED 2 -> Allumée
        En attante de lancement (arrachement de la prise jack)
  */
  Etat_vol = false;
  timer_ms = 0;
  while (!Etat_vol){
    Verif_Sol();
  }
  Emission()
}

void Vol(){
  /*
  Fonction de la fusée lorsqu'elle est en vol.
  */
  timer_ms = 0;
  delay(Val_Timer);
  servo.write(0);
}

void write_LED_Sol(){
  /*
  Fonction qui gère l'état des LED en fonction de la variable Etat_sol
  */
  switch (Etat_sol) {
  case 1:
    digitalWrite(Led_pin_sol_1, HIGH);
    digitalWrite(Led_pin_sol_2, LOW);
    break;
  case 2:
    digitalWrite(Led_pin_sol_1, LOW);
    digitalWrite(Led_pin_sol_2, HIGH);
    break;
  case 3:
    digitalWrite(Led_pin_sol_1, HIGH);
    digitalWrite(Led_pin_sol_2, HIGH);
    break;
  default:
    digitalWrite(Led_pin_sol_1, LOW);
    digitalWrite(Led_pin_sol_2, LOW);
    break;
  }
}

void setup() {
  Serial.begin(9600);
  Init_Sol();
  Init_Vol();


  while(digitalRead(Switch_pin_sol_1) || digitalRead(Switch_pin_sol_2));
  Etat_sol = 1;
  count_ms = 0;
  count_s = 0;
  write_LED_Sol();
  
  /*  Main program  */
  Sol();
  Vol();
}
void loop() {}
