#define INIT '1'

char c;

void setup() {
    Serial.begin(115200);
    Serial.flush();

    while(Serial.available() == 0);
    c = Serial.read();
    if(c == INIT) Serial.write(INIT);
}



void loop() {
    if(Serial.available()>0){
        c = Serial.read();
        Serial.write(c);
    }
}