#include "stm32f1xx_hal.h"


#define SUPPORT_8347D //HX8347-D, HX8347-G, HX8347-I, HX8367-A +520 bytes, 0.27s


#include "MCUFRIEND_kbv.h"

void Gp_out(GPIO_TypeDef *GPIOx, uint32_t Pin)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = Pin;
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
uint32_t test;
void write_8(uint8_t d)                         
    {                                      
				//D0_GPIO_Port->BSRR 
			
//			if((val&0xff)>0)
//				test=D0_Pin;
//			else
//				test=(D0_Pin)<<16;
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
	return 
	((((D0_GPIO_Port->IDR & D0_Pin) > 0) ? 1 : 0) << 0) | 
	((((D1_GPIO_Port->IDR & D1_Pin) > 0) ? 1 : 0) << 1) | 
	((((D2_GPIO_Port->IDR & D2_Pin) > 0) ? 1 : 0) << 2) | 
	((((D3_GPIO_Port->IDR & D3_Pin) > 0) ? 1 : 0) << 3) | 
	((((D4_GPIO_Port->IDR & D4_Pin) > 0) ? 1 : 0) << 4) | 
	((((D5_GPIO_Port->IDR & D5_Pin) > 0) ? 1 : 0) << 5) | 
	((((D6_GPIO_Port->IDR & D6_Pin) > 0) ? 1 : 0) << 6) | 
	((((D7_GPIO_Port->IDR & D7_Pin) > 0) ? 1 : 0) << 7);
}

#define MIPI_DCS_REV1 (1 << 0)
#define AUTO_READINC (1 << 1)
#define READ_BGR (1 << 2)
#define READ_LOWHIGH (1 << 3)
#define READ_24BITS (1 << 4)
#define XSA_XEA_16BIT (1 << 5)
#define READ_NODUMMY (1 << 6)
#define INVERT_GS (1 << 8)
#define INVERT_SS (1 << 9)
#define MV_AXIS (1 << 10)
#define INVERT_RGB (1 << 11)
#define REV_SCREEN (1 << 12)
#define FLIP_VERT (1 << 13)
#define FLIP_HORIZ (1 << 14)

#if (defined(USES_16BIT_BUS)) //only comes from SPECIALs
#define USING_16BIT_BUS 1
#else
#define USING_16BIT_BUS 0
#endif

MCUFRIEND_kbv() 
{
    // we can not access GPIO pins until AHB has been enabled.
}

static uint8_t done_reset, is8347, is555, is9797;
static uint16_t color565_to_555(uint16_t color)
{
    return (color & 0xFFC0) | ((color & 0x1F) << 1) | ((color & 0x01)); //lose Green LSB, extend Blue LSB
}
static uint16_t color555_to_565(uint16_t color)
{
    return (color & 0xFFC0) | ((color & 0x0400) >> 5) | ((color & 0x3F) >> 1); //extend Green LSB
}
static uint8_t color565_to_r(uint16_t color)
{
    return ((color & 0xF800) >> 8); // transform to rrrrrxxx
}
static uint8_t color565_to_g(uint16_t color)
{
    return ((color & 0x07E0) >> 3); // transform to ggggggxx
}
static uint8_t color565_to_b(uint16_t color)
{
    return ((color & 0x001F) << 3); // transform to bbbbbxxx
}
static void write24(uint16_t color)
{
    uint8_t r = color565_to_r(color);
    uint8_t g = color565_to_g(color);
    uint8_t b = color565_to_b(color);
    write8(r);
    write8(g);
    write8(b);
}

void reset(void)
{
    done_reset = 1;
    setWriteDir();
    CTL_INIT();
    CS_IDLE;
    RD_IDLE;
    WR_IDLE;
    RESET_IDLE;
    delay(50);
    RESET_ACTIVE;
    delay(100);
    RESET_IDLE;
    delay(100);
    WriteCmdData(0xB0, 0x0000); //R61520 needs this to read ID
}

static void writecmddata(uint16_t cmd, uint16_t dat)
{
    CS_ACTIVE;
    WriteCmd(cmd);
    WriteData(dat);
    CS_IDLE;
}

void WriteCmdData(uint16_t cmd, uint16_t dat) { writecmddata(cmd, dat); }

