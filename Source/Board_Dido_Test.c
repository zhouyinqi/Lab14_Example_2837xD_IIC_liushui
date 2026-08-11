#include "Board_Dido_Test.h"
#include "Board_Fpga_Test.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

volatile BoardDido_FpgaDoSnapshot gBoardDidoFpgaDoSnapshot =
{
    0U,
    BOARD_DIDO_FAIL_NONE,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U
};

static BoardTest_U16 BoardDido_GetRequiredStatusMask(
    BoardTest_U16 channelMask)
{
    BoardTest_U16 requiredMask;

    requiredMask = BOARD_DIDO_STATUS_CONFIGURED |
                   BOARD_DIDO_STATUS_MAGIC_VALID |
                   BOARD_DIDO_STATUS_OUTPUTS_DISABLED;

    if((channelMask & BOARD_DIDO_CHANNEL_1) != 0U)
    {
        requiredMask |= BOARD_DIDO_STATUS_CHANNEL_1_TOGGLED;
    }

    if((channelMask & BOARD_DIDO_CHANNEL_2) != 0U)
    {
        requiredMask |= BOARD_DIDO_STATUS_CHANNEL_2_TOGGLED;
    }

    if((channelMask & BOARD_DIDO_CHANNEL_3) != 0U)
    {
        requiredMask |= BOARD_DIDO_STATUS_CHANNEL_3_TOGGLED;
    }

    return requiredMask;
}

BoardTest_Result BoardDido_EvaluateFpgaDoStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 channelMask,
    BoardTest_Record *record)
{
    BoardTest_U16 requiredMask;

    requiredMask = BoardDido_GetRequiredStatusMask(channelMask);
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)failCode & 0xFFFFUL);
    record->measuredValue = (float)(channelMask &
                                    BOARD_DIDO_ALLOWED_CHANNEL_MASK);
    record->expectedMin = (float)(channelMask &
                                  BOARD_DIDO_ALLOWED_CHANNEL_MASK);
    record->expectedMax = (float)BOARD_DIDO_ALLOWED_CHANNEL_MASK;

    if((failCode == BOARD_DIDO_FAIL_NOT_ARMED) ||
       (failCode == BOARD_DIDO_FAIL_INVALID_MASK))
    {
        record->errorCode = BOARD_TEST_ERROR_SAFETY_LOCK;
        return BOARD_TEST_RESULT_SAFETY_LOCKED;
    }

    if(((statusMask & requiredMask) == requiredMask) &&
       (failCode == BOARD_DIDO_FAIL_NONE))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        /* PLC input is not returned to DSP1 yet, so confirmation is manual. */
        return BOARD_TEST_RESULT_WARN;
    }

    record->errorCode = BOARD_TEST_ERROR_DIDO_EXTERNAL;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST
static BoardTest_U16 BoardDido_Read16(BoardTest_U32 address)
{
    return *((volatile Uint16 *)address);
}

static void BoardDido_Write16(BoardTest_U32 address, BoardTest_U16 value)
{
    *((volatile Uint16 *)address) = (Uint16)value;
    asm(" RPT #7 || NOP");
}

void BoardDido_ForceFpgaDoSafe(void)
{
    BoardDido_Write16(BOARD_DIDO_FPGA_DO_ENABLE_ADDR, 0U);
    BoardDido_Write16(BOARD_DIDO_FPGA_DO_DATA_ADDR, 0U);
}

static void BoardDido_ToggleChannel(BoardTest_U16 channelBit)
{
    /*
     * Both logical levels are applied because the external DO polarity has
     * not yet been confirmed. The enable word limits activity to one channel.
     */
    BoardDido_Write16(BOARD_DIDO_FPGA_DO_ENABLE_ADDR, 0U);
    BoardDido_Write16(BOARD_DIDO_FPGA_DO_DATA_ADDR, channelBit);
    BoardDido_Write16(BOARD_DIDO_FPGA_DO_ENABLE_ADDR, channelBit);
    gBoardDidoFpgaDoSnapshot.activeChannelMask = channelBit;
    gBoardDidoFpgaDoSnapshot.activeDataValue = channelBit;
    gBoardDidoFpgaDoSnapshot.phase = BOARD_DIDO_PHASE_DATA_SET;
    DELAY_US(BOARD_DIDO_PHASE_HOLD_US);

    BoardDido_Write16(BOARD_DIDO_FPGA_DO_DATA_ADDR, 0U);
    gBoardDidoFpgaDoSnapshot.activeDataValue = 0U;
    gBoardDidoFpgaDoSnapshot.phase = BOARD_DIDO_PHASE_DATA_CLEAR;
    DELAY_US(BOARD_DIDO_PHASE_HOLD_US);
    BoardDido_Write16(BOARD_DIDO_FPGA_DO_ENABLE_ADDR, 0U);
    gBoardDidoFpgaDoSnapshot.activeChannelMask = 0U;
    gBoardDidoFpgaDoSnapshot.phase = BOARD_DIDO_PHASE_SAFE;
}

