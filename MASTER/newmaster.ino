// self defined library import area
#include "./library/retro2.h"
#include "./library/wreakralph2.h"
#include "./library/rickroll.h"
#include "./library/mario2.h"
#include "./library/colorswitch.h"

// Arduino library import area
#include <Wire.h>
#include <WiFi.h>
#include <Chrono.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_SSD1306.h>


// MACRO definition area
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define MATRIX_PIN 14
#define LED_PIN     12
#define SOUND_PIN   15

#define LED_COUNT   144
#define SMODE_CNT   6         // the number of strip mode

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif


// Master Global Variable Area
WebServer server(80);  // define server object, at port 80
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16,16, MATRIX_PIN, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
const uint16_t matrix_colors[] = {matrix.Color(150, 0, 0), matrix.Color(0, 150, 0), matrix.Color(0, 0, 150) };
uint32_t color_seq[3] = {
  strip.Color(150, 0,  0),  // red
  strip.Color(0, 150,  0),  // green
  strip.Color(0,   0, 150)  // blue
};  // color sequence
int color_seq_len = sizeof(color_seq) / 4; // each color is 4bytes(32bits)

TaskHandle_t strip_task = NULL;    // neopixel strip task tracker
TaskHandle_t matrix_task = NULL;  // matrix task tracker

volatile SemaphoreHandle_t binsem1;
volatile int modeNum = 0;
volatile int strip_mode = 0;  // default to off mode
volatile int matrix_mode = 0; // default to off mode
byte FrameNumber = 0;     // frame number of matrix animation
const char* mode_string = "<p style=\"font-size: 10em\">Arm Lighting Control</a><br>"\
                          "<a href=\"/set?value=0\">Off</a><br>"\
                          "<a  href=\"/set?value=1\">Swipe</a><br>"\
                          "<a  href=\"/set?value=2\">Rainbow</a><br>"\
                          "<a  href=\"/set?value=3\">Color Seg</a><br>"\
                          "<a  href=\"/set?value=4\">Color Bounce</a><br>"\
                          "<a  href=\"/set?value=5\">Sound Control Swipe</a><br>"\
              "<br><br><br><p style=\"font-size: 10em\">Matrix Control</a><br>"\
              "<a  href=\"/set?value=6\">Off</a><br>"\
              "<a  href=\"/set?value=7\">Display Time</a><br>"\
              "<a  href=\"/set?value=8\">Display Temerature</a><br>"\
              "<a  href=\"/set?value=9\">WreckRalph</a><br>"\
              "<a  href=\"/set?value=10\">Mario</a><br>"\
              "<a  href=\"/set?value=11\">Rick Roll</a><br>"\
              "<a  href=\"/set?value=12\">Bomb Jack</a><br>"\
              "<a  href=\"/set?value=13\">QBert</a><br>"\
              "<a  href=\"/set?value=14\">Dig Dug</a><br>";
Adafruit_SSD1306 lcd(128, 64);
const char* mdns_name = "glowhoodie"; // mdns doesn't work on Android (mdns_name.local/)


// Struct Declaration
struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};
struct RGB colorConverter(int hexValue)
{
  struct RGB rgbColor;
  rgbColor.r = ((hexValue >> 16) & 0xFF);  // Extract the RR byte
  rgbColor.g = ((hexValue >> 8) & 0xFF);   // Extract the GG byte
  rgbColor.b = ((hexValue) & 0xFF);        // Extract the BB byte

  return rgbColor;
}


void on_home() {
  server.send(200, "text/html", mode_string);
}

void on_set() {
  if (server.hasArg("value")) {
    Serial.print("Updated modeNum to: ");
    if (xSemaphoreTake( binsem1, ( TickType_t ) 1000 ) == pdTRUE) {
      modeNum = server.arg("value").toInt();
      Serial.println(modeNum);
      xSemaphoreGive(binsem1);
    }
    if (modeNum <= SMODE_CNT && strip_mode != modeNum) {
      if (strip_task != NULL) {     // if the strip is already running
        vTaskDelete( strip_task );
        clear_strip();        // always clear before switching
      }
      strip_mode = modeNum;
      xTaskCreatePinnedToCore(strip_light, "strip_light", 4096, NULL, 1, &strip_task, ARDUINO_RUNNING_CORE);
    } // change strip light scheme
    if (modeNum > SMODE_CNT && matrix_mode != modeNum) {
      if (matrix_task != NULL) {     // if the matrix is already running
        vTaskDelete( strip_task );
        clear_matrix();       // always clear before switching
      }
      matrix_mode = modeNum;
      xTaskCreatePinnedToCore(matrix_animation, "matrix_animation", 4096, NULL, 1, &matrix_task, ARDUINO_RUNNING_CORE);
    } // change matrix animation
    server.send(200, "text/html", mode_string);
  } else {
    server.send(200, "text/html", strcat("<p>Error: No value found.</p><br>", mode_string));
  }
}

