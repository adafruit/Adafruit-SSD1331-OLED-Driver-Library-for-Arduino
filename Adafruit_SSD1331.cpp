/*!
 * @file Adafruit_SSD1331.cpp
 *
 * @mainpage Adafruit SSD1331 Arduino Library
 *
 * @section intro_sec Introduction
 *
 * This is a library for the 0.96" 16-bit Color OLED with SSD1331 driver chip
 *
 * Pick one up today in the adafruit shop!
 * ------> http://www.adafruit.com/products/684
 *
 * These displays use SPI to communicate, 4 or 5 pins are required to
 * interface
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text above must be included in any redistribution
 */

#include "Adafruit_SSD1331.h"
#include "pins_arduino.h"
#include "wiring_private.h"

/***********************************/

/*!
  @brief   SPI displays set an address window rectangle for blitting pixels
  @param  x  Top left corner x coordinate
  @param  y  Top left corner x coordinate
  @param  w  Width of window
  @param  h  Height of window
*/
void Adafruit_SSD1331::setAddrWindow(uint16_t x, uint16_t y, uint16_t w,
                                     uint16_t h) {

  uint8_t x1 = x;
  uint8_t y1 = y;
  if (x1 > 95)
    x1 = 95;
  if (y1 > 63)
    y1 = 63;

  uint8_t x2 = (x + w - 1);
  uint8_t y2 = (y + h - 1);
  if (x2 > 95)
    x2 = 95;
  if (y2 > 63)
    y2 = 63;

  if (x1 > x2) {
    uint8_t t = x2;
    x2 = x1;
    x1 = t;
  }
  if (y1 > y2) {
    uint8_t t = y2;
    y2 = y1;
    y1 = t;
  }

  sendCommand(0x15); // Column addr set
  sendCommand(x1);
  sendCommand(x2);

  sendCommand(0x75); // Column addr set
  sendCommand(y1);
  sendCommand(y2);

  startWrite();
}

/**************************************************************************/
/*!
    @brief   Initialize SSD1331 chip
    Connects to the SSD1331 over SPI and sends initialization procedure commands
    @param    freq  Desired SPI clock frequency
*/
/**************************************************************************/
void Adafruit_SSD1331::begin(uint32_t freq) {
  initSPI(freq);

  // Initialization Sequence
  sendCommand(SSD1331_CMD_DISPLAYOFF); // 0xAE
  sendCommand(SSD1331_CMD_SETREMAP);   // 0xA0
#if defined SSD1331_COLORORDER_RGB
  sendCommand(0x72); // RGB Color
#else
  sendCommand(0x76); // BGR Color
#endif
  sendCommand(SSD1331_CMD_STARTLINE); // 0xA1
  sendCommand(0x0);
  sendCommand(SSD1331_CMD_DISPLAYOFFSET); // 0xA2
  sendCommand(0x0);
  sendCommand(SSD1331_CMD_NORMALDISPLAY); // 0xA4
  sendCommand(SSD1331_CMD_SETMULTIPLEX);  // 0xA8
  sendCommand(0x3F);                      // 0x3F 1/64 duty
  sendCommand(SSD1331_CMD_SETMASTER);     // 0xAD
  sendCommand(0x8E);
  sendCommand(SSD1331_CMD_POWERMODE); // 0xB0
  sendCommand(0x0B);
  sendCommand(SSD1331_CMD_PRECHARGE); // 0xB1
  sendCommand(0x31);
  sendCommand(SSD1331_CMD_CLOCKDIV); // 0xB3
  sendCommand(0xF0); // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio
                     // (A[3:0]+1 = 1..16)
  sendCommand(SSD1331_CMD_PRECHARGEA); // 0x8A
  sendCommand(0x64);
  sendCommand(SSD1331_CMD_PRECHARGEB); // 0x8B
  sendCommand(0x78);
  sendCommand(SSD1331_CMD_PRECHARGEC); // 0x8C
  sendCommand(0x64);
  sendCommand(SSD1331_CMD_PRECHARGELEVEL); // 0xBB
  sendCommand(0x3A);
  sendCommand(SSD1331_CMD_VCOMH); // 0xBE
  sendCommand(0x3E);
  sendCommand(SSD1331_CMD_MASTERCURRENT); // 0x87
  sendCommand(0x06);
  sendCommand(SSD1331_CMD_CONTRASTA); // 0x81
  sendCommand(0x91);
  sendCommand(SSD1331_CMD_CONTRASTB); // 0x82
  sendCommand(0x50);
  sendCommand(SSD1331_CMD_CONTRASTC); // 0x83
  sendCommand(0x7D);
  sendCommand(SSD1331_CMD_DISPLAYON); //--turn on oled panel
  _width = TFTWIDTH;
  _height = TFTHEIGHT;
}

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit SSD1331 driver with software SPI
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    mosi  SPI MOSI pin #
    @param    sclk  SPI Clock pin #
    @param    rst   Reset pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
