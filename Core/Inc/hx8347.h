/****************************************************
320x240 LCD from eBay
HX8347-a Controller

Ryan Milewski 5/2/11
blackrynius.wordpress.com
*****************************************************/
//#include <p18f4580.h> 
//#include <delays.h>
#include <math.h>

#include "stm32f1xx_hal.h"
#include "main.h"
//Pin Defines------------------------
#define DBlow	PORTD	//Low byte of data
#define	DBhigh	PORTC	//High byte of data

#define	LCD_RS_LOW		HAL_GPIO_WritePin(RS_GPIO_Port,RS_Pin,GPIO_PIN_RESET)	//Register Select
#define	LCD_RS_HIGH		HAL_GPIO_WritePin(RS_GPIO_Port,RS_Pin,GPIO_PIN_SET)	//Register Select
			
#define	LCD_WR		PORTEbits.RE1	//Write Select
//#define	LCD_RD			//Read Select (no reading in program , pull up w/10kohm)
#define	LCD_CS		PORTAbits.RA5	//Chip Select
#define	LCD_REST	PORTEbits.RE0	//Reset Pin

//Color Defines------------------------------------
#define	BLACK	0x0000
#define	RED		0xf800
#define	GREEN	0x07e0
#define	BLUE	0x001f
#define	WHITE	0xffff
#define PURPLE	0xf11f
#define YELLOW	0xffe0
#define	CYAN	0x07ff


//Function prototypes------------------------------
void LCD_write_COM(int data);
void LCD_write_DATA(int data);
void LCD_write_COM_DATA(int com, int data);
void LCD_init(void);
void LCD_address_set(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_fill_screen(unsigned int color);
void LCD_set_pixel(unsigned int x, unsigned int y, unsigned int color);
void LCD_draw_rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color, int fill);
void LCD_color_test(void);
void LCD_put_char(unsigned char c, int x, int y, int fcolor, int bcolor);
void LCD_put_str(unsigned char *pString, int x, int y, int fcolor, int bcolor);
void LCD_put_rom_str(unsigned const char *pString, int x, int y, int fcolor, int bcolor);
void LCD_draw_circle(unsigned int x, unsigned int y, int radius,int fill, unsigned int color);
void LCD_draw_line(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2, unsigned int color);


