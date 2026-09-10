#include "Board_Hdo_Test.h"
#include "Board_Fpga_Test.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

volatile BoardHdo_FpgaSnapshot gBoardHdoFpgaSnapshot =
{
    0U,
    BOARD_HDO_FAIL_NONE,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    BOARD_HDO_PHASE_IDLE
};

static BoardTest_U16 BoardHdo_GetRequiredStatusMask(
    BoardTest_U16 channelMask)
{
    BoardTest_U16 requiredMask;

    requiredMask = BOARD_HDO_STATUS_CONFIGURED |
                   BOARD_HDO_STATUS_MAGIC_VALID |
                   BOARD_HDO_STATUS_OUTPUTS_DISABLED;

    if((channelMask & BOARD_HDO_CHANNEL_1) != 0U)
    {
        requiredMask |= BOARD_HDO_STATUS_CHANNEL_1_TOGGLED;
    }

    if((channelMask & BOARD_HDO_CHANNEL_2) != 0U)
    {
        requiredMask |= BOARD_HDO_STATUS_CHANNEL_2_TOGGLED;
    }

    return requiredMask;
}

BoardTest_Result BoardHdo_EvaluateFpgaStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 channelMask,
    BoardTest_Record *record)
{
    BoardTest_U16 requiredMask;

    requiredMask = BoardHdo_GetRequiredStatusMask(channelMask);
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)failCode & 0xFFFFUL);
    record->measuredValue = (float)(channelMask &
                                    BOARD_HDO_ALLOWED_CHANNEL_MASK);
    record->expectedMin = (float)(channelMask &
                                  BOARD_HDO_ALLOWED_CHANNEL_MASK);
    record->expectedMax = (float)BOARD_HDO_ALLOWED_CHANNEL_MASK;

    if((failCode == BOARD_HDO_FAIL_NOT_ARMED) ||
       (failCode == BOARD_HDO_FAIL_INVALID_MASK))
    {
        record->errorCode = BOARD_TEST_ERROR_SAFETY_LOCK;
        return BOARD_TEST_RESULT_SAFETY_LOCKED;
    }

    if(((statusMask & requiredMask) == requiredMask) &&
       (failCode == BOARD_HDO_FAIL_NONE))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        /* PLC high-speed input feedback is confirmed manually for now. */
        return BOARD_TEST_RESULT_WARN;
    }

    record->errorCode = BOARD_TEST_ERROR_HDO_EXTERNAL;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST
static BoardTest_U16 BoardHdo_Read16(BoardTest_U32 address)
{
    return *((volatile Uint16 *)address);
}

static void BoardHdo_Write16(BoardTest_U32 address, BoardTest_U16 value)
{
    *((volatile Uint16 *)address) = (Uint16)value;
    asm(" RPT #7 || NOP");
}

void BoardHdo_ForceFpgaSafe(void)
{
    BoardHdo_Write16(BOARD_HDO_FPGA_DO_ENABLE_ADDR, 0U);
    BoardHdo_Write16(BOARD_HDO_FPGA_DO_DATA_ADDR, 0U);
}

static void BoardHdo_ToggleOutput(BoardTest_U16 outputBit)
{
    /* The FPGA write port does not provide a reliable data readback. */
    BoardHdo_Write16(BOARD_HDO_FPGA_DO_ENABLE_ADDR, 0U);
    BoardHdo_Write16(BOARD_HDO_FPGA_DO_DATA_ADDR, outputBit);
    BoardHdo_Write16(BOARD_HDO_FPGA_DO_ENABLE_ADDR, outputBit);

    gBoardHdoFpgaSnapshot.activeOutputBit = outputBit;
    gBoardHdoFpgaSnapshot.activeDataValue = outputBit;
    gBoardHdoFpgaSnapshot.phase = BOARD_HDO_PHASE_DATA_SET;
    DELAY_US(BOARD_HDO_PHASE_HOLD_US);

    BoardHdo_Write16(BOARD_HDO_FPGA_DO_DATA_ADDR, 0U);
    gBoardHdoFpgaSnapshot.activeDataValue = 0U;
    gBoardHdoFpgaSnapshot.phase = BOARD_HDO_PHASE_DATA_CLEAR;
    DELAY_US(BOARD_HDO_PHASE_HOLD_US);

    BoardHdo_Write16(BOARD_HDO_FPGA_DO_ENABLE_ADDR, 0U);
    gBoardHdoFpgaSnapshot.activeOutputBit = 0U;
    gBoardHdoFpgaSnapshot.phase = BOARD_HDO_PHASE_SAFE;
}

