#include "Board_Di_Test.h"
#include "Board_Fpga_Test.h"
#include "Board_Profile.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

volatile BoardDi_FpgaSnapshot gBoardDiFpgaSnapshot =
{
    0U,
    BOARD_DI_FAIL_NONE,
    BOARD_DI_DEFAULT_CHANNEL_MASK,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U
};

volatile BoardDi_DspSnapshot gBoardDiDspSnapshot =
{
    0U,
    BOARD_DI_FAIL_NONE,
    BOARD_DI_DSP_DEFAULT_CHANNEL_MASK,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U
};

volatile BoardDi_DspSnapshot gBoardDiDriverFaultSnapshot =
{
    0U,
    BOARD_DI_FAIL_NONE,
    BOARD_DI_DRIVER_FAULT_DEFAULT_CHANNEL_MASK,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U
};

volatile BoardDi_DspSnapshot gBoardDiProtectionFaultSnapshot =
{
    0U,
    BOARD_DI_FAIL_NONE,
    BOARD_DI_PROTECTION_FAULT_DEFAULT_CHANNEL_MASK,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U
};

BoardTest_Result BoardDi_EvaluateFpgaStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_Record *record)
{
    BoardTest_U16 expectedMask;

    expectedMask = requestedChannelMask & BOARD_DI_INPUT_MASK;
    record->rawValue =
        (((BoardTest_U32)observedActiveMask & 0xFFFFUL) << 16U) |
        ((BoardTest_U32)observedInactiveMask & 0xFFFFUL);
    record->measuredValue = (float)(transitionMask & expectedMask);
    record->expectedMin = (float)expectedMask;
    record->expectedMax = (float)expectedMask;

    if((expectedMask != 0U) &&
       ((statusMask & BOARD_DI_REQUIRED_STATUS_MASK) ==
        BOARD_DI_REQUIRED_STATUS_MASK) &&
       (failCode == BOARD_DI_FAIL_NONE) &&
       ((observedInactiveMask & expectedMask) == expectedMask) &&
       ((observedActiveMask & expectedMask) == expectedMask) &&
       ((transitionMask & expectedMask) == expectedMask))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_DI_EXTERNAL;
    return (failCode == BOARD_DI_FAIL_TIMEOUT) ?
           BOARD_TEST_RESULT_TIMEOUT : BOARD_TEST_RESULT_FAIL;
}

static BoardTest_U16 BoardDi_CaptureComplete(
    BoardTest_U16 expectedMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_U16 acceptAnyChannel)
{
    BoardTest_U16 completedMask;

    completedMask = expectedMask & observedInactiveMask &
                    observedActiveMask & transitionMask;
    return (expectedMask != 0U) &&
           ((acceptAnyChannel != 0U) ? (completedMask != 0U) :
                                      (completedMask == expectedMask));
}

static BoardTest_Result BoardDi_EvaluateMappedDspStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 inputMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_U16 errorCode,
    BoardTest_U16 acceptAnyChannel,
    BoardTest_Record *record)
{
    BoardTest_U16 expectedMask;

    expectedMask = requestedChannelMask & inputMask;
    record->rawValue =
        (((BoardTest_U32)observedActiveMask & 0xFFFFUL) << 16U) |
        ((BoardTest_U32)observedInactiveMask & 0xFFFFUL);
    record->measuredValue = (float)(transitionMask & expectedMask);
    record->expectedMin = (float)expectedMask;
    record->expectedMax = (float)expectedMask;

    if(((requestedChannelMask & ~inputMask) == 0U) &&
       ((statusMask & BOARD_DI_DSP_REQUIRED_STATUS_MASK) ==
        BOARD_DI_DSP_REQUIRED_STATUS_MASK) &&
       (failCode == BOARD_DI_FAIL_NONE) &&
       (BoardDi_CaptureComplete(expectedMask, observedInactiveMask,
                                observedActiveMask, transitionMask,
                                acceptAnyChannel) != 0U))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = errorCode;
    return (failCode == BOARD_DI_FAIL_TIMEOUT) ?
           BOARD_TEST_RESULT_TIMEOUT : BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardDi_EvaluateDspStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_Record *record)
{
    return BoardDi_EvaluateMappedDspStatus(
        statusMask,
        failCode,
        requestedChannelMask,
        BOARD_DI_DSP_INPUT_MASK,
        observedInactiveMask,
        observedActiveMask,
        transitionMask,
        BOARD_TEST_ERROR_DI_EXTERNAL,
        0U,
        record);
}

