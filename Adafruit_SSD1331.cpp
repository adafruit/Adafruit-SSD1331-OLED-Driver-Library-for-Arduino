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
void Adafruit_SSD1331::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {

  uint8_t x1 = x;
  uint8_t y1 = y;
  if (x1 > 95) x1 = 95;
  if (y1 > 63) y1 = 63;

  uint8_t x2 = (x+w-1);
  uint8_t y2 = (y+h-1);
  if (x2 > 95) x2 = 95;
  if (y2 > 63) y2 = 63;

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


  startWrite();
  writeCommand(0x15); // Column addr set
  writeCommand(x1);
  writeCommand(x2);
  endWrite();

  startWrite();
  writeCommand(0x75); // Column addr set
  writeCommand(y1);
  writeCommand(y2);
  endWrite();

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

    startWrite();

    // Initialization Sequence
    writeCommand(SSD1331_CMD_DISPLAYOFF);  	// 0xAE
    writeCommand(SSD1331_CMD_SETREMAP); 	// 0xA0
#if defined SSD1331_COLORORDER_RGB
    writeCommand(0x72);				// RGB Color
#else
    writeCommand(0x76);				// BGR Color
#endif
    writeCommand(SSD1331_CMD_STARTLINE); 	// 0xA1
    writeCommand(0x0);
    writeCommand(SSD1331_CMD_DISPLAYOFFSET); 	// 0xA2
    writeCommand(0x0);
    writeCommand(SSD1331_CMD_NORMALDISPLAY);  	// 0xA4
    writeCommand(SSD1331_CMD_SETMULTIPLEX); 	// 0xA8
    writeCommand(0x3F);  			// 0x3F 1/64 duty
    writeCommand(SSD1331_CMD_SETMASTER);  	// 0xAD
    writeCommand(0x8E);
    writeCommand(SSD1331_CMD_POWERMODE);  	// 0xB0
    writeCommand(0x0B);
    writeCommand(SSD1331_CMD_PRECHARGE);  	// 0xB1
    writeCommand(0x31);
    writeCommand(SSD1331_CMD_CLOCKDIV);  	// 0xB3
    writeCommand(0xF0);  // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    writeCommand(SSD1331_CMD_PRECHARGEA);  	// 0x8A
    writeCommand(0x64);
    writeCommand(SSD1331_CMD_PRECHARGEB);  	// 0x8B
    writeCommand(0x78);
    writeCommand(SSD1331_CMD_PRECHARGEA);  	// 0x8C
    writeCommand(0x64);
    writeCommand(SSD1331_CMD_PRECHARGELEVEL);  	// 0xBB
    writeCommand(0x3A);
    writeCommand(SSD1331_CMD_VCOMH);  		// 0xBE
    writeCommand(0x3E);
    writeCommand(SSD1331_CMD_MASTERCURRENT);  	// 0x87
    writeCommand(0x06);
    writeCommand(SSD1331_CMD_CONTRASTA);  	// 0x81
    writeCommand(0x91);
    writeCommand(SSD1331_CMD_CONTRASTB);  	// 0x82
    writeCommand(0x50);
    writeCommand(SSD1331_CMD_CONTRASTC);  	// 0x83
    writeCommand(0x7D);
    writeCommand(SSD1331_CMD_DISPLAYON);	//--turn on oled panel   

    endWrite(); 
    _width  = TFTWIDTH;
    _height = TFTHEIGHT;
}




/**************************************************************************/
/*!
    @brief  Instantiate Adafruit ILI9341 driver with software SPI
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    mosi  SPI MOSI pin #
    @param    sclk  SPI Clock pin #
    @param    rst   Reset pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
Adafruit_SSD1331::Adafruit_SSD1331(uint8_t cs, uint8_t dc, uint8_t mosi, uint8_t sclk, uint8_t rst) :  Adafruit_SPITFT(TFTWIDTH, TFTHEIGHT, cs, dc, mosi, sclk, rst, -1) {
}

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit ILI9341 driver with hardware SPI
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    rst   Reset pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
Adafruit_SSD1331::Adafruit_SSD1331(uint8_t cs, uint8_t dc, uint8_t rst) : Adafruit_SPITFT(TFTWIDTH, TFTHEIGHT, cs, dc, rst) {

}

