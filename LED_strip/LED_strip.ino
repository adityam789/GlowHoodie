#include <Adafruit_NeoPixel.h>
#define LED_PIN     12
#define LED_COUNT   31
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();           // INITIALIZE NeoPixel strip object
  strip.show();            // Turn OFF all pixels
  strip.setBrightness(50); // Set BRIGHTNESS of all pixels(max = 255)
}

void loop() {
  colorWipe(strip.Color(255,   0,   0), 50); // Red
  colorWipe(strip.Color(0,   255,   0), 50); // Green
  colorWipe(strip.Color(0,   0,   255), 50); // Blue
  colorWipe(strip.Color(0,   0,   0), 50); // Off
}

// Helper functions (mostly for animations)

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
