#ifndef MCUFRIEND_KEIL_H_
#define MCUFRIEND_KEIL_H_
#include "stm32f1xx_hal.h"
#define __ARM_ARCH_7M__


#include "main.h"
// control pins as used in MCUFRIEND shields 
  #define RD_PORT RD_GPIO_Port
  #define RD_PIN  RD_Pin
  #define WR_PORT WR_GPIO_Port
  #define WR_PIN  WR_Pin
  #define CD_PORT RS_GPIO_Port
  #define CD_PIN  RS_Pin
  #define CS_PORT CS_GPIO_Port
  #define CS_PIN  CS_Pin
  #define RESET_PORT RST_GPIO_Port
  #define RESET_PIN  RST_Pin

// general purpose pin macros
 #define RD_ACTIVE  HAL_GPIO_WritePin(RD_PORT, RD_PIN, GPIO_PIN_RESET)
 #define RD_IDLE    HAL_GPIO_WritePin(RD_PORT, RD_PIN, GPIO_PIN_SET)
 #define RD_OUTPUT  Gp_out(RD_PORT,RD_PIN,GPIO_NOPULL)
 #define WR_ACTIVE  HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_RESET)
 #define WR_IDLE    HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET)
 #define WR_OUTPUT  Gp_out(WR_PORT,WR_PIN,GPIO_NOPULL)
 #define CD_COMMAND HAL_GPIO_WritePin(CD_PORT, CD_PIN, GPIO_PIN_RESET)
 #define CD_DATA    HAL_GPIO_WritePin(CD_PORT, CD_PIN, GPIO_PIN_SET)
 #define CD_OUTPUT  Gp_out(CD_PORT, CD_PIN,GPIO_NOPULL)
 #define CS_ACTIVE  HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET)
 #define CS_IDLE    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET)
 #define CS_OUTPUT  Gp_out(CS_PORT, CS_PIN,GPIO_NOPULL)
 #define RESET_ACTIVE  HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, GPIO_PIN_RESET)
 #define RESET_IDLE    HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, GPIO_PIN_SET)
 #define RESET_OUTPUT  Gp_out(RESET_PORT, RESET_PIN,GPIO_NOPULL)

#define WR_ACTIVE2  {WR_ACTIVE; WR_ACTIVE;}
#define WR_ACTIVE4  {WR_ACTIVE2; WR_ACTIVE2;}
#define WR_ACTIVE8  {WR_ACTIVE4; WR_ACTIVE4;}
#define RD_ACTIVE2  {RD_ACTIVE; RD_ACTIVE;}
#define RD_ACTIVE4  {RD_ACTIVE2; RD_ACTIVE2;}
#define RD_ACTIVE8  {RD_ACTIVE4; RD_ACTIVE4;}
#define RD_ACTIVE16 {RD_ACTIVE8; RD_ACTIVE8;}
#define WR_IDLE2  {WR_IDLE; WR_IDLE;}
#define WR_IDLE4  {WR_IDLE2; WR_IDLE2;}
#define RD_IDLE2  {RD_IDLE; RD_IDLE;}
#define RD_IDLE4  {RD_IDLE2; RD_IDLE2;}

// General macros.   IOCLR registers are 1 cycle when optimised.
#define WR_STROBE { WR_ACTIVE; WR_IDLE; }         //PWLW=TWRL=50ns
#define RD_STROBE RD_IDLE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE   //PWLR=TRDL=150ns

#if defined(TEENSY) || defined(__ARM_ARCH_7EM__) // -O2: F411@100MHz = 1.44s 
//#define WRITE_DELAY { WR_ACTIVE; WR_ACTIVE; WR_ACTIVE; WR_ACTIVE; }
//#define READ_DELAY  { RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; }
#if 0
#elif defined(STM32F401xx)
#warning 84MHz
#define WRITE_DELAY { WR_ACTIVE2; } //100MHz
#define READ_DELAY  { RD_ACTIVE4; }
#elif defined(STM32F411xx)
#define WRITE_DELAY { WR_ACTIVE2; WR_ACTIVE; } //100MHz
#define READ_DELAY  { RD_ACTIVE4; RD_ACTIVE2; }
#elif defined(STM32F446xx)
#warning 180MHz
#define WRITE_DELAY { WR_ACTIVE8; } //180MHz
#define IDLE_DELAY  { WR_IDLE2;WR_IDLE; }
#define READ_DELAY  { RD_ACTIVE16;}
#elif defined(STM32F767xx)
#warning 216MHz
#define WRITE_DELAY { WR_ACTIVE8; WR_ACTIVE8; } //216MHz
#define IDLE_DELAY  { WR_IDLE4;WR_IDLE4; }
#define READ_DELAY  { RD_ACTIVE16;RD_ACTIVE16;RD_ACTIVE16;}
#elif defined(STM32H743xx) //STM32H743 GPIO needs testing
#define WRITE_DELAY { WR_ACTIVE8;WR_ACTIVE2; } //F_CPU=400MHz
#define IDLE_DELAY  { WR_IDLE2;WR_IDLE; }
#define READ_DELAY  { RD_ACTIVE16;RD_ACTIVE16;RD_ACTIVE4;}
#else 
#error check specific STM32
#endif
#elif defined(__ARM_ARCH_7M__) // -O2: F103@72MHz = 2.68s
#define WRITE_DELAY { }
#define READ_DELAY  { RD_ACTIVE;  }
#elif defined(__ARM_ARCH_6M__) // -O2: F072@48MHz = 5.03s
#define WRITE_DELAY { }
#define READ_DELAY  { }
#endif

#ifndef IDLE_DELAY
#define IDLE_DELAY    { WR_IDLE; }
#endif

#define write8(x)     { write_8(x); WRITE_DELAY; WR_STROBE; IDLE_DELAY; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; READ_DELAY; dst = read_8(); RD_IDLE2; RD_IDLE; } // read 250ns after RD_ACTIVE goes low
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define CTL_INIT()   { RD_OUTPUT; WR_OUTPUT; CD_OUTPUT; CS_OUTPUT; RESET_OUTPUT; }
#define WriteCmd(x)  { CD_COMMAND; write16(x); CD_DATA; }
#define WriteData(x) { write16(x); }


#endif   //MCUFRIEND_KEIL_H_