BoardTest_Result BoardDi_EvaluateDriverFaultStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_Record *record)
{
    return BoardDi_EvaluateMappedDspStatus(
        statusMask,
        failCode,
        requestedChannelMask,
        BOARD_DI_DRIVER_FAULT_INPUT_MASK,
        observedInactiveMask,
        observedActiveMask,
        transitionMask,
        BOARD_TEST_ERROR_DRIVER_FAULT_DI_EXTERNAL,
        0U,
        record);
}

BoardTest_Result BoardDi_EvaluateProtectionFaultStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_Record *record)
{
    return BoardDi_EvaluateMappedDspStatus(
        statusMask,
        failCode,
        requestedChannelMask,
        BOARD_DI_PROTECTION_FAULT_INPUT_MASK,
        observedInactiveMask,
        observedActiveMask,
        transitionMask,
        BOARD_TEST_ERROR_PROTECTION_FAULT_DI_EXTERNAL,
        0U,
        record);
}

#ifndef BOARD_TEST_HOST
static BoardTest_U16 BoardDi_Read16(BoardTest_U32 address)
{
    return *((volatile Uint16 *)address);
}

BoardTest_Result BoardDi_RunFpgaExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 requestedChannelMask)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 expectedMask;
    BoardTest_U16 rawValue;
    BoardTest_U16 previousRawValue;
    BoardTest_U16 observedInactiveMask;
    BoardTest_U16 observedActiveMask;
    BoardTest_U16 transitionMask;
    BoardTest_U16 sampleCount;
    BoardTest_U16 magic2Value;
    BoardTest_U16 magic3Value;

    statusMask = 0U;
    failCode = BOARD_DI_FAIL_NONE;
    expectedMask = requestedChannelMask & BOARD_DI_INPUT_MASK;
    observedInactiveMask = 0U;
    observedActiveMask = 0U;
    transitionMask = 0U;
    sampleCount = 0U;

    gBoardDiFpgaSnapshot.statusMask = 0U;
    gBoardDiFpgaSnapshot.failCode = BOARD_DI_FAIL_NONE;
    gBoardDiFpgaSnapshot.requestedChannelMask = expectedMask;
    gBoardDiFpgaSnapshot.firstRawValue = 0U;
    gBoardDiFpgaSnapshot.lastRawValue = 0U;
    gBoardDiFpgaSnapshot.observedInactiveMask = 0U;
    gBoardDiFpgaSnapshot.observedActiveMask = 0U;
    gBoardDiFpgaSnapshot.transitionMask = 0U;
    gBoardDiFpgaSnapshot.sampleCount = 0U;

    if(expectedMask == 0U)
    {
        failCode = BOARD_DI_FAIL_CHANNEL_MASK;
    }
    else if(BoardFpga_PrepareEmif2Interface() != 0U)
    {
        statusMask |= BOARD_DI_STATUS_CONFIGURED;
    }
    else
    {
        failCode = BOARD_DI_FAIL_CONFIG;
    }

    if(failCode == BOARD_DI_FAIL_NONE)
    {
        magic2Value = BoardDi_Read16(BOARD_FPGA_EMIF2_TEST2_ADDR);
        magic3Value = BoardDi_Read16(BOARD_FPGA_EMIF2_TEST3_ADDR);
        if((magic2Value == BOARD_FPGA_TEST2_EXPECTED) &&
           (magic3Value == BOARD_FPGA_TEST3_EXPECTED))
        {
            statusMask |= BOARD_DI_STATUS_MAGIC_VALID;
        }
        else
        {
            failCode = BOARD_DI_FAIL_MAGIC;
        }
    }

    if(failCode == BOARD_DI_FAIL_NONE)
    {
        previousRawValue =
            BoardDi_Read16(BOARD_DI_FPGA_INPUT_ADDR) &
            BOARD_DI_INPUT_MASK;
        gBoardDiFpgaSnapshot.firstRawValue = previousRawValue;
        observedInactiveMask |= previousRawValue;
        observedActiveMask |=
            (BoardTest_U16)(~previousRawValue) & BOARD_DI_INPUT_MASK;
        sampleCount = 1U;

        while((sampleCount < BOARD_DI_SAMPLE_LIMIT) &&
              (((observedInactiveMask & expectedMask) != expectedMask) ||
               ((observedActiveMask & expectedMask) != expectedMask) ||
               ((transitionMask & expectedMask) != expectedMask)))
        {
            DELAY_US(BOARD_DI_SAMPLE_INTERVAL_US);
            rawValue = BoardDi_Read16(BOARD_DI_FPGA_INPUT_ADDR) &
                       BOARD_DI_INPUT_MASK;
            observedInactiveMask |= rawValue;
            observedActiveMask |=
                (BoardTest_U16)(~rawValue) & BOARD_DI_INPUT_MASK;
            transitionMask |= rawValue ^ previousRawValue;
            previousRawValue = rawValue;
            sampleCount++;
        }

        gBoardDiFpgaSnapshot.lastRawValue = previousRawValue;

        if((observedInactiveMask & expectedMask) == expectedMask)
        {
            statusMask |= BOARD_DI_STATUS_INACTIVE_SEEN;
        }

        if((observedActiveMask & expectedMask) == expectedMask)
        {
            statusMask |= BOARD_DI_STATUS_ACTIVE_SEEN;
        }

        if((transitionMask & expectedMask) == expectedMask)
        {
            statusMask |= BOARD_DI_STATUS_TRANSITION_SEEN;
        }

        if((statusMask & BOARD_DI_REQUIRED_STATUS_MASK) !=
           BOARD_DI_REQUIRED_STATUS_MASK)
        {
            failCode = BOARD_DI_FAIL_TIMEOUT;
        }
    }

    gBoardDiFpgaSnapshot.statusMask = statusMask;
    gBoardDiFpgaSnapshot.failCode = failCode;
    gBoardDiFpgaSnapshot.observedInactiveMask = observedInactiveMask;
    gBoardDiFpgaSnapshot.observedActiveMask = observedActiveMask;
    gBoardDiFpgaSnapshot.transitionMask = transitionMask;
    gBoardDiFpgaSnapshot.sampleCount = sampleCount;

    return BoardDi_EvaluateFpgaStatus(statusMask,
                                      failCode,
                                      expectedMask,
                                      observedInactiveMask,
                                      observedActiveMask,
                                      transitionMask,
                                      record);
}

