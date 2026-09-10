#include "Board_Dido_Test.h"
#include "Board_Fpga_Test.h"
#include "Board_Profile.h"

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

volatile BoardDido_DspDoSnapshot gBoardDidoDspDoSnapshot =
{
    0U,
    BOARD_DIDO_FAIL_NONE,
    0U,
    0U,
    0U,
    0U,
    0U,
    BOARD_DIDO_PHASE_IDLE
};

volatile BoardDido_DspDoSnapshot gBoardDidoDriverResetSnapshot =
{
    0U,
    BOARD_DIDO_FAIL_NONE,
    0U,
    0U,
    0U,
    0U,
    0U,
    BOARD_DIDO_PHASE_IDLE
};

static BoardTest_U16 BoardDido_GetRequiredStatusMask(
    BoardTest_U16 channelMask,
    BoardTest_U16 requireFpgaMagic)
{
    BoardTest_U16 requiredMask;

    requiredMask = BOARD_DIDO_STATUS_CONFIGURED |
                   BOARD_DIDO_STATUS_OUTPUTS_DISABLED;

    if(requireFpgaMagic != 0U)
    {
        requiredMask |= BOARD_DIDO_STATUS_MAGIC_VALID;
    }

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

    requiredMask = BoardDido_GetRequiredStatusMask(channelMask, 1U);
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

BoardTest_Result BoardDido_EvaluateDspDoStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 channelMask,
    BoardTest_Record *record)
{
    BoardTest_U16 requiredMask;

    requiredMask = BoardDido_GetRequiredStatusMask(channelMask, 0U);
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
        return BOARD_TEST_RESULT_WARN;
    }

    record->errorCode = BOARD_TEST_ERROR_DIDO_EXTERNAL;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardDido_EvaluateDriverResetStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 channelMask,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)failCode & 0xFFFFUL);
    record->measuredValue =
        (float)(channelMask & BOARD_DRIVER_RESET_ALLOWED_MASK);
    record->expectedMin =
        (float)(channelMask & BOARD_DRIVER_RESET_ALLOWED_MASK);
    record->expectedMax = (float)BOARD_DRIVER_RESET_ALLOWED_MASK;

    if((failCode == BOARD_DIDO_FAIL_NOT_ARMED) ||
       (failCode == BOARD_DIDO_FAIL_INVALID_MASK))
    {
        record->errorCode = BOARD_TEST_ERROR_SAFETY_LOCK;
        return BOARD_TEST_RESULT_SAFETY_LOCKED;
    }

    if(((statusMask & BOARD_DRIVER_RESET_REQUIRED_STATUS) ==
        BOARD_DRIVER_RESET_REQUIRED_STATUS) &&
       (failCode == BOARD_DIDO_FAIL_NONE))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_WARN;
    }

    record->errorCode = BOARD_TEST_ERROR_DRIVER_RESET_DO_EXTERNAL;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST
typedef struct
{
    BoardTest_U16 active;
    BoardTest_U16 requestedMask;
    BoardTest_U16 selectedPin;
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U32 elapsedUs;
} BoardDido_DriverResetRunState;

static BoardDido_DriverResetRunState BoardDido_DriverResetState =
{
    0U,
    0U,
    BOARD_PROFILE_PIN_UNUSED,
    0U,
    BOARD_DIDO_FAIL_NONE,
    0UL
};

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

static BoardTest_U16 BoardDido_GetDspDoPin(
    const BoardProfile_PinMap *pins,
    BoardTest_U16 channelBit)
{
    if(channelBit == BOARD_DIDO_CHANNEL_1)
    {
        return pins->dspDo1;
    }
    if(channelBit == BOARD_DIDO_CHANNEL_2)
    {
        return pins->dspDo2;
    }
    if(channelBit == BOARD_DIDO_CHANNEL_3)
    {
        return pins->dspDo3;
    }
    return BOARD_PROFILE_PIN_UNUSED;
}

static BoardTest_U16 BoardDido_ReadDspOutputMask(
    const BoardProfile_PinMap *pins)
{
    BoardTest_U16 outputMask;

    outputMask = 0U;
    if((pins->dspDo1 != BOARD_PROFILE_PIN_UNUSED) &&
       (GPIO_ReadPin(pins->dspDo1) != 0U))
    {
        outputMask |= BOARD_DIDO_CHANNEL_1;
    }
    if((pins->dspDo2 != BOARD_PROFILE_PIN_UNUSED) &&
       (GPIO_ReadPin(pins->dspDo2) != 0U))
    {
        outputMask |= BOARD_DIDO_CHANNEL_2;
    }
    if((pins->dspDo3 != BOARD_PROFILE_PIN_UNUSED) &&
       (GPIO_ReadPin(pins->dspDo3) != 0U))
    {
        outputMask |= BOARD_DIDO_CHANNEL_3;
    }
    return outputMask;
}

