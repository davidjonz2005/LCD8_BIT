#ifndef __TouchScreen
#define __TouchScreen
#include "main.h"
#include "MCUFRIEND_kbv.h"
#include "stm32f1xx_hal.h"
#include "GFX_lib.h"
#include "string.h"
typedef volatile uint32_t RwReg;
//#define XP_PORT GPIOC
//#define XP_PIN GPIO_PIN_7
//#define YP_PORT GPIOC
//#define YP_PIN GPIO_PIN_1
//#define XM_PORT GPIOC
//#define XM_PIN GPIO_PIN_0
//#define YM_PORT GPIOC
//#define YM_PIN GPIO_PIN_8

#define XP_PORT GPIOA
#define XP_PIN GPIO_PIN_9
#define YP_PORT GPIOB
#define YP_PIN GPIO_PIN_0
#define XM_PORT GPIOA
#define XM_PIN GPIO_PIN_4
#define YM_PORT GPIOC
#define YM_PIN GPIO_PIN_7

#define XM_ADC hadc2
#define YP_ADC hadc1
typedef struct TSPoint TSPoint;
struct TSPoint{
  int16_t x, y, z;
};


void TouchScreen_init(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym, uint16_t rx);
uint8_t isTouching(void);
uint16_t pressure(void);
int readTouchY();
int readTouchX();
TSPoint getPoint();
#endif