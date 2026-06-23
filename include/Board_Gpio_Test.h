#ifndef BOARD_GPIO_TEST_H
#define BOARD_GPIO_TEST_H

#include "Board_Test.h"

#define BOARD_GPIO_LED_HOLD_US            250000UL
#define BOARD_GPIO_LED_READBACK_MASK      0x003FU

BoardTest_Result BoardGpio_EvaluateProgramLedReadback(BoardTest_U16 readbackMask,
                                                       BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardGpio_RunProgramLedTest(BoardTest_Record *record);
void BoardGpio_ServiceHeartbeat(void);
#endif

#endif
