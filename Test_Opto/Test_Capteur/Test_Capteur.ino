#define pin_opto 6

void setup(){
    Serial.begin(9600);
    pinMode(pin_opto, INPUT_PULLUP);
}

void loop(){
    if(digitalRead(pin_opto)){
        Serial.println("LED allumee");
    }
    else{
        Serial.println("LED eteinte");
    }
}