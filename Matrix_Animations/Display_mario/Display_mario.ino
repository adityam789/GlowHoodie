#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Chrono.h>
#include "./library/marioheader.h"
#ifndef PSTR
  #define PSTR
#endif
#define PIN 14
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16,16, PIN, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
Chrono chrono;
byte FrameNumber = 0;
void setup() {
  Serial.begin(9600);
  matrix.begin();
  matrix.setBrightness(16);
  matrix.fillScreen(0);
  matrix.show();
  delay(100);
}
void loop() {
  if (chrono.hasPassed(72)) {
    chrono.restart();
    TimerEvent();
  }
}
uint16_t drawRGB24toRGB565(byte r, byte g, byte b) {
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}
void TimerEvent() {
  if (FrameNumber == 0) {
      for (byte y = 0; y < 16; y++) {
        for (byte x = 0; x < 16; x++) {
          byte loc = x + y*16;
          matrix.drawPixel(x, y, drawRGB24toRGB565((marioNewRedFrame0[loc]), (marioNewGreenFrame0[loc]), (marioNewBlueFrame0[loc])));
        }
      }
      FrameNumber = 1;
      Serial.println(" Frame 0 completed");
  }
   else if (FrameNumber == 1) {
      for (byte y = 0; y < 16; y++) {
        for (byte x = 0; x < 16; x++) {
          byte loc = x + y*16;
          matrix.drawPixel(x, y, drawRGB24toRGB565((marioNewRedFrame1[loc]), (marioNewGreenFrame1[loc]), (marioNewBlueFrame1[loc])));
        }
      }
      FrameNumber = 2;
      Serial.println(" Frame 1 completed");
  }
   else if (FrameNumber == 2) {
      for (byte y = 0; y < 16; y++) {
        for (byte x = 0; x < 16; x++) {
          byte loc = x + y*16;
          matrix.drawPixel(x, y, drawRGB24toRGB565((marioNewRedFrame2[loc]), (marioNewGreenFrame2[loc]), (marioNewBlueFrame2[loc])));
        }
      }
      FrameNumber = 0;
  }
  matrix.show();
}
