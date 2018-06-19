/*************************************************** 
  This is a example sketch demonstrating bitmap drawing
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

  The Adafruit GFX Graphics core library is also required
  https://github.com/adafruit/Adafruit-GFX-Library
  Be sure to install it!
 ****************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SD.h>
#include <SPI.h>


// If we are using the hardware SPI interface, these are the pins (for future ref)
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

// to draw images from the SD card, we will share the hardware SPI interface
Adafruit_SSD1331 tft = Adafruit_SSD1331(cs, dc, rst);  

// For Arduino Uno/Duemilanove, etc
//  connect the SD card with MOSI going to pin 11, MISO going to pin 12 and SCK going to pin 13 (standard)
//  Then pin 4 goes to CS (or whatever you have set up)
#define SD_CS 4    // Set the chip select line to whatever you use (4 doesnt conflict with the library)

// the file itself
File bmpFile;

// information we extract about the bitmap file
int bmpWidth, bmpHeight;
uint8_t bmpDepth, bmpImageoffset;


void setup(void) {
  Serial.begin(9600);
   
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
     
  // initialize the OLED
  tft.begin();

  Serial.println("init");
  
  tft.fillScreen(BLUE);
  
  delay(500);
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    while(1);
  }
  Serial.println("SD OK!");

  bmpDraw("violet.bmp", 0, 0);
}

void loop() {
}


// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

void bmpDraw(char *filename, int16_t x, int16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col, x2, y2, bx1, by1;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        x2 = x + bmpWidth  - 1; // Lower-right corner
        y2 = y + bmpHeight - 1;
        if((x2 >= 0) && (y2 >= 0)) { // On screen?
          w = bmpWidth; // Width/height of section to load/display
          h = bmpHeight;
          bx1 = by1 = 0; // UL coordinate in BMP file
          if(x < 0) { // Clip left
            bx1 = -x;
            x   = 0;
            w   = x2 + 1;
          }
          if(y < 0) { // Clip top
            by1 = -y;
            y   = 0;
            h   = y2 + 1;
          }
          if(x2 >= tft.width())  w = tft.width()  - x; // Clip right
          if(y2 >= tft.height()) h = tft.height() - y; // Clip bottom
  
          // Set TFT address window to clipped image bounds
          tft.startWrite(); // Requires start/end transaction now
          tft.setAddrWindow(x, y, w, h);
  
          for (row=0; row<h; row++) { // For each scanline...
  
            // Seek to start of scan line.  It might seem labor-
            // intensive to be doing this on every line, but this
            // method covers a lot of gritty details like cropping
            // and scanline padding.  Also, the seek only takes
            // place if the file position actually needs to change
            // (avoids a lot of cluster math in SD library).
            if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
              pos = bmpImageoffset + (bmpHeight - 1 - (row + by1)) * rowSize;
            else     // Bitmap is stored top-to-bottom
              pos = bmpImageoffset + (row + by1) * rowSize;
            pos += bx1 * 3; // Factor in starting column (bx1)
            if(bmpFile.position() != pos) { // Need seek?
              tft.endWrite(); // End TFT transaction
              bmpFile.seek(pos);
              buffidx = sizeof(sdbuffer); // Force buffer reload
              tft.startWrite(); // Start new TFT transaction
            }
            for (col=0; col<w; col++) { // For each pixel...
              // Time to read more pixel data?
              if (buffidx >= sizeof(sdbuffer)) { // Indeed
                tft.endWrite(); // End TFT transaction
                bmpFile.read(sdbuffer, sizeof(sdbuffer));
                buffidx = 0; // Set index to beginning
                tft.startWrite(); // Start new TFT transaction
              }
              // Convert pixel from BMP to TFT format, push to display
              b = sdbuffer[buffidx++];
              g = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];
              tft.writePixel(tft.color565(r,g,b));
            } // end pixel
          } // end scanline
          tft.endWrite(); // End last TFT transaction
        } // end onscreen
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
