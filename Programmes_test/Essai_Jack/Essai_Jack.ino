#define _BV(n) (1<<n)

#define Jack_pin PD4 // pin 4

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(6, OUTPUT);

  digitalWrite(6, LOW);
}

void loop() {
  if (digitalRead(2)) digitalWrite(6, HIGH);
  else digitalWrite(6, LOW);
  delay(100);
}


