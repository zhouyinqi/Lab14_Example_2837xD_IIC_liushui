#include "Board_Di_Test.h"
#include "Board_Fpga_Test.h"
#include "Board_Profile.h"

volatile BoardDi_DspSnapshot gBoardDiLowVoltageSnapshot = {0U};

BoardTest_U16 BoardDi_LowVoltageSelectionMask(BoardTest_U16 testId,
                                             BoardTest_U16 selection)
{
    BoardTest_U16 count;
    if(testId == BOARD_TEST_ID_LV_DI_EXTERNAL) count = 6U;
    else if(testId == BOARD_TEST_ID_LV_STO_EXTERNAL) count = 2U;
    else return 0U;
    if(selection > count) return 0U;
    return (selection == 0U) ? (BoardTest_U16)((1U << count) - 1U) :
        (BoardTest_U16)(1U << (selection - 1U));
}

void BoardDi_InitLowVoltageCapture(BoardDi_LowVoltageCapture *capture,
                                  BoardTest_U16 mask)
{
    capture->expectedMask = mask;
    capture->lowSeen = 0U;
    capture->highSeen = 0U;
    capture->transitions = 0U;
    capture->previous = 0U;
    capture->sampled = 0U;
}

BoardTest_Result BoardDi_UpdateLowVoltageCapture(BoardDi_LowVoltageCapture *capture,
    BoardTest_U16 currentMask, BoardTest_U32 elapsedMs, BoardTest_Record *record)
{
    BoardTest_U16 mask = capture->expectedMask;
    currentMask &= mask;
    if((mask == 0U) || ((mask & ~0x003FU) != 0U))
    {
        record->errorCode = BOARD_TEST_ERROR_PROFILE_PINMAP;
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }
    /* Samples arriving after the deadline cannot turn TIMEOUT into PASS. */
    if(elapsedMs <= BOARD_DI_LV_TIMEOUT_MS)
    {
        capture->lowSeen |= (BoardTest_U16)(~currentMask & mask);
        capture->highSeen |= currentMask;
        if(capture->sampled != 0U)
            capture->transitions |= capture->previous ^ currentMask;
        capture->previous = currentMask;
        capture->sampled = 1U;
    }
    record->rawValue = ((BoardTest_U32)currentMask << 24U) |
        ((BoardTest_U32)capture->highSeen << 16U) | capture->lowSeen;
    record->measuredValue = (float)capture->transitions;
    record->expectedMin = (float)mask;
    record->expectedMax = (float)mask;
    record->errorCode = BOARD_TEST_ERROR_NONE;
    if((capture->lowSeen & capture->highSeen & capture->transitions & mask) == mask)
        return BOARD_TEST_RESULT_PASS;
    if(elapsedMs >= BOARD_DI_LV_TIMEOUT_MS)
    {
        record->errorCode = BOARD_TEST_ERROR_DI_EXTERNAL;
        return BOARD_TEST_RESULT_TIMEOUT;
    }
    return BOARD_TEST_RESULT_RUNNING;
}

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"

static BoardDi_LowVoltageCapture BoardDi_LvCapture;
static BoardTest_U16 BoardDi_LvActive = 0U;
static BoardTest_U16 BoardDi_LvTestId;
static BoardTest_U32 BoardDi_LvStart;
static Uint32 BoardDi_LvOldPeriod, BoardDi_LvOldCounter;
static Uint16 BoardDi_LvOldTcr, BoardDi_LvOldTpr, BoardDi_LvOldTprh;
static BoardTest_U16 BoardDi_LvOutputPin = BOARD_PROFILE_PIN_UNUSED;
static BoardTest_U16 BoardDi_LvOutputSeen;

static void BoardDi_StartLvTimer(void)
{
    BoardDi_LvOldPeriod = CpuTimer2Regs.PRD.all;
    BoardDi_LvOldCounter = CpuTimer2Regs.TIM.all;
    BoardDi_LvOldTcr = CpuTimer2Regs.TCR.all;
    BoardDi_LvOldTpr = CpuTimer2Regs.TPR.all;
    BoardDi_LvOldTprh = CpuTimer2Regs.TPRH.all;
    CpuTimer2Regs.TCR.bit.TSS = 1U;
    CpuTimer2Regs.PRD.all = 0xFFFFFFFFUL;
    CpuTimer2Regs.TPR.all = 199U;
    CpuTimer2Regs.TPRH.all = 0U;
    CpuTimer2Regs.TCR.bit.TIE = 0U;
    CpuTimer2Regs.TCR.bit.FREE = 1U;
    CpuTimer2Regs.TCR.bit.TRB = 1U;
    CpuTimer2Regs.TCR.bit.TSS = 0U;
    BoardDi_LvStart = CpuTimer2Regs.TIM.all;
    BoardDi_LvActive = 1U;
}

