// Graphics library by ladyada/adafruit with init code from Rossum
// MIT license
#define HEX__(n) 0x##n##LU
#define B8__(x) ((x&0x0000000FLU)?1:0) \
+((x&0x000000F0LU)?2:0) \
+((x&0x00000F00LU)?4:0) \
+((x&0x0000F000LU)?8:0) \
+((x&0x000F0000LU)?16:0) \
+((x&0x00F00000LU)?32:0) \
+((x&0x0F000000LU)?64:0) \
+((x&0xF0000000LU)?128:0)

// User-visible Macros
#define B8(d) ((unsigned char)B8__(HEX__(d)))
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) + B8(dlsb))
#define B32(dmsb,db2,db3,dlsb) \
(((unsigned long)B8(dmsb)<<24) \
+ ((unsigned long)B8(db2)<<16) \
+ ((unsigned long)B8(db3)<<8) \
+ B8(dlsb))
#include "Adafruit_TFTLCD_8bit_STM32.h"
  uint8_t  driver;
//#include "ili932x.h"
//#include "ili9341.h"
#include "main.h"
#include "hx8347g.h"

//#include "hx8357x.h"

//gpio_reg_map * cntrlRegs;
//gpio_reg_map * dataRegs;
int16_t _width,_height,rotation;
/*****************************************************************************/
// Constructor
/*****************************************************************************/


/*****************************************************************************/
uint16_t setrgb(uint8_t r, uint8_t g, uint8_t b) { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3); }
//volatile uint16_t delaycounter=0;
//	void delayMicroseconds(uint16_t d){
//	for(int i=0;i<d*64;i++)
//		delaycounter++;
//}
void GFX_setRotation(uint8_t x) {
    rotation = (x & 3);
    switch(rotation) {
        case 0:
        case 2:
            _width  = TFTWIDTH;
            _height = TFTHEIGHT;
            break;
        case 1:
        case 3:
            _width  = TFTHEIGHT;
            _height = TFTWIDTH;
            break;
    }
}


