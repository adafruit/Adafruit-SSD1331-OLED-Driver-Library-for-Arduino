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

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1331.h"
#include "glcdfont.c"

#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

/********************************** low level pin interface */

inline void Adafruit_SSD1331::spiwrite(uint8_t c) {
    
    if (!_sid) {
        SPI.transfer(c);
        return;
    }
        
    int8_t i;
    
    *sclkportreg |= sclkpin;
    
    for (i=7; i>=0; i--) {
        *sclkportreg &= ~sclkpin;
        //SCLK_PORT &= ~_BV(SCLK);
        
        if (c & _BV(i)) {
            *sidportreg |= sidpin;
            //digitalWrite(_sid, HIGH);
            //SID_PORT |= _BV(SID);
        } else {
            *sidportreg &= ~sidpin;
            //digitalWrite(_sid, LOW);
            //SID_PORT &= ~_BV(SID);
        }
        
        *sclkportreg |= sclkpin;
        //SCLK_PORT |= _BV(SCLK);
    }
}


void Adafruit_SSD1331::writeCommand(uint8_t c) {
    *rsportreg &= ~ rspin;
    //digitalWrite(_rs, LOW);
    
    *csportreg &= ~ cspin;
    //digitalWrite(_cs, LOW);
    
    //Serial.print("C ");
    spiwrite(c);
    
    *csportreg |= cspin;
    //digitalWrite(_cs, HIGH);
}


void Adafruit_SSD1331::writeData(uint8_t c) {
    *rsportreg |= rspin;
    //digitalWrite(_rs, HIGH);
    
    *csportreg &= ~ cspin;
    //digitalWrite(_cs, LOW);
    
    //Serial.print("D ");
    spiwrite(c);
    
    *csportreg |= cspin;
    //digitalWrite(_cs, HIGH);
} 

/***********************************/

void Adafruit_SSD1331::goHome(void) {
  goTo(0,0);
}

void Adafruit_SSD1331::goTo(int x, int y) {
  if ((x >= WIDTH) || (y >= HEIGHT)) return;
  
  // set x and y coordinate
  writeCommand(SSD1331_CMD_SETCOLUMN);
  writeCommand(x);
  writeCommand(WIDTH-1);

  writeCommand(SSD1331_CMD_SETROW);
  writeCommand(y);
  writeCommand(HEIGHT-1);
}

uint16_t Adafruit_SSD1331::Color565(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t c;
  c = r >> 3;
  c <<= 6;
  c |= g >> 2;
  c <<= 5;
  c |= b >> 3;

  return c;
}

/**************************************************************************/
/*! 
    @brief  Draws a filled rectangle using HW acceleration
*/
/**************************************************************************/
/*
void Adafruit_SSD1331::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fillcolor) 
{
//Serial.println("fillRect");
  // check rotation, move rect around if necessary
  switch (getRotation()) {
  case 1:
    swap(x, y);
    swap(w, h);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    swap(x, y);
    swap(w, h);
    y = HEIGHT - y - 1;
    break;
  }

  // Bounds check
  if ((x >= TFTWIDTH) || (y >= TFTHEIGHT))
	return;

  // Y bounds check
  if (y+h > TFTHEIGHT)
  {
    h = TFTHEIGHT - y;
  }

  // X bounds check
  if (x+w > TFTWIDTH)
  {
    w = TFTWIDTH - x;
  }
  
  // fill!
  writeCommand(SSD1331_CMD_FILL);
  writeCommand(0x01);

  writeCommand(SSD1331_CMD_DRAWRECT);
  writeCommand(x & 0xFF);							// Starting column
  writeCommand(y & 0xFF);							// Starting row
  writeCommand((x+w-1) & 0xFF);	// End column
  writeCommand((y+h-1) & 0xFF);	// End row
  
  // Outline color
  writeCommand((uint8_t)((fillcolor >> 11) << 1));
  writeCommand((uint8_t)((fillcolor >> 5) & 0x3F));
  writeCommand((uint8_t)((fillcolor << 1) & 0x3F));
  // Fill color
  writeCommand((uint8_t)((fillcolor >> 11) << 1));
  writeCommand((uint8_t)((fillcolor >> 5) & 0x3F));
  writeCommand((uint8_t)((fillcolor << 1) & 0x3F));
 
  // Delay while the fill completes
  delay(SSD1331_DELAYS_HWFILL); 
}
*/