static void WriteCmdParamN(uint16_t cmd, int8_t N, uint8_t *block)
{
    CS_ACTIVE;
    WriteCmd(cmd);
    while (N-- > 0)
    {
        uint8_t u8 = *block++;
        write8(u8);
        if (N && is8347)
        {
            cmd++;
            WriteCmd(cmd);
        }
    }
    CS_IDLE;
}

static inline void WriteCmdParam4(uint8_t cmd, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4)
{
    uint8_t d[4];
    d[0] = d1, d[1] = d2, d[2] = d3, d[3] = d4;
    WriteCmdParamN(cmd, 4, d);
}

//#define WriteCmdParam4(cmd, d1, d2, d3, d4) {uint8_t d[4];d[0] = d1, d[1] = d2, d[2] = d3, d[3] = d4;WriteCmdParamN(cmd, 4, d);}
void pushCommand(uint16_t cmd, uint8_t *block, int8_t N) { WriteCmdParamN(cmd, N, block); }

static uint16_t read16bits(void)
{
    uint16_t ret;
    uint8_t lo;

    READ_8(ret);

    //all MIPI_DCS_REV1 style params are 8-bit
    READ_8(lo);
    return (ret << 8) | lo;
}

uint16_t readReg(uint16_t reg, int8_t index)
{
    uint16_t ret;
    uint8_t lo;
    if (!done_reset)
        reset();
    CS_ACTIVE;
    WriteCmd(reg);
    setReadDir();
    delay(1); //1us should be adequate
    //    READ_16(ret);
    do
    {
        ret = read16bits();
    } while (--index >= 0); //need to test with SSD1963
    RD_IDLE;
    CS_IDLE;
    setWriteDir();
    return ret;
}

uint32_t readReg32(uint16_t reg)
{
    uint16_t h = readReg(reg, 0);
    uint16_t l = readReg(reg, 1);
    return ((uint32_t)h << 16) | (l);
}

uint32_t readReg40(uint16_t reg)
{
    uint16_t h = readReg(reg, 0);
    uint16_t m = readReg(reg, 1);
    uint16_t l = readReg(reg, 2);
    return ((uint32_t)h << 24) | (m << 8) | (l >> 8);
}

uint16_t readID(void)
{
    uint16_t ret, ret2;
    uint8_t msb;
    ret = readReg(0);  //forces a reset() if called before begin()
    if (ret == 0x5408) //the SPFD5408 fails the 0xD3D3 test.
        return 0x5408;
    if (ret == 0x5420) //the SPFD5420 fails the 0xD3D3 test.
        return 0x5420;
    if (ret == 0x8989) //SSD1289 is always 8989
        return 0x1289;
    ret = readReg(0x67); //HX8347-A
    if (ret == 0x4747)
        return 0x8347;
    //#if defined(SUPPORT_1963) && USING_16BIT_BUS
    ret = readReg32(0xA1); //SSD1963: [01 57 61 01]
    if (ret == 0x6101)
        return 0x1963;
    if (ret == 0xFFFF) //R61526: [xx FF FF FF]
        return 0x1526; //subsequent begin() enables Command Access
                       //    if (ret == 0xFF00)          //R61520: [xx FF FF 00]
                       //        return 0x1520;          //subsequent begin() enables Command Access
                       //#endif
    ret = readReg40(0xBF);
    if (ret == 0x8357) //HX8357B: [xx 01 62 83 57 FF]
        return 0x8357;
    if (ret == 0x9481) //ILI9481: [xx 02 04 94 81 FF]
        return 0x9481;
    if (ret == 0x1511) //?R61511: [xx 02 04 15 11] not tested yet
        return 0x1511;
    if (ret == 0x1520) //?R61520: [xx 01 22 15 20]
        return 0x1520;
    if (ret == 0x1526) //?R61526: [xx 01 22 15 26]
        return 0x1526;
    if (ret == 0x1581) //R61581:  [xx 01 22 15 81]
        return 0x1581;
    if (ret == 0x1400) //?RM68140:[xx FF 68 14 00] not tested yet
        return 0x6814;
    ret = readReg32(0xD4);
    if (ret == 0x5310) //NT35310: [xx 01 53 10]
        return 0x5310;
    ret = readReg32(0xD7);
    if (ret == 0x8031) //weird unknown from BangGood [xx 20 80 31] PrinceCharles
        return 0x8031;
    ret = readReg40(0xEF); //ILI9327: [xx 02 04 93 27 FF]
    if (ret == 0x9327)
        return 0x9327;
    ret = readReg32(0xFE) >> 8; //weird unknown from BangGood [04 20 53]
    if (ret == 0x2053)
        return 0x2053;
    uint32_t ret32 = readReg32(0x04);
    msb = ret32 >> 16;
    ret = ret32;
    //    if (msb = 0x38 && ret == 0x8000) //unknown [xx 38 80 00] with D3 = 0x1602
    if (msb == 0x00 && ret == 0x8000)
    { //HX8357-D [xx 00 80 00]
#if 1
        uint8_t cmds[] = {0xFF, 0x83, 0x57};
        pushCommand(0xB9, cmds, 3);
        msb = readReg(0xD0);
        if (msb == 0x99)
            return 0x0099; //HX8357-D from datasheet
        if (msb == 0x90)   //HX8357-C undocumented
#endif
            return 0x9090; //BIG CHANGE: HX8357-D was 0x8357
    }
    //    if (msb == 0xFF && ret == 0xFFFF) //R61526 [xx FF FF FF]
    //        return 0x1526;          //subsequent begin() enables Command Access
    if (ret == 0x1526) //R61526 [xx 06 15 26] if I have written NVM
        return 0x1526; //subsequent begin() enables Command Access
    if (ret == 0x89F0) //ST7735S: [xx 7C 89 F0]
        return 0x7735;
    if (ret == 0x8552) //ST7789V: [xx 85 85 52]
        return 0x7789;
    if (ret == 0xAC11) //?unknown [xx 61 AC 11]
        return 0xAC11;
    ret32 = readReg32(0xD3); //[xx 91 63 00]
    ret = ret32 >> 8;
    if (ret == 0x9163)
        return ret;
    ret = readReg32(0xD3); //for ILI9488, 9486, 9340, 9341
    msb = ret >> 8;
    if (msb == 0x93 || msb == 0x94 || msb == 0x98 || msb == 0x77 || msb == 0x16)
        return ret; //0x9488, 9486, 9340, 9341, 7796
    if (ret == 0x00D3 || ret == 0xD3D3)
        return ret;    //16-bit write-only bus

    return readReg(0); //0154, 7783, 9320, 9325, 9335, B505, B509
}

