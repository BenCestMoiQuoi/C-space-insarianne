/*
Programme Arduino
Nom : Main.ino
Auteur : Roche Corentin
Version : 1
Date : 30/11/2023

Programme regroupant : 
  - Timer (v1 - 30/11/2023)
  - Sol (v1 - 30/11/2023)

*/

#define _BV(n) (1<<n)


/*  Define pins  */

#define Jack_pin 4

#define Led_pin_sol_1 6
#define Led_pin_sol_2 7
#define Switch_pin_sol_1 2
#define Switch_pin_sol_2 3

#define Val_Timer 3000 // 3s ici

/*  Declare the variables  */

int Etat_sol;
bool Etat_vol;

unsigned long timer_ms;

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
}

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

  Etat_sol = 0;
  write_LED_Sol();
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
  Serial.print(digitalRead(Switch_pin_sol_1));
  Serial.println(digitalRead(Switch_pin_sol_2));
  if (!digitalRead(Switch_pin_sol_1) && (Etat_sol == 2 || Etat_sol == 3)){
    Etat_sol = 1;
    write_LED_Sol();
  }
  if (Etat_sol == 1 && digitalRead(Switch_pin_sol_1) && !digitalRead(Switch_pin_sol_2) 
        && !digitalRead(Jack_pin)){
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
  if (Etat_sol == 3 && digitalRead(Jack_pin)) Etat_vol = true;
}

void Ouverture_porte(){
  /*
  Fonction qui permet d'ouvrir la trappe tout en transmettant 
  les données des capteurs

  Ouverture du servo haut (locket)
  Puis 
  */
  Etat_sol = 0;
  write_LED_Sol();
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
      Entre P1 et P2, Servo_Haut se ferme
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
  Serial.println("Décolage");
}

void Vol(){
  /*
  Fonction de la fusée lorsqu'elle est en vol.
  

  Toutes les 200ms on envoi des infos des capteurs
  */
  timer_ms = 0;
  while (timer_ms < Val_Timer) Serial.println(timer_ms);
  Ouverture_porte();
  Serial.println("Ouverture porte");
// Fin du vol
  while(1);
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
  Init_Timer();

  while(digitalRead(Switch_pin_sol_1) || digitalRead(Switch_pin_sol_2)) Serial.println("OUI");
  
  Serial.println("Initialisation Finis");
  Etat_sol = 1;
  write_LED_Sol();
  
  /*  Main program  */
  Sol();
  Vol();
}

void loop() {}

ISR(TIMER0_COMPA_vect){    //This  is the interrupt request
  timer_ms++;
}
