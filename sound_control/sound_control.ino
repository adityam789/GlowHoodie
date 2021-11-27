uint8_t soundSensorPin=15;
uint8_t neoPin = 2;
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
//  uint16_t val = analogRead(soundSensorPin);
//  unsigned long now = millis();
//  Serial.println(val);
//  add debouncing and LED color control
//  delay(10);
  if(val >= 2500 && now > lastSound + 500) {
//    Serial.print(now);
//    Serial.print(": ");
//    Serial.println("I heard it");
//    digitalWrite(neoPin,!digitalRead(neoPin)); // toggles the neopixel
//    Serial.println(sizeof(uint32_t));
//    lastSound = now;
    Serial.print("in loop: ");
    Serial.println(millis());
    delay(1000);
  }
} 