static BoardTest_U16 BoardDi_PrepareMappedInputs(
    const BoardTest_U16 *pinMap,
    BoardTest_U16 pinCount,
    BoardTest_U16 channelMask)
{
    BoardTest_U16 channelBit;
    BoardTest_U16 index;
    BoardTest_U16 pin;

    channelBit = 1U;
    for(index = 0U; index < pinCount; index++)
    {
        if((channelMask & channelBit) != 0U)
        {
            pin = pinMap[index];
            if(pin == BOARD_PROFILE_PIN_UNUSED)
            {
                return 0U;
            }
            GPIO_SetupPinMux(pin, GPIO_MUX_CPU1, 0U);
            GPIO_SetupPinOptions(pin, GPIO_INPUT,
                                 GPIO_PULLUP | GPIO_ASYNC);
        }
        channelBit <<= 1U;
    }
    return 1U;
}

static BoardTest_U16 BoardDi_ReadMappedInputs(
    const BoardTest_U16 *pinMap,
    BoardTest_U16 pinCount,
    BoardTest_U16 channelMask)
{
    BoardTest_U16 channelBit;
    BoardTest_U16 index;
    BoardTest_U16 pin;
    BoardTest_U16 rawValue;

    rawValue = 0U;
    channelBit = 1U;
    for(index = 0U; index < pinCount; index++)
    {
        if((channelMask & channelBit) != 0U)
        {
            pin = pinMap[index];
            if(GPIO_ReadPin(pin) != 0U)
            {
                rawValue |= channelBit;
            }
        }
        channelBit <<= 1U;
    }
    return rawValue;
}

