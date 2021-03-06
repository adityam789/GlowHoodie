// self defined library import area
#include "./library/retro2.h"
#include "./library/wreakralph2.h"
#include "./library/rickroll.h"
#include "./library/mario2.h"

// Arduino library import area
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Chrono.h>

// MACRO definition area
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif
#define MATRIX_PIN 14
#define LED_PIN     12
#define LED_COUNT   144
#define SOUND_PIN   15

// Master Global Variable Area
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16,16, MATRIX_PIN, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
const uint16_t matrix_colors[] = {matrix.Color(150, 0, 0), matrix.Color(0, 150, 0), matrix.Color(0, 0, 150) };
uint32_t color_seq[3] = {
    strip.Color(150, 0,  0),  // red
    strip.Color(0, 150,  0),  // green
    strip.Color(0,   0,150)   // blue
    };  // color sequence for LED strip
int color_seq_len = sizeof(color_seq)/4;  // each color is 4bytes(32bits)

struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

void setup() {
  Serial.begin(9600);
	matrix.begin();
	matrix.setTextWrap(false);
	matrix.setBrightness(16);
	matrix.setTextSize(1);
	matrix.setTextColor(matrix_colors[0]);
  strip.begin();           // INITIALIZE NeoPixel strip object
  strip.show();            // Turn OFF all pixels
  strip.setBrightness(5); // Set BRIGHTNESS of all pixels(max = 255)
  pinMode(SOUND_PIN,INPUT);  // initialize sound sensor pin
}

struct RGB colorConverter(int hexValue)
{
  struct RGB rgbColor;
  rgbColor.r = ((hexValue >> 16) & 0xFF);  // Extract the RR byte
  rgbColor.g = ((hexValue >> 8) & 0xFF);   // Extract the GG byte
  rgbColor.b = ((hexValue) & 0xFF);        // Extract the BB byte

  return rgbColor;
}


int x = matrix.width();
int pass = 0;
char lines[][100] = {"Impossible is Nothing","I am Ironman"};
// forever loop display text from lines
void display_text() {
	while(1) {
		matrix.fillScreen(0);
		matrix.setCursor(x, 4);
		matrix.print(F(lines[1]));
		if(--x < -6*((int)strlen(lines[1]))) {
			x = matrix.width();
			if(++pass >= 3) pass = 0;
			matrix.setTextColor(matrix_colors[pass]);
		}
		matrix.show();
		delay(100);
	}
}

byte FrameNumber = 0;
void wreckralph() {
  FrameNumber = 0;
  Chrono chrono;
	while(1){
		if (chrono.hasPassed(72)) {
			chrono.restart();
			TimerEventRalph();
  	}
	}
}
void mario() {
  FrameNumber = 0;
  Chrono chrono;
	while(1){
		if (chrono.hasPassed(72)) {
			chrono.restart();
			TimerEventMario();
  	}
	}
}
void rickroll() {
  FrameNumber = 0;
  Chrono chrono;
	while(1){
		if (chrono.hasPassed(72)) {
			chrono.restart();
			TimerEventRickRoll();
  	}
	}
}
void retro(int id) {
  FrameNumber = 0;
  Chrono chrono;
  while(1){
    if (chrono.hasPassed(500)) {
      chrono.restart();
      TimerEventRetro(id);
  	}
  }
}
uint16_t drawRGB24toRGB565(byte r, byte g, byte b) {
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

void TimerEventRalph() {
  for (byte y = 0; y < 16; y++) {
      for (byte x = 0; x < 16; x++) {
          byte loc = x + y*16;
          matrix.drawPixel(x, y, drawRGB24toRGB565((wreakralphFrames[3 * FrameNumber][loc]), (wreakralphFrames[3 * FrameNumber + 1][loc]), (wreakralphFrames[3 * FrameNumber + 2][loc])));
      }
  }
  if(FrameNumber == 3) {
      FrameNumber = 0;
  } else {
      FrameNumber++;
  }
  Serial.println("Frame completed");
  matrix.show();
}

void TimerEventMario() {
  for (byte y = 0; y < 16; y++) {
    for (byte x = 0; x < 16; x++) {
      byte loc = x + y*16;
      matrix.drawPixel(x, y, drawRGB24toRGB565((marioNewFrames[3 * FrameNumber][loc]), (marioNewFrames[3 * FrameNumber + 1][loc]), (marioNewFrames[3 * FrameNumber + 2][loc])));
    }
  }
  if (FrameNumber == 2) {
    FrameNumber = 0;
  } else {
    FrameNumber++;
  }
  Serial.println("Frame completed");
  matrix.show();
}

void TimerEventRickRoll() {
  for (byte y = 0; y < 16; y++) {
    for (byte x = 0; x < 16; x++) {
      byte loc = x + y * 16;
      matrix.drawPixel(x, y, drawRGB24toRGB565((rickrollFrames[3 * FrameNumber][loc]), (rickrollFrames[3 * FrameNumber + 1][loc]), (rickrollFrames[3 * FrameNumber + 2][loc])));
    }
  }
  if(FrameNumber == 26){
    FrameNumber = 0;  
  }
  else{
    FrameNumber += 1;  
  }
  Serial.println(" Frame completed");
  matrix.show();
}

void TimerEventRetro(int character_id){
  matrix.clear();
  for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
          if (i % 2 == 0) {
              struct RGB c = colorConverter(pgm_read_dword(&(retros[2 * character_id + FrameNumber][16 * i + 15 - j])));
              uint16_t c_u = matrix.Color(c.r, c.g, c.b);
              Serial.println(c.r);
              matrix.drawPixel(j, i, matrix.Color(c.r, c.g, c.b));
          }   
          else {
              struct RGB c = colorConverter(pgm_read_dword(&(retros[2 * character_id + FrameNumber][16 * i + j])));
              uint16_t c_u = matrix.Color(c.r, c.g, c.b);
              Serial.println(c.r);
              matrix.drawPixel(j, i, matrix.Color(c.r, c.g, c.b));
          }
      }
  }
  FrameNumber = 1 - FrameNumber;
  matrix.show();
  //delay(500);
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
	// display_text();
  // retro(0);
    // id = 0 for display_BombJack(); 
    // id = 1 for display_Qbert();  
    // id = 2 for display_DigDug(); 
  // mario();
  // rickroll();
  // wreckralph();
  // colorWipe();
  // colorSeg();
  // colorBounce();
  // rainbow();
  // soundControlWipe();
}
