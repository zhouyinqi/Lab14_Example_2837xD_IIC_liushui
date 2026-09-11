#ifndef BOARD_SYSTEM_TEST_H
#define BOARD_SYSTEM_TEST_H

#include "Board_Test.h"

#define BOARD_SYSTEM_CLOCK_MCLK_OK       0x0001UL
#define BOARD_SYSTEM_CLOCK_PLL_LOCKED    0x0002UL
#define BOARD_SYSTEM_CLOCK_PLL_ENABLED   0x0004UL
#define BOARD_SYSTEM_CLOCK_PLLCLK_USED   0x0008UL
#define BOARD_SYSTEM_CLOCK_IMULT_OK      0x0010UL
#define BOARD_SYSTEM_CLOCK_FMULT_OK      0x0020UL
#define BOARD_SYSTEM_CLOCK_DIV_OK        0x0040UL
#define BOARD_SYSTEM_CLOCK_SOURCE_OK     0x0080UL
#define BOARD_SYSTEM_CLOCK_EXPECTED_MASK 0x00FFUL

#define BOARD_SYSTEM_FLASH_LINKED        0x0001UL
#define BOARD_SYSTEM_FLASH_RWAIT_OK      0x0002UL
#define BOARD_SYSTEM_FLASH_CACHE_OK      0x0004UL
#define BOARD_SYSTEM_FLASH_PREFETCH_OK   0x0008UL
#define BOARD_SYSTEM_FLASH_ECC_OK        0x0010UL
#define BOARD_SYSTEM_FLASH_BANK_OK       0x0020UL
#define BOARD_SYSTEM_FLASH_PUMP_OK       0x0040UL
#define BOARD_SYSTEM_FLASH_EXPECTED_MASK 0x007FUL

#define BOARD_SYSTEM_WATCHDOG_DISABLED   0x0001UL
#define BOARD_SYSTEM_RAM_WORD_COUNT      64U

/* Configuration/health check, not an independent frequency measurement.
 * Allow instruction overhead/interrupt latency up to 100 us. */
#define BOARD_SYSTEM_TIMER_MIN_TICKS     3000UL
#define BOARD_SYSTEM_TIMER_MAX_TICKS     20000UL

#define BOARD_SYSTEM_STARTUP_CPU1        0x0001UL
#define BOARD_SYSTEM_STARTUP_PIE_READY   0x0002UL
#define BOARD_SYSTEM_STARTUP_PLL_LOCKED  0x0004UL
#define BOARD_SYSTEM_STARTUP_WD_DISABLED 0x0008UL
#define BOARD_SYSTEM_STARTUP_EXPECTED_MASK 0x000FUL
#define BOARD_SYSTEM_INTERRUPT_PIE_ENABLED 0x0001UL
#define BOARD_SYSTEM_INTERRUPT_TIMER_VECTOR 0x0002UL
#define BOARD_SYSTEM_INTERRUPT_XINT_VECTOR 0x0004UL
#define BOARD_SYSTEM_INTERRUPT_EXPECTED_MASK 0x0007UL

BoardTest_Result BoardSystem_EvaluateMask(BoardTest_U32 actualMask,
                                           BoardTest_U32 expectedMask,
                                           BoardTest_U16 errorCode,
                                           BoardTest_Record *record);
BoardTest_Result BoardSystem_EvaluateTimerDelta(BoardTest_U32 beforeCount,
                                                 BoardTest_U32 afterCount,
                                                 BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
void BoardSystem_CaptureStartupState(void);
BoardTest_Result BoardSystem_RunStartupTest(BoardTest_Record *record);
BoardTest_Result BoardSystem_RunInterruptConfigTest(BoardTest_Record *record);
BoardTest_Result BoardSystem_RunClockTest(BoardTest_Record *record);
BoardTest_Result BoardSystem_RunTimerTest(BoardTest_Record *record);
BoardTest_Result BoardSystem_RunWatchdogTest(BoardTest_Record *record);
BoardTest_Result BoardSystem_RunRamTest(BoardTest_Record *record);
BoardTest_Result BoardSystem_RunFlashTest(BoardTest_Record *record);
#endif

#endif
