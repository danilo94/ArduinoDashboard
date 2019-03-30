#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define RED2RED 0
#define GREEN2GREEN 1
#define BLUE2BLUE 2
#define BLUE2RED 3
#define GREEN2RED 4
#define RED2GREEN 5

#include <SPI.h>          // f.k. for Arduino-1.5.2
#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h>
#include "images.c"
MCUFRIEND_kbv tft;

unsigned int statuss=0, velocity=0, nitro=0, rpm=0, posi=0, points=0, gas=0;
bool flagUpdate = false;



void setup() {
    Serial.begin(9600);
    uint16_t ID = tft.readID(); //
    if (ID == 0xD3D3) ID = 0x9481; 
    tft.begin(ID);
    tft.setRotation(3);
    tft.fillScreen(BLACK);
    tft.drawRGBBitmap(20, 80, oxygen, 32, 32);
    tft.drawRGBBitmap(130, 80, scoreboard,32,32);
    tft.drawRGBBitmap(240, 80, pos,32,32);
    tft.setCursor(25,0);
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(2);    

    tft.print(F("Top Gear Dashboard 1.0"));
    tft.setCursor(20,40);
    tft.println(F("Status: "));


    
    tft.setCursor(55,102);
    tft.println(F("x"));
    tft.setCursor(170,102);
    tft.println(F("x"));
    tft.setCursor(280,102);
    tft.println(F("x"));
    updateNitro(0);
    updateScore(0);
    updatePosition(0);
    updateStatus(0);
}

void updateStatus(int newStatus){
  if (newStatus==0){
      tft.setTextColor(RED, BLACK);
      tft.setCursor(110,40);
      tft.print(F("Offline"));
  }
  else{
      tft.setTextColor(GREEN, BLACK);
      tft.setCursor(110,40);
      tft.print(F(" Online"));    
  }
}


void updatePosition(int pos){
  tft.setTextColor(WHITE, BLACK);
   tft.setCursor(295,102);
   if (pos<=20)
    tft.print(String(pos)); 
}
void updateScore(int score){
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(185,102);
  tft.print(String(score));
}
void updateNitro(int value){
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(70,102);
  if (nitro<=3)
    tft.print(String(value));
}


void loop() {

  
 if (Serial.available()>0){ 
    char buf[Serial.available()];
    Serial.readBytes(buf,Serial.available());
    sscanf(buf,"%d;%d;%d;%d;%d;%d;%d;",&statuss,&velocity,&gas,&points,&posi,&rpm,&nitro);
    flagUpdate=true;
 }
  
 if (flagUpdate==true) {
    flagUpdate=false;
    // Test with a slowly changing value from a Sine function
    // Set the the position, gap between meters, and inner radius of the meters
    int xpos = 0, ypos = 128, gap = 10, radius = 52;

    // Draw meter and get back x position of next meter

    // Test with Sine wave function, normally reading will be from a sensor
    xpos = gap + ringMeter(velocity, 0, 235, xpos, ypos, radius, "Km/h", GREEN2RED); // Draw analogue meter

    ringMeter(gas, 0, 100, xpos, ypos, radius, "Gas", RED2GREEN); // Draw analogue meter
    updateStatus(statuss);
    updateNitro(nitro);
    updateScore(points);
    updatePosition(posi);    
   }
}




int ringMeter(int value, int vmin, int vmax, int x, int y, int r, char *units, byte scheme)
{
  // Minimum value of r is about 52 before value text intrudes on ring
  // drawing the text first is an option
  
  x += r; y += r;   // Calculate coords of centre of ring

  int w = r / 4;    // Width of outer ring is 1/4 of radius
  
  int angle = 150;  // Half the sweep angle of meter (300 degrees)

  int text_colour = 0; // To hold the text colour

  int v = map(value, vmin, vmax, -angle, angle); // Map the value to an angle v

  byte seg = 5; // Segments are 5 degrees wide = 60 segments for 300 degrees
  byte inc = 5; // Draw segments every 5 degrees, increase to 10 for segmented ring

  // Draw colour blocks every inc degrees
  for (int i = -angle; i < angle; i += inc) {

    // Choose colour from scheme
    int colour = 0;
    switch (scheme) {
      case 0: colour = RED; break; // Fixed colour
      case 1: colour = GREEN; break; // Fixed colour
      case 2: colour = BLUE; break; // Fixed colour
      case 3: colour = rainbow(map(i, -angle, angle, 0, 127)); break; // Full spectrum blue to red
      case 4: colour = rainbow(map(i, -angle, angle, 63, 127)); break; // Green to red (high temperature etc)
      case 5: colour = rainbow(map(i, -angle, angle, 127, 63)); break; // Red to green (low battery etc)
      default: colour = BLUE; break; // Fixed colour
    }

    // Calculate pair of coordinates for segment start
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (r - w) + x;
    uint16_t y0 = sy * (r - w) + y;
    uint16_t x1 = sx * r + x;
    uint16_t y1 = sy * r + y;

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * 0.0174532925);
    float sy2 = sin((i + seg - 90) * 0.0174532925);
    int x2 = sx2 * (r - w) + x;
    int y2 = sy2 * (r - w) + y;
    int x3 = sx2 * r + x;
    int y3 = sy2 * r + y;

    if (i < v) { // Fill in coloured segments with 2 triangles
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);
      text_colour = colour; // Save the last colour drawn
    }
    else // Fill in blank segments
    {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, BLACK);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, BLACK);
    }
  }

  // Convert value to a string
  char buf[10];
  byte len = 4; if (value > 999) len = 5;
  dtostrf(value, len, 0, buf);
            

  // Set the text colour to default
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2);
  // Uncomment next line to set the text colour to the last segment value!
  // tft.setTextColor(text_colour, ILI9341_BLACK);
  
  // Print value, if the meter is large then use big font 6, othewise use 4


  // Print units, if the meter is large then use big font 4, othewise use 2

  // Uncomment next line to set the text colour to the last segment value!
  // tft.setTextColor(text_colour, ILI9341_BLACK);

  // Print value, if the meter is large then use big font 6, othewise use 4
  if (r > 84){
    tft.setCursor(x-25,y-20);
    tft.println(units);    
  }
  else{
    tft.setCursor(x-16,y-20);
    tft.println(units);
  }

  // Print units, if the meter is large then use big font 4, othewise use 2
  if (r > 84){
    tft.setCursor(x-30,y+30);
    tft.println(buf);
  }
  else{
    tft.setCursor(x-30,y+5);
    tft.println(buf);
  }
  // Calculate and return right hand side x coordinate
  return x + r;
}

unsigned int rainbow(byte value)
{
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = blue through to 127 = red

  byte red = 0; // Red is the top 5 bits of a 16 bit colour value
  byte green = 0;// Green is the middle 6 bits
  byte blue = 0; // Blue is the bottom 5 bits

  byte quadrant = value / 32;

  if (quadrant == 0) {
    blue = 31;
    green = 2 * (value % 32);
    red = 0;
  }
  if (quadrant == 1) {
    blue = 31 - (value % 32);
    green = 63;
    red = 0;
  }
  if (quadrant == 2) {
    blue = 0;
    green = 63;
    red = value % 32;
  }
  if (quadrant == 3) {
    blue = 0;
    green = 63 - 2 * (value % 32);
    red = 31;
  }
  return (red << 11) + (green << 5) + blue;
}



float sineWave(int phase) {
  return sin(phase * 0.0174532925);
}
