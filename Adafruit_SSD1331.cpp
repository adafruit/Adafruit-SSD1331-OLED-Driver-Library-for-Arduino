#include "Adafruit_SSD1331.h"

// Graphics library by ladyada/adafruit

#include "glcdfont.c"
#include <avr/pgmspace.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

/********************************** low level pin interface */

inline void Adafruit_SSD1331::spiwrite(uint8_t c) {
    
    //Serial.println(c, HEX);
    
    if (!_sid) {
        SPI.transfer(c);
        return;
    }
    
    volatile uint8_t *sclkportreg = portOutputRegister(sclkport);
    volatile uint8_t *sidportreg = portOutputRegister(sidport);
    
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
    *portOutputRegister(rsport) &= ~ rspin;
    //digitalWrite(_rs, LOW);
    
    *portOutputRegister(csport) &= ~ cspin;
    //digitalWrite(_cs, LOW);
    
    //Serial.print("C ");
    spiwrite(c);
    
    *portOutputRegister(csport) |= cspin;
    //digitalWrite(_cs, HIGH);
}


void Adafruit_SSD1331::writeData(uint8_t c) {
    *portOutputRegister(rsport) |= rspin;
    //digitalWrite(_rs, HIGH);
    
    *portOutputRegister(csport) &= ~ cspin;
    //digitalWrite(_cs, LOW);
    
    //Serial.print("D ");
    spiwrite(c);
    
    *portOutputRegister(csport) |= cspin;
    //digitalWrite(_cs, HIGH);
} 

/***********************************/

void Adafruit_SSD1331::goHome(void) {
  goTo(0,0);
}

uint16_t Adafruit_SSD1331::width(void) {
  return _width;
}

uint16_t Adafruit_SSD1331::height(void) {
  return _height;
}

void Adafruit_SSD1331::goTo(int x, int y) {
  if ((x >= TFTWIDTH) || (y >= TFTHEIGHT)) return;
  
  // set x and y coordinate
  writeCommand(SSD1331_CMD_SETCOLUMN);
  writeCommand(x);
  writeCommand(TFTWIDTH-1);

  writeCommand(SSD1331_CMD_SETROW);
  writeCommand(y);
  writeCommand(TFTHEIGHT-1);
}

void Adafruit_SSD1331::setCursor(uint16_t x, uint16_t y) {
  cursor_x = x;
  cursor_y = y;
}

void Adafruit_SSD1331::setTextSize(uint8_t s) {
  textsize = s;
}

void Adafruit_SSD1331::setTextColor(uint16_t c) {
  textcolor = c;
}

#if ARDUINO >= 100
size_t Adafruit_SSD1331::write(uint8_t c) {
#else
void Adafruit_SSD1331::write(uint8_t c) {
#endif
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(cursor_x, cursor_y, c, textcolor, textsize);
    cursor_x += textsize*6;
  }
#if ARDUINO >= 100
  return 1;
#endif
}

void Adafruit_SSD1331::drawString(uint16_t x, uint16_t y, char *c, 
			uint16_t color, uint8_t size) {
  while (c[0] != 0) {
    drawChar(x, y, c[0], color, size);
    x += size*6;
    c++;
  }
}
// draw a character
void Adafruit_SSD1331::drawChar(uint16_t x, uint16_t y, char c, 
		      uint16_t color, uint8_t size) {
  for (uint8_t i =0; i<5; i++ ) {
    uint8_t line = pgm_read_byte(font+(c*5)+i);
    for (uint8_t j = 0; j<8; j++) {
      if (line & 0x1) {
	if (size == 1) // default size
	  drawPixel(x+i, y+j, color);
	else {  // big size
	  fillRect(x+i*size, y+j*size, size, size, color);
	} 
      }
      line >>= 1;
    }
  }
}