void Gp_out(GPIO_TypeDef *GPIOx, uint32_t Pin)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = Pin;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
void Gp_in(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
void write8(uint8_t d)                         
    {                                      

				D0_GPIO_Port->BSRR = ((d&1)>0?D0_Pin:D0_Pin<<16);
        D1_GPIO_Port->BSRR = ((d&2)>0?D1_Pin:D1_Pin<<16);
        D2_GPIO_Port->BSRR = ((d&4)>0?D2_Pin:D2_Pin<<16);
        D3_GPIO_Port->BSRR = ((d&8)>0?D3_Pin:D3_Pin<<16);
        D4_GPIO_Port->BSRR = ((d&16)>0?D4_Pin:D4_Pin<<16);
        D5_GPIO_Port->BSRR = ((d&32)>0?D5_Pin:D5_Pin<<16);
        D6_GPIO_Port->BSRR = ((d&64)>0?D6_Pin:D6_Pin<<16);
        D7_GPIO_Port->BSRR = ((d&128)>0?D7_Pin:D7_Pin<<16);
    }
uint8_t read_8(void) {
	
	 RD_ACTIVE;
  DWT_Delay_us(10);
  uint8_t temp = 	((((D0_GPIO_Port->IDR & D0_Pin) > 0) ? 1 : 0) << 0) | 
	((((D1_GPIO_Port->IDR & D1_Pin) > 0) ? 1 : 0) << 1) | 
	((((D2_GPIO_Port->IDR & D2_Pin) > 0) ? 1 : 0) << 2) | 
	((((D3_GPIO_Port->IDR & D3_Pin) > 0) ? 1 : 0) << 3) | 
	((((D4_GPIO_Port->IDR & D4_Pin) > 0) ? 1 : 0) << 4) | 
	((((D5_GPIO_Port->IDR & D5_Pin) > 0) ? 1 : 0) << 5) | 
	((((D6_GPIO_Port->IDR & D6_Pin) > 0) ? 1 : 0) << 6) | 
	((((D7_GPIO_Port->IDR & D7_Pin) > 0) ? 1 : 0) << 7);
  DWT_Delay_us(10);
  RD_IDLE;
  DWT_Delay_us(10);
  return temp;
	

}

void setWriteDir()
{
    Gp_out(D0_GPIO_Port, D0_Pin);
    Gp_out(D1_GPIO_Port, D1_Pin);
    Gp_out(D2_GPIO_Port, D2_Pin);
    Gp_out(D3_GPIO_Port, D3_Pin);
    Gp_out(D4_GPIO_Port, D4_Pin);
    Gp_out(D5_GPIO_Port, D5_Pin);
    Gp_out(D6_GPIO_Port, D6_Pin);
    Gp_out(D7_GPIO_Port, D7_Pin);
}
void setReadDir()
{
    Gp_in(D0_GPIO_Port, D0_Pin);
    Gp_in(D1_GPIO_Port, D1_Pin);
    Gp_in(D2_GPIO_Port, D2_Pin);
    Gp_in(D3_GPIO_Port, D3_Pin);
    Gp_in(D4_GPIO_Port, D4_Pin);
    Gp_in(D5_GPIO_Port, D5_Pin);
    Gp_in(D6_GPIO_Port, D6_Pin);
    Gp_in(D7_GPIO_Port, D7_Pin);
}

void begin(uint16_t id)
{
//	_height=TFT
	reset();

/*	// test routine for the port pins:
	//Serial.print("CR: "); Serial.println(dataRegs->CRL, HEX);
	//Serial.println("testing TFT data pins...");
	uint8_t c = 0;
	while(1) {
		write8(c);
		c++;
	}
*/
	if ((id == 0x9325) || (id == 0x9328)) {
		driver = ID_932X;
		//ili932x_begin();
	} else if (id == 0x9341) {
		driver = ID_9341;
		//ili9341_begin();
	} else if (id == 0x8357) {
		// HX8357D
		driver = ID_HX8357D;
		//hx8357x_begin();
	} else if (id == 0x9338) {
		// ILI9338?
		driver = ID_9338;
		//hx8357x_begin();
	} else if(id == 0x7575) {
		driver = ID_7575;
		hx8347g_begin();
	} else {
		driver = ID_UNKNOWN;
	}
}

/*****************************************************************************/
void reset(void)
{
//	cntrlRegs = TFT_CNTRL_PORT->regs;
//	dataRegs = TFT_DATA_PORT->regs;
	//Set control lines as output
	//cntrlRegs->CRL = (cntrlRegs->CRL & 0xFFFF0000) | 0x00003333;
//	Gp_out(RD_GPIO_Port,RD_Pin);
//	Gp_out(WR_GPIO_Port,WR_Pin);
//	Gp_out(RD_GPIO_Port,RD_Pin);
//	Gp_out(RD_GPIO_Port,RD_Pin);
//	Gp_out(RD_GPIO_Port,RD_Pin);
//	
//	pinMode(TFT_RD, OUTPUT);
//	pinMode(TFT_WR, OUTPUT);
//	pinMode(TFT_RS, OUTPUT);
//	pinMode(TFT_CS, OUTPUT);
	CS_IDLE; // Set all control bits to HIGH (idle)
	CD_DATA; // Signals are ACTIVE LOW
	
	WR_IDLE;
	RD_IDLE;
/* testing PB4 - sometimes reserved by debug port, see http://www.stm32duino.com/viewtopic.php?f=35&t=1130&p=24289#p24289
	pinMode(PB4, OUTPUT);
	digitalWrite(PB4, HIGH);
	while (1) {
		CS_ACTIVE;
		//WR_STROBE;
		digitalWrite(PB4, LOW);
		digitalWrite(PB4, HIGH);
		CS_IDLE;
		HAL_Delay(1000);
	}
*/
	//set up 8 bit parallel port to write mode.
	setWriteDir();

	// toggle RST low to reset
//	if (TFT_RST > 0) {
		//pinMode(TFT_RST, OUTPUT);
		//digitalWrite(TFT_RST, HIGH);
		RESET_IDLE;
		
		HAL_Delay(100);
		RESET_ACTIVE;
		//digitalWrite(TFT_RST, LOW);
		HAL_Delay(100);
		RESET_IDLE;
//		digitalWrite(TFT_RST, HIGH);
		HAL_Delay(100);
//	}
}

/*****************************************************************************/
// Sets the LCD address window (and address counter, on 932X).
// Relevant to rect/screen fills and H/V lines.  Input coordinates are
// assumed pre-sorted (e.g. x2 >= x1).
/*****************************************************************************/
void setAddrWindow(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	if(driver == ID_932X) {
		//ili932x_setAddrWindow(x1, y1, x2, y2);
	} else if(driver == ID_7575) {
		hx8347g_setAddrWindow(x1, y1, x2, y2);
	} else if ((driver == ID_9341) || (driver == ID_HX8357D) || (driver == ID_9338)){
		//ili9341_setAddrWindow(x1, y1, x2, y2);
	}
}

/*****************************************************************************/
// Fast block fill operation for fillScreen, fillRect, H/V line, etc.
// Requires setAddrWindow() has previously been called to set the fill
// bounds.  'len' is inclusive, MUST be >= 1.
/*****************************************************************************/
void flood(uint16_t color, uint32_t len)
{
  uint16_t blocks;
  uint8_t  i, hi = color >> 8,
              lo = color;

	CS_ACTIVE_CD_COMMAND;
  if (driver == ID_9341) {
    //writeCommand(ILI9341_MEMORYWRITE); // 16 bit
  } else if (driver == ID_932X) {
    //writeCommand(ILI932X_RW_GRAM); // 16 bit
  } else if (driver == ID_HX8357D || driver == ID_9338) {
    //write8(HX8357_RAMWR);
  } else {
    write8(0x22); // Write data to GRAM
  }

  // Write first pixel normally, decrement counter by 1
  CD_DATA;
  write8(hi);
  write8(lo);
  len--;

  blocks = (uint16_t)(len / 64); // 64 pixels/block
  if(hi == lo) {
    // High and low bytes are identical.  Leave prior data
    // on the port(s) and just toggle the write strobe.
    while(blocks--) {
      i = 16; // 64 pixels/block / 4 pixels/pass
      do {
        WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; // 2 bytes/pixel
        WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; // x 4 pixels
      } while(--i);
    }
    // Fill any remaining pixels (1 to 64)
	i = len & 63;
    while (i--) {
		WR_STROBE; WR_STROBE;
	}
  } else {
    while(blocks--) {
      i = 16; // 64 pixels/block / 4 pixels/pass
      do {
        write8(hi); write8(lo); write8(hi); write8(lo);
        write8(hi); write8(lo); write8(hi); write8(lo);
      } while(--i);
    }
	i = len & 63;
    while (i--) { // write here the remaining data
      write8(hi); write8(lo);
    }
  }
  CS_IDLE;
}

/*****************************************************************************/
void drawFastHLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
  int16_t x2;

  // Initial off-screen clipping
  if((length <= 0     ) ||
     (y      <  0     ) || ( y                  >= _height) ||
     (x      >= _width) || ((x2 = (x+length-1)) <  0      )) return;

  if(x < 0) {        // Clip left
    length += x;
    x       = 0;
  }
  if(x2 >= _width) { // Clip right
    x2      = _width - 1;
    length  = x2 - x + 1;
  }

  setAddrWindow(x, y, x2, y);
  flood(color, length);
  if(driver == ID_932X) setAddrWindow(0, 0, _width - 1, _height - 1);
  else                  hx8347g_setLR();
}

