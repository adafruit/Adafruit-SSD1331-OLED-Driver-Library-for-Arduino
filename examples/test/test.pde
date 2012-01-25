// You can use any (4 or) 5 pins 
#define sclk 4
#define mosi 5
#define cs 6
#define dc 7
#define rst 8  // you can also connect this to the Arduino reset

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

#include <SSD1331.h>
#include <SPI.h>

// Option 1: use any pins but a little slower
SSD1331 tft = SSD1331(cs, dc, mosi, sclk, rst);  

// Option 2: must use the hardware SPI pins 
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be 
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
//SSD1331 tft = SSD1331(cs, dc, rst);    

float p = 3.1415926;

void fillpixelbypixel(uint16_t color) 
{
  for (uint8_t x=0; x < tft.width(); x++) 
  {
    for (uint8_t y=0; y < tft.height(); y++) 
    {
      tft.drawPixel(x, y, color);
    }
  }
  delay(100);
}

void setup(void) 
{
  Serial.begin(9600);
  Serial.print("hello!");
  tft.initDisplay();

  Serial.println("init");
  
  uint16_t time = millis();  
  tft.fillScreen(BLUE);
  time = millis() - time;
  delay(500);  

  Serial.println(time, DEC);
  delay(1000);

  testtext(RED);
  delay(2000);
  
  testlines(GREEN);
  delay(2000);

  testfastlines(GREEN, BLUE);
  delay(2000);
  
  //a single pixel
  tft.drawPixel(tft.width()/2, tft.height()/2, GREEN);
  delay(500);
  
  tft.fillScreen(BLACK);
  tft.drawHorizontalLine(5, 5, tft.width()-5, RED);
  delay(500);
  tft.drawVerticalLine(5, 5, tft.height()-10, WHITE);
  delay(3000);

  tft.fillCircle(tft.width()/2, tft.height()/2, tft.height()/4, BLUE);
  delay(500);
  tft.drawRoundRect(5, 5, 30, 20, 5, GREEN);  
  delay(1000);
  tft.fillRoundRect(5, 5, 30, 20, 5, GREEN);  
  
  Serial.println("done");
  delay(1000);
  
  // lcdTest();
}

void loop() 
{
}

/**************************************************************************/
/*! 
    @brief  Renders a simple test pattern on the LCD
*/
/**************************************************************************/
void lcdTest(void)
{
  uint32_t i,j;
  tft.goTo(0, 0);
  
  for(i=0;i<64;i++)
  {
    for(j=0;j<96;j++)
    {
      if(i>55){tft.writeData(WHITE>>8);tft.writeData(WHITE);}
      else if(i>47){tft.writeData(BLUE>>8);tft.writeData(BLUE);}
      else if(i>39){tft.writeData(GREEN>>8);tft.writeData(GREEN);}
      else if(i>31){tft.writeData(CYAN>>8);tft.writeData(CYAN);}
      else if(i>23){tft.writeData(RED>>8);tft.writeData(RED);}
      else if(i>15){tft.writeData(MAGENTA>>8);tft.writeData(MAGENTA);}
      else if(i>7){tft.writeData(YELLOW>>8);tft.writeData(YELLOW);}
      else {tft.writeData(BLACK>>8);tft.writeData(BLACK);}
    }
  }
}

void testlines(uint16_t color) {
   tft.fillScreen(BLACK);
   for (uint16_t x=0; x < tft.width(); x+=6) {
     tft.drawLine(0, 0, x, tft.height()-1, color);
   }
   for (uint16_t y=0; y < tft.height(); y+=6) {
     tft.drawLine(0, 0, tft.width()-1, y, color);
   }
   
   tft.fillScreen(BLACK);
   for (uint16_t x=0; x < tft.width(); x+=6) {
     tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
   }
   for (uint16_t y=0; y < tft.height(); y+=6) {
     tft.drawLine(tft.width()-1, 0, 0, y, color);
   }
   
   tft.fillScreen(BLACK);
   for (uint16_t x=0; x < tft.width(); x+=6) {
     tft.drawLine(0, tft.height()-1, x, 0, color);
   }
   for (uint16_t y=0; y < tft.height(); y+=6) {
     tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
   }

   tft.fillScreen(BLACK);
   for (uint16_t x=0; x < tft.width(); x+=6) {
     tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
   }
   for (uint16_t y=0; y < tft.height(); y+=6) {
     tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
   }
   
}

void testtext(uint16_t color) {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 20);
  tft.setTextColor(color);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.println(1234.56);
  tft.setTextSize(1);
  tft.println(0xDEADBEEF, HEX);
}

void testfastlines(uint16_t color1, uint16_t color2) {
   tft.fillScreen(BLACK);
   for (uint16_t y=0; y < tft.height(); y+=5) {
     tft.drawHorizontalLine(0, y, tft.width()-1, color1);
   }
   for (uint16_t x=0; x < tft.width(); x+=5) {
     tft.drawVerticalLine(x, 0, tft.height()-1, color2);
   }
}


