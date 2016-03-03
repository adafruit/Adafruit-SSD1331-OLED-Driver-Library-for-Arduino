/*************************************************** 
  This is a library for the 0.96" 16-bit Color OLED with SSD1331 driver chip

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

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define gfx_swap(a, b) { uint16_t t = a; a = b; b = t; }

#ifdef __SAM3X8E__
typedef volatile RwReg PortReg;
typedef uint32_t PortMask;
#define _BV(b) (1<<(b))
#else
typedef volatile uint8_t PortReg;
typedef uint8_t PortMask;
#endif

// Select one of these defines to set the pixel color order
#define SSD1331_COLORORDER_RGB
// #define SSD1331_COLORORDER_BGR

#if defined SSD1331_COLORORDER_RGB && defined SSD1331_COLORORDER_BGR
  #error "RGB and BGR can not both be defined for SSD1331_COLORODER."
#endif

// Timing Delays
#define SSD1331_DELAYS_HWFILL		(3)
#define SSD1331_DELAYS_HWLINE       (1)

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

class Adafruit_SSD1331 : public virtual Adafruit_GFX {
 public:
  Adafruit_SSD1331(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST);
  Adafruit_SSD1331(uint8_t CS, uint8_t RS, uint8_t RST);

  uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);

  // drawing primitives!
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  //void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t fillcolor);
  void pushColor(uint16_t c);

  // commands
  void begin(void);
  void goHome(void);
  void goTo(int x, int y);

  void reset(void);

  /* low level */

  void writeData(uint8_t d);
  void writeCommand(uint8_t c);

  static const int16_t TFTWIDTH = 96;
  static const int16_t TFTHEIGHT = 64;

  void writeData_unsafe(uint16_t d);

  void setWriteDir(void);
  void write8(uint8_t d);

 private:
  void spiwrite(uint8_t);

  uint8_t _cs, _rs, _rst, _sid, _sclk;
  PortReg *csportreg, *rsportreg, *sidportreg, *sclkportreg;
  PortMask cspin, rspin, sidpin, sclkpin;
};