void setup(void) {
  lcd.begin(SSD1306_SWITCHCAPVCC, 0x3C); lcd.setTextColor(WHITE); lcd.clearDisplay();
  lcd.display();
  Serial.begin(9600);

  WiFi.mode(WIFI_STA); // set ESP in AP mode
  WiFi.begin("george", "12345678"); // sets ssid and password
  lcd.setCursor(0,0); lcd.print("Connecting..."); lcd.display();
  while(WiFi.status() != WL_CONNECTED) { delay(500); }
  lcd.clearDisplay(); lcd.setCursor(0,0); lcd.print("Connected.\nIP:");
  lcd.println(WiFi.localIP()); lcd.display(); // print out assigned IP address
  lcd.setCursor(0,30); lcd.println("Register mDNS..."); lcd.display();
  if (MDNS.begin(mdns_name)) {  // register mDNS name
    lcd.println("success."); lcd.print(mdns_name); lcd.print(".local/"); lcd.display();
  } else { lcd.print("failed."); lcd.display(); }

  server.on("/", on_home);  // home callback function
  server.on("/set", on_set); // set callback function
  server.begin();

  strip.begin();           // INITIALIZE NeoPixel strip object
  strip.show();            // Turn OFF all pixels
  strip.setBrightness(5); // Set BRIGHTNESS of all pixels(max = 255)

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(16);
  matrix.setTextSize(1);
  matrix.setTextColor(matrix_colors[0]);

  pinMode(SOUND_PIN,INPUT);  // initialize sound sensor pin
  binsem1 = xSemaphoreCreateBinary();
  if (binsem1 == NULL) {
    Serial.println("binsem1 create unsucessful");
  } else {
    Serial.println("binsem1 created sucessfully");
  }
  xSemaphoreGive(binsem1);
}

// ------------------------ clear functions ------------------------------------
void clear_matrix() {
  for (byte y=0; y<16; y++) {
    for(byte x=0; x<16; x++) {
      matrix.drawPixel(x, y, matrix.Color(0, 0, 0));
    }
  }
  matrix.show();
  delay(100);
}

void clear_strip() {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, strip.Color(0, 0, 0));  //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(10);                           //  Pause for a moment
  }
}

//led strip
volatile unsigned long prev = 0;
volatile unsigned long lastSound = 0;
volatile int color_ind = 0;
const int seg_len = 5;              // length of color segment (30 by default)

// Helper functions (mostly for animations)

void colorWipe() {
  for (int i = 0; i < color_seq_len; i++) {
    colorWipeHelper(color_seq[i], 20);
  }
}

void colorWipeHelper(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void colorSeg() {
  for (int i = 0; i < color_seq_len; i++) {
    colorSegHelper(color_seq[i], seg_len, 20);
  }
}

//send a color segment forward (color: color, length of segment: len, speed: wait).
void colorSegHelper(uint32_t color, int len, int wait) {
  for (int i = 0; i < len; i++) { // initialize segment
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
  for (int i = len; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.setPixelColor(i - len, strip.Color(0,   0,   0));
    strip.show();
    delay(wait);
  }
  for (int i = strip.numPixels() - len; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0,   0,   0));
    strip.show();
    delay(wait);
  }
}

void colorBounce() {
  for (int i = 0; i < color_seq_len; i++) {
    colorBounceHelper(color_seq[i], seg_len, 20);
  }
}

