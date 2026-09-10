#include "F2837xD_Device.h"
#include "F2837xD_Examples.h"

void InitXintf16Gpio();

void InitXintf(void)
{
    EALLOW;
    Emif1Regs.ASYNC_CS3_CR.bit.SS = 0;
    Emif1Regs.ASYNC_CS3_CR.bit.EW = 0;
    Emif1Regs.ASYNC_CS3_CR.bit.TA = 1;
    Emif1Regs.ASYNC_CS3_CR.bit.ASIZE = 1;
    Emif1Regs.ASYNC_CS3_CR.bit.W_SETUP = 2;
    Emif1Regs.ASYNC_CS3_CR.bit.W_STROBE = 2;
    Emif1Regs.ASYNC_CS3_CR.bit.W_HOLD = 2;
    Emif1Regs.ASYNC_CS3_CR.bit.R_SETUP = 2;
    Emif1Regs.ASYNC_CS3_CR.bit.R_STROBE = 2;
    Emif1Regs.ASYNC_CS3_CR.bit.R_HOLD = 2;

    Emif1Regs.ASYNC_CS2_CR.bit.SS = 0;
    Emif1Regs.ASYNC_CS2_CR.bit.EW = 0;
    Emif1Regs.ASYNC_CS2_CR.bit.TA = 1;
    Emif1Regs.ASYNC_CS2_CR.bit.ASIZE = 1;
    Emif1Regs.ASYNC_CS2_CR.bit.W_SETUP = 2 ;
    Emif1Regs.ASYNC_CS2_CR.bit.W_STROBE = 7;
    Emif1Regs.ASYNC_CS2_CR.bit.W_HOLD = 2;
    Emif1Regs.ASYNC_CS2_CR.bit.R_SETUP = 2;
    Emif1Regs.ASYNC_CS2_CR.bit.R_STROBE = 7;
    Emif1Regs.ASYNC_CS2_CR.bit.R_HOLD = 2;

    Emif1Regs.ASYNC_CS4_CR.bit.SS = 0;
    Emif1Regs.ASYNC_CS4_CR.bit.EW = 0;
    Emif1Regs.ASYNC_CS4_CR.bit.TA = 1;
    Emif1Regs.ASYNC_CS4_CR.bit.ASIZE = 1;
    Emif1Regs.ASYNC_CS4_CR.bit.W_SETUP = 5;
    Emif1Regs.ASYNC_CS4_CR.bit.W_STROBE =10;
    Emif1Regs.ASYNC_CS4_CR.bit.W_HOLD =5;
    Emif1Regs.ASYNC_CS4_CR.bit.R_SETUP =5;
    Emif1Regs.ASYNC_CS4_CR.bit.R_STROBE = 10;
    Emif1Regs.ASYNC_CS4_CR.bit.R_HOLD = 5;
    InitXintf16Gpio();

   asm(" RPT #7 || NOP");

}


void InitXintf16Gpio()
{
     EALLOW;
     GpioCtrlRegs.GPCGMUX1.bit.GPIO69 = 0;
     GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 2;  // XD15
     GpioCtrlRegs.GPCGMUX1.bit.GPIO70 = 0;
     GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 2;  // XD14
     GpioCtrlRegs.GPCGMUX1.bit.GPIO71 = 0;
     GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 2;  // XD13
     GpioCtrlRegs.GPCGMUX1.bit.GPIO72 = 0;
     GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 2;  // XD12
     GpioCtrlRegs.GPCGMUX1.bit.GPIO73 = 0;
     GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 2;  // XD11
     GpioCtrlRegs.GPCGMUX1.bit.GPIO74 = 0;
     GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 2;  // XD10
     GpioCtrlRegs.GPCGMUX1.bit.GPIO75 = 0;
     GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 2;  // XD9
     GpioCtrlRegs.GPCGMUX1.bit.GPIO76 = 0;
     GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 2;  // XD8
     GpioCtrlRegs.GPCGMUX1.bit.GPIO77 = 0;
     GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 2;  // XD7
     GpioCtrlRegs.GPCGMUX1.bit.GPIO78 = 0;
     GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 2;  // XD6
     GpioCtrlRegs.GPCGMUX1.bit.GPIO79 = 0;
     GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 2;  // XD5
     GpioCtrlRegs.GPCGMUX2.bit.GPIO80 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 2;  // XD4
     GpioCtrlRegs.GPCGMUX2.bit.GPIO81 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 2;  // XD3
     GpioCtrlRegs.GPCGMUX2.bit.GPIO82 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 2;  // XD2
     GpioCtrlRegs.GPCGMUX2.bit.GPIO83 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 2;  // XD1
     GpioCtrlRegs.GPCGMUX2.bit.GPIO85 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 2;  // XD0

     GpioCtrlRegs.GPCGMUX2.bit.GPIO92 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO92 = 3;  // XBA1
     GpioCtrlRegs.GPBGMUX1.bit.GPIO38 = 0;
     GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 2;  //XA0
     GpioCtrlRegs.GPBGMUX1.bit.GPIO39 = 0;
     GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 2;  //XA1
     GpioCtrlRegs.GPBGMUX1.bit.GPIO40 = 0;
     GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 2;  //XA2
     GpioCtrlRegs.GPBGMUX1.bit.GPIO41 = 0;
     GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 2;  //XA3
     GpioCtrlRegs.GPBGMUX1.bit.GPIO44 = 0;
     GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 2;  //XA4
     GpioCtrlRegs.GPBGMUX1.bit.GPIO45 = 0;
     GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 2;  //XA5
     GpioCtrlRegs.GPBGMUX1.bit.GPIO46 = 0;
     GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 2;  //XA6
     GpioCtrlRegs.GPBGMUX1.bit.GPIO47 = 0;
     GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 2;  //XA7
     GpioCtrlRegs.GPBGMUX2.bit.GPIO48 = 0;
     GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 2;  //XA8
     GpioCtrlRegs.GPBGMUX2.bit.GPIO49 = 0;
     GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 2;  //XA9
     GpioCtrlRegs.GPBGMUX2.bit.GPIO50 = 0;
     GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 2;  //XA10
     GpioCtrlRegs.GPBGMUX2.bit.GPIO51 = 0;
     GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 2;  //XA11
     GpioCtrlRegs.GPBGMUX2.bit.GPIO52 = 0;
     GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 2;  //XA12
     GpioCtrlRegs.GPCGMUX2.bit.GPIO86 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 2;  // XA13
     GpioCtrlRegs.GPCGMUX2.bit.GPIO87 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 2;  // XA14
     GpioCtrlRegs.GPCGMUX2.bit.GPIO88 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO88 = 2;  // XA15
     GpioCtrlRegs.GPCGMUX2.bit.GPIO89 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO89 = 2;  // XA16
     GpioCtrlRegs.GPCGMUX2.bit.GPIO90 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO90 = 2;  // XA17
     GpioCtrlRegs.GPCGMUX2.bit.GPIO91 = 0;
     GpioCtrlRegs.GPCMUX2.bit.GPIO91 = 2;  // XA18

     GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;
     GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 2;  //EM1CS2
     GpioCtrlRegs.GPBGMUX1.bit.GPIO35 = 0;
     GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 2;  //EM1CS3
     GpioCtrlRegs.GPAGMUX2.bit.GPIO28 = 0;
     GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 2;  //EM1CS4

     GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0;
     GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 2;  //EM1WE
     GpioCtrlRegs.GPBGMUX1.bit.GPIO37 = 0;
     GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 2;  //EM1OE

     EDIS;
}

//===========================================================================
// No more.
//===========================================================================
