#include <Adafruit_NeoPixel.h>
#define LED_PIN     12
#define LED_COUNT   31    // actual: 144
#define SOUND_PIN   15
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint32_t color_seq[3] = {
    strip.Color(150, 0,  0),  // red
    strip.Color(0, 150,  0),  // green
    strip.Color(0,   0,150)   // blue
    };  // color sequence
int color_seq_len = sizeof(color_seq)/4;  // each color is 4bytes(32bits)

void setup() {
  strip.begin();           // INITIALIZE NeoPixel strip object
  strip.show();            // Turn OFF all pixels
  strip.setBrightness(5); // Set BRIGHTNESS of all pixels(max = 255)
  pinMode(SOUND_PIN,INPUT);  // initialize sound sensor pin
}

// Global Variables for helper functions

unsigned long prev = 0;
unsigned long lastSound = 0;
int color_ind = 0;
int seg_len = 5;              // length of color segment (30 by default)

// Helper functions (mostly for animations)

void colorWipe() {
  for(int i=0; i<color_seq_len; i++) {
    colorWipeHelper(color_seq[i], 20);
  }
}

void colorWipeHelper(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void colorSeg() {
  for(int i=0; i<color_seq_len; i++) {
    colorSegHelper(color_seq[i], seg_len, 20);
  }
}

//send a color segment forward (color: color, length of segment: len, speed: wait).
void colorSegHelper(uint32_t color, int len, int wait) {
  for(int i=0; i<len; i++) { // initialize segment
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
  for(int i=len; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.setPixelColor(i-len, strip.Color(0,   0,   0));
    strip.show();
    delay(wait);
  }
  for(int i=strip.numPixels()-len; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0,   0,   0));
    strip.show();
    delay(wait);
  }
}

void colorBounce() {
  for(int i=0; i<color_seq_len; i++) {
    colorBounceHelper(color_seq[i], seg_len, 20);
  }
}

// bounce light segment (forward and back once) (color: color, length of segment: len, speed: wait).
void colorBounceHelper(uint32_t color, int len, int wait) {
  colorSegHelper(color, len, wait);  // send a segment forward
  for(int i=strip.numPixels()-1; i>strip.numPixels()-len; i--) { // initialize segment
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
  for(int i=strip.numPixels()-len-1; i>-1; i--) {
    strip.setPixelColor(i, color);
    strip.setPixelColor(i+len, strip.Color(0,   0,   0));
    strip.show();
    delay(wait);
  }
  for(int i=len; i>=0; i--) {
    strip.setPixelColor(i, strip.Color(0,   0,   0));
    strip.show();
    delay(wait);
  }
}

void rainbow() {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i*1+j) & 255));
    }
    strip.show();
    delay(20);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


void soundControlWipe() {
  int val = analogRead(SOUND_PIN);
  unsigned long now = millis();
  if(val >= 2700 && now > lastSound + 250) {
    colorWipeHelper(color_seq[color_ind], 20);
    color_ind += 1;
    if(color_ind == color_seq_len) {
      color_ind = 0;
    }
  }
}

void loop() {
//  colorWipe();
//  colorSeg();
//  colorBounce();
//  rainbow();
//  soundControlWipe();
}