/*****************************************************************************/
void drawFastVLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
  int16_t y2;

  // Initial off-screen clipping
  if((length <= 0      ) ||
     (x      <  0      ) || ( x                  >= _width) ||
     (y      >= _height) || ((y2 = (y+length-1)) <  0     )) return;
  if(y < 0) {         // Clip top
    length += y;
    y       = 0;
  }
  if(y2 >= _height) { // Clip bottom
    y2      = _height - 1;
    length  = y2 - y + 1;
  }

  setAddrWindow(x, y, x, y2);
  flood(color, length);
  if(driver == ID_932X) setAddrWindow(0, 0, _width - 1, _height - 1);
  else                  hx8347g_setLR();
}

/*****************************************************************************/
void fillRect(int16_t x1, int16_t y1, int16_t w, int16_t h, uint16_t fillcolor)
{
	//Serial.println("\n::fillRect...");
  int16_t  x2, y2;

  // Initial off-screen clipping
  if( (w            <= 0     ) ||  (h             <= 0      ) ||
      (x1           >= _width) ||  (y1            >= _height) ||
     ((x2 = x1+w-1) <  0     ) || ((y2  = y1+h-1) <  0      )) return;
  if(x1 < 0) { // Clip left
    w += x1;
    x1 = 0;
  }
  if(y1 < 0) { // Clip top
    h += y1;
    y1 = 0;
  }
  if(x2 >= _width) { // Clip right
    x2 = _width - 1;
    w  = x2 - x1 + 1;
  }
  if(y2 >= _height) { // Clip bottom
    y2 = _height - 1;
    h  = y2 - y1 + 1;
  }

  setAddrWindow(x1, y1, x2, y2);
  flood(fillcolor, (uint32_t)w * (uint32_t)h);
  if(driver == ID_932X) setAddrWindow(0, 0, _width - 1, _height - 1);
  //else                  hx8347g_setLR();
}