void BoardDido_ForceDspDoSafe(void)
{
    const BoardProfile_HardwareDescriptor *hardware;
    const BoardProfile_PinMap *pins;

    hardware = BoardProfile_GetCurrentHardware();
    if(hardware == 0)
    {
        return;
    }

    pins = &hardware->pins;
    if(pins->dspDo1 != BOARD_PROFILE_PIN_UNUSED)
    {
        GPIO_WritePin(pins->dspDo1, 0U);
    }
    if(pins->dspDo2 != BOARD_PROFILE_PIN_UNUSED)
    {
        GPIO_WritePin(pins->dspDo2, 0U);
    }
    if(pins->dspDo3 != BOARD_PROFILE_PIN_UNUSED)
    {
        GPIO_WritePin(pins->dspDo3, 0U);
    }
}

static BoardTest_U16 BoardDido_PrepareDspOutputs(
    const BoardProfile_PinMap *pins,
    BoardTest_U16 channelMask)
{
    BoardTest_U16 channelBit;
    BoardTest_U16 pin;

    for(channelBit = BOARD_DIDO_CHANNEL_1;
        channelBit <= BOARD_DIDO_CHANNEL_3;
        channelBit <<= 1U)
    {
        if((channelMask & channelBit) != 0U)
        {
            pin = BoardDido_GetDspDoPin(pins, channelBit);
            if(pin == BOARD_PROFILE_PIN_UNUSED)
            {
                return 0U;
            }
            GPIO_WritePin(pin, 0U);
            GPIO_SetupPinMux(pin, GPIO_MUX_CPU1, 0U);
            GPIO_SetupPinOptions(pin, GPIO_OUTPUT, GPIO_ASYNC);
            GPIO_WritePin(pin, 0U);
        }
    }
    return 1U;
}

