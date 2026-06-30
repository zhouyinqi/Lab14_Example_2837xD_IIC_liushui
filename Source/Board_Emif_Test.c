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
#define BOARD_EMIF_ASYNC_HOLD         2U

volatile BoardEmif_BasicSnapshot gBoardEmifBasicSnapshot =
{
    0U,
    0U,
    0U,
    0U,
    0U
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

#ifndef BOARD_TEST_HOST

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

static BoardTest_U16 BoardEmif_IsPinmapValid(void)
{
    return (BOARD_PIN_EMIF1_XA(0U) == 38U) &&
           (BOARD_PIN_EMIF1_XA(17U) == 90U) &&
           (BOARD_PIN_EMIF1_XD(0U) == 85U) &&
           (BOARD_PIN_EMIF1_XD(15U) == 69U) &&
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
#endif