// draw a triangle!
void Adafruit_SSD1331::drawTriangle(uint16_t x0, uint16_t y0,
			  uint16_t x1, uint16_t y1,
			  uint16_t x2, uint16_t y2, uint16_t color)
{
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color); 
}

void Adafruit_SSD1331::fillTriangle ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color)
{
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }

  int32_t dx1, dx2, dx3; // Interpolation deltas
  int32_t sx1, sx2, sy; // Scanline co-ordinates

  sx2=(int32_t)x0 * (int32_t)1000; // Use fixed point math for x axis values
  sx1 = sx2;
  sy=y0;

  // Calculate interpolation deltas
  if (y1-y0 > 0) dx1=((x1-x0)*1000)/(y1-y0);
    else dx1=0;
  if (y2-y0 > 0) dx2=((x2-x0)*1000)/(y2-y0);
    else dx2=0;
  if (y2-y1 > 0) dx3=((x2-x1)*1000)/(y2-y1);
    else dx3=0;

  // Render scanlines (horizontal lines are the fastest rendering method)
  if (dx1 > dx2)
  {
    for(; sy<=y1; sy++, sx1+=dx2, sx2+=dx1)
    {
      drawHorizontalLine(sx1/1000, sy, (sx2-sx1)/1000, color);
    }
    sx2 = x1*1000;
    sy = y1;
    for(; sy<=y2; sy++, sx1+=dx2, sx2+=dx3)
    {
      drawHorizontalLine(sx1/1000, sy, (sx2-sx1)/1000, color);
    }
  }
  else
  {
    for(; sy<=y1; sy++, sx1+=dx1, sx2+=dx2)
    {
      drawHorizontalLine(sx1/1000, sy, (sx2-sx1)/1000, color);
    }
    sx1 = x1*1000;
    sy = y1;
    for(; sy<=y2; sy++, sx1+=dx3, sx2+=dx2)
    {
      drawHorizontalLine(sx1/1000, sy, (sx2-sx1)/1000, color);
    }
  }
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

// draw a rectangle
void Adafruit_SSD1331::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
		      uint16_t color) {
  // smarter version
  drawHorizontalLine(x, y, w, color);
  drawHorizontalLine(x, y+h-1, w, color);
  drawVerticalLine(x, y, h, color);
  drawVerticalLine(x+w-1, y, h, color);
}

// draw a rounded rectangle
void Adafruit_SSD1331::drawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r,
			   uint16_t color) {
  // smarter version
  drawHorizontalLine(x+r, y, w-2*r, color);
  drawHorizontalLine(x+r, y+h-1, w-2*r, color);
  drawVerticalLine(x, y+r, h-2*r, color);
  drawVerticalLine(x+w-1, y+r, h-2*r, color);
  // draw four corners
  drawCircleHelper(x+r, y+r, r, 1, color);
  drawCircleHelper(x+w-r-1, y+r, r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r, y+h-r-1, r, 8, color);
}


// fill a rounded rectangle
void Adafruit_SSD1331::fillRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r,
			   uint16_t color) {
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r, y+r, r, 2, h-2*r-1, color);
}

// fill a circle
void Adafruit_SSD1331::fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
  drawVerticalLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

// used to do circles and roundrects!
void Adafruit_SSD1331::fillCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, uint16_t delta,
			uint16_t color) {

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    if (cornername & 0x1) {
      drawVerticalLine(x0+x, y0-y, 2*y+1+delta, color);
      drawVerticalLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      drawVerticalLine(x0-x, y0-y, 2*y+1+delta, color);
      drawVerticalLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}


// draw a circle outline

void Adafruit_SSD1331::drawCircle(uint16_t x0, uint16_t y0, uint16_t r, 
			uint16_t color) {
  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  drawCircleHelper(x0, y0, r, 0xF, color);
}

void Adafruit_SSD1331::drawCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername,
			uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;


  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    } 
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

