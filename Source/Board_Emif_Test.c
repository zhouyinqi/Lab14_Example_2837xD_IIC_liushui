#include "Board_Emif_Test.h"
#include "Board_Pinmap.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#include "F2837xD_Emif_defines.h"
#endif

#define BOARD_EMIF_ASYNC_ASIZE_16     1U
#define BOARD_EMIF_ASYNC_TA           1U
#define BOARD_EMIF_ASYNC_SETUP        2U
#define BOARD_EMIF_ASYNC_STROBE       7U
#define BOARD_EMIF_SRAM_STROBE        2U
#define BOARD_EMIF_ASYNC_HOLD         2U

#define BOARD_EMIF1_CS3_SRAM_BASE_ADDR  0x00300000UL
#define BOARD_EMIF1_CS3_SRAM_TEST_WORDS 0x00010000UL
#define BOARD_EMIF_SRAM_LIVE_OFFSET     0x00000010UL

volatile BoardEmif_BasicSnapshot gBoardEmifBasicSnapshot =
{
    0U,
    0U,
    0U,
    0U,
    0U
};

volatile BoardEmif_SramSnapshot gBoardEmifSramSnapshot =
{
    0U,
    0U,
    BOARD_EMIF_SRAM_FAIL_NONE,
    0U,
    0U,
    0U,
    0U,
    (BoardTest_U16)(BOARD_EMIF1_CS3_SRAM_TEST_WORDS & 0xFFFFUL),
    (BoardTest_U16)((BOARD_EMIF1_CS3_SRAM_TEST_WORDS >> 16U) & 0xFFFFUL)
};