static void BoardDido_ToggleDspChannel(
    const BoardProfile_PinMap *pins,
    BoardTest_U16 channelBit)
{
    BoardTest_U16 pin;

    pin = BoardDido_GetDspDoPin(pins, channelBit);
    BoardDido_ForceDspDoSafe();
    GPIO_WritePin(pin, 1U);
    gBoardDidoDspDoSnapshot.activeChannelMask = channelBit;
    gBoardDidoDspDoSnapshot.phase = BOARD_DIDO_PHASE_DATA_SET;
    DELAY_US(BOARD_DIDO_PHASE_HOLD_US);

    GPIO_WritePin(pin, 0U);
    gBoardDidoDspDoSnapshot.phase = BOARD_DIDO_PHASE_DATA_CLEAR;
    DELAY_US(BOARD_DIDO_PHASE_HOLD_US);
    gBoardDidoDspDoSnapshot.activeChannelMask = 0U;
    gBoardDidoDspDoSnapshot.phase = BOARD_DIDO_PHASE_SAFE;
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

BoardTest_Result BoardDido_RunDspDoExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 armKey,
    BoardTest_U16 channelMask)
{
    const BoardProfile_HardwareDescriptor *hardware;
    const BoardProfile_PinMap *pins;
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 requestedMask;

    hardware = BoardProfile_GetCurrentHardware();
    pins = (hardware != 0) ? &hardware->pins : 0;
    statusMask = 0U;
    failCode = BOARD_DIDO_FAIL_NONE;
    requestedMask = channelMask & BOARD_DIDO_ALLOWED_CHANNEL_MASK;

    gBoardDidoDspDoSnapshot.statusMask = 0U;
    gBoardDidoDspDoSnapshot.failCode = BOARD_DIDO_FAIL_NONE;
    gBoardDidoDspDoSnapshot.requestedChannelMask = requestedMask;
    gBoardDidoDspDoSnapshot.initialOutputMask = 0U;
    gBoardDidoDspDoSnapshot.toggledChannelMask = 0U;
    gBoardDidoDspDoSnapshot.finalOutputMask = 0U;
    gBoardDidoDspDoSnapshot.activeChannelMask = 0U;
    gBoardDidoDspDoSnapshot.phase = BOARD_DIDO_PHASE_IDLE;

    if(armKey != BOARD_DIDO_TEST_ARM_KEY)
    {
        failCode = BOARD_DIDO_FAIL_NOT_ARMED;
    }
    else if((requestedMask == 0U) ||
            ((channelMask & ~BOARD_DIDO_ALLOWED_CHANNEL_MASK) != 0U))
    {
        failCode = BOARD_DIDO_FAIL_INVALID_MASK;
    }
    else if((pins == 0) ||
            (BoardDido_PrepareDspOutputs(pins, requestedMask) == 0U))
    {
        failCode = BOARD_DIDO_FAIL_CONFIG;
    }
    else
    {
        statusMask |= BOARD_DIDO_STATUS_CONFIGURED;
    }

    if(failCode == BOARD_DIDO_FAIL_NONE)
    {
        gBoardDidoDspDoSnapshot.initialOutputMask =
            BoardDido_ReadDspOutputMask(pins);

        if((requestedMask & BOARD_DIDO_CHANNEL_1) != 0U)
        {
            BoardDido_ToggleDspChannel(pins, BOARD_DIDO_CHANNEL_1);
            statusMask |= BOARD_DIDO_STATUS_CHANNEL_1_TOGGLED;
        }
        if((requestedMask & BOARD_DIDO_CHANNEL_2) != 0U)
        {
            BoardDido_ToggleDspChannel(pins, BOARD_DIDO_CHANNEL_2);
            statusMask |= BOARD_DIDO_STATUS_CHANNEL_2_TOGGLED;
        }
        if((requestedMask & BOARD_DIDO_CHANNEL_3) != 0U)
        {
            BoardDido_ToggleDspChannel(pins, BOARD_DIDO_CHANNEL_3);
            statusMask |= BOARD_DIDO_STATUS_CHANNEL_3_TOGGLED;
        }

        gBoardDidoDspDoSnapshot.toggledChannelMask = requestedMask;
        BoardDido_ForceDspDoSafe();
        gBoardDidoDspDoSnapshot.finalOutputMask =
            BoardDido_ReadDspOutputMask(pins);
        if(gBoardDidoDspDoSnapshot.finalOutputMask == 0U)
        {
            statusMask |= BOARD_DIDO_STATUS_OUTPUTS_DISABLED;
        }
        else
        {
            failCode = BOARD_DIDO_FAIL_CONFIG;
        }
        gBoardDidoDspDoSnapshot.phase = BOARD_DIDO_PHASE_SAFE;
    }

    if((failCode != BOARD_DIDO_FAIL_NONE) && (pins != 0))
    {
        BoardDido_ForceDspDoSafe();
    }

    gBoardDidoDspDoSnapshot.statusMask = statusMask;
    gBoardDidoDspDoSnapshot.failCode = failCode;
    return BoardDido_EvaluateDspDoStatus(statusMask,
                                         failCode,
                                         requestedMask,
                                         record);
}

static BoardTest_U16 BoardDido_GetDriverResetPin(
    const BoardProfile_PinMap *pins,
    BoardTest_U16 channelBit)
{
    BoardTest_U16 index;

    for(index = 0U; index < BOARD_PROFILE_DRIVER_RESET_DO_COUNT; index++)
    {
        if(channelBit == ((BoardTest_U16)1U << index))
        {
            return pins->driverResetOutputs[index];
        }
    }
    return BOARD_PROFILE_PIN_UNUSED;
}

static BoardTest_U16 BoardDido_ReadDriverResetMask(
    const BoardProfile_PinMap *pins)
{
    BoardTest_U16 index;
    BoardTest_U16 mask;

    mask = 0U;
    for(index = 0U; index < BOARD_PROFILE_DRIVER_RESET_DO_COUNT; index++)
    {
        if((pins->driverResetOutputs[index] != BOARD_PROFILE_PIN_UNUSED) &&
           (GPIO_ReadPin(pins->driverResetOutputs[index]) != 0U))
        {
            mask |= ((BoardTest_U16)1U << index);
        }
    }
    return mask;
}

static void BoardDido_UpdateDriverResetRunningRecord(
    BoardTest_Record *record,
    const BoardProfile_PinMap *pins)
{
    BoardTest_U16 liveMask;

    liveMask = BoardDido_ReadDriverResetMask(pins);
    gBoardDidoDriverResetSnapshot.statusMask =
        BoardDido_DriverResetState.statusMask;
    gBoardDidoDriverResetSnapshot.failCode =
        BoardDido_DriverResetState.failCode;
    gBoardDidoDriverResetSnapshot.activeChannelMask = liveMask;
    gBoardDidoDriverResetSnapshot.phase = BOARD_DIDO_PHASE_DATA_SET;

    record->errorCode = BOARD_TEST_ERROR_NONE;
    record->rawValue = (((BoardTest_U32)liveMask & 0x000FUL) << 16U) |
                       ((BoardTest_U32)BoardDido_DriverResetState.statusMask &
                        0xFFFFUL);
    record->measuredValue =
        ((liveMask & BoardDido_DriverResetState.requestedMask) != 0U) ?
            3.3F : 0.0F;
    record->expectedMin =
        (float)BoardDido_DriverResetState.requestedMask;
    record->expectedMax =
        (float)BoardDido_DriverResetState.elapsedUs / 1000.0F;
}

