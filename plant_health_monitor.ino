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
#include <Stepper.h>
#include <Sensirion.h>

const uint8_t dataPin  =  A0;
const uint8_t clockPin =  6;

float temperature;
float humidity;
float dewpoint;

Sensirion tempSensor = Sensirion(dataPin, clockPin);

//the steps of a circle
#define STEPS 100

//set steps and the connection with MCU IN1, IN3, IN2, IN4
Stepper stepper(STEPS, 2, 3, 7, 5);

//available to store previous value
int previousStep = 0;

enum guiStates{
   MAIN, //Main Gui
   ABSORB, //Absorbtion menu
   MOISTURE, //Reflectance menu
   TEMPERATURE //Temp & Moisture menu
};

//enum settings
guiStates STATE;

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
//#define LED1 16
//#define LED2 17

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TS_Point p;

// Color definitions
#define LTBLUE    0xB6DF
#define LTTEAL    0xBF5F
#define LTGREEN   0xBFF7
#define LTCYAN    0xC7FF
#define LTRED     0xFD34
#define LTMAGENTA 0xFD5F
#define LTYELLOW  0xFFF8
#define LTORANGE  0xFE73
#define LTPINK    0xFDDF
#define LTPURPLE  0xCCFF
#define LTGREY    0xE71C

#define BLUE      0x001F
#define TEAL      0x0438
#define GREEN     0x07E0
#define CYAN      0x07FF
#define RED       0xF800
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define ORANGE    0xFC00
#define PINK      0xF81F
#define PURPLE    0x8010
#define GREY      0xC618
#define WHITE     0xFFFF
#define BLACK     0x0000

#define DKBLUE    0x000D
#define DKTEAL    0x020C
#define DKGREEN   0x03E0
#define DKCYAN    0x03EF
#define DKRED     0x6000
#define DKMAGENTA 0x8008
#define DKYELLOW  0x8400
#define DKORANGE  0x8200
#define DKPINK    0x9009
#define DKPURPLE  0x4010
#define DKGREY    0x4A49

//Graphing functionality
double a1, b1, c1, d1, r2, r1, vo;
boolean graph1 = true;
boolean graph2 = true;
boolean graph3 = true;





double ox , oy;
int prevX = 0, prevY = 0;
void setup() {
  Serial.begin(9600);
  tft.begin();

  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  //PINMODES
  pinMode(A0, INPUT); //MOISTURE DATA DIGITAL
  pinMode(A1, OUTPUT); //LED1 DIGITAL
  pinMode(A2, OUTPUT); //LED2 DIGITAL
  pinMode(A3, OUTPUT); //PHOTODIODE 5V OUT
  pinMode(A4, INPUT);  //PHOTODIODE COM
  pinMode(A5, OUTPUT); //MOISTURE 5V OUT
  

  //speed of 180 per minute
  stepper.setSpeed(180);

  Serial.println("Begin Plant Monitor");
  //tft.setRotation(0);
  tft.fillScreen(BLACK);
  initGUI(0,0);
}


