// Graphics library by ladyada/adafruit 
// MIT license

#define swap(a, b) { uint16_t t = a; a = b; b = t; }

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Select one of these defines to set the pixel color order
#define SSD1331_COLORORDER_RGB
// #define SSD1331_COLORORDER_BGR

#if defined SSD1331_COLORORDER_RGB && defined SSD1331_COLORORDER_BGR
  #error "RGB and BGR can not both be defined for SSD1331_COLORODER."
#endif

// Timing Delays
#define SSD1331_DELAYS_HWFILL		(5)
#define SSD1331_DELAYS_HWLINE       (3)

// SSD1331 Commands
#define SSD1331_CMD_DRAWLINE 		0x21
#define SSD1331_CMD_DRAWRECT 		0x22
#define SSD1331_CMD_FILL 			0x26
#define SSD1331_CMD_SETCOLUMN 		0x15
#define SSD1331_CMD_SETROW    		0x75
#define SSD1331_CMD_CONTRASTA 		0x81
#define SSD1331_CMD_CONTRASTB 		0x82
#define SSD1331_CMD_CONTRASTC		0x83
#define SSD1331_CMD_MASTERCURRENT 	0x87
#define SSD1331_CMD_SETREMAP 		0xA0
#define SSD1331_CMD_STARTLINE 		0xA1
#define SSD1331_CMD_DISPLAYOFFSET 	0xA2
#define SSD1331_CMD_NORMALDISPLAY 	0xA4
#define SSD1331_CMD_DISPLAYALLON  	0xA5
#define SSD1331_CMD_DISPLAYALLOFF 	0xA6
#define SSD1331_CMD_INVERTDISPLAY 	0xA7
#define SSD1331_CMD_SETMULTIPLEX  	0xA8
#define SSD1331_CMD_SETMASTER 		0xAD
#define SSD1331_CMD_DISPLAYOFF 		0xAE
#define SSD1331_CMD_DISPLAYON     	0xAF
#define SSD1331_CMD_POWERMODE 		0xB0
#define SSD1331_CMD_PRECHARGE 		0xB1
#define SSD1331_CMD_CLOCKDIV 		0xB3
#define SSD1331_CMD_PRECHARGEA 		0x8A
#define SSD1331_CMD_PRECHARGEB 		0x8B
#define SSD1331_CMD_PRECHARGEC 		0x8C
#define SSD1331_CMD_PRECHARGELEVEL 	0xBB
#define SSD1331_CMD_VCOMH 			0xBE

class Adafruit_SSD1331 : public Print {
 public:
  Adafruit_SSD1331(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST);
  Adafruit_SSD1331(uint8_t CS, uint8_t RS, uint8_t RST);

  uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);

  // drawing primitives!
  void drawPixel(uint16_t x, uint16_t y, uint16_t color);
  void fillScreen(uint16_t color);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void drawFastLine(uint16_t x0, uint16_t y0, uint16_t l, uint16_t color, uint8_t flag);
  void drawVerticalLine(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);
  void drawHorizontalLine(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);
  void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
  void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);
  void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
  void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
  void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);
  void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);
  void drawCircle(uint16_t x0, uint16_t y0, uint16_t r,	uint16_t color);
  void fillCircle(uint16_t x0, uint16_t y0, uint16_t r,	uint16_t color);

  void setCursor(uint16_t x, uint16_t y);
  void setTextColor(uint16_t c);
  void setTextSize(uint8_t s);
#if ARDUINO >= 100
  virtual size_t write(uint8_t);
#else
  virtual void write(uint8_t);
#endif

  void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint8_t s = 1);
  void drawString(uint16_t x, uint16_t y, char *c, uint16_t color, uint8_t s = 1);

  // commands
  void initDisplay(void);
  void goHome(void);
  void goTo(int x, int y);

  void reset(void);

  /* low level */

  void writeData(uint8_t d);
  void writeCommand(uint8_t c);

  uint16_t width();
  uint16_t height();

  static const uint16_t TFTWIDTH = 96;
  static const uint16_t TFTHEIGHT = 64;

  void writeData_unsafe(uint16_t d);

  void setWriteDir(void);
  void write8(uint8_t d);

 private:
  void drawCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t corner, uint16_t color);
  void fillCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t corner, uint16_t delta, uint16_t color);
  void spiwrite(uint8_t);

  uint8_t _cs, _rs, _rst, _sid, _sclk;
  uint8_t csport, rsport, sidport, sclkport;
  uint8_t cspin, rspin, sidpin, sclkpin;

  uint16_t _width, _height;
  uint8_t textsize;
  uint16_t cursor_x, cursor_y;
  uint16_t textcolor;
};
