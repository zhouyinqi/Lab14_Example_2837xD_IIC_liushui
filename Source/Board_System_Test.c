#include "Board_System_Test.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

BoardTest_Result BoardSystem_EvaluateMask(BoardTest_U32 actualMask,
                                           BoardTest_U32 expectedMask,
                                           BoardTest_U16 errorCode,
                                           BoardTest_Record *record)
{
    record->rawValue = actualMask;
    record->measuredValue = (float)actualMask;
    record->expectedMin = (float)expectedMask;
    record->expectedMax = (float)expectedMask;

    if((actualMask & expectedMask) == expectedMask)
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = errorCode;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardSystem_EvaluateTimerDelta(BoardTest_U32 beforeCount,
                                                 BoardTest_U32 afterCount,
                                                 BoardTest_Record *record)
{
    BoardTest_U32 delta = 0UL;

    if(beforeCount > afterCount)
    {
        delta = beforeCount - afterCount;
        record->rawValue = delta;
        record->measuredValue = (float)delta;
        record->expectedMin = 1.0F;
        record->expectedMax = 4294967295.0F;
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->rawValue = ((beforeCount & 0xFFFF0000UL) |
                        (afterCount & 0x0000FFFFUL));
    record->measuredValue = 0.0F;
    record->expectedMin = 1.0F;
    record->expectedMax = 4294967295.0F;
    record->errorCode = BOARD_TEST_ERROR_SYS_TIMER;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST
#define BOARD_SYSTEM_EXPECTED_CPU_MHZ 200.0F
#define BOARD_SYSTEM_TIMER_DELAY_US   20UL
#define BOARD_SYSTEM_TIMER_PERIOD     0xFFFFFFFFUL

static volatile Uint32 BoardSystem_RamScratch[BOARD_SYSTEM_RAM_WORD_COUNT];

static BoardTest_U32 BoardSystem_MakeRamPattern(BoardTest_U16 index)
{
    BoardTest_U32 high = ((BoardTest_U32)index << 16U);
    BoardTest_U32 low = ((BoardTest_U32)(index * 37U) & 0x0000FFFFUL);

    return 0xA5A50000UL ^ high ^ low;
}

BoardTest_Result BoardSystem_RunClockTest(BoardTest_Record *record)
{
    BoardTest_U32 status = 0UL;
    BoardTest_Result result;

    if(ClkCfgRegs.MCDCR.bit.MCLKSTS == 0U)
    {
        status |= BOARD_SYSTEM_CLOCK_MCLK_OK;
    }
    if(ClkCfgRegs.SYSPLLSTS.bit.LOCKS == 1U)
    {
        status |= BOARD_SYSTEM_CLOCK_PLL_LOCKED;
    }
    if(ClkCfgRegs.SYSPLLCTL1.bit.PLLEN == 1U)
    {
        status |= BOARD_SYSTEM_CLOCK_PLL_ENABLED;
    }
    if(ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN == 1U)
    {
        status |= BOARD_SYSTEM_CLOCK_PLLCLK_USED;
    }
    if(ClkCfgRegs.SYSPLLMULT.bit.IMULT == IMULT_20)
    {
        status |= BOARD_SYSTEM_CLOCK_IMULT_OK;
    }
    if(ClkCfgRegs.SYSPLLMULT.bit.FMULT == FMULT_1)
    {
        status |= BOARD_SYSTEM_CLOCK_FMULT_OK;
    }
    if(ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV == PLLCLK_BY_2)
    {
        status |= BOARD_SYSTEM_CLOCK_DIV_OK;
    }
    if(ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL == XTAL_OSC)
    {
        status |= BOARD_SYSTEM_CLOCK_SOURCE_OK;
    }

    result = BoardSystem_EvaluateMask(status,
                                      BOARD_SYSTEM_CLOCK_EXPECTED_MASK,
                                      BOARD_TEST_ERROR_SYS_CLOCK,
                                      record);
    record->measuredValue = BOARD_SYSTEM_EXPECTED_CPU_MHZ;
    record->expectedMin = BOARD_SYSTEM_EXPECTED_CPU_MHZ;
    record->expectedMax = BOARD_SYSTEM_EXPECTED_CPU_MHZ;
    return result;
}

BoardTest_Result BoardSystem_RunTimerTest(BoardTest_Record *record)
{
    Uint32 oldPeriod;
    Uint32 oldCounter;
    Uint16 oldTcr;
    Uint16 oldTpr;
    Uint16 oldTprh;
    Uint32 beforeCount;
    Uint32 afterCount;
    BoardTest_Result result;

    oldPeriod = CpuTimer0Regs.PRD.all;
    oldCounter = CpuTimer0Regs.TIM.all;
    oldTcr = CpuTimer0Regs.TCR.all;
    oldTpr = CpuTimer0Regs.TPR.all;
    oldTprh = CpuTimer0Regs.TPRH.all;

    CpuTimer0Regs.TCR.bit.TSS = 1U;
    CpuTimer0Regs.PRD.all = BOARD_SYSTEM_TIMER_PERIOD;
    CpuTimer0Regs.TPR.all = 0U;
    CpuTimer0Regs.TPRH.all = 0U;
    CpuTimer0Regs.TCR.bit.TIE = 0U;
    CpuTimer0Regs.TCR.bit.SOFT = 1U;
    CpuTimer0Regs.TCR.bit.FREE = 1U;
    CpuTimer0Regs.TCR.bit.TRB = 1U;
    CpuTimer0Regs.TCR.bit.TSS = 0U;

    beforeCount = CpuTimer0Regs.TIM.all;
    DELAY_US(BOARD_SYSTEM_TIMER_DELAY_US);
    afterCount = CpuTimer0Regs.TIM.all;

    CpuTimer0Regs.TCR.bit.TSS = 1U;
    CpuTimer0Regs.PRD.all = oldPeriod;
    CpuTimer0Regs.TIM.all = oldCounter;
    CpuTimer0Regs.TPR.all = oldTpr;
    CpuTimer0Regs.TPRH.all = oldTprh;
    CpuTimer0Regs.TCR.all = oldTcr;

    result = BoardSystem_EvaluateTimerDelta(beforeCount, afterCount, record);
    return result;
}

BoardTest_Result BoardSystem_RunWatchdogTest(BoardTest_Record *record)
{
    BoardTest_U32 status = 0UL;

    if(WdRegs.WDCR.bit.WDDIS == 1U)
    {
        status |= BOARD_SYSTEM_WATCHDOG_DISABLED;
    }

    record->rawValue = (BoardTest_U32)WdRegs.WDCR.all;
    record->measuredValue = (float)status;
    record->expectedMin = (float)BOARD_SYSTEM_WATCHDOG_DISABLED;
    record->expectedMax = (float)BOARD_SYSTEM_WATCHDOG_DISABLED;

    if(status == BOARD_SYSTEM_WATCHDOG_DISABLED)
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_WATCHDOG_CFG;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardSystem_RunRamTest(BoardTest_Record *record)
{
    BoardTest_U16 index;
    BoardTest_U32 pattern;
    BoardTest_U32 checksum = 0UL;

    for(index = 0U; index < BOARD_SYSTEM_RAM_WORD_COUNT; index++)
    {
        pattern = BoardSystem_MakeRamPattern(index);
        BoardSystem_RamScratch[index] = pattern;
        if(BoardSystem_RamScratch[index] != pattern)
        {
            record->rawValue = (BoardTest_U32)index;
            record->measuredValue = (float)index;
            record->expectedMin = (float)BOARD_SYSTEM_RAM_WORD_COUNT;
            record->expectedMax = (float)BOARD_SYSTEM_RAM_WORD_COUNT;
            record->errorCode = BOARD_TEST_ERROR_RAM_PATTERN;
            return BOARD_TEST_RESULT_FAIL;
        }
    }

    for(index = 0U; index < BOARD_SYSTEM_RAM_WORD_COUNT; index++)
    {
        pattern = ~BoardSystem_MakeRamPattern(index);
        BoardSystem_RamScratch[index] = pattern;
        if(BoardSystem_RamScratch[index] != pattern)
        {
            record->rawValue = 0x80000000UL | (BoardTest_U32)index;
            record->measuredValue = (float)index;
            record->expectedMin = (float)BOARD_SYSTEM_RAM_WORD_COUNT;
            record->expectedMax = (float)BOARD_SYSTEM_RAM_WORD_COUNT;
            record->errorCode = BOARD_TEST_ERROR_RAM_PATTERN;
            return BOARD_TEST_RESULT_FAIL;
        }
        checksum ^= (BoardTest_U32)BoardSystem_RamScratch[index];
    }

    for(index = 0U; index < BOARD_SYSTEM_RAM_WORD_COUNT; index++)
    {
        BoardSystem_RamScratch[index] = 0UL;
    }

    record->rawValue = checksum;
    record->measuredValue = (float)BOARD_SYSTEM_RAM_WORD_COUNT;
    record->expectedMin = (float)BOARD_SYSTEM_RAM_WORD_COUNT;
    record->expectedMax = (float)BOARD_SYSTEM_RAM_WORD_COUNT;
    record->errorCode = BOARD_TEST_ERROR_NONE;
    return BOARD_TEST_RESULT_PASS;
}

BoardTest_Result BoardSystem_RunFlashTest(BoardTest_Record *record)
{
    BoardTest_U32 status = 0UL;

#ifdef _FLASH
    status |= BOARD_SYSTEM_FLASH_LINKED;
#endif

    if(FlashCtrlRegs.FRDCNTL.bit.RWAIT == 0x3U)
    {
        status |= BOARD_SYSTEM_FLASH_RWAIT_OK;
    }
    if(FlashCtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN == 1U)
    {
        status |= BOARD_SYSTEM_FLASH_CACHE_OK;
    }
    if(FlashCtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN == 1U)
    {
        status |= BOARD_SYSTEM_FLASH_PREFETCH_OK;
    }
    if(FlashEccRegs.ECC_ENABLE.bit.ENABLE == 0xAU)
    {
        status |= BOARD_SYSTEM_FLASH_ECC_OK;
    }
    if(FlashCtrlRegs.FBFALLBACK.bit.BNKPWR0 == 0x3U)
    {
        status |= BOARD_SYSTEM_FLASH_BANK_OK;
    }
    if(FlashCtrlRegs.FPAC1.bit.PMPPWR == 0x1U)
    {
        status |= BOARD_SYSTEM_FLASH_PUMP_OK;
    }

    return BoardSystem_EvaluateMask(status,
                                    BOARD_SYSTEM_FLASH_EXPECTED_MASK,
                                    BOARD_TEST_ERROR_FLASH_CFG,
                                    record);
}
#endif
