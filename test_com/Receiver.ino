
typedef enum {AX,AY,AZ,RX,RY,RZ,P,T,ALT} index;
int progress =0;
float values[9];


void setup() {
  Serial.begin(38400);
}

void loop() {
  if (Serial.available() >= sizeof(values)) {
    Serial.readBytes((char*)values, sizeof(values));
    Serial.print("Ax = ");
    Serial.print(values[AX]);
    Serial.print(" Ay = ");
    Serial.print(values[AY]);
    Serial.print(" Az = ");
    Serial.println(values[AZ]);
    Serial.print("Rx = ");
    Serial.print(values[RX]);
    Serial.print(" Ry = ");
    Serial.print(values[RY]);
    Serial.print(" Rz = ");
    Serial.print(values[RZ]);
    Serial.print("P = ");
    Serial.println(values[P]);
    Serial.print("T = ");
    Serial.println(values[T]);
    Serial.print("Alt = ");
    Serial.println(values[ALT]);
    }
}