/**************************************************************************/
/*! 
    @brief  Draws a filled rectangle using HW acceleration
*/
/**************************************************************************/
void Adafruit_SSD1331::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fillcolor) 
{
  if ((x >= TFTWIDTH) || (x+w >= TFTWIDTH) ||
      (y >= TFTHEIGHT) || (y+h >= TFTHEIGHT)) {
      return;
	}

  // fill!
  writeCommand(SSD1331_CMD_FILL);
  writeCommand(0x01);

  writeCommand(SSD1331_CMD_DRAWRECT);
  writeCommand(x & 0xFF);							// Starting column
  writeCommand(y & 0xFF);							// Starting row
  writeCommand((x+w) & 0xFF);						// End column
  writeCommand((y+h) & 0xFF);						// End row

  // Outline color
  writeCommand((uint8_t)((fillcolor >> 11) & 0x1F));
  writeCommand((uint8_t)((fillcolor >> 5) & 0x3F));
  writeCommand((uint8_t)(fillcolor & 0x1F));
  // Fill color
  writeCommand((uint8_t)((fillcolor >> 11) & 0x1F));
  writeCommand((uint8_t)((fillcolor >> 5) & 0x3F));
  writeCommand((uint8_t)(fillcolor & 0x1F));
 
  // Delay while the fill completes
  delay(SSD1331_DELAYS_HWFILL); 
}

void Adafruit_SSD1331::drawVerticalLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color)
{
  if (x >= _width) return;
  drawLine(x, y, x, y+length, color);
}

void Adafruit_SSD1331::drawHorizontalLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color)
{
  if (y >= _height) return;
  drawLine(x, y, x+length, y, color);
}

void Adafruit_SSD1331::drawFastLine(uint16_t x, uint16_t y, uint16_t length, 
			  uint16_t color, uint8_t rotflag) {
			  
	// Normal drawLine is already HW accelerated ... keep this method for backward
	// compatibility, but use drawLine instead
	if ((x >= TFTWIDTH) || (x+length >= TFTWIDTH) || (y >= TFTHEIGHT)) 
	{
	  return;
	}
	
	if (rotflag)
	{
		// Vertical line
		drawLine(x, y, x+length, y, color);
	}
	else
	{
		// Horizontal line
		drawLine(x, y, x, y+length, color);
	}
}

void Adafruit_SSD1331::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) 
{
  uint16_t x, pixels;

  // Switch x1 and x0 if required
  if (x1 < x0)
  {
    x = x1;
    x1 = x0;
    x0 = x;
  }

  // Switch y1 and y0 if required
  if (y1 < y0)
  {
    x = y1;
    y1 = y0;
    y0 = x;
  }

  writeCommand(SSD1331_CMD_DRAWLINE);
  writeCommand(x0);
  writeCommand(y0);
  writeCommand(x1);
  writeCommand(y1);
  delay(SSD1331_DELAYS_HWLINE);  
  writeCommand((uint8_t)((color >> 11) & 0x1F));
  writeCommand((uint8_t)((color >> 5) & 0x3F));
  writeCommand((uint8_t)(color & 0x1F));
  delay(SSD1331_DELAYS_HWLINE);  
  delay(SSD1331_DELAYS_HWLINE);  
}

void Adafruit_SSD1331::fillScreen(uint16_t color) {
	fillRect(0, 0, TFTWIDTH-1, TFTHEIGHT-1, color);
}

void Adafruit_SSD1331::drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  if ((x >= TFTWIDTH) || (y >= TFTHEIGHT)) return;
  
  goTo(x, y);
  
  // setup for data
  *portOutputRegister(rsport) |= rspin;
  *portOutputRegister(csport) &= ~ cspin;
  
  spiwrite(color >> 8);    
  spiwrite(color);
  
  *portOutputRegister(csport) |= cspin;  
}