Adafruit_SSD1331::Adafruit_SSD1331(int8_t cs, int8_t dc, int8_t mosi,
                                   int8_t sclk, int8_t rst)
    : Adafruit_SPITFT(TFTWIDTH, TFTHEIGHT, cs, dc, mosi, sclk, rst, -1) {}

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit SSD1331 driver with hardware SPI
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    rst   Reset pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
Adafruit_SSD1331::Adafruit_SSD1331(int8_t cs, int8_t dc, int8_t rst)
    : Adafruit_SPITFT(TFTWIDTH, TFTHEIGHT, cs, dc, rst) {}

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit SSD1331 driver with hardware SPI
    @param    spi   Pointer to an existing SPIClass instance (e.g. &SPI, the
                    microcontroller's primary SPI bus).
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    rst   Reset pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
Adafruit_SSD1331::Adafruit_SSD1331(SPIClass *spi, int8_t cs, int8_t dc,
                                   int8_t rst)
    :
#if defined(ESP8266)
      Adafruit_SPITFT(TFTWIDTH, TFTWIDTH, cs, dc, rst) {
#else
      Adafruit_SPITFT(TFTWIDTH, TFTWIDTH, spi, cs, dc, rst) {
#endif
}

/**************************************************************************/
/*!
    @brief  Change whether display is on or off
    @param	 enable True if you want the display ON, false OFF
*/
/**************************************************************************/
void Adafruit_SSD1331::enableDisplay(boolean enable) {
  sendCommand(enable ? SSD1331_CMD_DISPLAYON : SSD1331_CMD_DISPLAYOFF);
}

#ifdef SSD1331_ENABLE_ACCELERATION

// split the color into channels, with the top bit of each channel in bit 5.
#define split_color(color) \
uint8_t r = (color >> 10) & 0x3E;\
uint8_t g = (color >> 5) & 0x3F;\
uint8_t b = (color << 1) & 0x3E;


/*!
    @brief  A lower-level version of writeFillRect(). This version requires
            all inputs are in-bounds, that width and height are positive,
            and no part extends offscreen. NO EDGE CLIPPING OR REJECTION IS
            PERFORMED. If higher-level graphics primitives are written to
            handle their own clipping earlier in the drawing process, this
            can avoid unnecessary function calls and repeated clipping
            operations in the lower-level functions.
    @param  x      Horizontal position of first corner. MUST BE WITHIN
                   SCREEN BOUNDS.
    @param  y      Vertical position of first corner. MUST BE WITHIN SCREEN
                   BOUNDS.
    @param  w      Rectangle width in pixels. MUST BE POSITIVE AND NOT
                   EXTEND OFF SCREEN.
    @param  h      Rectangle height in pixels. MUST BE POSITIVE AND NOT
                   EXTEND OFF SCREEN.
    @param  color  16-bit fill color in '565' RGB format.
    @note   This is a new function, no graphics primitives besides rects
            and horizontal/vertical lines are written to best use this yet.
*/
// void Adafruit_SSD1331::writeFillRectPreclipped(int16_t x, int16_t y,
//                                                      int16_t w, int16_t h,
//                                                      uint16_t color) {
// #if 1
//   // For comparison/testing
//   Adafruit_SPITFT::writeFillRectPreclipped(x, y, w, h, color);
// #else
//   split_color(color);
  
//   sendCommand(SSD1331_CMD_FILL); // enable fill
//   sendCommand(0x01);
//   sendCommand(SSD1331_CMD_DRAWRECT); // enter "draw rectangle" mode
//   sendCommand(x); // starting column
//   sendCommand(y); // starting row
//   sendCommand(x + w); // finishing column
//   sendCommand(y + h); // finishing row
//   sendCommand(r);  // outline color
//   sendCommand(g);
//   sendCommand(b);
//   sendCommand(r);  // fill color
//   sendCommand(g);
//   sendCommand(b);
// #endif
// }

/**************************************************************************/
/*!
   @brief   Draw a rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void Adafruit_SSD1331::drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
                            uint16_t color) {
  startWrite();

  split_color(color);

  SPI_DC_LOW();  // enter command mode
  
  spiWrite(SSD1331_CMD_FILL); // disble fill
  spiWrite(0x00);
  spiWrite(SSD1331_CMD_DRAWRECT); // enter "draw rectangle" mode
  spiWrite(x); // starting column
  spiWrite(y); // starting row
  spiWrite(x + w); // finishing column
  spiWrite(y + h); // finishing row
  spiWrite(r);  // outline color
  spiWrite(g);
  spiWrite(b);
  spiWrite(r);  // fill color (unused, but the command apparently still requires it)
  spiWrite(g);
  spiWrite(b);

  SPI_DC_HIGH(); // exit command mode

  endWrite();
}

/**************************************************************************/
/*!
   @brief    Write a rectangle completely with one color, overwrite in
   subclasses if startWrite is defined!
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void Adafruit_SSD1331::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                 uint16_t color) {
  if ((x >= _width) || y >= _height)
    return;

  int16_t x1 = x + w;
  int16_t y1 = y + h;

  if (x1 >= _width)
    x1 = _width - 1;
  if (y1 >= _height)
    y1 = _height - 1;

  split_color(color);

  SPI_DC_LOW();  // enter command mode
  
  spiWrite(SSD1331_CMD_FILL); // enable fill
  spiWrite(0x01);
  spiWrite(SSD1331_CMD_DRAWRECT); // enter "draw rectangle" mode
  spiWrite(x); // starting column
  spiWrite(y); // starting row
  spiWrite(x1); // finishing column
  spiWrite(y1); // finishing row
  spiWrite(r);  // outline color
  spiWrite(g);
  spiWrite(b);
  spiWrite(r);  // fill color
  spiWrite(g);
  spiWrite(b);

  SPI_DC_HIGH(); // exit command mode
}

/**************************************************************************/
/*!
   @brief    Draw a line
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void Adafruit_SSD1331::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                            uint16_t color) {

  split_color(color);

  SPI_DC_LOW();  // enter command mode

  spiWrite(SSD1331_CMD_DRAWLINE); // enter "draw rectangle" mode
  spiWrite(x0); // starting column
  spiWrite(y0); // starting row
  spiWrite(x1); // finishing column
  spiWrite(y1); // finishing row
  spiWrite(r);  // color
  spiWrite(g);
  spiWrite(b);

  SPI_DC_HIGH(); // exit command mode
}
#endif