static BoardTest_Result BoardDi_RunMappedDspExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 inputMask,
    const BoardTest_U16 *pinMap,
    BoardTest_U16 pinCount,
    BoardTest_U16 activeHigh,
    volatile BoardDi_DspSnapshot *snapshot,
    BoardTest_U16 errorCode,
    BoardTest_U16 acceptAnyChannel)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 expectedMask;
    BoardTest_U16 rawValue;
    BoardTest_U16 previousRawValue;
    BoardTest_U16 observedInactiveMask;
    BoardTest_U16 observedActiveMask;
    BoardTest_U16 transitionMask;
    BoardTest_U16 sampleCount;
    BoardTest_U16 validatedMask;

    statusMask = 0U;
    failCode = BOARD_DI_FAIL_NONE;
    expectedMask = requestedChannelMask & inputMask;
    observedInactiveMask = 0U;
    observedActiveMask = 0U;
    transitionMask = 0U;
    sampleCount = 0U;

    snapshot->statusMask = 0U;
    snapshot->failCode = BOARD_DI_FAIL_NONE;
    snapshot->requestedChannelMask = expectedMask;
    snapshot->firstRawValue = 0U;
    snapshot->lastRawValue = 0U;
    snapshot->observedInactiveMask = 0U;
    snapshot->observedActiveMask = 0U;
    snapshot->transitionMask = 0U;
    snapshot->sampleCount = 0U;

    if((expectedMask == 0U) ||
       ((requestedChannelMask & ~inputMask) != 0U))
    {
        failCode = BOARD_DI_FAIL_CHANNEL_MASK;
    }
    else if((pinMap == 0) ||
            (BoardDi_PrepareMappedInputs(
                 pinMap, pinCount, expectedMask) == 0U))
    {
        failCode = BOARD_DI_FAIL_CONFIG;
    }
    else
    {
        statusMask |= BOARD_DI_STATUS_CONFIGURED;
    }

    if(failCode == BOARD_DI_FAIL_NONE)
    {
        previousRawValue = BoardDi_ReadMappedInputs(
            pinMap, pinCount, expectedMask);
        snapshot->firstRawValue = previousRawValue;
        if(activeHigh != 0U)
        {
            observedActiveMask |= previousRawValue;
            observedInactiveMask |=
                (BoardTest_U16)(~previousRawValue) & expectedMask;
        }
        else
        {
            observedInactiveMask |= previousRawValue;
            observedActiveMask |=
                (BoardTest_U16)(~previousRawValue) & expectedMask;
        }
        sampleCount = 1U;

        while((sampleCount < BOARD_DI_SAMPLE_LIMIT) &&
              (BoardDi_CaptureComplete(expectedMask,
                                       observedInactiveMask,
                                       observedActiveMask,
                                       transitionMask,
                                       acceptAnyChannel) == 0U))
        {
            DELAY_US(BOARD_DI_SAMPLE_INTERVAL_US);
            rawValue = BoardDi_ReadMappedInputs(
                pinMap, pinCount, expectedMask);
            if(activeHigh != 0U)
            {
                observedActiveMask |= rawValue;
                observedInactiveMask |=
                    (BoardTest_U16)(~rawValue) & expectedMask;
            }
            else
            {
                observedInactiveMask |= rawValue;
                observedActiveMask |=
                    (BoardTest_U16)(~rawValue) & expectedMask;
            }
            transitionMask |= rawValue ^ previousRawValue;
            previousRawValue = rawValue;
            sampleCount++;
        }

        snapshot->lastRawValue = previousRawValue;
        // Driver-fault DI recognizes whichever input completes a transition;
        // ordinary DI/protection tests still require every requested input.
        validatedMask = (acceptAnyChannel != 0U) ?
            expectedMask & observedInactiveMask & observedActiveMask &
            transitionMask : expectedMask;
        if((validatedMask != 0U) &&
           ((observedInactiveMask & validatedMask) == validatedMask))
        {
            statusMask |= BOARD_DI_STATUS_INACTIVE_SEEN;
        }
        if((validatedMask != 0U) &&
           ((observedActiveMask & validatedMask) == validatedMask))
        {
            statusMask |= BOARD_DI_STATUS_ACTIVE_SEEN;
        }
        if((validatedMask != 0U) &&
           ((transitionMask & validatedMask) == validatedMask))
        {
            statusMask |= BOARD_DI_STATUS_TRANSITION_SEEN;
        }
        if((statusMask & BOARD_DI_DSP_REQUIRED_STATUS_MASK) !=
           BOARD_DI_DSP_REQUIRED_STATUS_MASK)
        {
            failCode = BOARD_DI_FAIL_TIMEOUT;
        }
    }

    snapshot->statusMask = statusMask;
    snapshot->failCode = failCode;
    snapshot->observedInactiveMask = observedInactiveMask;
    snapshot->observedActiveMask = observedActiveMask;
    snapshot->transitionMask = transitionMask;
    snapshot->sampleCount = sampleCount;

    return BoardDi_EvaluateMappedDspStatus(
        statusMask,
        failCode,
        expectedMask,
        inputMask,
        observedInactiveMask,
        observedActiveMask,
        transitionMask,
        errorCode,
        acceptAnyChannel,
        record);
}

