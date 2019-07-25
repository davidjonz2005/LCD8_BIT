#include "main.h"
#include "MCUFRIEND_kbv.h"
#include "stm32f1xx_hal.h"
typedef volatile uint32_t RwReg;
#define XP_PORT GPIOC
#define XP_PIN GPIO_PIN_7
#define YP_PORT GPIOC
#define YP_PIN GPIO_PIN_1
#define XM_PORT GPIOC
#define XM_PIN GPIO_PIN_0
#define YM_PORT GPIOC
#define YM_PIN GPIO_PIN_8




void TouchScreen_init(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym, uint16_t rx);

  uint8_t isTouching(void);
  uint16_t pressure(void);
  int readTouchY();
  int readTouchX();
  TSPoint getPoint();
  int16_t pressureThreshhold;


  