BoardTest_Result BoardDido_RunFpgaDoExternalTest(
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
    failCode = BOARD_DIDO_FAIL_NONE;
    requestedMask = channelMask & BOARD_DIDO_ALLOWED_CHANNEL_MASK;

    gBoardDidoFpgaDoSnapshot.statusMask = 0U;
    gBoardDidoFpgaDoSnapshot.failCode = BOARD_DIDO_FAIL_NONE;
    gBoardDidoFpgaDoSnapshot.requestedChannelMask = requestedMask;
    gBoardDidoFpgaDoSnapshot.initialDataValue = 0U;
    gBoardDidoFpgaDoSnapshot.initialEnableValue = 0U;
    gBoardDidoFpgaDoSnapshot.finalDataValue = 0U;
    gBoardDidoFpgaDoSnapshot.finalEnableValue = 0U;
    gBoardDidoFpgaDoSnapshot.activeChannelMask = 0U;
    gBoardDidoFpgaDoSnapshot.activeDataValue = 0U;
    gBoardDidoFpgaDoSnapshot.phase = BOARD_DIDO_PHASE_IDLE;

    if(armKey != BOARD_DIDO_TEST_ARM_KEY)
    {
        failCode = BOARD_DIDO_FAIL_NOT_ARMED;
    }
    else if((requestedMask == 0U) ||
            ((channelMask & ~BOARD_DIDO_ALLOWED_CHANNEL_MASK) != 0U))
    {
        failCode = BOARD_DIDO_FAIL_INVALID_MASK;
    }

    if(failCode == BOARD_DIDO_FAIL_NONE)
    {
        if(BoardFpga_PrepareEmif2Interface() != 0U)
        {
            statusMask |= BOARD_DIDO_STATUS_CONFIGURED;
        }
        else
        {
            failCode = BOARD_DIDO_FAIL_CONFIG;
        }
    }

    if(failCode == BOARD_DIDO_FAIL_NONE)
    {
        magic2Value = BoardDido_Read16(BOARD_FPGA_EMIF2_TEST2_ADDR);
        magic3Value = BoardDido_Read16(BOARD_FPGA_EMIF2_TEST3_ADDR);
        if((magic2Value == BOARD_FPGA_TEST2_EXPECTED) &&
           (magic3Value == BOARD_FPGA_TEST3_EXPECTED))
        {
            statusMask |= BOARD_DIDO_STATUS_MAGIC_VALID;
        }
        else
        {
            failCode = BOARD_DIDO_FAIL_MAGIC;
        }
    }

    if(failCode == BOARD_DIDO_FAIL_NONE)
    {
        gBoardDidoFpgaDoSnapshot.initialDataValue =
            BoardDido_Read16(BOARD_DIDO_FPGA_DO_DATA_ADDR);
        gBoardDidoFpgaDoSnapshot.initialEnableValue =
            BoardDido_Read16(BOARD_DIDO_FPGA_DO_ENABLE_ADDR);

        BoardDido_ForceFpgaDoSafe();

        if((requestedMask & BOARD_DIDO_CHANNEL_1) != 0U)
        {
            BoardDido_ToggleChannel(BOARD_DIDO_CHANNEL_1);
            statusMask |= BOARD_DIDO_STATUS_CHANNEL_1_TOGGLED;
        }

        if((requestedMask & BOARD_DIDO_CHANNEL_2) != 0U)
        {
            BoardDido_ToggleChannel(BOARD_DIDO_CHANNEL_2);
            statusMask |= BOARD_DIDO_STATUS_CHANNEL_2_TOGGLED;
        }

        if((requestedMask & BOARD_DIDO_CHANNEL_3) != 0U)
        {
            BoardDido_ToggleChannel(BOARD_DIDO_CHANNEL_3);
            statusMask |= BOARD_DIDO_STATUS_CHANNEL_3_TOGGLED;
        }

        BoardDido_ForceFpgaDoSafe();
        gBoardDidoFpgaDoSnapshot.finalDataValue =
            BoardDido_Read16(BOARD_DIDO_FPGA_DO_DATA_ADDR);
        gBoardDidoFpgaDoSnapshot.finalEnableValue =
            BoardDido_Read16(BOARD_DIDO_FPGA_DO_ENABLE_ADDR);
        gBoardDidoFpgaDoSnapshot.phase = BOARD_DIDO_PHASE_SAFE;
        statusMask |= BOARD_DIDO_STATUS_OUTPUTS_DISABLED;
    }

    if((failCode != BOARD_DIDO_FAIL_NONE) &&
       ((statusMask & BOARD_DIDO_STATUS_CONFIGURED) != 0U))
    {
        BoardDido_ForceFpgaDoSafe();
    }

    gBoardDidoFpgaDoSnapshot.statusMask = statusMask;
    gBoardDidoFpgaDoSnapshot.failCode = failCode;

    return BoardDido_EvaluateFpgaDoStatus(statusMask,
                                          failCode,
                                          requestedMask,
                                          record);
}
#endif
