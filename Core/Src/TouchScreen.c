#include "MCUFRIEND_kbv.h"
#include "stm32f1xx_hal.h"

#include "TouchScreen.h"
#include "adc.h"
#define NUMSAMPLES 2
uint16_t _rxplate;

int16_t pressureThreshhold;

#if (NUMSAMPLES > 2)
static void insert_sort(int array[], uint8_t size) {
  uint8_t j;
  int save;
  
  for (int i = 1; i < size; i++) {
    save = array[i];
    for (j = i; j >= 1 && save < array[j - 1]; j--)
      array[j] = array[j - 1];
    array[j] = save; 
  }
}
#endif

TSPoint getPoint(void)
{
  int x, y, z;
  int samples[NUMSAMPLES];
  uint8_t i, valid;

  valid = 1;

  Gp_in(YP_PORT, YP_PIN, GPIO_NOPULL);
  Gp_in(YM_PORT, YM_PIN, GPIO_NOPULL);
//	HAL_GPIO_WritePin(YP_PORT, YP_PIN, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(YM_PORT, YM_PIN, GPIO_PIN_RESET);
  Gp_out(XP_PORT, XP_PIN, GPIO_NOPULL);
  Gp_out(XM_PORT, XM_PIN, GPIO_NOPULL);

  HAL_GPIO_WritePin(XP_PORT, XP_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(XM_PORT, XM_PIN, GPIO_PIN_RESET);

  DWT_Delay_us(20);

  HAL_ADC_MspInit(&YP_ADC);
  MX_ADC1_Init();
  for (i = 0; i < NUMSAMPLES; i++)
  {
		//HAL_ADCEx_Calibration_Start(&YP_ADC);
    HAL_ADCEx_InjectedStart(&YP_ADC);
    HAL_ADCEx_InjectedPollForConversion(&YP_ADC, 10);
    
      /*##-5- Get the converted value of regular channel #######################*/
      samples[i] = HAL_ADCEx_InjectedGetValue(&YP_ADC,1);
    
    HAL_ADCEx_InjectedStop(&YP_ADC);
  }
  HAL_ADC_MspDeInit(&YP_ADC);
	
#if NUMSAMPLES > 2
  insert_sort(samples, NUMSAMPLES);
#endif
#if NUMSAMPLES == 2
  // Allow small amount of measurement noise, because capacitive
  // coupling to a TFT display's signals can induce some noise.
  if (samples[0] - samples[1] < 0 || samples[0] - samples[1] > 0)
  {
    valid = 0;
  }
  else
  {
    samples[1] = (samples[0] + samples[1]) >> 1; // average 2 samples
  }
#endif

  x = (4095 - samples[NUMSAMPLES / 2]);

  Gp_in(XP_PORT, XP_PIN, GPIO_NOPULL);
  Gp_in(XM_PORT, XM_PIN, GPIO_NOPULL);
	
//	HAL_GPIO_WritePin(XP_PORT, XP_PIN, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(XM_PORT, XM_PIN, GPIO_PIN_RESET);
  Gp_out(YP_PORT, YP_PIN, GPIO_NOPULL);
  Gp_out(YM_PORT, YM_PIN, GPIO_NOPULL);


  HAL_GPIO_WritePin(YM_PORT, YM_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(YP_PORT, YP_PIN, GPIO_PIN_SET);


  DWT_Delay_us(20); // Fast ARM chips need to allow voltages to settle
  HAL_ADC_MspInit(&XM_ADC);
  MX_ADC2_Init();
  for (i = 0; i < NUMSAMPLES; i++)
  {
//		HAL_ADCEx_Calibration_Start(&XM_ADC);
    HAL_ADCEx_InjectedStart(&XM_ADC);
    HAL_ADCEx_InjectedPollForConversion(&XM_ADC, 10);
    
      /*##-5- Get the converted value of regular channel #######################*/
      samples[i] = HAL_ADCEx_InjectedGetValue(&XM_ADC,1);
    
    HAL_ADCEx_InjectedStop(&XM_ADC);
  }
  HAL_ADC_MspDeInit(&XM_ADC);
#if NUMSAMPLES > 2
  insert_sort(samples, NUMSAMPLES);
#endif
#if NUMSAMPLES == 2
  // Allow small amount of measurement noise, because capacitive
  // coupling to a TFT display's signals can induce some noise.
  if (samples[0] - samples[1] < -16 || samples[0] - samples[1] > 16)
  {
    valid = 0;
  }
  else
  {
    samples[1] = (samples[0] + samples[1]) >> 1; // average 2 samples
  }
#endif

  y = (4095 - samples[NUMSAMPLES / 2]);

  // Set X+ to ground
  // Set Y- to VCC
  // Hi-Z X- and Y+
  Gp_out(XP_PORT, XP_PIN, GPIO_NOPULL);
  Gp_in(YP_PORT, YP_PIN, GPIO_NOPULL);


  HAL_GPIO_WritePin(XP_PORT, XP_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(YM_PORT, YM_PIN, GPIO_PIN_SET);

  HAL_ADC_MspInit(&YP_ADC);
  HAL_ADC_MspInit(&XM_ADC);
  MX_ADC1_Init();
  MX_ADC2_Init();
  int z1, z2;
	
//  HAL_ADCEx_Calibration_Start(&XM_ADC);
    HAL_ADCEx_InjectedStart(&XM_ADC);
    HAL_ADCEx_InjectedPollForConversion(&XM_ADC, 10);
 
    /*##-5- Get the converted value of regular channel #######################*/
    z1 = HAL_ADCEx_InjectedGetValue(&XM_ADC,1);
  
  HAL_ADCEx_InjectedStop(&XM_ADC);

  HAL_ADC_MspDeInit(&XM_ADC);

//  HAL_ADCEx_Calibration_Start(&YP_ADC);
    HAL_ADCEx_InjectedStart(&YP_ADC);
    HAL_ADCEx_InjectedPollForConversion(&YP_ADC, 10);
    /*##-5- Get the converted value of regular channel #######################*/
    z2 = HAL_ADCEx_InjectedGetValue(&YP_ADC,1);
  
  HAL_ADCEx_InjectedStop(&YP_ADC);
  HAL_ADC_MspDeInit(&YP_ADC);
  if (_rxplate != 0)
  {
    // now read the x
    float rtouch;
    rtouch = z2;
    rtouch /= z1;
    rtouch -= 1;
    rtouch *= x;
    rtouch *= _rxplate;
    rtouch /= 4095;

    z = rtouch;
  }
  else
  {
    z = (4095 - (z2 - z1));
  }

  if (!valid)
  {
    z = 0;
  }
TSPoint val;
	val.x=x;
	val.y=y;
	val.z=z;
  return val;
}

void TouchScreen_init(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym,
                      uint16_t rxplate)
{

  _rxplate = rxplate;

  pressureThreshhold = 10;
}

int readTouchX(void)
{
  Gp_in(YP_PORT, YP_PIN, GPIO_PULLDOWN);
  Gp_in(YM_PORT, YM_PIN, GPIO_PULLDOWN);

  Gp_out(XP_PORT, XP_PIN, GPIO_NOPULL);
  HAL_GPIO_WritePin(XP_PORT, XP_PIN, GPIO_PIN_SET);
  Gp_out(XM_PORT, XM_PIN, GPIO_NOPULL);
  HAL_GPIO_WritePin(XM_PORT, XM_PIN, GPIO_PIN_RESET);

  uint16_t val;
  HAL_ADC_MspInit(&YP_ADC);
  HAL_ADC_Start(&YP_ADC);
  HAL_ADC_PollForConversion(&YP_ADC, 10);
  
    /*##-5- Get the converted value of regular channel #######################*/
    val = HAL_ADC_GetValue(&YP_ADC);
  
  HAL_ADC_Stop(&YP_ADC);
  HAL_ADC_MspDeInit(&YP_ADC);
  return (4095 - val);
}

int readTouchY(void)
{
  Gp_in(XP_PORT,XP_PIN,GPIO_PULLDOWN);
	Gp_in(XM_PORT,XM_PIN,GPIO_PULLDOWN);
	
	Gp_out(YP_PORT,YP_PIN,GPIO_NOPULL);
	HAL_GPIO_WritePin(YP_PORT, YP_PIN, GPIO_PIN_SET);

	Gp_out(YM_PORT,YM_PIN,GPIO_NOPULL);
	HAL_GPIO_WritePin(YM_PORT, YM_PIN, GPIO_PIN_RESET);

    uint16_t val;
  HAL_ADC_MspInit(&XM_ADC);
  HAL_ADC_Start(&XM_ADC);
  HAL_ADC_PollForConversion(&XM_ADC, 10);
  
    val = HAL_ADC_GetValue(&XM_ADC);
  
  HAL_ADC_Stop(&XM_ADC);
  HAL_ADC_MspDeInit(&XM_ADC);

  return (4095 - val);
}

uint16_t pressure(void)
{
	
  // Set X+ to ground
	Gp_out(XP_PORT,XP_PIN,GPIO_NOPULL);
	HAL_GPIO_WritePin(XP_PORT, XP_PIN, GPIO_PIN_RESET);
  // Set Y- to VCC
	Gp_out(YM_PORT,YM_PIN,GPIO_NOPULL);
	HAL_GPIO_WritePin(YM_PORT, YM_PIN, GPIO_PIN_SET);




  // Hi-Z X- and Y+
	Gp_in(XM_PORT,XM_PIN,GPIO_PULLDOWN);
	Gp_in(YP_PORT,YP_PIN,GPIO_PULLDOWN);
	

  
	HAL_ADC_MspInit(&YP_ADC);
  HAL_ADC_MspInit(&XM_ADC);
  MX_ADC1_Init();
  MX_ADC2_Init();
  int z1, z2;
	
  HAL_ADC_Start(&XM_ADC);
  HAL_ADC_PollForConversion(&XM_ADC, 10);
  z1 = HAL_ADC_GetValue(&XM_ADC);
  
  HAL_ADC_Stop(&XM_ADC);

  HAL_ADC_MspDeInit(&XM_ADC);

  HAL_ADC_Start(&YP_ADC);
  HAL_ADC_PollForConversion(&YP_ADC, 10);
  z2 = HAL_ADC_GetValue(&YP_ADC);
  
  HAL_ADC_Stop(&YP_ADC);
  HAL_ADC_MspDeInit(&YP_ADC);
	
	

  if (_rxplate != 0)
  {
    // now read the x
    float rtouch;
    rtouch = z2;
    rtouch /= z1;
    rtouch -= 1;
    rtouch *= readTouchX();
    rtouch *= _rxplate;
    rtouch /= 1024;

    return rtouch;
  }
  else
  {
    return (4095 - (z2 - z1));
  }
}