void Adafruit_SSD1331::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {	
  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    gfx_swap(x0, y0);
    gfx_swap(x1, y1);
    x0 = WIDTH - x0 - 1;
    x1 = WIDTH - x1 - 1;
    break;
  case 2:
    x0 = WIDTH - x0 - 1;
    y0 = HEIGHT - y0 - 1;
    x1 = WIDTH - x1 - 1;
    y1 = HEIGHT - y1 - 1;
    break;
  case 3:
    gfx_swap(x0, y0);
    gfx_swap(x1, y1);
    y0 = HEIGHT - y0 - 1;
    y1 = HEIGHT - y1 - 1;
    break;
  }

  // Boundary check
  if ((y0 >= TFTHEIGHT) && (y1 >= TFTHEIGHT))
	return;
  if ((x0 >= TFTWIDTH) && (x1 >= TFTWIDTH))
	return;	
  if (x0 >= TFTWIDTH)
    x0 = TFTWIDTH - 1;
  if (y0 >= TFTHEIGHT)
    y0 = TFTHEIGHT - 1;
  if (x1 >= TFTWIDTH)
    x1 = TFTWIDTH - 1;
  if (y1 >= TFTHEIGHT)
    y1 = TFTHEIGHT - 1;
  
  writeCommand(SSD1331_CMD_DRAWLINE);
  writeCommand(x0);
  writeCommand(y0);
  writeCommand(x1);
  writeCommand(y1);
  delay(SSD1331_DELAYS_HWLINE);  
  writeCommand((uint8_t)((color >> 11) << 1));
  writeCommand((uint8_t)((color >> 5) & 0x3F));
  writeCommand((uint8_t)((color << 1) & 0x3F));
  delay(SSD1331_DELAYS_HWLINE);  
}

void Adafruit_SSD1331::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    gfx_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    gfx_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

  goTo(x, y);
  
  // setup for data
  *rsportreg |= rspin;
  *csportreg &= ~ cspin;
  
  spiwrite(color >> 8);    
  spiwrite(color);
  
  *csportreg |= cspin;  
}

void Adafruit_SSD1331::pushColor(uint16_t color) {
  // setup for data
  *rsportreg |= rspin;
  *csportreg &= ~ cspin;
  
  spiwrite(color >> 8);    
  spiwrite(color);
  
  *csportreg |= cspin; 
}


void Adafruit_SSD1331::begin(void) {
    // set pin directions
    pinMode(_rs, OUTPUT);
    
    if (_sclk) {
        pinMode(_sclk, OUTPUT);
        sclkportreg = portOutputRegister(digitalPinToPort(_sclk));
        sclkpin = digitalPinToBitMask(_sclk);
        
        pinMode(_sid, OUTPUT);
        sidportreg = portOutputRegister(digitalPinToPort(_sid));
        sidpin = digitalPinToBitMask(_sid);
    } else {
        // using the hardware SPI
        SPI.begin();
        SPI.setDataMode(SPI_MODE3);
    }
	
    // Toggle RST low to reset; CS low so it'll listen to us
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, LOW);
    cspin = digitalPinToBitMask(_cs);
    csportreg = portOutputRegister(digitalPinToPort(_cs));
    
    rspin = digitalPinToBitMask(_rs);
    rsportreg = portOutputRegister(digitalPinToPort(_rs));
    
    if (_rst) {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, HIGH);
        delay(500);
        digitalWrite(_rst, LOW);
        delay(500);
        digitalWrite(_rst, HIGH);
        delay(500);
    }
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
}

/********************************* low level pin initialization */

Adafruit_SSD1331::Adafruit_SSD1331(uint8_t cs, uint8_t rs, uint8_t sid, uint8_t sclk, uint8_t rst) : Adafruit_GFX(TFTWIDTH, TFTHEIGHT) {
    _cs = cs;
    _rs = rs;
    _sid = sid;
    _sclk = sclk;
    _rst = rst;
}

Adafruit_SSD1331::Adafruit_SSD1331(uint8_t cs, uint8_t rs, uint8_t rst) : Adafruit_GFX(TFTWIDTH, TFTHEIGHT) {
    _cs = cs;
    _rs = rs;
    _sid = 0;
    _sclk = 0;
    _rst = rst;
}
