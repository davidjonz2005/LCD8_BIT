
#ifndef touch_calibration
#define touch_calibration
#include "stm32f1xx_hal.h"
#include "TouchScreen.h"



#define PORTRAIT  0
#define LANDSCAPE 1

#define TOUCH_ORIENTATION  PORTRAIT
#define TITLE "TouchScreen.h GFX Calibration"

void readResistiveTouch(void);

uint8_t ISPRESSED(void);





void Calibrate_Procedure();

void readCoordinates();

void calibrate(int x, int y, int i);

void report();

void drawCrossHair(int x, int y, uint16_t color);
void centerprint(const char *s, int y);

void centertitle(const char *s);
void startup();
void fail();
#endif