void loop(void) {
  //Serial.println("Looping through program");
  // See if there's any  touch data for us
  //No one is touching the screen
  //ts.bufferEmpty()
  if (ts.bufferEmpty()) {
    Serial.println("ts buffer empty:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
    return;
  }else{
    Serial.println("ts buffer !empty:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
    p.x = p.y = p.z = -1;
  }
  //Serial.println(TS_MINX);
  //Serial.println(TS_MINY);
  p = ts.getPoint();
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  Serial.println("loop x: " + String(p.x) + "y: " + String(p.y));
  //enum switch for GUI
  switch(STATE){
    case MAIN :
      Serial.println("main state");
      tft.fillScreen(BLACK);
      initGUI(p.x, p.y); //initialize GUI
      p.x = p.y = p.z = -1;
      break;
    case ABSORB :
    Serial.println("absorb state");
      if(!graph1){
        tft.fillScreen(BLACK);
      }
      Serial.println("abs state select ts buffer:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
      absorbtionMenu(); //initialize absorbtion graph
      //p.x = p.y = p.z = -1;
      break;
    case MOISTURE :
      if(!graph2){
        tft.fillScreen(BLACK);
      }
      //digitalWrite(A5, HIGH);
      Serial.println("moist state select ts buffer:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
      moistureMenu(); //initialize absorbtion graph
      //p.x = p.y = p.z = -1;
      break;
    case TEMPERATURE :
      if(!graph3){
        tft.fillScreen(BLACK);
      }
      //digitalWrite(A5, HIGH);
      Serial.println("temp state select ts buffer:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
      temperatureMenu(); //initialize absorbtion graph
      //p.x = p.y = p.z = -1;
      break;
  }
  //ts.bufferClear();
  //ts.writeRegister8(STMPE_INT_STA, 0xFF);
  //p.x = p.y = p.z = -1;
  delay(100);
}

void initGUI(uint16_t x, uint16_t y){
  //Start GUI Program
  graph1 = graph2 = graph3 = true;
  tft.setRotation(0);
  if(x > 23 && x < 228 && y > 102 && y < 133){
    Serial.println("Absorption Button Pressed");
    Serial.println("gui absorb x: " + String(x) + "y: " + String(y));
    //tft.fillScreen(BLACK);
    graph1 = true;
    STATE = ABSORB;
    Serial.println("gui absorb ts buffer:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
    //p.x = p.y = p.z = -1;
    return;
  }
  
  if(x > 25 && x < 229 && y > 144 && y < 183){
    Serial.println("Moisture Button Pressed");
    //tft.fillScreen(BLACK);
    graph2 = true;
    STATE = MOISTURE;
    //p.x = p.y = p.z = -1;
    return;
  }  

  if(x > 25 && x < 225 && y > 195 && y < 238){
    Serial.println("Temp/H2O Button Pressed");
    //tft.fillScreen(BLACK);
    graph3 = true;
    STATE = TEMPERATURE;
    //p.x = p.y = p.z = -1;
    return;
  }

  Serial.println("Initialize Main GUI");
  //tft.fillScreen(BLACK);
  int cx = (tft.width() / 4) - 1, cy = (tft.height() / 2) - 1;
  int fx = tft.width() - 1;
  int fy = tft.height() - 1;
  
  tft.fillRoundRect(20, 110, 200, 40, 1, tft.color565(254, 254, 254));
  tft.fillRoundRect(20, 160, 200, 40, 1, tft.color565(254, 254, 254));
  tft.fillRoundRect(20, 210, 200, 40, 1, tft.color565(254, 254, 254));

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
  tft.println(" Moisture");
  tft.setCursor(48,220);
  tft.setTextColor(BLUE);
  tft.println(" Temp");
  
  tft.setCursor(0,tft.height()-45);
  tft.setTextColor(CYAN);
  tft.setTextSize(2);
  tft.println();
  tft.setTextSize(1);
  tft.println(" ");
  tft.setTextSize(2); 
  tft.println("   Made By Eric Kim");
  Serial.println("End GUI");
  //End GUI Program
}

void absorbtionMenu(){
  ts.writeRegister8(STMPE_INT_STA, 0xFF);
    int myrand = 10;
    digitalWrite(A1, HIGH);
    digitalWrite(A2, HIGH);
    digitalWrite(A3, HIGH);
    //myrand = random(1024);
    Serial.println("begin absorbtion menu ts.buffer:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
    //ts.writeRegister8(STMPE_INT_STA, 0xFF);
    //Serial.println("after clear absorbtion menu ts.buffer:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
    double xdata, ydata;
    tft.setRotation(3);
    int val = -50;
    int stepSum = 0;
    //collectdata here or read from sdcard save
    for (xdata = 400; xdata <= 700; xdata += 50) {
      //ydata = ydata + 0.5;
      stepSum = stepSum + val;
      stepper.step(val);
      int photoSensorVal1 = analogRead(A4);
      float voltage1 = 5 - photoSensorVal1 * (5.0 / 1023.0);
      ydata = voltage1;
      Graph(tft, xdata, ydata, tft.width()/8, 7*tft.height()/8, 3*tft.width()/4, 
        3*tft.height()/4, 400, 700, 50, 0, 2, 0.1, 
        "Absorbtion (V/nm)", "Wavelen (nm)", "Abs (V)", DKBLUE, WHITE, RED, LTCYAN, BLACK, graph1);
    }
    stepper.step(-stepSum);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
    if (ts.bufferEmpty()) {
      STATE = MAIN;
      Serial.println("Abs menu buffer empty in asborb:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
      ts.writeRegister8(STMPE_INT_STA, 0xFF);
      //tft.fillScreen(BLACK);
    }else{
      graph1 = true;
      Serial.println("ABS menu buffer !empty in absorb" + String(ts.bufferEmpty()) + " state:" + String(STATE));
      p = ts.getPoint();
      //p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
      //p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    }    
}

void moistureMenu(){
    ts.writeRegister8(STMPE_INT_STA, 0xFF);
    int myrand = 10;
    digitalWrite(A5, HIGH);
    Serial.println("begin moisture menu ts.buffer:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
    //ts.writeRegister8(STMPE_INT_STA, 0xFF);
    //Serial.println("after clear moisture menu ts.buffer:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
    //tft.setRotation(2);
    double xdata, ydata;
    tft.setRotation(3);
    //collectdata here or read from sdcard save
    for (xdata = 0; xdata <= 3; xdata += .1) {
      tempSensor.measure(&temperature, &humidity, &dewpoint);
      ydata = humidity;
      Serial.println(ydata);
      Graph(tft, xdata, ydata, tft.width()/8, 7*tft.height()/8, 3*tft.width()/4, 
        3*tft.height()/4, 0, 3, 0.5, 0, 100, 10, 
        "Moisture Graph", "Moisture (%)", "Time (s)", DKBLUE, WHITE, RED, LTCYAN, BLACK, graph2);
        delay(100);
    }
    digitalWrite(A5, LOW);
    if (ts.bufferEmpty()) {
      STATE = MAIN;
      Serial.println("Abs menu buffer empty in asborb:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
      ts.writeRegister8(STMPE_INT_STA, 0xFF);
      //tft.fillScreen(BLACK);
    }else{
      graph2 = true;
      Serial.println("ABS menu buffer !empty in absorb" + String(ts.bufferEmpty()) + " state:" + String(STATE));
      p = ts.getPoint();
      //p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
      //p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    } 
}

void temperatureMenu(){
    ts.writeRegister8(STMPE_INT_STA, 0xFF);
    digitalWrite(A5, HIGH);  
    // Read values from the sensor
    //temp_c = sht1x.readTemperatureC();
    //temp_f = sht1x.readTemperatureF();
    //myrand = random(1024);
    Serial.println("begin temp menu ts.buffer:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
    //ts.writeRegister8(STMPE_INT_STA, 0xFF);
    //Serial.println("after clear temp menu ts.buffer:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
    //tft.setRotation(2);
    double xdata, ydata;
    tft.setRotation(3);
    //collectdata here or read from sdcard save
    for (xdata = 0; xdata <= 3; xdata += 0.1) {
      tempSensor.measure(&temperature, &humidity, &dewpoint);
      ydata = 9.0*temperature/5.0 + 32.0;
      Serial.println(ydata);
      Graph(tft, xdata, ydata, tft.width()/8, 7*tft.height()/8, 3*tft.width()/4, 
        3*tft.height()/4, 0, 3, 0.5, 0, 120, 10, 
        "Temperature (F)", "Time (s)", "Abs (V)", DKBLUE, WHITE, RED, LTCYAN, BLACK, graph3);
        delay(100);
    }
    digitalWrite(A5, LOW);
    if (ts.bufferEmpty()) {
      STATE = MAIN;
      Serial.println("Abs menu buffer empty in asborb:" + String(ts.bufferEmpty()) + " state:" + String(STATE));
      ts.writeRegister8(STMPE_INT_STA, 0xFF);
      //tft.fillScreen(BLACK);
    }else{
      graph3 = true;
      Serial.println("ABS menu buffer !empty in absorb" + String(ts.bufferEmpty()) + " state:" + String(STATE));
      p = ts.getPoint();
      //p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
      //p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    } 
}

/*

  function to draw a cartesian coordinate system and plot whatever data you want
  just pass x and y and the graph will be drawn

  huge arguement list
  &d name of your display object
  xdata = x data point
  ydata = y datapont
  xgloc = x graph location (lower left)
  ygloc = y graph location (lower left)
  gwidth = width of graph
  gheight = height of graph
  xaxis0 = lower bound of x axis
  xaxisn = upper bound of x asis
  xinc = division of x axis (distance not count)
  yaxis0 = lower bound of y axis
  yaxisn = upper bound of y axis
  yinc = division of y axis (distance not count)
  title = title of graph
  xlabel = x axis label
  ylabel = y axis label
  gcolor = graph line colors
  acolor = axi ine colors
  pcolor = color of your plotted data
  tcolor = text color
  bcolor = background color
  &redraw = flag to redraw graph on fist call only
*/
void Graph(Adafruit_ILI9341 &d, double xdata, double ydata, double xgloc, double ygloc, double gwidth, double gheight, double xaxis0, 
  double xaxisn, double xinc, double yaxis0, double yaxisn, double yinc, String title, String xlabel, String ylabel, unsigned int gcolor,
  unsigned int acolor, unsigned int pcolor, unsigned int tcolor, unsigned int bcolor, boolean &redraw) {
  double ydiv, xdiv;
  // Graph(Adafruit_ILI9341 &d, double xdata, double ydata, double xgloc, double ygloc, double gwidth, double gheight, double xaxis0, double xaxisn, double xinc, double yaxis0, double yaxisn, double yinc, String title, String xlabel, String ylabel, unsigned int gcolor, unsigned int acolor, unsigned int pcolor, unsigned int tcolor, unsigned int bcolor, boolean &redraw)
  // initialize old x and old y in order to draw the first point of the graph but save the transformed value
  // note my transform funcition is the same as the map function, except the map uses long and we need doubles
  // static double ox = (xdata - xaxis0) * ( gwidth) / (xaxisn - xaxis0) + xgloc;
  // static double oy = (ydata - yaxis0) * (ygloc - gheight - ygloc) / (yaxisn - yaxis0) + ygloc;
  double i;
  double temp;
  int rot, newrot;
   if (redraw == true) {
    redraw = false;
    ox = (xdata - xaxis0) * ( gwidth) / (xaxisn - xaxis0) + xgloc;
    oy = (ydata - yaxis0) * (ygloc - gheight - ygloc) / (yaxisn - yaxis0) + ygloc;
    // draw y scale
    for ( i = yaxis0; i <= yaxisn; i += yinc) {
      // compute the transform
      temp =  (i - yaxis0) * (ygloc - gheight - ygloc) / (yaxisn - yaxis0) + ygloc;
      if (i == 0) {
        d.drawLine(xgloc, temp, xgloc + gwidth, temp, acolor);
      }
      else {
        d.drawLine(xgloc, temp, xgloc + gwidth, temp, gcolor);
      }
      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(xgloc - 40, temp);
      // precision is default Arduino--this could really use some format control
      d.println(i);
    }
    // draw x scale
    for (i = xaxis0; i <= xaxisn; i += xinc) {
      // compute the transform
      temp =  (i - xaxis0) * ( gwidth) / (xaxisn - xaxis0) + xgloc;
      if (i == 0) {
        d.drawLine(temp, ygloc, temp, ygloc - gheight, acolor);
      }
      else {
        d.drawLine(temp, ygloc, temp, ygloc - gheight, gcolor);
      }

      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(temp, ygloc + 10);
      // precision is default Arduino--this could really use some format control
      d.println(i);
    }

    //now draw the labels
    d.setTextSize(2);
    d.setTextColor(tcolor, bcolor);
    d.setCursor(xgloc , ygloc - gheight - 30);
    d.println(title);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(xgloc , ygloc + 20);
    d.println(xlabel);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(xgloc - 30, ygloc - gheight - 10);
    d.println(ylabel);
  }

  //graph drawn now plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized as static above
  xdata =  (xdata - xaxis0) * ( gwidth) / (xaxisn - xaxis0) + xgloc;
  ydata =  (ydata - yaxis0) * (ygloc - gheight - ygloc) / (yaxisn - yaxis0) + ygloc;
  d.drawLine(ox, oy, xdata, ydata, pcolor);
  d.drawLine(ox, oy + 1, xdata, ydata + 1, pcolor);
  d.drawLine(ox, oy - 1, xdata, ydata - 1, pcolor);
  ox = xdata;
  oy = ydata;
}
