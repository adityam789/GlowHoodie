int soundSensorPin=15;
int neoPin = 2;
unsigned long lastSound = 0;

void setup() {
  pinMode(soundSensorPin,INPUT);
  pinMode(neoPin, OUTPUT);
  Serial.begin(9600);
//  attachInterrupt(digitalPinToInterrupt(soundSensorPin), isr, RISING);
}

//void isr() {
//  Serial.print("isr triggered at: ");
//  uint16_t val = analogRead(soundSensorPin);
//  unsigned long now = millis();
//  Serial.println(now);
//  if(val >= 2500 && now > lastSound + 500) {
//    Serial.print("val triggered: ");
//    Serial.println(val);
//  }
//}

void loop() {
  Serial.println(analogRead(soundSensorPin));
//  unsigned long val = analogRead(15);
//  unsigned long now = millis();
//  Serial.println(val);
//  delay(10);
//  Serial.print(" ");
//  Serial.print(1500);
//  Serial.print(" ");
//  Serial.println(2100);
//  add debouncing and LED color control
//  if((val >= 2200 || val <= 1200) && now > lastSound + 1000) {
//    Serial.print(now);
//    Serial.print(": ");
//    Serial.println("I heard it");
//    digitalWrite(neoPin,!digitalRead(neoPin)); // toggles the neopixel
//    Serial.println(sizeof(uint32_t));
//    lastSound = now;
//    delay(10);
//    Serial.print("in loop: ");
//    Serial.println(millis());
//    delay(1000);
//  }
} 
