/***************************************************
  This is a example sketch demonstrating the graphics
  capabilities of the SSD1331 library  for the 0.96"
  16-bit Color OLED with SSD1331 driver chip

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/684

  These displays use SPI to communicate, 4 or 5 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>


// You can use any (4 or) 5 pins
#define sclk 13
#define mosi 11
#define cs   10
#define rst  9
#define dc   8


// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

// Option 1: use any pins but a little slower
//Adafruit_SSD1331 display = Adafruit_SSD1331(cs, dc, mosi, sclk, rst);

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, cs, dc, rst);

float p = 3.1415926;

void setup(void) {
  Serial.begin(9600);
  Serial.print("hello!");
  display.begin();

  Serial.println("init");
}

void loop(){
  for (int i = 0; i < 4; i++)
  {
    display.setRotation(i);
    all_tests();
  }
}

void all_tests() {
  Serial.printf("Running all tests at rotation %d\r\n", display.getRotation());
  unsigned long time = millis();
  display.fillScreen(BLACK);
  Serial.printf("Clear screen took %ldms\r\n", millis() - time);
  delay(500);

  time = millis();
  lcdTestPattern();
  Serial.printf("Test pattern took %ldms\r\n", millis() - time);
  delay(1000);
  scroll_off();

  // scrolling small text
  time = millis();
  display.fillScreen(BLACK);
  display.setCursor(0,0);
#ifdef SSD1331_EXTRAS
  display.setTextScroll(true);
#endif
  for (int i = 0; i < 10; i++) {
    display.print("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa");
  }
  time = millis() - time;
  Serial.printf("Small text scrolling test took %ldms\r\n", time);
  display.setTextColor(GREEN);
  display.printf("\r\n-> %ldms", time);
#ifdef SSD1331_EXTRAS
  display.setTextScroll(false);
#endif
  delay(2000);
  scroll_off();

  // scrolling big text
  time = millis();
  display.fillScreen(BLACK);
  display.setCursor(0,0);
#ifdef SSD1331_EXTRAS
  display.setTextScroll(true);
#endif
  display.setTextSize(2);
  display.setTextColor(BLUE);
  for (int i = 0; i < 10; i++) {
    display.print("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa");
  }
  time = millis() - time;
  Serial.printf("Large text scrolling test took %ldms\r\n", time);
  display.setTextColor(GREEN);
  display.printf("\r\n%ldms", time);
#ifdef SSD1331_EXTRAS
  display.setTextScroll(false);
#endif
  display.setTextSize(1);
  delay(2000);
  scroll_off();

  // tft print function!
  time = millis();
  tftPrintTest();
  // The print test contains a 1500ms delay.
  Serial.printf("Print test took %ldms\r\n", (millis() - time) - 1500);
  delay(2000);
  scroll_off();
  
  //a single pixel
  display.drawPixel(display.width()/2, display.height()/2, GREEN);
  delay(500);
  scroll_off();

  // line draw test
  time = millis();
  testlines(YELLOW);
  // The lines test contains four 250ms delays.
  Serial.printf("Lines test took %ldms\r\n", (millis() - time) - 1000);
  delay(500);
  scroll_off();

  // optimized lines
  time = millis();
  testfastlines(RED, BLUE);
  Serial.printf("Optimized Lines test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  time = millis();
  testdrawrects(GREEN);
  Serial.printf("Drawrects test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  time = millis();
  testfillrects(YELLOW, MAGENTA);
  Serial.printf("Fillrects test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  time = millis();
  display.fillScreen(BLACK);
  testfillcircles(10, BLUE);
  testdrawcircles(10, WHITE);
  Serial.printf("Circles test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  time = millis();
  testroundrects();
  Serial.printf("Round rects test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  time = millis();
  testtriangles();
  Serial.printf("Triangles test took %ldms\r\n", millis() - time);
  delay(500);
  scroll_off();

  Serial.printf("*** done ***\r\n\r\n");
  delay(1000);
}


void testlines(uint16_t color) {
   display.fillScreen(BLACK);
   for (int16_t x=0; x < display.width()-1; x+=6) {
     display.drawLine(0, 0, x, display.height()-1, color);
   }
   for (int16_t y=0; y < display.height()-1; y+=6) {
     display.drawLine(0, 0, display.width()-1, y, color);
   }
   delay(250);

   display.fillScreen(BLACK);
   for (int16_t x=0; x < display.width()-1; x+=6) {
     display.drawLine(display.width()-1, 0, x, display.height()-1, color);
   }
   for (int16_t y=0; y < display.height()-1; y+=6) {
     display.drawLine(display.width()-1, 0, 0, y, color);
   }
   delay(250);

   // To avoid ESP8266 watchdog timer resets when not using the hardware SPI pins
   delay(0);

   display.fillScreen(BLACK);
   for (int16_t x=0; x < display.width()-1; x+=6) {
     display.drawLine(0, display.height()-1, x, 0, color);
   }
   for (int16_t y=0; y < display.height()-1; y+=6) {
     display.drawLine(0, display.height()-1, display.width()-1, y, color);
   }
   delay(250);

   display.fillScreen(BLACK);
   for (int16_t x=0; x < display.width()-1; x+=6) {
     display.drawLine(display.width()-1, display.height()-1, x, 0, color);
   }
   for (int16_t y=0; y < display.height()-1; y+=6) {
     display.drawLine(display.width()-1, display.height()-1, 0, y, color);
   }
   delay(250);

}

void testdrawtext(char *text, uint16_t color) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
}

void testfastlines(uint16_t color1, uint16_t color2) {
   display.fillScreen(BLACK);
   for (int16_t y=0; y < display.height()-1; y+=5) {
     display.drawFastHLine(0, y, display.width()-1, color1);
   }
   for (int16_t x=0; x < display.width()-1; x+=5) {
     display.drawFastVLine(x, 0, display.height()-1, color2);
   }
}

void testdrawrects(uint16_t color) {
 display.fillScreen(BLACK);
 int size = ((display.height() > display.width())?display.width():display.height()) - 1;
 for (int16_t x=0; x < size; x+=6) {
   display.drawRect((display.width()-1)/2 -x/2, (display.height()-1)/2 -x/2 , x, x, color);
 }
}

void testfillrects(uint16_t color1, uint16_t color2) {
 display.fillScreen(BLACK);
 int size = ((display.height() > display.width())?display.width():display.height()) - 1;
 for (int16_t x=size; x > 6; x-=6) {
   display.fillRect((display.width()-1)/2 -x/2, (display.height()-1)/2 -x/2 , x, x, color1);
   display.drawRect((display.width()-1)/2 -x/2, (display.height()-1)/2 -x/2 , x, x, color2);
 }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (uint8_t x=radius; x < display.width()-1; x+=radius*2) {
    for (uint8_t y=radius; y < display.height()-1; y+=radius*2) {
      display.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < display.width()-1+radius; x+=radius*2) {
    for (int16_t y=0; y < display.height()-1+radius; y+=radius*2) {
      display.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  display.fillScreen(BLACK);
  int color = 0xF800;
  int t;
  int w = display.width()/2;
  int x = display.height();
  int y = 0;
  int z = display.width();
  for (t = 0 ; t <= 15; t+=1) {
    display.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  display.fillScreen(BLACK);
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = display.width();
    int h = display.height();
    for(i = 0 ; i <= 8; i+=1) {
      display.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void tftPrintTest() {
  display.fillScreen(BLACK);
  display.setCursor(0, 5);
  display.setTextColor(RED);
  display.setTextSize(1);
  display.println("Hello World!");
  display.setTextColor(YELLOW, GREEN);
  display.setTextSize(2);
  display.print("Hello Wo");
  display.setTextColor(BLUE);
  display.setTextSize(3);
  display.print(1234.567);
  delay(1500);
  display.setCursor(0, 5);
  display.fillScreen(BLACK);
  display.setTextColor(WHITE);
  display.setTextSize(0);
  display.println("Hello World!");
  display.setTextSize(1);
  display.setTextColor(GREEN);
  display.print(p, 5);
  display.println(" Want pi?");
  display.print(8675309, HEX); // print 8,675,309 out in HEX!
  display.print(" Print HEX");
  display.setTextColor(WHITE);
  display.println("Sketch has been");
  display.println("running for: ");
  display.setTextColor(MAGENTA);
  display.print(millis() / 1000);
  display.setTextColor(WHITE);
  display.print(" seconds.");
}

void mediabuttons() {
 // play
  display.fillScreen(BLACK);
  display.fillRoundRect(25, 10, 78, 60, 8, WHITE);
  display.fillTriangle(42, 20, 42, 60, 90, 40, RED);
  delay(500);
  // pause
  display.fillRoundRect(25, 90, 78, 60, 8, WHITE);
  display.fillRoundRect(39, 98, 20, 45, 5, GREEN);
  display.fillRoundRect(69, 98, 20, 45, 5, GREEN);
  delay(500);
  // play color
  display.fillTriangle(42, 20, 42, 60, 90, 40, BLUE);
  delay(50);
  // pause color
  display.fillRoundRect(39, 98, 20, 45, 5, RED);
  display.fillRoundRect(69, 98, 20, 45, 5, RED);
  // play color
  display.fillTriangle(42, 20, 42, 60, 90, 40, GREEN);
}

/**************************************************************************/
/*!
    @brief  Renders a simple test pattern on the LCD
*/
/**************************************************************************/
void lcdTestPattern(void)
{
  uint8_t w,h;
  display.startWrite();
  int stripe_width = (display.width() / 8);
  for (h = 0; h < display.height() - 1; h++) {
    for (w = 0; w < display.width() - 1; w++) {
      if (w > stripe_width * 7) {
        display.writePixel(w, h, WHITE);
      } else if (w > stripe_width * 6) {
        display.writePixel(w, h, BLUE);
      } else if (w > stripe_width * 5) {
        display.writePixel(w, h, GREEN);
      } else if (w > stripe_width * 4) {
        display.writePixel(w, h, CYAN);
      } else if (w > stripe_width * 3) {
        display.writePixel(w, h, RED);
      } else if (w > stripe_width * 2) {
        display.writePixel(w, h, MAGENTA);
      } else if (w > stripe_width) {
        display.writePixel(w, h, YELLOW);
      } else {
        display.writePixel(w, h, BLACK);
      }
    }
  }
  display.endWrite();
}