// independent cursor and window registers.   S6D0154, ST7781 increments.  ILI92320/5 do not.
int16_t readGRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h)
{
    uint16_t ret, dummy, _MR = _MW;
    int16_t n = w * h, row = 0, col = 0;
    uint8_t r, g, b, tmp;
    if (!is8347 && _lcd_capable & MIPI_DCS_REV1) // HX8347 uses same register
        _MR = 0x2E;
    if (_lcd_ID == 0x1602)
        _MR = 0x2E;
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    while (n > 0)
    {
        if (!(_lcd_capable & MIPI_DCS_REV1))
        {
            WriteCmdData(_MC, x + col);
            WriteCmdData(_MP, y + row);
        }
        CS_ACTIVE;
        WriteCmd(_MR);
        setReadDir();
        if (_lcd_capable & READ_NODUMMY)
        {
            ;
        }
        else if ((_lcd_capable & MIPI_DCS_REV1) || _lcd_ID == 0x1289)
        {
            READ_8(r);
        }
        else
        {
            READ_16(dummy);
        }
        if (_lcd_ID == 0x1511)
            READ_8(r); //extra dummy for R61511
        while (n)
        {
            if (_lcd_capable & READ_24BITS)
            {
                READ_8(r);
                READ_8(g);
                READ_8(b);
                if (_lcd_capable & READ_BGR)
                    ret = color565(b, g, r);
                else
                    ret = color565(r, g, b);
            }
            else
            {
                READ_16(ret);
                if (_lcd_capable & READ_LOWHIGH)
                    ret = (ret >> 8) | (ret << 8);
                if (_lcd_capable & READ_BGR)
                    ret = (ret & 0x07E0) | (ret >> 11) | (ret << 11);
            }

            *block++ = ret;
            n--;
            if (!(_lcd_capable & AUTO_READINC))
                break;
        }
        if (++col >= w)
        {
            col = 0;
            if (++row >= h)
                row = 0;
        }
        RD_IDLE;
        CS_IDLE;
        setWriteDir();
    }
    if (!(_lcd_capable & MIPI_DCS_REV1))
        setAddrWindow(0, 0, width() - 1, height() - 1);
    return 0;
}

