/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

void setup() {
  Serial.begin(9600);
  tft.begin();

  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  
  tft.fillScreen(BLACK);

  int cx = (tft.width() / 4) - 1, cy = (tft.height() / 2) - 1;
  
  tft.fillRoundRect(20, 110, 200, 40, 1, tft.color565(254, 254, 254));
  tft.fillRoundRect(20, 160, 200, 40, 1, tft.color565(254, 254, 254));
  tft.fillRoundRect(20, 210, 200, 40, 1, tft.color565(254, 254, 254));

  //Start GUI Program
  tft.setCursor(0, 0);
  tft.setTextSize(3);
  tft.println();
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(" ");
  tft.setTextSize(3);
  tft.println("Plant Health");
  tft.setTextSize(1);
  tft.println(" ");
  tft.setTextSize(3);
  tft.println("   Monitor");
  
  tft.setCursor(30,120);
  tft.setTextSize(3);
  tft.setTextColor(RED);
  tft.println("Absorption");
  tft.setCursor(23,170);
  tft.setTextColor(GREEN);
  tft.println("Reflectance");
  tft.setCursor(48,220);
  tft.setTextColor(BLUE);
  tft.println("Temp/H2O");
  
  tft.setCursor(0,tft.height()-45);
  tft.setTextColor(CYAN);
  tft.setTextSize(2);
  tft.println();
  tft.setTextSize(1);
  tft.println(" ");
  tft.setTextSize(2); 
  tft.println("   Made By Eric Kim");

  //End GUI Program
}


void loop(void) {

  // See if there's any  touch data for us
  if (ts.bufferEmpty()) {
    return;
  }

  // Retrieve a point  
  TS_Point p = ts.getPoint();
 
  // Scale from ~0->4000 to tft.width using the calibration #'s
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

  //Serial.println(TS_MINX);
  //Serial.println(TS_MINY);
  //Serial.println(p.y);
  //Serial.println(p.y);
  if(p.x > 23 && p.x < 228 && p.y > 102 && p.y < 133){
    Serial.println("Absorption Button Pressed");
  }
  
  if(p.x > 25 && p.x < 229 && p.y > 144 && p.y < 183){
    Serial.println("Reflection Button Pressed");
  }  

  if(p.x > 25 && p.x < 225 && p.y > 195 && p.y < 238){
    Serial.println("Temp/H2O Button Pressed");
  }

}