void scroll_off()
{
#ifdef SSD1331_EXTRAS
  int width = display.width();
  int height = display.height();
  int scroll_amount = 1;
  int xscroll = 0;
  int yscroll = 0;
  int count = 0;
  int clearx = 0;
  int cleary = 0;
  int clearw = width;
  int clearh = height;
  static int direction = 0;

  switch(direction)
  {
  case 0:
    // scroll up
    yscroll = -scroll_amount;
    count = height / scroll_amount;
    cleary = (height - 1) - scroll_amount;
    clearh = scroll_amount;
  break;
  case 1:
    // scroll right
    xscroll = scroll_amount;
    count = width / scroll_amount;
    clearw = scroll_amount;
  break;
  case 2:
    // scroll down
    yscroll = scroll_amount;
    count = height / scroll_amount;
    clearh = scroll_amount;
  break;
  case 3:
    // scroll left
    xscroll = -scroll_amount;
    count = width / scroll_amount;
    clearx = (width - 1) - scroll_amount;
    clearw = scroll_amount;
  break;
  }

  for (int i = 0; i < count; i++)
  {
    display.copyBits(0, 0, width, height, xscroll, yscroll);
    display.fillRect(clearx, cleary, clearw, clearh, 0);
  }

  // Increment the scroll direction each time.
  direction += 1;
  direction &= 0x03;
  delay(500);
#endif
}