/*****************************************************************************/
void fillScreen(uint16_t color)
{
  if(driver == ID_932X) {
    // For the 932X, a full-screen address window is already the default
    // state, just need to set the address pointer to the top-left corner.
    // Although we could fill in any direction, the code uses the current
    // screen rotation because some users find it disconcerting when a
    // fill does not occur top-to-bottom.
	//ili932x_fillScreen(color);
  } else if ((driver == ID_9341) || (driver == ID_7575) || (driver == ID_HX8357D) || (driver == ID_9338)) {
    // For these, there is no settable address pointer, instead the
    // address window must be set for each drawing operation.  However,
    // this display takes rotation into account for the parameters, no
    // need to do extra rotation math here.
    setAddrWindow(0, 0, _width - 1, _height - 1);
  }
  flood(color, (long)TFTWIDTH * (long)TFTHEIGHT);
}

/*****************************************************************************/
void drawPixel(int16_t x, int16_t y, uint16_t color)
{
  // Clip
  if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

  if(driver == ID_932X) {

	  //ili932x_drawPixel(x, y, color);

  } else if(driver == ID_7575) {

    uint8_t hi, lo;
    switch(rotation) {
     default: lo = 0   ; break;
     case 1 : lo = 0x60; break;
     case 2 : lo = 0xc0; break;
     case 3 : lo = 0xa0; break;
    }
    writeRegister8(   HX8347G_MEMACCESS      , lo);
    // Only upper-left is set -- bottom-right is full screen default
    writeRegisterPair(HX8347G_COLADDRSTART_HI, HX8347G_COLADDRSTART_LO, x);
    writeRegisterPair(HX8347G_ROWADDRSTART_HI, HX8347G_ROWADDRSTART_LO, y);
    hi = color >> 8; lo = color;
    CD_COMMAND; write8(0x22); CD_DATA; write8(hi); write8(lo);

  } else if ((driver == ID_9341) || (driver == ID_HX8357D) || (driver == ID_9338)) {

    setAddrWindow(x, y, x+1, y+1);
    writeRegister16(0x2C, color);
  }
}

/*****************************************************************************/
// Draw an image bitmap (16bits per color) at the specified position from the provided buffer.
/*****************************************************************************/
void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t * bitmap)
{
	if ( x>=0 && (x+w)<_width && y>=0 && (y+h)<=_height ) {
		// all pixel visible, do it in the fast way
		setAddrWindow(x,y,x+w-1,y+h-1);
		pushColors((uint16_t*)bitmap, w*h, 1);
	} else {
		// some pixels outside visible area, do it in the classical way to disable off-screen points
		int16_t i, j;
		uint16_t * colorP = (uint16_t*)bitmap;
		for(j=0; j<h; j++) {
			for(i=0; i<w; i++ ) {
				drawPixel(x+i, y+j, *colorP++);
			}
		}
	}
}

/*****************************************************************************/
// Issues 'raw' an array of 16-bit color values to the LCD; used
// externally by BMP examples.  Assumes that setWindowAddr() has
// previously been set to define the bounds.  Max 255 pixels at
// a time (BMP examples read in small chunks due to limited RAM).
/*****************************************************************************/
void pushColors(uint16_t *data, int16_t len, uint8_t first)
{
  uint16_t color;
  uint8_t  hi, lo;
  CS_ACTIVE;
  if(first > 0) { // Issue GRAM write command only on first call
    CD_COMMAND;
    if(driver == ID_932X) write8(0x00);
    if ((driver == ID_9341) || (driver == ID_HX8357D) || (driver == ID_9338)){
       write8(0x2C);
     }  else {
       write8(0x22);
     }
  }
  CD_DATA;
  while(len--) {
    color = *data++;
    hi    = color >> 8; // Don't simplify or merge these
    lo    = color;      // lines, there's macro shenanigans
    write8(hi);         // going on.
    write8(lo);
  }
  CS_IDLE;
}