void setRotation(uint8_t r)
{
    uint16_t GS, SS_v, ORG, REV = _lcd_rev;
    uint8_t val, d[3];
    rotation = r & 3; // just perform the operation ourselves on the protected variables
    _width = (rotation & 1) ? HEIGHT : WIDTH;
    _height = (rotation & 1) ? WIDTH : HEIGHT;

    Serial.print("_width:");
    Serial.println(_width);

    Serial.print("_height:");
    Serial.println(_height);
    switch (rotation)
    {
    case 0:         //PORTRAIT:
        val = 0x48; //MY=0, MX=1, MV=0, ML=0, BGR=1
        break;
    case 1:         //LANDSCAPE: 90 degrees
        val = 0x28; //MY=0, MX=0, MV=1, ML=0, BGR=1
        break;
    case 2:         //PORTRAIT_REV: 180 degrees
        val = 0x98; //MY=1, MX=0, MV=0, ML=1, BGR=1
        break;
    case 3:         //LANDSCAPE_REV: 270 degrees
        val = 0xF8; //MY=1, MX=1, MV=1, ML=1, BGR=1
        break;
    }
    if (_lcd_capable & INVERT_GS)
        val ^= 0x80;
    if (_lcd_capable & INVERT_SS)
        val ^= 0x40;
    if (_lcd_capable & INVERT_RGB)
        val ^= 0x08;
    if (_lcd_capable & MIPI_DCS_REV1)
    {
        else if (is8347)
        {
            _MC = 0x02, _MP = 0x06, _MW = 0x22, _SC = 0x02, _EC = 0x04, _SP = 0x06, _EP = 0x08;
            if (_lcd_ID == 0x0065)
            {                //HX8352-B
                val |= 0x01; //GS=1
                if ((val & 0x10))
                    val ^= 0xD3; //(ML) flip MY, MX, ML, SS, GS
                if (r & 1)
                    _MC = 0x82, _MP = 0x80;
                else
                    _MC = 0x80, _MP = 0x82;
            }
            if (_lcd_ID == 0x5252)
            {                //HX8352-A
                val |= 0x02; //VERT_SCROLLON
                if ((val & 0x10))
                    val ^= 0xD4; //(ML) flip MY, MX, SS. GS=1
            }
            goto common_BGR;
        }
    common_MC:
        _MC = 0x2A, _MP = 0x2B, _MW = 0x2C, _SC = 0x2A, _EC = 0x2A, _SP = 0x2B, _EP = 0x2B;
    common_BGR:
        WriteCmdParamN(is8347 ? 0x16 : 0x36, 1, &val);
        _lcd_madctl = val;
        //	    if (_lcd_ID	== 0x1963) WriteCmdParamN(0x13, 0, NULL);   //NORMAL mode
    }
    // cope with 9320 variants
    else
    {
        switch (_lcd_ID)
        {

        default:
            _MC = 0x20, _MP = 0x21, _MW = 0x22, _SC = 0x50, _EC = 0x51, _SP = 0x52, _EP = 0x53;
            GS = (val & 0x80) ? (1 << 15) : 0;
            WriteCmdData(0x60, GS | 0x2700); // Gate Scan Line (0xA700)
        common_SS:
            SS_v = (val & 0x40) ? (1 << 8) : 0;
            WriteCmdData(0x01, SS_v); // set Driver Output Control
        common_ORG:
            ORG = (val & 0x20) ? (1 << 3) : 0;

            if (val & 0x08)
                ORG |= 0x1000; //BGR
            _lcd_madctl = ORG | 0x0030;
            WriteCmdData(0x03, _lcd_madctl); // set GRAM write direction and BGR=1.
            break;

        }
    }
    if ((rotation & 1) && ((_lcd_capable & MV_AXIS) == 0))
    {
        uint16_t x;
        x = _MC, _MC = _MP, _MP = x;
        x = _SC, _SC = _SP, _SP = x; //.kbv check 0139
        x = _EC, _EC = _EP, _EP = x; //.kbv check 0139
    }
    setAddrWindow(0, 0, width() - 1, height() - 1);
    vertScroll(0, HEIGHT, 0); //reset scrolling after a rotation
}

void drawPixel(int16_t x, int16_t y, uint16_t color)
{
    // MCUFRIEND just plots at edge if you try to write outside of the box:
    if (x < 0 || y < 0 || x >= width() || y >= height())
        return;

    setAddrWindow(x, y, x, y);
    //    CS_ACTIVE; WriteCmd(_MW); write16(color); CS_IDLE; //-0.01s +98B
    if (is9797)
    {
        CS_ACTIVE;
        WriteCmd(_MW);
        write24(color);
        CS_IDLE;
    }
    else
        WriteCmdData(_MW, color);
}

void setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1)
{

#if 1
    if (_lcd_ID == 0x1526 && (rotation & 1))
    {
        int16_t dx = x1 - x, dy = y1 - y;
        if (dy == 0)
        {
            y1++;
        }
        else if (dx == 0)
        {
            x1 += dy;
            y1 -= dy;
        }
    }
#endif
    if (_lcd_capable & MIPI_DCS_REV1)
    {
        WriteCmdParam4(_SC, x >> 8, x, x1 >> 8, x1); //Start column instead of _MC
        WriteCmdParam4(_SP, y >> 8, y, y1 >> 8, y1); //
        if (is8347 && _lcd_ID == 0x0065)
        { //HX8352-B has separate _MC, _SC
            uint8_t d[2];
            d[0] = x >> 8;
            d[1] = x;
            WriteCmdParamN(_MC, 2, d); //allows !MV_AXIS to work
            d[0] = y >> 8;
            d[1] = y;
            WriteCmdParamN(_MP, 2, d);
        }
    }
    else
    {
        WriteCmdData(_MC, x);
        WriteCmdData(_MP, y);
        if (!(x == x1 && y == y1))
        { //only need MC,MP for drawPixel
            if (_lcd_capable & XSA_XEA_16BIT)
            {
                if (rotation & 1)
                    y1 = y = (y1 << 8) | y;
                else
                    x1 = x = (x1 << 8) | x;
            }
            WriteCmdData(_SC, x);
            WriteCmdData(_SP, y);
            WriteCmdData(_EC, x1);
            WriteCmdData(_EP, y1);
        }
    }
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    int16_t end;

    if (w < 0)
    {
        w = -w;
        x -= w;
    } //+ve w
    end = x + w;
    if (x < 0)
        x = 0;
    if (end > width())
        end = width();
    w = end - x;
    if (h < 0)
    {
        h = -h;
        y -= h;
    } //+ve h
    end = y + h;
    if (y < 0)
        y = 0;
    if (end > height())
        end = height();
    h = end - y;
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    CS_ACTIVE;
    WriteCmd(_MW);
    if (h > w)
    {
        end = h;
        h = w;
        w = end;
    }
    uint8_t hi = color >> 8, lo = color & 0xFF;
    while (h-- > 0)
    {
        end = w;

        do
        {
            write8(hi);
            write8(lo);
        } while (--end != 0);
    }
    CS_IDLE;
    if (!(_lcd_capable & MIPI_DCS_REV1) || ((_lcd_ID == 0x1526) && (rotation & 1)))
        setAddrWindow(0, 0, width() - 1, height() - 1);
}

static void pushColors_any(uint16_t cmd, uint8_t *block, int16_t n, bool first, uint8_t flags)
{
    uint16_t color;
    uint8_t h, l;
    bool isconst = flags & 1;
    bool isbigend = (flags & 2) != 0;
    CS_ACTIVE;
    if (first)
    {
        WriteCmd(cmd);
    }

    if (!isconst && !isbigend)
    {
        uint16_t *block16 = (uint16_t *)block;
        while (n-- > 0)
        {
            color = *block16++;
            write16(color);
        }
    }
    else

        while (n-- > 0)
        {
            if (isconst)
            {
                h = pgm_read_byte(block++);
                l = pgm_read_byte(block++);
            }
            else
            {
                h = (*block++);
                l = (*block++);
            }
            color = (isbigend) ? (h << 8 | l) : (l << 8 | h);

            if (is9797)
                write24(color);
            else
                write16(color);
        }
    CS_IDLE;
}

void pushColors(uint16_t *block, int16_t n, bool first)
{
    pushColors_any(_MW, (uint8_t *)block, n, first, 0);
}
void pushColors(uint8_t *block, int16_t n, bool first)
{
    pushColors_any(_MW, (uint8_t *)block, n, first, 2); //regular bigend
}
void pushColors(const uint8_t *block, int16_t n, bool first, bool bigend)
{
    pushColors_any(_MW, (uint8_t *)block, n, first, bigend ? 3 : 1);
}