// bounce light segment (forward and back once) (color: color, length of segment: len, speed: wait).
void colorBounceHelper(uint32_t color, int len, int wait) {
  colorSegHelper(color, len, wait);  // send a segment forward
  for (int i = strip.numPixels() - 1; i > strip.numPixels() - len; i--) { // initialize segment
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
  for (int i = strip.numPixels() - len - 1; i > -1; i--) {
    strip.setPixelColor(i, color);
    strip.setPixelColor(i + len, strip.Color(0,   0,   0));
    strip.show();
    delay(wait);
  }
  for (int i = len; i >= 0; i--) {
    strip.setPixelColor(i, strip.Color(0,   0,   0));
    strip.show();
    delay(wait);
  }
}

void rainbow() {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i * 1 + j) & 255));
    }
    strip.show();
    delay(20);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170) {
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
  if (val >= 2700 && now > lastSound + 250) {
    colorWipeHelper(color_seq[color_ind], 20);
    color_ind += 1;
    if (color_ind == color_seq_len) {
      color_ind = 0;
    }
  }
}

void strip_light(void *pvParameters) {
  while (1) {
    if (strip_mode == 1) {
      colorWipe();
    } else if (strip_mode == 2) {
      rainbow();
    } else if (strip_mode == 3) {
      colorSeg();
    } else if (strip_mode == 4) {
      colorBounce();
    } else if (strip_mode == 5) {
      soundControlWipe();
    }
  }
}

// forever loop display text
void display_text(char* text, int cursor_point) {
  int x = matrix.width();
  int pass = 0;
  
  while(1) {
    matrix.fillScreen(0);
    matrix.setCursor(x, cursor_point);
    matrix.print(F(text));
    if(--x < -6*((int)strlen(text))) {
      x = matrix.width();
      if(++pass >= 3) pass = 0;
      matrix.setTextColor(matrix_colors[pass]);
    }
    matrix.show();
    delay(100);
  }
}

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
void colorswitchmatrix() {
  FrameNumber = 0;
  Chrono chrono;
  while(1){
    if (chrono.hasPassed(72)) {
      chrono.restart();
      TimerEventColorSwitch();
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
void bombjack() {
  retro(0);
}
void qbert() {
  retro(1);
}
void digdug() {
  retro(2);
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

void TimerEventColorSwitch() {
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

String getTemperature(){
  HTTPClient http;
  http.begin("http://weather.opensprinkler.com/weatherData.py?loc=01003"); 
  int httpCode = http.GET();
  String temperature = "";
  if(httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    String word_temp = "temp";
    int index = payload.indexOf(word_temp) + 6;
    int temp = index;
    while(temp < payload.length()){
      if(payload.charAt(temp) == ','){
        break;  
      }  
      else{
        temperature.concat(payload.charAt(temp));
      }
      temp += 1;
    }
    Serial.println(temperature);
    unsigned long temperature_in_int = temperature.toInt();
    if(temperature_in_int > 25){\
      Serial.println("Lets go!!");
    }
  }
  http.end();
  return temperature;
}

String getTime(){
  HTTPClient http2;
  http2.begin("http://worldtimeapi.org/api/timezone/America/New_York"); 
  int httpCode2 = http2.GET();
  String current_time = "";
  if(httpCode2 == HTTP_CODE_OK) {
    String payload = http2.getString();
    String word_time = "datetime";
    int index = payload.indexOf(word_time) + 11;
    int newindex = payload.indexOf("T", index) + 1;
    int temp = newindex;
    while(temp < payload.length()){
      if(payload.charAt(temp) == '.'){
        break;  
      }
      else{
        current_time.concat(payload.charAt(temp)); 
      }
      temp += 1;
    }
    Serial.println(current_time);
  }
  http2.end();  
  return current_time;
}

void time() {
  String current_time = getTime();
  int len = current_time.length() + 1;
  char curr_time[len]; 
  current_time.toCharArray(curr_time, len);
  display_text(curr_time, 4);
  delay(500);
}

void temperature() {
  String temperature = getTemperature();
  int len = temperature.length() + 1;
  char curr_temperature[len]; 
  temperature.toCharArray(curr_temperature, len);
  display_text(curr_temperature, 4);
  delay(1000);
}

void matrix_animation(void *pvParameters) {
  while (1) {
    if (matrix_mode == 7) {
      time();
    } else if (matrix_mode == 8) {
      temperature();
    } else if (matrix_mode == 9) {
      wreckralph();
    } else if (matrix_mode == 10) {
      mario();
    } else if (matrix_mode == 11) {
      rickroll();
    }else if (matrix_mode == 12) {
      bombjack();
    }else if (matrix_mode == 12) {
      qbert();
    }else if (matrix_mode == 12) {
      digdug();
    }
  }
}

void loop(void) {
  server.handleClient();  // server runs on core 0 (loop()), while other events run on core1
}