/*****************************************************************************/
//void invertDisplay(uint8_t i)
//{
//	if ( driver==ID_932X ) ili932x_invertDisplay(i);
//	else if ( driver==ID_9341 ) {
//		writeCommand( (i>0) ? ILI9341_INVERTON : ILI9341_INVERTOFF);
//		CS_IDLE;
//	}
//}
/*****************************************************************************/
void setRotation(uint8_t x)
{
  // Call parent rotation func first -- sets up rotation flags, etc.
  //Adafruit_GFX::
	GFX_setRotation(x);
  // Then perform hardware-specific rotation operations...

  if (driver == ID_932X) {

    //ili932x_setRotation(x);

  } else if (driver == ID_7575) {

	hx8347g_setRotation(x);

  } 
//	else if (driver == ID_9341) {
//   // MEME, HX8357D uses same registers as 9341 but different values
//   uint16_t t;

//   switch (rotation) {
//   case 1:
//     //t = ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
//     break;
//   case 2:
//     //t = ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR;
//     break;
//   case 3:
//     t = ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
//     break;
//   case 0:
//   default:
//    t = ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR;
//    break;
//  }
//   writeRegister8(ILI9341_MADCTL, t ); // MADCTL
//   // For 9341, init default full-screen address window:
//   //setAddrWindow(0, 0, _width - 1, _height - 1); // CS_IDLE happens here

//  } 
//else if (driver == ID_HX8357D) {
//    // MEME, HX8357D uses same registers as 9341 but different values
//    uint16_t t;

//    switch (rotation) {
//      case 1:
//        t = HX8357B_MADCTL_MY | HX8357B_MADCTL_MV | HX8357B_MADCTL_RGB;
//        break;
//      case 2:
//        t = HX8357B_MADCTL_RGB;
//        break;
//      case 3:
//        t = HX8357B_MADCTL_MX | HX8357B_MADCTL_MV | HX8357B_MADCTL_RGB;
//        break;
//      case 0:
//      default:
//        t = HX8357B_MADCTL_MX | HX8357B_MADCTL_MY | HX8357B_MADCTL_RGB;
//        break;
//    }
//    writeRegister8(ILI9341_MADCTL, t ); // MADCTL
//    // For 8357, init default full-screen address window:
//    setAddrWindow(0, 0, _width - 1, _height - 1); // CS_IDLE happens here
//  } 
//else if (driver == ID_9338) {
//    // Like HX8357D, but mirrored
//    uint16_t t;
//    
//    switch (rotation) {
//      case 1:
//        t = HX8357B_MADCTL_MY | HX8357B_MADCTL_MX | HX8357B_MADCTL_MV | HX8357B_MADCTL_RGB;
//        break;
//      case 2:
//        t = HX8357B_MADCTL_MY | HX8357B_MADCTL_RGB;
//        break;
//      case 3:
//        t = HX8357B_MADCTL_MV | HX8357B_MADCTL_RGB;
//        break;
//      case 0:
//      default:
//        t = HX8357B_MADCTL_MH | HX8357B_MADCTL_MX | HX8357B_MADCTL_RGB;
//        break;
//    }
//    writeRegister8(ILI9341_MADCTL, t ); // MADCTL
//    // For 8357, init default full-screen address window:
//    setAddrWindow(0, 0, _width - 1, _height - 1); // CS_IDLE happens here
//  }
}

/*****************************************************************************/
//uint8_t read8_(void)
//{
//  RD_ACTIVE;
//  delayMicroseconds(10);
//  uint8_t temp = ( (dataRegs->IDR>>TFT_DATA_SHIFT) & 0x00FF);
//  delayMicroseconds(10);
//  RD_IDLE;
//  delayMicroseconds(10);
//  return temp;
//}

/*****************************************************************************/
inline void writeCommand(uint16_t c)
{
	CS_ACTIVE_CD_COMMAND;
	write8(c>>8);
	write8(c);
}

/*****************************************************************************/
// Because this function is used infrequently, it configures the ports for
// the read operation, reads the data, then restores the ports to the write
// configuration.  Write operations happen a LOT, so it's advantageous to
// leave the ports in that state as a default.
/*****************************************************************************/
uint16_t readPixel(int16_t x, int16_t y)
{
  if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return 0;

  if(driver == ID_932X) {

    //return ili932x_readPixel(x, y);

  } else if(driver == ID_7575) {

    uint8_t r, g, b;
    writeRegisterPair(HX8347G_COLADDRSTART_HI, HX8347G_COLADDRSTART_LO, x);
    writeRegisterPair(HX8347G_ROWADDRSTART_HI, HX8347G_ROWADDRSTART_LO, y);
    writeCommand(0x22); // Read data from GRAM
    setReadDir();  // Set up LCD data port(s) for READ operations
    CD_DATA;
    read8(r);      // First byte back is a dummy read
    read8(r);
    read8(g);
    read8(b);
    setWriteDir(); // Restore LCD data port(s) to WRITE configuration
    CS_IDLE;
    return (((uint16_t)r & B8(11111000)) << 8) |
           (((uint16_t)g & B8(11111100)) << 3) |
           (           b              >> 3);
  } else return 0;
}