BoardTest_Result BoardEmif_EvaluateBasicStatus(BoardTest_U16 statusMask,
                                               BoardTest_U16 emif1AsyncMask,
                                               BoardTest_U16 emif2AsyncMask,
                                               BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       (((BoardTest_U32)emif1AsyncMask & 0x00FFUL) << 8U) |
                       ((BoardTest_U32)emif2AsyncMask & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_EMIF_BASIC_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_EMIF_BASIC_DIAGNOSTIC_MASK;

    if(((statusMask & BOARD_EMIF_BASIC_REQUIRED_MASK) ==
        BOARD_EMIF_BASIC_REQUIRED_MASK) &&
       ((emif1AsyncMask & BOARD_EMIF_ASYNC_REQUIRED_MASK) ==
        BOARD_EMIF_ASYNC_REQUIRED_MASK) &&
       ((emif2AsyncMask & BOARD_EMIF_ASYNC_REQUIRED_MASK) ==
        BOARD_EMIF_ASYNC_REQUIRED_MASK))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_EMIF_BASIC;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardEmif_EvaluateSramStatus(BoardTest_U16 statusMask,
                                              BoardTest_U16 failCode,
                                              BoardTest_U16 expected,
                                              BoardTest_U16 actual,
                                              BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)failCode & 0xFFFFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_EMIF_SRAM_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_EMIF_SRAM_DIAGNOSTIC_MASK;

    if(((statusMask & BOARD_EMIF_SRAM_REQUIRED_MASK) ==
        BOARD_EMIF_SRAM_REQUIRED_MASK) &&
       (failCode == BOARD_EMIF_SRAM_FAIL_NONE) &&
       (expected == actual))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_EMIF_SRAM;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST

extern void InitXintf16Gpio(void);

static void BoardEmif_EnableClocksAndReleaseReset(void)
{
    EALLOW;
    CpuSysRegs.PCLKCR1.bit.EMIF1 = 1U;
    CpuSysRegs.PCLKCR1.bit.EMIF2 = 1U;
    DevCfgRegs.SOFTPRES1.bit.EMIF1 = 0U;
    DevCfgRegs.SOFTPRES1.bit.EMIF2 = 0U;
    EDIS;
}

static void BoardEmif_SelectEmif1Cpu1(void)
{
    EALLOW;
    if(Emif1ConfigRegs.EMIF1COMMIT.bit.COMMIT_EMIF1 == 0U)
    {
        Emif1ConfigRegs.EMIF1MSEL.all = MSEL_EMIF1_CPU1;
    }
    EDIS;
}

static void BoardEmif_ConfigureAsyncCs2(
    volatile union ASYNC_CS2_CR_REG *asyncCs2)
{
    EALLOW;
    asyncCs2->bit.SS = 0U;
    asyncCs2->bit.EW = 0U;
    asyncCs2->bit.TA = BOARD_EMIF_ASYNC_TA;
    asyncCs2->bit.ASIZE = BOARD_EMIF_ASYNC_ASIZE_16;
    asyncCs2->bit.W_SETUP = BOARD_EMIF_ASYNC_SETUP;
    asyncCs2->bit.W_STROBE = BOARD_EMIF_ASYNC_STROBE;
    asyncCs2->bit.W_HOLD = BOARD_EMIF_ASYNC_HOLD;
    asyncCs2->bit.R_SETUP = BOARD_EMIF_ASYNC_SETUP;
    asyncCs2->bit.R_STROBE = BOARD_EMIF_ASYNC_STROBE;
    asyncCs2->bit.R_HOLD = BOARD_EMIF_ASYNC_HOLD;
    EDIS;
}

static void BoardEmif_ConfigureAsyncCs3(
    volatile union ASYNC_CS3_CR_REG *asyncCs3)
{
    EALLOW;
    asyncCs3->bit.SS = 0U;
    asyncCs3->bit.EW = 0U;
    asyncCs3->bit.TA = BOARD_EMIF_ASYNC_TA;
    asyncCs3->bit.ASIZE = BOARD_EMIF_ASYNC_ASIZE_16;
    asyncCs3->bit.W_SETUP = BOARD_EMIF_ASYNC_SETUP;
    asyncCs3->bit.W_STROBE = BOARD_EMIF_SRAM_STROBE;
    asyncCs3->bit.W_HOLD = BOARD_EMIF_ASYNC_HOLD;
    asyncCs3->bit.R_SETUP = BOARD_EMIF_ASYNC_SETUP;
    asyncCs3->bit.R_STROBE = BOARD_EMIF_SRAM_STROBE;
    asyncCs3->bit.R_HOLD = BOARD_EMIF_ASYNC_HOLD;
    EDIS;
}

static BoardTest_U16 BoardEmif_CheckAsyncCs2(
    volatile union ASYNC_CS2_CR_REG *asyncCs2)
{
    BoardTest_U16 asyncMask;

    asyncMask = 0U;

    if(asyncCs2->bit.ASIZE == BOARD_EMIF_ASYNC_ASIZE_16)
    {
        asyncMask |= BOARD_EMIF_ASYNC_WIDTH_16;
    }

    if((asyncCs2->bit.SS == 0U) &&
       (asyncCs2->bit.EW == 0U) &&
       (asyncCs2->bit.TA == BOARD_EMIF_ASYNC_TA) &&
       (asyncCs2->bit.W_SETUP == BOARD_EMIF_ASYNC_SETUP) &&
       (asyncCs2->bit.W_STROBE == BOARD_EMIF_ASYNC_STROBE) &&
       (asyncCs2->bit.W_HOLD == BOARD_EMIF_ASYNC_HOLD) &&
       (asyncCs2->bit.R_SETUP == BOARD_EMIF_ASYNC_SETUP) &&
       (asyncCs2->bit.R_STROBE == BOARD_EMIF_ASYNC_STROBE) &&
       (asyncCs2->bit.R_HOLD == BOARD_EMIF_ASYNC_HOLD))
    {
        asyncMask |= BOARD_EMIF_ASYNC_TIMING_VALID;
    }

    return asyncMask;
}

static BoardTest_U16 BoardEmif_CheckAsyncCs3(
    volatile union ASYNC_CS3_CR_REG *asyncCs3)
{
    BoardTest_U16 asyncMask;

    asyncMask = 0U;

    if(asyncCs3->bit.ASIZE == BOARD_EMIF_ASYNC_ASIZE_16)
    {
        asyncMask |= BOARD_EMIF_ASYNC_WIDTH_16;
    }

    if((asyncCs3->bit.SS == 0U) &&
       (asyncCs3->bit.EW == 0U) &&
       (asyncCs3->bit.TA == BOARD_EMIF_ASYNC_TA) &&
       (asyncCs3->bit.W_SETUP == BOARD_EMIF_ASYNC_SETUP) &&
       (asyncCs3->bit.W_STROBE == BOARD_EMIF_SRAM_STROBE) &&
       (asyncCs3->bit.W_HOLD == BOARD_EMIF_ASYNC_HOLD) &&
       (asyncCs3->bit.R_SETUP == BOARD_EMIF_ASYNC_SETUP) &&
       (asyncCs3->bit.R_STROBE == BOARD_EMIF_SRAM_STROBE) &&
       (asyncCs3->bit.R_HOLD == BOARD_EMIF_ASYNC_HOLD))
    {
        asyncMask |= BOARD_EMIF_ASYNC_TIMING_VALID;
    }

    return asyncMask;
}

static BoardTest_U16 BoardEmif_IsPinmapValid(void)
{
    return (BOARD_PIN_EMIF1_XA(0U) == 38U) &&
           (BOARD_PIN_EMIF1_XA(17U) == 90U) &&
           (BOARD_PIN_EMIF1_XD(0U) == 85U) &&
           (BOARD_PIN_EMIF1_XD(15U) == 69U) &&
           (BOARD_PIN_EMIF1_SRAM_CS3 == 35U) &&
           (BOARD_PIN_EMIF2_XA(0U) == 98U) &&
           (BOARD_PIN_EMIF2_XA(11U) == 109U) &&
           (BOARD_PIN_EMIF2_XD(0U) == 68U) &&
           (BOARD_PIN_EMIF2_XD(15U) == 53U);
}

void BoardEmif_ConfigureExternalAsync(void)
{
    BoardEmif_EnableClocksAndReleaseReset();
    BoardEmif_SelectEmif1Cpu1();
    BoardEmif_ConfigureAsyncCs2(&Emif1Regs.ASYNC_CS2_CR);
    BoardEmif_ConfigureAsyncCs2(&Emif2Regs.ASYNC_CS2_CR);
}

BoardTest_Result BoardEmif_RunBasicConfigTest(BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 emif1AsyncMask;
    BoardTest_U16 emif2AsyncMask;
    BoardTest_U16 emif1MasterSelect;
    BoardTest_U16 deviceCapability;

    BoardEmif_ConfigureExternalAsync();

    statusMask = 0U;
    emif1AsyncMask = BoardEmif_CheckAsyncCs2(&Emif1Regs.ASYNC_CS2_CR);
    emif2AsyncMask = BoardEmif_CheckAsyncCs2(&Emif2Regs.ASYNC_CS2_CR);
    emif1MasterSelect = (BoardTest_U16)
        Emif1ConfigRegs.EMIF1MSEL.bit.MSEL_EMIF1;
    deviceCapability = (BoardTest_U16)
        ((DevCfgRegs.DC2.bit.EMIF1 & 0x0001U) |
         ((DevCfgRegs.DC2.bit.EMIF2 & 0x0001U) << 1U));

    if(deviceCapability == 0x0003U)
    {
        statusMask |= BOARD_EMIF_BASIC_DEVICE_PRESENT;
    }

    if((CpuSysRegs.PCLKCR1.bit.EMIF1 == 1U) &&
       (CpuSysRegs.PCLKCR1.bit.EMIF2 == 1U))
    {
        statusMask |= BOARD_EMIF_BASIC_CLOCKS_ENABLED;
    }

    if((DevCfgRegs.SOFTPRES1.bit.EMIF1 == 0U) &&
       (DevCfgRegs.SOFTPRES1.bit.EMIF2 == 0U))
    {
        statusMask |= BOARD_EMIF_BASIC_RESETS_RELEASED;
    }

    if(emif1MasterSelect == 1U)
    {
        statusMask |= BOARD_EMIF_BASIC_EMIF1_CPU1;
    }

    if((emif1AsyncMask & BOARD_EMIF_ASYNC_REQUIRED_MASK) ==
       BOARD_EMIF_ASYNC_REQUIRED_MASK)
    {
        statusMask |= BOARD_EMIF_BASIC_EMIF1_ASYNC_CS2;
    }

    if((emif2AsyncMask & BOARD_EMIF_ASYNC_REQUIRED_MASK) ==
       BOARD_EMIF_ASYNC_REQUIRED_MASK)
    {
        statusMask |= BOARD_EMIF_BASIC_EMIF2_ASYNC_CS2;
    }

    if((Emif1ConfigRegs.EMIF1COMMIT.bit.COMMIT_EMIF1 == 0U) &&
       (Emif2ConfigRegs.EMIF2COMMIT.bit.COMMIT_EMIF2 == 0U))
    {
        statusMask |= BOARD_EMIF_BASIC_CONFIG_UNLOCKED;
    }

    if(BoardEmif_IsPinmapValid() != 0U)
    {
        statusMask |= BOARD_EMIF_BASIC_PINMAP_VALID;
    }

    gBoardEmifBasicSnapshot.statusMask = statusMask;
    gBoardEmifBasicSnapshot.emif1AsyncMask = emif1AsyncMask;
    gBoardEmifBasicSnapshot.emif2AsyncMask = emif2AsyncMask;
    gBoardEmifBasicSnapshot.emif1MasterSelect = emif1MasterSelect;
    gBoardEmifBasicSnapshot.deviceCapability = deviceCapability;

    return BoardEmif_EvaluateBasicStatus(statusMask,
                                         emif1AsyncMask,
                                         emif2AsyncMask,
                                         record);
}

static void BoardEmif_UpdateSramSnapshot(BoardTest_U16 statusMask,
                                         BoardTest_U16 cs3AsyncMask,
                                         BoardTest_U16 failCode,
                                         BoardTest_U32 failOffset,
                                         BoardTest_U16 expected,
                                         BoardTest_U16 actual)
{
    gBoardEmifSramSnapshot.statusMask = statusMask;
    gBoardEmifSramSnapshot.cs3AsyncMask = cs3AsyncMask;
    gBoardEmifSramSnapshot.failCode = failCode;
    gBoardEmifSramSnapshot.expected = expected;
    gBoardEmifSramSnapshot.actual = actual;
    gBoardEmifSramSnapshot.failOffsetLow =
        (BoardTest_U16)(failOffset & 0xFFFFUL);
    gBoardEmifSramSnapshot.failOffsetHigh =
        (BoardTest_U16)((failOffset >> 16U) & 0xFFFFUL);
    gBoardEmifSramSnapshot.testWordsLow =
        (BoardTest_U16)(BOARD_EMIF1_CS3_SRAM_TEST_WORDS & 0xFFFFUL);
    gBoardEmifSramSnapshot.testWordsHigh =
        (BoardTest_U16)((BOARD_EMIF1_CS3_SRAM_TEST_WORDS >> 16U) & 0xFFFFUL);
}

static BoardTest_U16 BoardEmif_SramFillAndVerify(
    volatile Uint16 *sram,
    BoardTest_U16 pattern,
    BoardTest_U16 failCode,
    BoardTest_U32 *failOffset,
    BoardTest_U16 *expected,
    BoardTest_U16 *actual)
{
    BoardTest_U32 offset;

    for(offset = 0UL; offset < BOARD_EMIF1_CS3_SRAM_TEST_WORDS; offset++)
    {
        sram[offset] = pattern;
    }

    for(offset = 0UL; offset < BOARD_EMIF1_CS3_SRAM_TEST_WORDS; offset++)
    {
        *actual = sram[offset];
        if(*actual != pattern)
        {
            *failOffset = offset;
            *expected = pattern;
            return failCode;
        }
    }

    return BOARD_EMIF_SRAM_FAIL_NONE;
}

static BoardTest_U16 BoardEmif_SramAddressVerify(
    volatile Uint16 *sram,
    BoardTest_U32 *failOffset,
    BoardTest_U16 *expected,
    BoardTest_U16 *actual)
{
    BoardTest_U32 offset;

    for(offset = 0UL; offset < BOARD_EMIF1_CS3_SRAM_TEST_WORDS; offset++)
    {
        sram[offset] = (Uint16)offset;
    }

    for(offset = 0UL; offset < BOARD_EMIF1_CS3_SRAM_TEST_WORDS; offset++)
    {
        *expected = (Uint16)offset;
        *actual = sram[offset];
        if(*actual != *expected)
        {
            *failOffset = offset;
            return BOARD_EMIF_SRAM_FAIL_ADDRESS;
        }
    }

    return BOARD_EMIF_SRAM_FAIL_NONE;
}

static BoardTest_U16 BoardEmif_SramLiveSample(
    volatile Uint16 *sram,
    BoardTest_U32 *failOffset,
    BoardTest_U16 *expected,
    BoardTest_U16 *actual)
{
    *failOffset = BOARD_EMIF_SRAM_LIVE_OFFSET;
    *expected = 0xA5A5U;
    sram[*failOffset] = *expected;
    *actual = sram[*failOffset];

    if(*actual != *expected)
    {
        return BOARD_EMIF_SRAM_FAIL_LIVE;
    }

    return BOARD_EMIF_SRAM_FAIL_NONE;
}

BoardTest_Result BoardEmif_RunSramExternalTest(BoardTest_Record *record)
{
    volatile Uint16 *sram;
    BoardTest_U16 statusMask;
    BoardTest_U16 cs3AsyncMask;
    BoardTest_U16 failCode;
    BoardTest_U32 failOffset;
    BoardTest_U16 expected;
    BoardTest_U16 actual;

    sram = (volatile Uint16 *)BOARD_EMIF1_CS3_SRAM_BASE_ADDR;
    statusMask = 0U;
    failCode = BOARD_EMIF_SRAM_FAIL_NONE;
    failOffset = 0UL;
    expected = 0U;
    actual = 0U;

    BoardEmif_EnableClocksAndReleaseReset();
    BoardEmif_SelectEmif1Cpu1();
    BoardEmif_ConfigureAsyncCs3(&Emif1Regs.ASYNC_CS3_CR);
    InitXintf16Gpio();
    asm(" RPT #7 || NOP");

    statusMask |= BOARD_EMIF_SRAM_CONFIGURED;
    cs3AsyncMask = BoardEmif_CheckAsyncCs3(&Emif1Regs.ASYNC_CS3_CR);
    if((cs3AsyncMask & BOARD_EMIF_ASYNC_REQUIRED_MASK) ==
       BOARD_EMIF_ASYNC_REQUIRED_MASK)
    {
        statusMask |= BOARD_EMIF_SRAM_CS3_TIMING;
    }
    else
    {
        failCode = BOARD_EMIF_SRAM_FAIL_CONFIG;
    }

    if(failCode == BOARD_EMIF_SRAM_FAIL_NONE)
    {
        failCode = BoardEmif_SramFillAndVerify(
            sram,
            0x5555U,
            BOARD_EMIF_SRAM_FAIL_5555,
            &failOffset,
            &expected,
            &actual);
        if(failCode == BOARD_EMIF_SRAM_FAIL_NONE)
        {
            statusMask |= BOARD_EMIF_SRAM_PATTERN_5555;
        }
    }

    if(failCode == BOARD_EMIF_SRAM_FAIL_NONE)
    {
        failCode = BoardEmif_SramFillAndVerify(
            sram,
            0xAAAAU,
            BOARD_EMIF_SRAM_FAIL_AAAA,
            &failOffset,
            &expected,
            &actual);
        if(failCode == BOARD_EMIF_SRAM_FAIL_NONE)
        {
            statusMask |= BOARD_EMIF_SRAM_PATTERN_AAAA;
        }
    }

    if(failCode == BOARD_EMIF_SRAM_FAIL_NONE)
    {
        failCode = BoardEmif_SramAddressVerify(
            sram,
            &failOffset,
            &expected,
            &actual);
        if(failCode == BOARD_EMIF_SRAM_FAIL_NONE)
        {
            statusMask |= BOARD_EMIF_SRAM_ADDRESS_PATTERN;
        }
    }

    if(failCode == BOARD_EMIF_SRAM_FAIL_NONE)
    {
        failCode = BoardEmif_SramLiveSample(
            sram,
            &failOffset,
            &expected,
            &actual);
        if(failCode == BOARD_EMIF_SRAM_FAIL_NONE)
        {
            statusMask |= BOARD_EMIF_SRAM_LIVE_SAMPLE;
        }
    }

    BoardEmif_UpdateSramSnapshot(statusMask,
                                 cs3AsyncMask,
                                 failCode,
                                 failOffset,
                                 expected,
                                 actual);

    return BoardEmif_EvaluateSramStatus(statusMask,
                                        failCode,
                                        expected,
                                        actual,
                                        record);
}
#endif