BoardTest_Result BoardDi_RunDspExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 requestedChannelMask)
{
    const BoardProfile_HardwareDescriptor *hardware;
    BoardTest_U16 pinMap[3];

    hardware = BoardProfile_GetCurrentHardware();
    if(hardware == 0)
    {
        pinMap[0] = BOARD_PROFILE_PIN_UNUSED;
        pinMap[1] = BOARD_PROFILE_PIN_UNUSED;
        pinMap[2] = BOARD_PROFILE_PIN_UNUSED;
    }
    else
    {
        pinMap[0] = hardware->pins.dspDi1;
        pinMap[1] = hardware->pins.dspDi2;
        pinMap[2] = hardware->pins.dspDi3;
    }

    return BoardDi_RunMappedDspExternalTest(
        record,
        requestedChannelMask,
        BOARD_DI_DSP_INPUT_MASK,
        pinMap,
        3U,
        0U,
        &gBoardDiDspSnapshot,
        BOARD_TEST_ERROR_DI_EXTERNAL,
        0U);
}

BoardTest_Result BoardDi_RunDriverFaultExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 requestedChannelMask)
{
    const BoardProfile_HardwareDescriptor *hardware;
    BoardTest_U16 pinMap[BOARD_PROFILE_DRIVER_FAULT_DI_COUNT];
    BoardTest_U16 index;

    hardware = BoardProfile_GetCurrentHardware();
    for(index = 0U; index < BOARD_PROFILE_DRIVER_FAULT_DI_COUNT; index++)
    {
        pinMap[index] = (hardware == 0) ? BOARD_PROFILE_PIN_UNUSED :
                        hardware->pins.driverFaultInputs[index];
    }

    return BoardDi_RunMappedDspExternalTest(
        record,
        requestedChannelMask,
        BOARD_DI_DRIVER_FAULT_INPUT_MASK,
        pinMap,
        BOARD_PROFILE_DRIVER_FAULT_DI_COUNT,
        1U,
        &gBoardDiDriverFaultSnapshot,
        BOARD_TEST_ERROR_DRIVER_FAULT_DI_EXTERNAL,
        0U);
}

BoardTest_Result BoardDi_RunProtectionFaultExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 requestedChannelMask)
{
    const BoardProfile_HardwareDescriptor *hardware;
    BoardTest_U16 pinMap[BOARD_PROFILE_PROTECTION_FAULT_DI_COUNT];
    BoardTest_U16 index;

    hardware = BoardProfile_GetCurrentHardware();
    for(index = 0U;
        index < BOARD_PROFILE_PROTECTION_FAULT_DI_COUNT;
        index++)
    {
        pinMap[index] = (hardware == 0) ? BOARD_PROFILE_PIN_UNUSED :
                        hardware->pins.protectionFaultInputs[index];
    }

    return BoardDi_RunMappedDspExternalTest(
        record,
        requestedChannelMask,
        BOARD_DI_PROTECTION_FAULT_INPUT_MASK,
        pinMap,
        BOARD_PROFILE_PROTECTION_FAULT_DI_COUNT,
        0U,
        &gBoardDiProtectionFaultSnapshot,
        BOARD_TEST_ERROR_PROTECTION_FAULT_DI_EXTERNAL,
        0U);
}
#endif
