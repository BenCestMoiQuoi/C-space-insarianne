/*
Programme Arduino
Nom : Autonome.ino
Auteur : Roche Corentin (@BenCestMoiQuoi)
Léo-Paul You (@Lyouu)
Version : 1
Date : 07/07/2024
*/

/*  Includes  */

#include <Servo.h>

/*  Define pins  */

#define Jack_pin 4

#define Led_pin_sol_1 6
#define Led_pin_sol_2 7
#define Switch_pin_sol_1 2
#define Switch_pin_sol_2 3
#define Optocoupleur_pin 8

#define Val_Timer 3000 // 3s ici

#define Pin_Servo 10


/*  Declare the variables  */

int Etat_sol;
bool Etat_vol;

Servo servo;


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
  pinMode(Switch_pin_sol_1, INPUT_PULLUP);
  pinMode(Switch_pin_sol_2, INPUT_PULLUP);
  
  pinMode(Jack_pin, INPUT_PULLUP);
  pinMode(Optocoupleur_pin, OUTPUT);

  digitalWrite(Optocoupleur_pin, LOW);
  Etat_sol = 0;
  write_LED_Sol();
}

void Init_Vol(){
  /* 
  Initialisation de la phase de vol
  
  Avec 1 Servomoteur
  (Etat_vol = 0)
  */

  servo.attach(Pin_Servo);
  servo.write(0);

  Etat_vol = false;
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
      Servo --> 180
  Etat_sol = 3 (dernière validation)
      LED 1 --> Alumé
      LED 2 --> Alumé
      Servo --> 180
  Lorsque le Jack et débranché, Etat_vol = True
  */
  if (!digitalRead(Switch_pin_sol_1) && (Etat_sol == 2 || Etat_sol == 3)){
    Etat_sol = 1;
    write_LED_Sol();
    servo.write(0);
  }
  if (digitalRead(Switch_pin_sol_1) && Etat_sol == 1) {
    servo.write(90);
  }
  if (Etat_sol == 1 && digitalRead(Switch_pin_sol_1) && !digitalRead(Switch_pin_sol_2) 
        && !digitalRead(Jack_pin) && servo.read()==90){
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
    Etat_sol = 0;
    write_LED_Sol(); 
  }
}

void Ouverture_porte(){
  /*
  Fonction qui permet d'ouvrir la trappe
  Ouverture du servo (locket)
  */

  servo.write(0);
}

void Sol(){
  /*
  Fonction de la fusée lorsqu'elle est au sol.
  Et lorsque que l'initialisation est faite.
  S'arrete lorsque la prise jack (ici le 3e switch) est arraché.
  */
 
  Etat_vol = false;
  while (!Etat_vol){
    Verif_Sol();
  }
}

void Vol(){
  /*
  Fonction de la fusée lorsqu'elle est en vol.
  Attente du timmer avant d'ouvrir la porte.
  */

  delay(Val_Timer);
  Ouverture_porte();
  delay(Val_Timer);
  servo.detach();
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
  Init_Sol();
  Init_Vol();

  while(digitalRead(Switch_pin_sol_1) || digitalRead(Switch_pin_sol_2));
  
  Etat_sol = 1;
  write_LED_Sol();
  
  /*  Main program  */
  Sol();
  Vol();
}

void loop() {}