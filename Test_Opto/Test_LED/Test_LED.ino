#define pin_switch 2
#define pin_opto 1
#define pin_LED 6

void setup(){
    pinMode(pin_switch, INPUT_PULLUP);
    pinMode(Opto_pin, OUTPUT);
}

void loop(){
    if(digitalRead(pin_switch)){
        digitalWrite(pin_switch, HIGH);
        digitalWrite(pin_LED, HIGH);
        }
    else{
        digitalWrite(pin_switch, LOW);
        digitalWrite(pin_LED, LOW);
    }
}