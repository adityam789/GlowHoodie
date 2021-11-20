#include <Adafruit_NeoPixel.h>
#define LED_PIN     12
#define LED_COUNT   31
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();           // INITIALIZE NeoPixel strip object
  strip.show();            // Turn OFF all pixels
  strip.setBrightness(20); // Set BRIGHTNESS of all pixels(max = 255)
}

// Global Variables for helper functions

unsigned long prev = 0;

// Helper functions (mostly for animations)

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

//send a color segment forward (color: color, length of segment: len, speed: wait).
void colorSeg(uint32_t color, int len, int wait) {
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

// bounce light segment (forward and back once) (color: color, length of segment: len, speed: wait).
void colorBounce(uint32_t color, int len, int wait) {
  colorSeg(color, len, wait);  // send a segment forward
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
}

void loop() {
  colorWipe(strip.Color(255,   0,   0), 50); // Red
  colorWipe(strip.Color(0,   255,   0), 50); // Green
  colorWipe(strip.Color(0,   0,   255), 50); // Blue
  colorWipe(strip.Color(0,   0,   0), 50); // Off
  colorSeg(strip.Color(255,   0,   0), 3, 50); // Red
  colorSeg(strip.Color(0,   255,   0), 3, 50); // Green
  colorSeg(strip.Color(0,   0,   255), 3, 50); // Blue
  colorBounce(strip.Color(255,   0,   0), 3, 50); // Red
  colorBounce(strip.Color(0,   255,   0), 3, 50); // Green
  colorBounce(strip.Color(0,   0,   255), 3, 50); // Blue
}