BoardTest_Result BoardHdo_RunFpgaExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 armKey,
    BoardTest_U16 channelMask)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 requestedMask;
    BoardTest_U16 magic2Value;
    BoardTest_U16 magic3Value;

    statusMask = 0U;
    failCode = BOARD_HDO_FAIL_NONE;
    requestedMask = channelMask & BOARD_HDO_ALLOWED_CHANNEL_MASK;

    gBoardHdoFpgaSnapshot.statusMask = 0U;
    gBoardHdoFpgaSnapshot.failCode = BOARD_HDO_FAIL_NONE;
    gBoardHdoFpgaSnapshot.requestedChannelMask = requestedMask;
    gBoardHdoFpgaSnapshot.initialDataValue = 0U;
    gBoardHdoFpgaSnapshot.initialEnableValue = 0U;
    gBoardHdoFpgaSnapshot.finalDataValue = 0U;
    gBoardHdoFpgaSnapshot.finalEnableValue = 0U;
    gBoardHdoFpgaSnapshot.activeOutputBit = 0U;
    gBoardHdoFpgaSnapshot.activeDataValue = 0U;
    gBoardHdoFpgaSnapshot.phase = BOARD_HDO_PHASE_IDLE;

    if(armKey != BOARD_HDO_TEST_ARM_KEY)
    {
        failCode = BOARD_HDO_FAIL_NOT_ARMED;
    }
    else if((requestedMask == 0U) ||
            ((channelMask & ~BOARD_HDO_ALLOWED_CHANNEL_MASK) != 0U))
    {
        failCode = BOARD_HDO_FAIL_INVALID_MASK;
    }

    if(failCode == BOARD_HDO_FAIL_NONE)
    {
        if(BoardFpga_PrepareEmif2Interface() != 0U)
        {
            statusMask |= BOARD_HDO_STATUS_CONFIGURED;
        }
        else
        {
            failCode = BOARD_HDO_FAIL_CONFIG;
        }
    }

    if(failCode == BOARD_HDO_FAIL_NONE)
    {
        magic2Value = BoardHdo_Read16(BOARD_FPGA_EMIF2_TEST2_ADDR);
        magic3Value = BoardHdo_Read16(BOARD_FPGA_EMIF2_TEST3_ADDR);
        if((magic2Value == BOARD_FPGA_TEST2_EXPECTED) &&
           (magic3Value == BOARD_FPGA_TEST3_EXPECTED))
        {
            statusMask |= BOARD_HDO_STATUS_MAGIC_VALID;
        }
        else
        {
            failCode = BOARD_HDO_FAIL_MAGIC;
        }
    }

    if(failCode == BOARD_HDO_FAIL_NONE)
    {
        gBoardHdoFpgaSnapshot.initialDataValue =
            BoardHdo_Read16(BOARD_HDO_FPGA_DO_DATA_ADDR);
        gBoardHdoFpgaSnapshot.initialEnableValue =
            BoardHdo_Read16(BOARD_HDO_FPGA_DO_ENABLE_ADDR);

        BoardHdo_ForceFpgaSafe();

        if((requestedMask & BOARD_HDO_CHANNEL_1) != 0U)
        {
            BoardHdo_ToggleOutput(BOARD_HDO_FPGA_OUTPUT_1_BIT);
            statusMask |= BOARD_HDO_STATUS_CHANNEL_1_TOGGLED;
        }

        if((requestedMask & BOARD_HDO_CHANNEL_2) != 0U)
        {
            BoardHdo_ToggleOutput(BOARD_HDO_FPGA_OUTPUT_2_BIT);
            statusMask |= BOARD_HDO_STATUS_CHANNEL_2_TOGGLED;
        }

        BoardHdo_ForceFpgaSafe();
        gBoardHdoFpgaSnapshot.finalDataValue =
            BoardHdo_Read16(BOARD_HDO_FPGA_DO_DATA_ADDR);
        gBoardHdoFpgaSnapshot.finalEnableValue =
            BoardHdo_Read16(BOARD_HDO_FPGA_DO_ENABLE_ADDR);

        /* 0x2000 is a write port and may not echo the value written. */
        if((gBoardHdoFpgaSnapshot.finalEnableValue &
             BOARD_HDO_FPGA_OUTPUT_MASK) == 0U)
        {
            statusMask |= BOARD_HDO_STATUS_OUTPUTS_DISABLED;
        }
        else
        {
            failCode = BOARD_HDO_FAIL_OUTPUT_ACTIVE;
        }
        gBoardHdoFpgaSnapshot.phase = BOARD_HDO_PHASE_SAFE;
    }

    if((failCode != BOARD_HDO_FAIL_NONE) &&
       ((statusMask & BOARD_HDO_STATUS_CONFIGURED) != 0U))
    {
        BoardHdo_ForceFpgaSafe();
    }

    gBoardHdoFpgaSnapshot.statusMask = statusMask;
    gBoardHdoFpgaSnapshot.failCode = failCode;

    return BoardHdo_EvaluateFpgaStatus(statusMask,
                                       failCode,
                                       requestedMask,
                                       record);
}
#endif
