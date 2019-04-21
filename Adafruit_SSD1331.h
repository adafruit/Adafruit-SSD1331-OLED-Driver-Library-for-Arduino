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

#include "Arduino.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>


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

/// Class to manage hardware interface with SSD1331 chipset
class Adafruit_SSD1331 : public Adafruit_SPITFT {
 public:
  Adafruit_SSD1331(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst);
  Adafruit_SSD1331(int8_t cs, int8_t dc, int8_t rst);
  // 3-4 args using hardware SPI (must specify peripheral) (reset optional)
  Adafruit_SSD1331(SPIClass *spi, int8_t cs, int8_t dc, int8_t rst = -1);

  // commands
  void begin(uint32_t begin=8000000);

  void      setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

  void enableDisplay(boolean enable);

  static const int16_t TFTWIDTH = 96;    ///< The width of the display
  static const int16_t TFTHEIGHT = 64;   ///< The height of the display

 private:
};