void vertScroll(int16_t top, int16_t scrollines, int16_t offset)
{
    int16_t bfa = HEIGHT - top - scrollines; // bottom fixed area
    int16_t vsp;
    int16_t sea = top;
    if (_lcd_ID == 0x9327)
        bfa += 32;
    if (offset <= -scrollines || offset >= scrollines)
        offset = 0;     //valid scroll
    vsp = top + offset; // vertical start position
    if (offset < 0)
        vsp += scrollines; //keep in unsigned range
    sea = top + scrollines - 1;
    if (_lcd_capable & MIPI_DCS_REV1)
    {
        uint8_t d[6];    // for multi-byte parameters
                        

        d[0] = top >> 8; //TFA
        d[1] = top;
        d[2] = scrollines >> 8; //VSA
        d[3] = scrollines;
        d[4] = bfa >> 8; //BFA
        d[5] = bfa;
        WriteCmdParamN(is8347 ? 0x0E : 0x33, 6, d);
        //        if (offset == 0 && rotation > 1) vsp = top + scrollines;   //make non-valid
        d[0] = vsp >> 8; //VSP
        d[1] = vsp;
        WriteCmdParamN(is8347 ? 0x14 : 0x37, 2, d);
        if (is8347)
        {
            d[0] = (offset != 0) ? (_lcd_ID == 0x8347 ? 0x02 : 0x08) : 0;
            WriteCmdParamN(_lcd_ID == 0x8347 ? 0x18 : 0x01, 1, d); //HX8347-D
        }
        else if (offset == 0 && (_lcd_capable & MIPI_DCS_REV1))
        {
            WriteCmdParamN(0x13, 0, NULL); //NORMAL i.e. disable scroll
        }
        return;
    }
    // cope with 9320 style variants:
    switch (_lcd_ID)
    {
    default:
        // 0x6809, 0x9320, 0x9325, 0x9335, 0xB505 can only scroll whole screen
        WriteCmdData(0x61, (1 << 1) | _lcd_rev); //!NDL, VLE, REV
        WriteCmdData(0x6A, vsp);                 //VL#
        break;
    }
}

void invertDisplay(boolean i)
{
    uint8_t val;
    _lcd_rev = ((_lcd_capable & REV_SCREEN) != 0) ^ i;
    if (_lcd_capable & MIPI_DCS_REV1)
    {
        if (is8347)
        {
            // HX8347D: 0x36 Panel Characteristic. REV_Panel
            // HX8347A: 0x36 is Display Control 10
            if (_lcd_ID == 0x8347 || _lcd_ID == 0x5252) // HX8347-A, HX5352-A
                val = _lcd_rev ? 6 : 2;                 //INVON id bit#2,  NORON=bit#1
            else
                val = _lcd_rev ? 8 : 10; //HX8347-D, G, I: SCROLLON=bit3, INVON=bit1
            // HX8347: 0x01 Display Mode has diff bit mapping for A, D
            WriteCmdParamN(0x01, 1, &val);
        }
        else
            WriteCmdParamN(_lcd_rev ? 0x21 : 0x20, 0, NULL);
        return;
    }
 
        WriteCmdData(0x61, _lcd_rev);
     
}

#define TFTLCD_DELAY 0xFFFF
#define TFTLCD_DELAY8 0x7F
static void init_table(const void *table, int16_t size)
{

    uint8_t *p = (uint8_t *)table, dat[24]; //R61526 has GAMMA[22]

    while (size > 0)
    {
        uint8_t cmd = pgm_read_byte(p++);
        uint8_t len = pgm_read_byte(p++);
        if (cmd == TFTLCD_DELAY8)
        {
            delay(len);
            len = 0;
        }
        else
        {
            for (uint8_t i = 0; i < len; i++)
                dat[i] = pgm_read_byte(p++);
            WriteCmdParamN(cmd, len, dat);
        }
        size -= len + 2;
    }
}

static void init_table16(const void *table, int16_t size)
{
    uint16_t *p = (uint16_t *)table;
    while (size > 0)
    {
        uint16_t cmd = pgm_read_word(p++);
        uint16_t d = pgm_read_word(p++);
        if (cmd == TFTLCD_DELAY)
            delay(d);
        else
        {
            writecmddata(cmd, d); //static function
        }
        size -= 2 * sizeof(int16_t);
    }
}

