#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Metro.h>
#ifndef PSTR
  #define PSTR
#endif
#define PIN 14
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16,16, PIN, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
const unsigned char colorswitchframes[300] = {
  220, 20, 60, 221, 26, 69, 222, 31, 78, 223, 37, 87, 224, 42, 96, 225, 48, 105, 225, 53, 114, 226, 59, 123, 227, 64, 132, 228, 70, 141, 229, 76, 150, 230, 81, 159, 231, 87, 168, 232, 92, 177, 233, 98, 186, 234, 103, 195, 235, 109,
  204, 235, 114, 213, 236, 120, 222, 237, 126, 231, 236, 129, 238, 224, 122, 239, 212, 116, 240, 200, 109, 241, 188, 102, 242, 175, 96, 242, 163, 89, 243, 151, 83, 244, 139, 76, 245, 127, 70, 246, 115, 63, 247, 103, 56, 248, 91, 50,
  248, 79, 43, 249, 67, 37, 250, 55, 30, 251, 43, 24, 252, 31, 17, 253, 19, 11, 254, 7, 4, 254, 0, 3, 250, 0, 9, 237, 0, 16, 224, 0, 22, 211, 0, 28, 198, 0, 35, 185, 0, 41, 173, 0, 48, 160, 0, 54, 147, 0, 61, 134, 0, 67, 121, 0, 74,
  108, 0, 80, 95, 0, 87, 82, 0, 93, 70, 0, 100, 57, 0, 106, 44, 0, 112, 31, 0,
  119, 18, 0, 125, 5, 8, 132, 0, 21, 138, 0, 33, 145, 0, 46, 151, 0, 59, 158, 0, 72, 164, 0, 85, 170, 0, 98, 177, 0, 111, 183, 0, 124, 190, 0, 137, 196, 0,
  149, 202, 0, 162, 209, 0, 175, 215, 0, 188, 222, 0, 201, 228, 0, 214, 234, 0, 227, 241, 0, 240, 247, 0, 252, 254, 0, 254, 246, 2, 252, 234, 5, 250, 222, 8, 248, 210, 12, 247, 198, 15, 245, 186, 18, 243, 174, 21, 241, 162, 24, 239,
  151, 27, 238, 139, 30, 236, 127, 33, 234, 115, 36, 232, 103, 39, 231, 91, 42, 229, 79, 45, 227, 67, 48, 225, 56, 51, 224, 44, 54, 222, 32, 57, 220, 20, 60
};
Metro AnimateTimer = Metro(72);
byte FrameNumber = 0;
void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.setBrightness(40);
  matrix.fillScreen(0);
  matrix.show();
  delay(100);
}
void loop() {
  if (AnimateTimer.check() == 1) {
    TimerEvent();
  }
}
uint16_t drawRGB24toRGB565(byte r, byte g, byte b) {
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}
void TimerEvent() {
    z = FrameNumber;
    for (byte y = 0; y < 16; y++) {
        for (byte x = 0; x < 16; x++) {
            matrix.drawPixel(x, y, drawRGB24toRGB565((colorswitchframes[3 * z]), (colorswitchframes[3 * z + 1]), (colorswitchframes[3 * z + 2])));
        }
    }
    if(FrameNumber == 99) {
      FrameNumber = 0;
    } else {
      FrameNumber++;
    }
    Serial.println("Frame completed");
    matrix.show();
}