/*****************************************************************************/
uint16_t readID(void)
{
  /*
  for (uint8_t i=0; i<128; i++) {
    Serial.print("$"); Serial.print(i, HEX);
    Serial.print(" = 0x"); Serial.println(readReg(i), HEX);
  }
  */
    /*
      Serial.println("!");
      for (uint8_t i=0; i<4; i++) {
        Serial.print("$"); Serial.print(i, HEX);
        Serial.print(" = 0x"); Serial.println(readReg(i), HEX);
      }
    */
/**/
  if (readReg32(0x04) == 0x8000) { // eh close enough
    // setc!
    //writeRegister24(HX8357D_SETC, 0xFF8357);
    HAL_Delay(300);
    //Serial.println(readReg(0xD0), HEX);
    if (readReg32(0xD0) == 0x990000) {
      return 0x8357;
    }
  }

  uint16_t id = readReg32(0xD3);
  if (id != 0x9341 && id != 0x9338) {
    id = readReg(0);
  }
	//Serial.print("ID: "); Serial.println(id,HEX);
  return id;
}

/*****************************************************************************/
uint32_t readReg32(uint8_t r)
{
  uint32_t id;
  uint8_t x;

  // try reading register #4
  writeCommand(r);
  setReadDir();  // Set up LCD data port(s) for READ operations
  CD_DATA;
  DWT_Delay_us(50);
  read8(x);
  id = x;          // Do not merge or otherwise simplify
  id <<= 8;              // these lines.  It's an unfortunate
  read8(x);
  id  |= x;        // shenanigans that are going on.
  id <<= 8;              // these lines.  It's an unfortunate
  read8(x);
  id  |= x;        // shenanigans that are going on.
  id <<= 8;              // these lines.  It's an unfortunate
  read8(x);
  id  |= x;        // shenanigans that are going on.
  CS_IDLE;
  setWriteDir();  // Restore LCD data port(s) to WRITE configuration
  return id;
}
/*****************************************************************************/
uint16_t readReg(uint8_t r)
{
  uint16_t id;
  uint8_t x;

  writeCommand(r);
  setReadDir();  // Set up LCD data port(s) for READ operations
  CD_DATA;
  DWT_Delay_us(10);
  read8(x);
  id = x;          // Do not merge or otherwise simplify
  id <<= 8;              // these lines.  It's an unfortunate
  read8(x);
  id |= x;        // shenanigans that are going on.
  CS_IDLE;
  setWriteDir();  // Restore LCD data port(s) to WRITE configuration

  //Serial.print("Read $"); Serial.print(r, HEX); 
  //Serial.print(":\t0x"); Serial.println(id, HEX);
  return id;
}

/*****************************************************************************/
void writeRegister8(uint16_t a, uint8_t d)
{
  writeCommand(a);
  CD_DATA;
  write8(d);
  CS_IDLE;
}

/*****************************************************************************/
void writeRegister16(uint16_t a, uint16_t d)
{
  writeCommand(a);
  CD_DATA;
  write8(d>>8);
  write8(d);
  CS_IDLE;
}

/*****************************************************************************/
void writeRegisterPair(uint16_t aH, uint16_t aL, uint16_t d)
{
  writeRegister8(aH, d>>8);
  writeRegister8(aL, d);
}

/*****************************************************************************/
void writeRegister24(uint16_t r, uint32_t d)
{
  writeCommand(r); // includes CS_ACTIVE
  CD_DATA;
  write8(d >> 16);
  write8(d >> 8);
  write8(d);
  CS_IDLE;
}

/*****************************************************************************/
void writeRegister32(uint16_t r, uint32_t d)
{
  writeCommand(r);
  CD_DATA;
  write8(d >> 24);
  write8(d >> 16);
  write8(d >> 8);
  write8(d);
  CS_IDLE;
}

/****************************************************************************
void writeRegister32(uint16_t r, uint16_t d1, uint16_t d2)
{
  writeCommand(r);
  CD_DATA;
  write8(d1 >> 8);
  write8(d1);
  write8(d2 >> 8);
  write8(d2);
  CS_IDLE;
}
*/
//Adafruit_TFTLCD_8bit_STM32 tft;