void begin(uint16_t ID)
{
    int16_t *p16; //so we can "write" to a const protected variable.
    const uint8_t *table8_ads = NULL;
    int16_t table_size;
    reset();
    _lcd_xor = 0;
    switch (_lcd_ID = ID)
    {

    case 0x4747: //HX8347-D
        _lcd_capable = REV_SCREEN | MIPI_DCS_REV1 | MV_AXIS | INVERT_SS | AUTO_READINC | READ_24BITS;
        goto common_8347DGI;
    case 0x6767: //HX8367-A
    case 0x7575: //HX8347-G
    case 0x9595: //HX8347-I
        _lcd_capable = REV_SCREEN | MIPI_DCS_REV1 | MV_AXIS;
    common_8347DGI:
        is8347 = 1;
        static const uint8_t HX8347G_2_regValues[] PROGMEM = {
            0xEA,
            2,
            0x00,
            0x20, //PTBA[15:0]
            0xEC,
            2,
            0x0C,
            0xC4, //STBA[15:0]
            0xE8,
            1,
            0x38, //OPON[7:0]
            0xE9,
            1,
            0x10, //OPON1[7:0]
            0xF1,
            1,
            0x01, //OTPS1B
            0xF2,
            1,
            0x10, //GEN
            //Gamma 2.2 Setting
            0x40,
            13,
            0x01,
            0x00,
            0x00,
            0x10,
            0x0E,
            0x24,
            0x04,
            0x50,
            0x02,
            0x13,
            0x19,
            0x19,
            0x16,
            0x50,
            14,
            0x1B,
            0x31,
            0x2F,
            0x3F,
            0x3F,
            0x3E,
            0x2F,
            0x7B,
            0x09,
            0x06,
            0x06,
            0x0C,
            0x1D,
            0xCC,
            //Power Voltage Setting
            0x1B,
            1,
            0x1B, //VRH=4.65V
            0x1A,
            1,
            0x01, //BT (VGH~15V,VGL~-10V,DDVDH~5V)
            0x24,
            1,
            0x2F, //VMH(VCOM High voltage ~3.2V)
            0x25,
            1,
            0x57, //VML(VCOM Low voltage -1.2V)
            //****VCOM offset**///
            0x23,
            1,
            0x88, //for Flicker adjust //can reload from OTP
            //Power on Setting
            0x18,
            1,
            0x34, //I/P_RADJ,N/P_RADJ, Normal mode 60Hz
            0x19,
            1,
            0x01, //OSC_EN='1', start Osc
            0x01,
            1,
            0x00, //DP_STB='0', out deep sleep
            0x1F,
            1,
            0x88, // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0
            TFTLCD_DELAY8,
            5,
            0x1F,
            1,
            0x80, // GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
            TFTLCD_DELAY8,
            3,
            0x1F,
            1,
            0x90, // GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
            TFTLCD_DELAY8,
            5,
            0x1F,
            1,
            0xD0, // GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
            TFTLCD_DELAY8,
            5,
            //262k/65k color selection
            0x17,
            1,
            0x05, //default 0x06 262k color // 0x05 65k color
            //SET PANEL
            0x36,
            1,
            0x00, //SS_P, GS_P,REV_P,BGR_P
            //Display ON Setting
            0x28,
            1,
            0x38, //GON=1, DTE=1, D=1000
            TFTLCD_DELAY8,
            40,
            0x28,
            1,
            0x3F, //GON=1, DTE=1, D=1100

            0x16,
            1,
            0x18,
        };
        init_table(HX8347G_2_regValues, sizeof(HX8347G_2_regValues));
        Serial.println("table inited");

        break;

    default:
        p16 = (int16_t *)&WIDTH;
        *p16 = 0; //error value for WIDTH
        break;
    }
    _lcd_rev = ((_lcd_capable & REV_SCREEN) != 0);
    if (table8_ads != NULL)
    {
        static const uint8_t reset_off[] PROGMEM = {
            0x01, 0,            //Soft Reset
            TFTLCD_DELAY8, 150, // .kbv will power up with ONLY reset, sleep out, display on
            0x28, 0,            //Display Off
            0x3A, 1, 0x55,      //Pixel read=565, write=565.
        };
        static const uint8_t wake_on[] PROGMEM = {
            0x11, 0, //Sleep Out
            TFTLCD_DELAY8, 150,
            0x29, 0, //Display On
        };
        init_table(&reset_off, sizeof(reset_off));
        init_table(table8_ads, table_size); //can change PIXFMT
        init_table(&wake_on, sizeof(wake_on));
    }
    setRotation(0); //PORTRAIT
    invertDisplay(false);
}