void BoardDi_AbortLowVoltageInputTest(void)
{
    if(BoardDi_LvActive == 0U) return;
    if(BoardDi_LvOutputPin != BOARD_PROFILE_PIN_UNUSED)
    {
        GPIO_WritePin(BoardDi_LvOutputPin, 0U);
        BoardDi_LvOutputPin = BOARD_PROFILE_PIN_UNUSED;
    }
    CpuTimer2Regs.TCR.bit.TSS = 1U;
    CpuTimer2Regs.PRD.all = BoardDi_LvOldPeriod;
    CpuTimer2Regs.TIM.all = BoardDi_LvOldCounter;
    CpuTimer2Regs.TPR.all = BoardDi_LvOldTpr;
    CpuTimer2Regs.TPRH.all = BoardDi_LvOldTprh;
    CpuTimer2Regs.TCR.all = BoardDi_LvOldTcr;
    BoardDi_LvActive = 0U;
}

BoardTest_Result BoardDi_RunLowVoltageOutputTest(BoardTest_U16 testId,
    BoardTest_U16 selection, BoardTest_U16 armKey, BoardTest_Record *record)
{
    const BoardProfile_HardwareDescriptor *h = BoardProfile_GetCurrentHardware();
    BoardTest_U16 pin, level;
    BoardTest_U32 elapsed;
    if((armKey != 0xD012U) || !BoardProfile_IsConfirmed() || (h == 0) ||
       h->boardId != BOARD_PROFILE_ID_LOW_VOLTAGE_INVERTER || h->lowVoltagePins == 0)
    {
        BoardDi_AbortLowVoltageInputTest();
        return BOARD_TEST_RESULT_SAFETY_LOCKED;
    }
    if(testId == BOARD_TEST_ID_LV_DO_EXTERNAL && selection >= 1U && selection <= 5U)
        pin = selection <= 3U ? h->lowVoltagePins->digitalOutputs[selection-1U] :
            (selection == 4U ? h->lowVoltagePins->softStartOutput : h->lowVoltagePins->fanOutput);
    else if(testId == BOARD_TEST_ID_LV_HDO_EXTERNAL && selection == 1U)
        pin = h->lowVoltagePins->highCurrentOutput;
    else
    {
        BoardDi_AbortLowVoltageInputTest();
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }
    if(BoardDi_LvActive == 0U)
    {
        /* V04 sheet: ENPWM_SFT high locks power PWM. */
        GPIO_WritePin(91U, 1U);
        GPIO_SetupPinMux(91U, GPIO_MUX_CPU1, 0U);
        GPIO_SetupPinOptions(91U, GPIO_OUTPUT, GPIO_PUSHPULL);
        GPIO_WritePin(pin, 0U);
        GPIO_SetupPinMux(pin, GPIO_MUX_CPU1, 0U);
        GPIO_SetupPinOptions(pin, GPIO_OUTPUT, GPIO_PUSHPULL);
        BoardDi_LvOutputPin = pin;
        BoardDi_LvOutputSeen = 0U;
        BoardDi_LvTestId = testId;
        BoardDi_StartLvTimer();
    }
    if(BoardDi_LvOutputPin != pin || BoardDi_LvTestId != testId)
    {
        BoardDi_AbortLowVoltageInputTest();
        record->errorCode = BOARD_TEST_ERROR_ABORTED;
        return BOARD_TEST_RESULT_FAIL;
    }
    elapsed = ((BoardDi_LvStart - CpuTimer2Regs.TIM.all) & 0xFFFFFFFFUL) / 1000UL;
    level = elapsed >= 1000UL && elapsed < 3000UL ? 1U : 0U;
    GPIO_WritePin(pin, level);
    record->rawValue = ((BoardTest_U32)pin << 16U) | GPIO_ReadPin(pin);
    record->measuredValue = (float)level;
    record->expectedMin = 0.0F;
    record->expectedMax = 1.0F;
    record->errorCode = BOARD_TEST_ERROR_NONE;
    if(GPIO_ReadPin(pin) != level)
    {
        BoardDi_AbortLowVoltageInputTest();
        record->errorCode = BOARD_TEST_ERROR_PROFILE_PINMAP;
        return BOARD_TEST_RESULT_FAIL;
    }
    BoardDi_LvOutputSeen |= (1U << level);
    if(elapsed < 4000UL) return BOARD_TEST_RESULT_RUNNING;
    BoardDi_AbortLowVoltageInputTest();
    /* GPIO readback cannot certify the relay/terminal. */
    return BoardDi_LvOutputSeen == 3U ? BOARD_TEST_RESULT_WARN : BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardDi_RunLowVoltageInputTest(BoardTest_U16 testId,
    BoardTest_U16 selection, BoardTest_Record *record)
{
    const BoardProfile_HardwareDescriptor *hardware;
    const BoardTest_U16 *pins;
    BoardTest_U16 mask, count, i, currentMask;
    BoardTest_U32 elapsedMs;
    BoardTest_Result result;
    hardware = BoardProfile_GetCurrentHardware();
    mask = BoardDi_LowVoltageSelectionMask(testId, selection);
    if((BoardProfile_IsConfirmed() == 0U) || (hardware == 0) ||
       (hardware->boardId != BOARD_PROFILE_ID_LOW_VOLTAGE_INVERTER) ||
       (hardware->hardwareRevision != BOARD_PROFILE_HARDWARE_REVISION_LOW_VOLTAGE_V04) ||
       (hardware->lowVoltagePins == 0) || (mask == 0U))
    {
        BoardDi_AbortLowVoltageInputTest();
        record->errorCode = BOARD_TEST_ERROR_PROFILE_PINMAP;
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }
    pins = (testId == BOARD_TEST_ID_LV_DI_EXTERNAL) ?
        hardware->lowVoltagePins->digitalInputs : hardware->lowVoltagePins->stoInputs;
    count = (testId == BOARD_TEST_ID_LV_DI_EXTERNAL) ? 6U : 2U;
    if(BoardDi_LvActive == 0U)
    {
        for(i = 0U; i < count; i++)
        {
            if((mask & (1U << i)) == 0U) continue;
            if(pins[i] == BOARD_PROFILE_PIN_UNUSED)
            {
                record->errorCode = BOARD_TEST_ERROR_PROFILE_PINMAP;
                return BOARD_TEST_RESULT_NOT_SUPPORTED;
            }
            GPIO_SetupPinMux(pins[i], GPIO_MUX_CPU1, 0U);
            GPIO_SetupPinOptions(pins[i], GPIO_INPUT, GPIO_ASYNC);
        }
        BoardDi_InitLowVoltageCapture(&BoardDi_LvCapture, mask);
        gBoardDiLowVoltageSnapshot.statusMask = BOARD_DI_STATUS_CONFIGURED;
        gBoardDiLowVoltageSnapshot.failCode = BOARD_DI_FAIL_NONE;
        gBoardDiLowVoltageSnapshot.requestedChannelMask = mask;
        gBoardDiLowVoltageSnapshot.observedInactiveMask = 0U;
        gBoardDiLowVoltageSnapshot.observedActiveMask = 0U;
        gBoardDiLowVoltageSnapshot.transitionMask = 0U;
        gBoardDiLowVoltageSnapshot.sampleCount = 0U;
        BoardDi_LvOldPeriod = CpuTimer2Regs.PRD.all;
        BoardDi_LvOldCounter = CpuTimer2Regs.TIM.all;
        BoardDi_LvOldTcr = CpuTimer2Regs.TCR.all;
        BoardDi_LvOldTpr = CpuTimer2Regs.TPR.all;
        BoardDi_LvOldTprh = CpuTimer2Regs.TPRH.all;
        CpuTimer2Regs.TCR.bit.TSS = 1U;
        CpuTimer2Regs.PRD.all = 0xFFFFFFFFUL;
        CpuTimer2Regs.TPR.all = 199U;
        CpuTimer2Regs.TPRH.all = 0U;
        CpuTimer2Regs.TCR.bit.TIE = 0U;
        CpuTimer2Regs.TCR.bit.FREE = 1U;
        CpuTimer2Regs.TCR.bit.TRB = 1U;
        CpuTimer2Regs.TCR.bit.TSS = 0U;
        BoardDi_LvStart = CpuTimer2Regs.TIM.all;
        BoardDi_LvTestId = testId;
        BoardDi_LvActive = 1U;
    }
    if((BoardDi_LvTestId != testId) || (BoardDi_LvCapture.expectedMask != mask))
    {
        BoardDi_AbortLowVoltageInputTest();
        record->errorCode = BOARD_TEST_ERROR_ABORTED;
        return BOARD_TEST_RESULT_FAIL;
    }
    currentMask = 0U;
    for(i = 0U; i < count; i++)
        if(((mask & (1U << i)) != 0U) && (GPIO_ReadPin(pins[i]) != 0U))
            currentMask |= (1U << i);
    elapsedMs = ((BoardDi_LvStart - CpuTimer2Regs.TIM.all) & 0xFFFFFFFFUL) / 1000UL;
    if(gBoardDiLowVoltageSnapshot.sampleCount == 0U)
        gBoardDiLowVoltageSnapshot.firstRawValue = currentMask;
    if(gBoardDiLowVoltageSnapshot.sampleCount < 0xFFFFU)
        gBoardDiLowVoltageSnapshot.sampleCount++;
    result = BoardDi_UpdateLowVoltageCapture(&BoardDi_LvCapture, currentMask, elapsedMs, record);
    gBoardDiLowVoltageSnapshot.lastRawValue = currentMask;
    gBoardDiLowVoltageSnapshot.observedInactiveMask = BoardDi_LvCapture.lowSeen;
    gBoardDiLowVoltageSnapshot.observedActiveMask = BoardDi_LvCapture.highSeen;
    gBoardDiLowVoltageSnapshot.transitionMask = BoardDi_LvCapture.transitions;
    if(result == BOARD_TEST_RESULT_PASS)
        gBoardDiLowVoltageSnapshot.statusMask = BOARD_DI_DSP_REQUIRED_STATUS_MASK;
    if(result == BOARD_TEST_RESULT_TIMEOUT)
        gBoardDiLowVoltageSnapshot.failCode = BOARD_DI_FAIL_TIMEOUT;
    if(result != BOARD_TEST_RESULT_RUNNING) BoardDi_AbortLowVoltageInputTest();
    return result;
}
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