void Adafruit_SSD1331::initDisplay(void) {
    // set pin directions
    pinMode(_rs, OUTPUT);
    
    if (_sclk) {
        pinMode(_sclk, OUTPUT);
        sclkport = digitalPinToPort(_sclk);
        sclkpin = digitalPinToBitMask(_sclk);
        
        pinMode(_sid, OUTPUT);
        sidport = digitalPinToPort(_sid);
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
    csport = digitalPinToPort(_cs);
    
    rspin = digitalPinToBitMask(_rs);
    rsport = digitalPinToPort(_rs);
    
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
	writeCommand(SSD1331_CMD_DISPLAYOFF);  		// 0xAE
	writeCommand(SSD1331_CMD_SETREMAP); 		// 0xA0
	#if defined SSD1331_COLORORDER_RGB
	writeCommand(0x72);							// RGB Color
	#else
	writeCommand(0x76);							// BGR Color
	#endif
	writeCommand(SSD1331_CMD_STARTLINE); 		// 0xA1
	writeCommand(0x0);
	writeCommand(SSD1331_CMD_DISPLAYOFFSET); 	// 0xA2
	writeCommand(0x0);
	writeCommand(SSD1331_CMD_NORMALDISPLAY);  	// 0xA4
	writeCommand(SSD1331_CMD_SETMULTIPLEX); 	// 0xA8
	writeCommand(0x3F);  						// 0x3F 1/64 duty
	writeCommand(SSD1331_CMD_SETMASTER);  		// 0xAD
	writeCommand(0x8E);
	writeCommand(SSD1331_CMD_POWERMODE);  		// 0xB0
	writeCommand(0x0B);
	writeCommand(SSD1331_CMD_PRECHARGE);  		// 0xB1
	writeCommand(0x31);
	writeCommand(SSD1331_CMD_CLOCKDIV);  		// 0xB3
	writeCommand(0xF0);  						// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
	writeCommand(SSD1331_CMD_PRECHARGEA);  		// 0x8A
	writeCommand(0x64);
	writeCommand(SSD1331_CMD_PRECHARGEB);  		// 0x8B
	writeCommand(0x78);
	writeCommand(SSD1331_CMD_PRECHARGEA);  		// 0x8C
	writeCommand(0x64);
	writeCommand(SSD1331_CMD_PRECHARGELEVEL);  	// 0xBB
	writeCommand(0x3A);
	writeCommand(SSD1331_CMD_VCOMH);  			// 0xBE
	writeCommand(0x3E);
	writeCommand(SSD1331_CMD_MASTERCURRENT);  	// 0x87
	writeCommand(0x06);
	writeCommand(SSD1331_CMD_CONTRASTA);  		// 0x81
	writeCommand(0x91);
	writeCommand(SSD1331_CMD_CONTRASTB);  		// 0x82
	writeCommand(0x50);
	writeCommand(SSD1331_CMD_CONTRASTC);  		// 0x83
	writeCommand(0x7D);
	writeCommand(SSD1331_CMD_DISPLAYON);		//--turn on oled panel    
}

/********************************* low level pin initialization */

Adafruit_SSD1331::Adafruit_SSD1331(uint8_t cs, uint8_t rs, uint8_t sid, uint8_t sclk, uint8_t rst) {
    _cs = cs;
    _rs = rs;
    _sid = sid;
    _sclk = sclk;
    _rst = rst;

	_width = TFTWIDTH;
	_height = TFTHEIGHT;

	cursor_y = cursor_x = 0;
	textsize = 1;
	textcolor = 0xFFFF;
}

Adafruit_SSD1331::Adafruit_SSD1331(uint8_t cs, uint8_t rs,  uint8_t rst) {
    _cs = cs;
    _rs = rs;
    _sid = 0;
    _sclk = 0;
    _rst = rst;

	_width = TFTWIDTH;
	_height = TFTHEIGHT;

	cursor_y = cursor_x = 0;
	textsize = 1;
	textcolor = 0xFFFF;
}



































