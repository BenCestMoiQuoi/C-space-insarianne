#define _BV(n) (1<<n)
#define Val_Timer 3000 // 3s ici
#define Val_delay 5

#define Pin_Jack 4


unsigned long timer;

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
  OCR0A = 249; // = (16*10^6) / (1000*64) - 1 (must be <256)
  

  sei(); //Enable interrupt
  // Init variable timer
  timer = 0;
}

void setup() {
  pinMode(Pin_Jack, INPUT_PULLUP);

  Init_Timer();

  Serial.begin(9600);
}

void loop() {
  Serial.println(digitalRead(Pin_Jack));
  Prog();
}

void Prog(){
  Serial.println("Attente");
  while (digitalRead(Pin_Jack));
  Serial.println("Initialisation");
  timer = 0;
  while (timer <= Val_delay) Serial.println(timer);
  Serial.println("Pret");
  while (!digitalRead(Pin_Jack));
  timer = 0;
  Serial.println("Lancement");
  while (timer <= Val_Timer) Serial.println(timer);
  Serial.println("Ouverture Porte");
}

ISR(TIMER0_COMPA_vect){    //This  is the interrupt request
  timer++;
}