static BoardTest_Result BoardDido_FinishDriverResetExternalTest(
    BoardTest_Record *record,
    const BoardProfile_PinMap *pins)
{
    BoardTest_U16 index;
    BoardTest_U16 finalMask;
    BoardTest_U16 requestedMask;
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;

    requestedMask = BoardDido_DriverResetState.requestedMask;
    statusMask = BoardDido_DriverResetState.statusMask;
    failCode = BoardDido_DriverResetState.failCode;

    if(pins != 0)
    {
        for(index = 0U; index < BOARD_PROFILE_DRIVER_RESET_DO_COUNT; index++)
        {
            if(pins->driverResetOutputs[index] != BOARD_PROFILE_PIN_UNUSED)
            {
                GPIO_WritePin(pins->driverResetOutputs[index], 0U);
            }
        }
        DELAY_US(10U);
        finalMask = BoardDido_ReadDriverResetMask(pins);
    }
    else
    {
        finalMask = BOARD_DRIVER_RESET_ALLOWED_MASK;
    }

    if(finalMask == 0U)
    {
        statusMask |= BOARD_DRIVER_RESET_STATUS_SAFE_FINAL;
    }
    else
    {
        failCode = BOARD_DIDO_FAIL_CONFIG;
    }

    gBoardDidoDriverResetSnapshot.statusMask = statusMask;
    gBoardDidoDriverResetSnapshot.failCode = failCode;
    gBoardDidoDriverResetSnapshot.toggledChannelMask =
        ((statusMask & BOARD_DRIVER_RESET_STATUS_READBACK) != 0U) ?
            requestedMask : 0U;
    gBoardDidoDriverResetSnapshot.finalOutputMask = finalMask;
    gBoardDidoDriverResetSnapshot.activeChannelMask = 0U;
    gBoardDidoDriverResetSnapshot.phase = BOARD_DIDO_PHASE_SAFE;

    BoardDido_DriverResetState.active = 0U;
    BoardDido_DriverResetState.elapsedUs = 0UL;
    return BoardDido_EvaluateDriverResetStatus(statusMask,
                                                failCode,
                                                requestedMask,
                                                record);
}

void BoardDido_ForceDriverResetSafe(void)
{
    const BoardProfile_HardwareDescriptor *hardware;
    BoardTest_U16 index;

    BoardDido_DriverResetState.active = 0U;
    BoardDido_DriverResetState.elapsedUs = 0UL;
    hardware = BoardProfile_GetCurrentHardware();
    if(hardware == 0)
    {
        return;
    }

    for(index = 0U; index < BOARD_PROFILE_DRIVER_RESET_DO_COUNT; index++)
    {
        if(hardware->pins.driverResetOutputs[index] !=
           BOARD_PROFILE_PIN_UNUSED)
        {
            GPIO_WritePin(hardware->pins.driverResetOutputs[index], 0U);
        }
    }
}

