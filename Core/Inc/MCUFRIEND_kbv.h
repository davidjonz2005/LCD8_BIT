/*
 * MCUFRIEND_kbv class inherits from Adafruit_GFX class and the Arduino Print class.
 * Any use of MCUFRIEND_kbv class and examples is dependent on Adafruit and Arduino licenses
 * The license texts are in the accompanying license.txt file
 */

#ifndef MCUFRIEND_KBV_H_
#define MCUFRIEND_KBV_H_


#include "stm32f1xx_hal.h"
#include <gfxfont.h>
#include <mcufriend_keil.h>
#include "dwt_stm32_delay.h"
//#define USE_SERIAL

extern const GFXfont FreeSmallFont;
extern int16_t _width,_height;






#define PORTRAIT 0
#define LANDSCAPE 1
#define SmallFont &FreeSmallFont

//	MCUFRIEND_kbv(int CS=A3, int RS=A2, int WR=A1, int RD=A0, int RST=A4); //shield wiring
	//MCUFRIEND_kbv(int CS, int RS, int WR, int RD, int _RST);  //dummy arguments 
	//void Pin_output(GPIO_TypeDef  *GPIOx, uint32_t pin);
	int16_t width(void);
	int16_t height(void);
	void Gp_out(GPIO_TypeDef *GPIOx, uint32_t Pin);
void Gp_in(GPIO_TypeDef *GPIOx, uint32_t Pin);
	void InitLCD(uint8_t orientation);
	uint16_t setrgb(uint8_t r, uint8_t g, uint8_t b);
	void write_8(uint8_t d);
	void     reset(void);                                       // you only need the constructor
	void     begin(uint16_t ID );                       // you only need the constructor
	void     drawPixel(int16_t x, int16_t y, uint16_t color);  // and these three
	void     WriteCmdData(uint16_t cmd, uint16_t dat);                 // ?public methods !!!
  void     pushCommand(uint16_t cmd, uint8_t * block, int8_t N);
	uint16_t color565(uint8_t r, uint8_t g, uint8_t b) ;
	uint16_t readID(void);
	void     fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void     drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) ;
	void     drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	void     fillScreen(uint16_t color) ;
	void     setRotation(uint8_t r);
  void     invertDisplay(uint8_t i);
void setTextColor(uint16_t c, uint16_t bg);
uint16_t setrgb(uint8_t  r, uint8_t g, uint8_t b) ;
	 void setColor2(uint8_t  r, uint8_t g, uint8_t b) ;
void setColor1(uint16_t c);
void clrScr(void) ;
 void fillRect1(int x1, int y1, int x2, int y2) ;
	uint16_t readReg(uint16_t reg, int8_t index);
	int16_t  readGRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h);
	uint16_t readPixel(int16_t x, int16_t y);
	void     setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1);
	void     pushColors(const uint8_t *block, int16_t n, uint8_t first, uint8_t bigend );
  void     vertScroll(int16_t top, int16_t scrollines, int16_t offset);

void setTextSize(int sz) ;
void charBounds(char c, int16_t *x, int16_t *y,
  int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy) ;
void setFont(const GFXfont* font) ;
 
	uint32_t readReg32(uint16_t reg);
	uint32_t readReg40(uint16_t reg);

	


// New color definitions.  thanks to Bodmer
#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
#define TFT_PINK        0xFC9F

#endif
