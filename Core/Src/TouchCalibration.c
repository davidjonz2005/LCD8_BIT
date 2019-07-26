
#include <TouchCalibration.h>         //Adafruit Library


TSPoint tp;
uint32_t cx, cy, cz;
uint32_t rx[8], ry[8];
int32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center, swapxy;
uint32_t calx, caly, cals;
void readResistiveTouch(void)
{
	tp = getPoint();
	Gp_out(YP_PORT,YP_PIN,GPIO_NOPULL);      //restore shared pins
	Gp_out(XM_PORT,XM_PIN,GPIO_NOPULL);
	Gp_out(XP_PORT,XP_PIN,GPIO_NOPULL);      //restore shared pins
	Gp_out(YM_PORT,YM_PIN,GPIO_NOPULL);
	HAL_GPIO_WritePin(YP_PORT,YP_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(XM_PORT,XM_PIN,GPIO_PIN_SET);
   
}

uint8_t ISPRESSED(void)
{
    // .kbv this was too sensitive !!
    // now touch has to be stable for 50ms
    int count = 0;
    uint8_t state, oldstate;
    while (count < 10) {
        readResistiveTouch();
        state = tp.z > 800;     //ADJUST THIS VALUE TO SUIT YOUR SCREEN e.g. 20 ... 250
        if (state == oldstate) count++;
        else count = 0;
        oldstate = state;
        HAL_Delay(5);
    }
    return oldstate;
}






void Calibrate_Procedure()
{
	 dispx = width();
    dispy = height();
    text_y_center = (dispy / 2) - 6;
    startup();

    fillScreen(BLACK);
    drawCrossHair(dispx - 11, 10, GRAY);
    drawCrossHair(dispx / 2, 10, GRAY);
    drawCrossHair(10, 10, GRAY);
    drawCrossHair(dispx - 11, dispy / 2, GRAY);
    drawCrossHair(10, dispy / 2, GRAY);
    drawCrossHair(dispx - 11, dispy - 11, GRAY);
    drawCrossHair(dispx / 2, dispy - 11, GRAY);
    drawCrossHair(10, dispy - 11, GRAY);
    centerprint("***********", text_y_center - 12);
    centerprint("***********", text_y_center + 12);

    calibrate(10, 10, 0);
    calibrate(10, dispy / 2, 1);
    calibrate(10, dispy - 11, 2);
    calibrate(dispx / 2, 10, 3);
    calibrate(dispx / 2, dispy - 11, 4);
    calibrate(dispx - 11, 10, 5);
    calibrate(dispx - 11, dispy / 2, 6);
    calibrate(dispx - 11, dispy - 11, 7);

    cals = ((long)(dispx - 1) << 12) + (dispy - 1);
    if (TOUCH_ORIENTATION == PORTRAIT) swapxy = rx[2] - rx[0];
    else swapxy = ry[2] - ry[0];
    swapxy = (swapxy < -500 || swapxy > 500);
    if ((TOUCH_ORIENTATION == PORTRAIT) ^ (swapxy != 0)) {
        clx = (rx[0] + rx[1] + rx[2]) / 3;
        crx = (rx[5] + rx[6] + rx[7]) / 3;
        cty = (ry[0] + ry[3] + ry[5]) / 3;
        cby = (ry[2] + ry[4] + ry[7]) / 3;
    } else {
        clx = (ry[0] + ry[1] + ry[2]) / 3;
        crx = (ry[5] + ry[6] + ry[7]) / 3;
        cty = (rx[0] + rx[3] + rx[5]) / 3;
        cby = (rx[2] + rx[4] + rx[7]) / 3;
    }
    px = (float)(crx - clx) / (dispx - 20);
    py = (float)(cby - cty) / (dispy - 20);
    //  px = 0;
    clx -= px * 10;
    crx += px * 10;
    cty -= py * 10;
    cby += py * 10;

    calx = ((long)(clx) << 14) + (long)(crx);
    caly = ((long)(cty) << 14) + (long)(cby);
    if (swapxy)
        cals |= (1L << 31);

    report();          // report results
    while (1) {}    // tread water
}

void readCoordinates()
{
    int iter = 5000;
    int failcount = 0;
    int cnt = 0;
    uint32_t tx = 0;
    uint32_t ty = 0;
    uint8_t OK = 0;
		centerprint("*  PRESS  *", text_y_center);
    while (OK == 0)
    {
       
				HAL_Delay(100);
        while (ISPRESSED() == 0) {}
//				
//        centerprint("*  HOLD!  *", text_y_center);
				//
        cnt = 0;
        iter = 400;
        do
        {
            //readResistiveTouch();
            if (tp.z > 20)
            {
                tx += tp.x;
                ty += tp.y;
                cnt++;
            }
            else
                failcount++;
        } while ((cnt < iter) && (failcount < 10000));
        if (cnt >= iter)
        {
            OK = 1;
        }
        else
        {
            tx = 0;
            ty = 0;
            cnt = 0;
        }
        if (failcount >= 10000)
            fail();
    }

    cx = tx / iter;
    cy = ty / iter;
    cz = tp.z;
}

void calibrate(int x, int y, int i)
{
    drawCrossHair(x, y, WHITE);
    readCoordinates();
		HAL_Delay(1000);
    centerprint("* RELEASE *", text_y_center);
		HAL_Delay(1000);
    drawCrossHair(x, y, GRAY);
    rx[i] = cx;
    ry[i] = cy;
//    Serial.print("\r\ncx="); Serial.print(cx);
//    Serial.print(" cy="); Serial.print(cy);
//    Serial.print(" cz="); Serial.print(cz);
//    if (msg) Serial.print(msg);
    while (ISPRESSED() == 1) {}
}

void report()
{
    uint16_t TS_LEFT, TS_RT, TS_TOP, TS_BOT, TS_WID, TS_HT, TS_SWAP;
    int16_t tmp;
    char buf[60];
    centertitle(TITLE);

    print(("To use the new calibration\n"));
    print(("settings you must map the values\n"));
    print(("from Point p = ts.getPoint() e.g.\n"));
    print(("x = map(p.x, LEFT, RT, 0, width());\n"));
    print(("y = map(p.y, TOP, BOT, 0, height());\n"));
    print(("swap p.x and p.y if diff ORIENTATION\n"));

    //.kbv show human values
    TS_LEFT = (calx >> 14) & 0x3FFF;
    TS_RT   = (calx >>  0) & 0x3FFF;
    TS_TOP  = (caly >> 14) & 0x3FFF;
    TS_BOT  = (caly >>  0) & 0x3FFF;
    TS_WID  = ((cals >> 12) & 0x0FFF) + 1;
    TS_HT   = ((cals >>  0) & 0x0FFF) + 1;
    TS_SWAP = (cals >> 31);
    if (TOUCH_ORIENTATION == LANDSCAPE) { //always show PORTRAIT first
        tmp = TS_LEFT, TS_LEFT = TS_BOT, TS_BOT = TS_RT, TS_RT = TS_TOP, TS_TOP = tmp;
        tmp = TS_WID, TS_WID = TS_HT, TS_HT = tmp;
    }
    setCursor(0, 120);
    //Serial.println("");
    sprintf(buf, "MCUFRIEND_kbv ID=0x%04X  %d x %d",
            readID(), TS_WID, TS_HT);
    print(buf);
//    Serial.println(buf);
//    sprintf(buf, "\nconst int XP=%d,XM=A%d,YP=A%d,YM=%d; //%dx%d ID=0x%04X", 
//            XP, XM - A0, YP - A0, YM, TS_WID, TS_HT, tft.readID());
//    Serial.println(buf);
    sprintf(buf, "const int TS_LEFT=%d,TS_RT=%d,TS_TOP=%d,TS_BOT=%d;", 
            TS_LEFT, TS_RT, TS_TOP, TS_BOT);
//    Serial.println(buf);
    sprintf(buf, "PORTRAIT CALIBRATION     %d x %d\n", TS_WID, TS_HT);
    print("\n");
    print(buf);
//    Serial.println(buf);
    sprintf(buf, "x = map(p.x, LEFT=%d, RT=%d, 0, %d)", TS_LEFT, TS_RT, TS_WID);
    print(buf);
//    Serial.println(buf);
    sprintf(buf, "y = map(p.y, TOP=%d, BOT=%d, 0, %d\n)", TS_TOP, TS_BOT, TS_HT);
    print(buf);
//    Serial.println(buf);
//    sprintf(buf, "Touch Pin Wiring XP=%d XM=%s YP=%s YM=%d\n",
//            XP, Aval(XM), Aval(YP), YM);
//    print("\n");
//    print(buf);
//    Serial.println(buf);

    tmp = TS_LEFT, TS_LEFT = TS_TOP, TS_TOP = TS_RT, TS_RT = TS_BOT, TS_BOT = tmp;
    tmp = TS_WID, TS_WID = TS_HT, TS_HT = tmp;

    sprintf(buf, "LANDSCAPE CALIBRATION    %d x %d\n", TS_WID, TS_HT);
    print("\n");
    print(buf);
//    Serial.println(buf);
    sprintf(buf, "x = map(p.y, LEFT=%d, RT=%d, 0, %d)\n", TS_LEFT, TS_RT, TS_WID);
    print(buf);
//    Serial.println(buf);
    sprintf(buf, "y = map(p.x, TOP=%d, BOT=%d, 0, %d)\n", TS_TOP, TS_BOT, TS_HT);
    print(buf);
//    Serial.println(buf);
}

void drawCrossHair(int x, int y, uint16_t color)
{
    drawRect(x - 10, y - 10, 20, 20, color);
    drawLine(x - 5, y, x + 5, y, color);
    drawLine(x, y - 5, x, y + 5, color);
}

void centerprint(const char *s, int y)
{
    int len = strlen(s) * 6;
    setTextColor(WHITE, RED);
    setCursor((dispx - len) / 2, y);
    print(s);
}

void centertitle(const char *s)
{
    fillScreen(BLACK);
    fillRect(0, 0, dispx, 14, RED);
    fillRect(0, 14, dispx, 1, WHITE);
    centerprint(s, 1);
    setCursor(0, 30);
    setTextColor(WHITE, BLACK);
}

void startup()
{
    centertitle(TITLE);

//    tft.print(F("#define NUMSAMPLES 3 in Library\n"));
//    tft.println(F("Use a stylus or something"));
//    tft.println(F("similar to touch as close"));
//    tft.println(F("to the center of the"));
//    tft.println(F("highlighted crosshair as"));
//    tft.println(F("possible. Keep as still as"));
//    tft.println(F("possible and keep holding"));
//    tft.println(F("until the highlight is"));
//    tft.println(F("removed. Repeat for all"));
//    tft.println(F("crosshairs in sequence.\n"));
//    tft.println(F("Report can be pasted from Serial\n"));
//    tft.println(F("Touch screen to continue"));

    while (ISPRESSED() == 0) {}
    while (ISPRESSED() == 1) {}
    //    waitForTouch();
}

void fail()
{
    centertitle("Touch Calibration FAILED");

//    tft.println(F("Unable to read the position"));
//    tft.println(F("of the press. This is a"));
//    tft.println(F("hardware issue and can"));
//    tft.println(F("not be corrected in"));
//    tft.println(F("software."));
//    tft.println(F("check XP, XM pins with a multimeter"));
//    tft.println(F("check YP, YM pins with a multimeter"));
//    tft.println(F("should be about 300 ohms"));

    while (1) {};
}