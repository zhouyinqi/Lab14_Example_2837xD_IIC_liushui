//###########################################################################
//
// F28377D DSP1 board diagnostic firmware entry point.
// The default CCS configurations link this image for boot from Flash.
//
//###########################################################################


#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "Board_Gpio_Test.h"
#include "Board_Test.h"
#include "Board_Test_Target.h"

// Note: I2C Macros used in this example can be found in the
// DSP2833x_I2C_defines.h file

// Prototype statements for functions found within this file.
/*********Gio begin*******************/

struct I2CMSG *CurrentMsgPtr;				// Used in interrupts
Uint16 PassCount;
Uint16 FailCount;

void main(void)
{




// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP2833x_SysCtrl.c file.
   InitSysCtrl();


// Step 2. Initalize GPIO:
// This example function is found in the DSP2833x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();
// Setup only the GP I/O only for I2C functionality

// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
   DINT;

// Initialize PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP2833x_PieCtrl.c file.
   InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
// This function is found in DSP2833x_PieVect.c.
   InitPieVectTable();

// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.


// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP2833x_InitPeripherals.c
// InitPeripherals(); // Not required for this example

// Step 5. User specific code

   // Clear Counters
   PassCount = 0;
   FailCount = 0;

   BoardTest_Init();



// Enable interrupts required for this example


// Enable CPU INT8 which is connected to PIE group 8

   EINT;   // Enable Global interrupt INTM

   for(;;)
     {
	   BoardTest_TargetPoll();
	   BoardGpio_ServiceHeartbeat();
     }

}   // end of main






//===========================================================================
// No more.
//===========================================================================
