void setup() {
  // put your setup code here, to run once:
  pinMode(27, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(27, HIGH);
  delay(10000);
  digitalWrite(27, LOW);
  delay(10000);
}