BoardTest_Result BoardDido_RunDriverResetExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 armKey,
    BoardTest_U16 channelMask)
{
    const BoardProfile_HardwareDescriptor *hardware;
    const BoardProfile_PinMap *pins;
    BoardTest_U16 index;
    BoardTest_U16 pin;
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 requestedMask;
    BoardTest_U16 liveMask;

    hardware = BoardProfile_GetCurrentHardware();
    pins = (hardware != 0) ? &hardware->pins : 0;

    if(BoardDido_DriverResetState.active != 0U)
    {
        DELAY_US(BOARD_DRIVER_RESET_SERVICE_STEP_US);
        BoardDido_DriverResetState.elapsedUs +=
            BOARD_DRIVER_RESET_SERVICE_STEP_US;
        liveMask = (pins != 0) ? BoardDido_ReadDriverResetMask(pins) : 0U;
        if((liveMask & BoardDido_DriverResetState.requestedMask) ==
           BoardDido_DriverResetState.requestedMask)
        {
            BoardDido_DriverResetState.statusMask |=
                BOARD_DRIVER_RESET_STATUS_READBACK;
        }
        else
        {
            BoardDido_DriverResetState.failCode = BOARD_DIDO_FAIL_CONFIG;
        }

        if((BoardDido_DriverResetState.failCode != BOARD_DIDO_FAIL_NONE) ||
           (BoardDido_DriverResetState.elapsedUs >=
            BOARD_DRIVER_RESET_HIGH_HOLD_US))
        {
            return BoardDido_FinishDriverResetExternalTest(record, pins);
        }

        BoardDido_UpdateDriverResetRunningRecord(record, pins);
        return BOARD_TEST_RESULT_RUNNING;
    }

    statusMask = 0U;
    failCode = BOARD_DIDO_FAIL_NONE;
    requestedMask = channelMask & BOARD_DRIVER_RESET_ALLOWED_MASK;

    gBoardDidoDriverResetSnapshot.statusMask = 0U;
    gBoardDidoDriverResetSnapshot.failCode = BOARD_DIDO_FAIL_NONE;
    gBoardDidoDriverResetSnapshot.requestedChannelMask = requestedMask;
    gBoardDidoDriverResetSnapshot.initialOutputMask = 0U;
    gBoardDidoDriverResetSnapshot.toggledChannelMask = 0U;
    gBoardDidoDriverResetSnapshot.finalOutputMask = 0U;
    gBoardDidoDriverResetSnapshot.activeChannelMask = 0U;
    gBoardDidoDriverResetSnapshot.phase = BOARD_DIDO_PHASE_IDLE;

    if(armKey != BOARD_DIDO_TEST_ARM_KEY)
    {
        failCode = BOARD_DIDO_FAIL_NOT_ARMED;
    }
    else if((requestedMask == 0U) ||
            ((channelMask & ~BOARD_DRIVER_RESET_ALLOWED_MASK) != 0U) ||
            ((requestedMask & (requestedMask - 1U)) != 0U))
    {
        failCode = BOARD_DIDO_FAIL_INVALID_MASK;
    }
    else if(pins == 0)
    {
        failCode = BOARD_DIDO_FAIL_CONFIG;
    }

    if(failCode == BOARD_DIDO_FAIL_NONE)
    {
        for(index = 0U; index < BOARD_PROFILE_DRIVER_RESET_DO_COUNT; index++)
        {
            pin = pins->driverResetOutputs[index];
            if(pin == BOARD_PROFILE_PIN_UNUSED)
            {
                failCode = BOARD_DIDO_FAIL_CONFIG;
                break;
            }
            GPIO_WritePin(pin, 0U);
            GPIO_SetupPinMux(pin, GPIO_MUX_CPU1, 0U);
            GPIO_SetupPinOptions(pin, GPIO_OUTPUT, GPIO_ASYNC);
            GPIO_WritePin(pin, 0U);
        }
        if(failCode == BOARD_DIDO_FAIL_NONE)
        {
            statusMask |= BOARD_DRIVER_RESET_STATUS_CONFIGURED;
        }
    }

    if(failCode == BOARD_DIDO_FAIL_NONE)
    {
        gBoardDidoDriverResetSnapshot.initialOutputMask =
            BoardDido_ReadDriverResetMask(pins);
        pin = BoardDido_GetDriverResetPin(pins, requestedMask);
        GPIO_WritePin(pin, 1U);
        DELAY_US(10U);
        gBoardDidoDriverResetSnapshot.activeChannelMask = requestedMask;
        gBoardDidoDriverResetSnapshot.phase = BOARD_DIDO_PHASE_DATA_SET;
        statusMask |= BOARD_DRIVER_RESET_STATUS_ASSERTED;
        if((BoardDido_ReadDriverResetMask(pins) & requestedMask) ==
           requestedMask)
        {
            statusMask |= BOARD_DRIVER_RESET_STATUS_READBACK;
        }
        else
        {
            failCode = BOARD_DIDO_FAIL_CONFIG;
        }
    }

    if(failCode != BOARD_DIDO_FAIL_NONE)
    {
        BoardDido_DriverResetState.requestedMask = requestedMask;
        BoardDido_DriverResetState.statusMask = statusMask;
        BoardDido_DriverResetState.failCode = failCode;
        return BoardDido_FinishDriverResetExternalTest(record, pins);
    }

    BoardDido_DriverResetState.active = 1U;
    BoardDido_DriverResetState.requestedMask = requestedMask;
    BoardDido_DriverResetState.selectedPin = pin;
    BoardDido_DriverResetState.statusMask = statusMask;
    BoardDido_DriverResetState.failCode = BOARD_DIDO_FAIL_NONE;
    BoardDido_DriverResetState.elapsedUs = 0UL;
    BoardDido_UpdateDriverResetRunningRecord(record, pins);
    return BOARD_TEST_RESULT_RUNNING;
}
#endif
