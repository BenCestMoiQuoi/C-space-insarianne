#define Led_pin 8
#define Button_pin 10

#define _BV(n) (1<<n)

int timer_1s = 0;
int timer = 0;

void Init_Timer(){
  /*  Set the CTC mode  
      TCCR0A = 0b0000xx10
  TCCR0A |= 0B00000010;
  TCCR0A &= 0B00001110;
  */ 
  TCCR0A |= _BV(WGM01); 
  TCCR0B|= _BV(CS01) | _BV(CS00);    //Set the prescale 1/64 clock
  
  TIMSK0 |= _BV(OCIE0A);   //Set  the interrupt request
  sei(); //Enable interrupt
  
  OCR0A=0xF9; //Value for ORC0A for 1ms
}

void setup() {
  pinMode(Button_pin, INPUT_PULLUP);
  pinMode(Led_pin, OUTPUT);
  digitalWrite(Led_pin, LOW);

  Init_Timer();
  
  Serial.begin(9600);
}

void loop() {
  Serial.println(digitalRead(Button_pin));
  while (digitalRead(Button_pin));
  timer = 0;
  digitalWrite(Led_pin, LOW);
  while (timer<=3) {
    timer_1s = 0;
    while (timer_1s<=1000) Serial.print("");
    timer++;
  }
  digitalWrite(Led_pin, HIGH);
  Serial.println("Ouverture porte")
  while(1);
}


ISR(TIMER0_COMPA_vect){    //This  is the interrupt request
  timer_1s++;
}
