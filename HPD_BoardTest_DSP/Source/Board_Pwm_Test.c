#include "Board_Pwm_Test.h"
#include "Board_Pinmap.h"
#include "Board_Profile.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#include "F2837xD_EPwm_defines.h"
#endif

volatile BoardPwm_SafetySnapshot gBoardPwmSafetySnapshot =
{
    0U,
    0U,
    0U,
    0U,
    0U
};

volatile BoardPwm_DidoSnapshot gBoardPwmDidoSnapshot =
{
    0U,
    BOARD_PWM_DIDO_FAIL_NONE,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    BOARD_PWM_DIDO_PHASE_IDLE,
    0U,
    0U
};

BoardTest_Result BoardPwm_EvaluateSafetyStatus(BoardTest_U16 statusMask,
                                               BoardTest_U16 checkedPairs,
                                               BoardTest_U16 failedChannel,
                                               BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       (((BoardTest_U32)checkedPairs & 0x00FFUL) << 8U) |
                       ((BoardTest_U32)failedChannel & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_PWM_SAFE_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_PWM_SAFE_DIAGNOSTIC_MASK;

    if(((statusMask & BOARD_PWM_SAFE_REQUIRED_MASK) ==
        BOARD_PWM_SAFE_REQUIRED_MASK) &&
       (checkedPairs == BOARD_EPWM_PAIR_COUNT) &&
       (failedChannel == 0U))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_PWM_SAFE_CFG;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardPwm_EvaluateDidoStatus(BoardTest_U16 statusMask,
                                             BoardTest_U16 failCode,
                                             BoardTest_U16 toggledOutputMask,
                                             BoardTest_U16 expectedOutputMask,
                                             BoardTest_Record *record)
{
    record->rawValue =
        (((BoardTest_U32)toggledOutputMask & 0xFFFFUL) << 16U) |
        ((BoardTest_U32)statusMask & 0xFFFFUL);
    record->measuredValue = (float)toggledOutputMask;
    record->expectedMin = (float)expectedOutputMask;
    record->expectedMax = (float)expectedOutputMask;

    if(failCode == BOARD_PWM_DIDO_FAIL_NOT_ARMED)
    {
        record->errorCode = BOARD_TEST_ERROR_SAFETY_LOCK;
        return BOARD_TEST_RESULT_SAFETY_LOCKED;
    }

    if((failCode == BOARD_PWM_DIDO_FAIL_NONE) &&
       ((statusMask & BOARD_PWM_DIDO_REQUIRED_MASK) ==
        BOARD_PWM_DIDO_REQUIRED_MASK) &&
       (toggledOutputMask == expectedOutputMask))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_WARN;
    }

    record->errorCode = (failCode == BOARD_PWM_DIDO_FAIL_PROFILE) ?
        BOARD_TEST_ERROR_PROFILE_PINMAP :
        BOARD_TEST_ERROR_PWM_DIDO_EXTERNAL;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST

#define BOARD_PWM_FORCE_LOW_READBACK_INVALID 0xFFFFU
#define BOARD_PWM_FORCE_LOW_EXPECTED_READBACK \
    ((BoardTest_U16)AQ_CLEAR |                \
     ((BoardTest_U16)AQ_CLEAR << 2U) |        \
     ((BoardTest_U16)TZ_FORCE_LO << 4U) |     \
     ((BoardTest_U16)TZ_FORCE_LO << 6U))

typedef struct
{
    BoardTest_U16 active;
    BoardTest_U16 phase;
    BoardTest_U16 outputMask;
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 currentOutputIndex;
    BoardTest_U16 currentEnableIndex;
    BoardTest_U32 elapsedUs;
    BoardTest_U16 timerConfigured;
    BoardTest_U32 phaseStartCounter;
    BoardTest_U32 previousTimerPeriod;
    BoardTest_U32 previousTimerCounter;
    BoardTest_U16 previousTimerTcr;
    BoardTest_U16 previousTimerTpr;
    BoardTest_U16 previousTimerTprh;
} BoardPwm_DidoRunState;

static BoardPwm_DidoRunState BoardPwm_DidoRun =
{
    0U,
    BOARD_PWM_DIDO_PHASE_IDLE,
    0U,
    0U,
    BOARD_PWM_DIDO_FAIL_NONE,
    0U,
    0U,
    0UL,
    0U,
    0UL,
    0UL,
    0UL,
    0U,
    0U,
    0U
};

static BoardTest_U16 BoardPwm_DidoPinsAreValid(
    const BoardProfile_PinMap *pins)
{
    BoardTest_U16 index;

    for(index = 0U; index < BOARD_PROFILE_PWM_DIDO_OUTPUT_COUNT; index++)
    {
        if(pins->pwmDidoOutputs[index] == BOARD_PROFILE_PIN_UNUSED)
        {
            return 0U;
        }
    }
    for(index = 0U; index < BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT; index++)
    {
        if((pins->pwmDidoEnables[index] == BOARD_PROFILE_PIN_UNUSED) ||
           (pins->pwmDidoPowerEnables[index] == BOARD_PROFILE_PIN_UNUSED))
        {
            return 0U;
        }
    }
    return 1U;
}

static void BoardPwm_ConfigureDidoOutputLow(BoardTest_U16 pin)
{
    GPIO_WritePin(pin, 0U);
    GPIO_SetupPinMux(pin, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinOptions(pin, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_WritePin(pin, 0U);
}

static void BoardPwm_ForceDidoPinsLow(const BoardProfile_PinMap *pins)
{
    BoardTest_U16 index;

    for(index = 0U; index < BOARD_PROFILE_PWM_DIDO_OUTPUT_COUNT; index++)
    {
        if(pins->pwmDidoOutputs[index] != BOARD_PROFILE_PIN_UNUSED)
        {
            GPIO_WritePin(pins->pwmDidoOutputs[index], 0U);
        }
    }
    for(index = 0U; index < BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT; index++)
    {
        if(pins->pwmDidoEnables[index] != BOARD_PROFILE_PIN_UNUSED)
        {
            GPIO_WritePin(pins->pwmDidoEnables[index], 0U);
        }
        if(pins->pwmDidoPowerEnables[index] != BOARD_PROFILE_PIN_UNUSED)
        {
            GPIO_WritePin(pins->pwmDidoPowerEnables[index], 0U);
        }
    }
}

static void BoardPwm_ConfigureDidoTimer(void)
{
    BoardPwm_DidoRun.previousTimerPeriod = CpuTimer2Regs.PRD.all;
    BoardPwm_DidoRun.previousTimerCounter = CpuTimer2Regs.TIM.all;
    BoardPwm_DidoRun.previousTimerTcr = CpuTimer2Regs.TCR.all;
    BoardPwm_DidoRun.previousTimerTpr = CpuTimer2Regs.TPR.all;
    BoardPwm_DidoRun.previousTimerTprh = CpuTimer2Regs.TPRH.all;

    CpuTimer2Regs.TCR.bit.TSS = 1U;
    CpuTimer2Regs.PRD.all = 0xFFFFFFFFUL;
    CpuTimer2Regs.TPR.all = BOARD_PWM_DIDO_TIMER_DIVIDER;
    CpuTimer2Regs.TPRH.all = 0U;
    CpuTimer2Regs.TCR.bit.TIE = 0U;
    CpuTimer2Regs.TCR.bit.SOFT = 1U;
    CpuTimer2Regs.TCR.bit.FREE = 1U;
    CpuTimer2Regs.TCR.bit.TRB = 1U;
    CpuTimer2Regs.TCR.bit.TSS = 0U;
    BoardPwm_DidoRun.timerConfigured = 1U;
    BoardPwm_DidoRun.phaseStartCounter = CpuTimer2Regs.TIM.all;
}

static void BoardPwm_RestoreDidoTimer(void)
{
    if(BoardPwm_DidoRun.timerConfigured == 0U)
    {
        return;
    }

    CpuTimer2Regs.TCR.bit.TSS = 1U;
    CpuTimer2Regs.PRD.all = BoardPwm_DidoRun.previousTimerPeriod;
    CpuTimer2Regs.TIM.all = BoardPwm_DidoRun.previousTimerCounter;
    CpuTimer2Regs.TPR.all = BoardPwm_DidoRun.previousTimerTpr;
    CpuTimer2Regs.TPRH.all = BoardPwm_DidoRun.previousTimerTprh;
    CpuTimer2Regs.TCR.all = BoardPwm_DidoRun.previousTimerTcr;
    BoardPwm_DidoRun.timerConfigured = 0U;
}

static void BoardPwm_BeginDidoPhase(BoardTest_U16 phase)
{
    BoardPwm_DidoRun.phase = phase;
    BoardPwm_DidoRun.elapsedUs = 0UL;
    BoardPwm_DidoRun.phaseStartCounter = CpuTimer2Regs.TIM.all;
}

static void BoardPwm_UpdateDidoElapsed(void)
{
    BoardPwm_DidoRun.elapsedUs =
        BoardPwm_DidoRun.phaseStartCounter - CpuTimer2Regs.TIM.all;
}

void BoardPwm_ForceDidoSafe(void)
{
    const BoardProfile_HardwareDescriptor *hardware;

    hardware = BoardProfile_GetCurrentHardware();
    if(hardware != 0)
    {
        BoardPwm_ForceDidoPinsLow(&hardware->pins);
    }
    BoardPwm_RestoreDidoTimer();
    BoardPwm_DidoRun.active = 0U;
    BoardPwm_DidoRun.phase = BOARD_PWM_DIDO_PHASE_IDLE;
    BoardPwm_DidoRun.elapsedUs = 0UL;
    gBoardPwmDidoSnapshot.activeOutputIndex = 0U;
    gBoardPwmDidoSnapshot.phase = BOARD_PWM_DIDO_PHASE_IDLE;
    gBoardPwmDidoSnapshot.elapsedMilliseconds = 0U;
    gBoardPwmDidoSnapshot.liveOutputReadbackMask = 0U;
}

static BoardTest_U16 BoardPwm_ReadDidoMask(
    const BoardTest_U16 *pins,
    BoardTest_U16 count)
{
    BoardTest_U16 index;
    BoardTest_U16 mask;

    mask = 0U;
    for(index = 0U; index < count; index++)
    {
        if(GPIO_ReadPin(pins[index]) != 0U)
        {
            mask |= (BoardTest_U16)(1U << index);
        }
    }
    return mask;
}

static BoardTest_U16 BoardPwm_DidoEnableBaseForOutput(
    BoardTest_U16 outputIndex)
{
    if(outputIndex < BOARD_PWM_DIDO_OUTPUTS_PER_WINDING)
    {
        return 0U;
    }

    return 2U;
}

static BoardTest_U16 BoardPwm_PrepareDidoPins(
    const BoardProfile_PinMap *pins)
{
    BoardTest_U16 index;

    if(BoardPwm_DidoPinsAreValid(pins) == 0U)
    {
        return 0U;
    }

    for(index = 0U; index < BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT; index++)
    {
        BoardPwm_ConfigureDidoOutputLow(pins->pwmDidoEnables[index]);
        BoardPwm_ConfigureDidoOutputLow(pins->pwmDidoPowerEnables[index]);
    }
    for(index = 0U; index < BOARD_PROFILE_PWM_DIDO_OUTPUT_COUNT; index++)
    {
        BoardPwm_ConfigureDidoOutputLow(pins->pwmDidoOutputs[index]);
    }
    return 1U;
}

static volatile struct EPWM_REGS *BoardPwm_GetRegs(BoardTest_U16 channel)
{
    switch(channel)
    {
        case 1U:
            return &EPwm1Regs;

        case 2U:
            return &EPwm2Regs;

        case 3U:
            return &EPwm3Regs;

        case 4U:
            return &EPwm4Regs;

        case 5U:
            return &EPwm5Regs;

        case 6U:
            return &EPwm6Regs;

        case 7U:
            return &EPwm7Regs;

        case 8U:
            return &EPwm8Regs;

        case 9U:
            return &EPwm9Regs;

        case 10U:
            return &EPwm10Regs;

        case 11U:
            return &EPwm11Regs;

        case 12U:
            return &EPwm12Regs;

        default:
            return 0;
    }
}

static void BoardPwm_SelectCpu1AndEnableClocks(void)
{
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM1 = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM2 = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM3 = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM4 = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM5 = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM6 = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM7 = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM8 = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM9 = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM10 = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM11 = 0U;
    DevCfgRegs.CPUSEL0.bit.EPWM12 = 0U;
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1U;
    CpuSysRegs.PCLKCR2.bit.EPWM2 = 1U;
    CpuSysRegs.PCLKCR2.bit.EPWM3 = 1U;
    CpuSysRegs.PCLKCR2.bit.EPWM4 = 1U;
    CpuSysRegs.PCLKCR2.bit.EPWM5 = 1U;
    CpuSysRegs.PCLKCR2.bit.EPWM6 = 1U;
    CpuSysRegs.PCLKCR2.bit.EPWM7 = 1U;
    CpuSysRegs.PCLKCR2.bit.EPWM8 = 1U;
    CpuSysRegs.PCLKCR2.bit.EPWM9 = 1U;
    CpuSysRegs.PCLKCR2.bit.EPWM10 = 1U;
    CpuSysRegs.PCLKCR2.bit.EPWM11 = 1U;
    CpuSysRegs.PCLKCR2.bit.EPWM12 = 1U;
    EDIS;
}

static BoardTest_U16 BoardPwm_AreClocksEnabled(void)
{
    return (CpuSysRegs.PCLKCR2.bit.EPWM1 == 1U) &&
           (CpuSysRegs.PCLKCR2.bit.EPWM2 == 1U) &&
           (CpuSysRegs.PCLKCR2.bit.EPWM3 == 1U) &&
           (CpuSysRegs.PCLKCR2.bit.EPWM4 == 1U) &&
           (CpuSysRegs.PCLKCR2.bit.EPWM5 == 1U) &&
           (CpuSysRegs.PCLKCR2.bit.EPWM6 == 1U) &&
           (CpuSysRegs.PCLKCR2.bit.EPWM7 == 1U) &&
           (CpuSysRegs.PCLKCR2.bit.EPWM8 == 1U) &&
           (CpuSysRegs.PCLKCR2.bit.EPWM9 == 1U) &&
           (CpuSysRegs.PCLKCR2.bit.EPWM10 == 1U) &&
           (CpuSysRegs.PCLKCR2.bit.EPWM11 == 1U) &&
           (CpuSysRegs.PCLKCR2.bit.EPWM12 == 1U);
}

static BoardTest_U16 BoardPwm_IsCpu1Selected(void)
{
    return (DevCfgRegs.CPUSEL0.bit.EPWM1 == 0U) &&
           (DevCfgRegs.CPUSEL0.bit.EPWM2 == 0U) &&
           (DevCfgRegs.CPUSEL0.bit.EPWM3 == 0U) &&
           (DevCfgRegs.CPUSEL0.bit.EPWM4 == 0U) &&
           (DevCfgRegs.CPUSEL0.bit.EPWM5 == 0U) &&
           (DevCfgRegs.CPUSEL0.bit.EPWM6 == 0U) &&
           (DevCfgRegs.CPUSEL0.bit.EPWM7 == 0U) &&
           (DevCfgRegs.CPUSEL0.bit.EPWM8 == 0U) &&
           (DevCfgRegs.CPUSEL0.bit.EPWM9 == 0U) &&
           (DevCfgRegs.CPUSEL0.bit.EPWM10 == 0U) &&
           (DevCfgRegs.CPUSEL0.bit.EPWM11 == 0U) &&
           (DevCfgRegs.CPUSEL0.bit.EPWM12 == 0U);
}

static void BoardPwm_ConfigureSafeState(volatile struct EPWM_REGS *regs)
{
    EALLOW;
    regs->TBCTL.bit.CTRMODE = TB_FREEZE;
    regs->TBCTR = 0U;
    regs->TBPRD = 0U;
    regs->CMPA.all = 0UL;
    regs->CMPB.all = 0UL;
    regs->CMPC = 0U;
    regs->CMPD = 0U;
    regs->AQCTLA.all = 0U;
    regs->AQCTLA2.all = 0U;
    regs->AQCTLB.all = 0U;
    regs->AQCTLB2.all = 0U;
    regs->AQCSFRC.bit.CSFA = AQ_CLEAR;
    regs->AQCSFRC.bit.CSFB = AQ_CLEAR;
    regs->DBCTL.bit.OUT_MODE = DB_DISABLE;
    regs->DBRED = 0U;
    regs->DBFED = 0U;
    regs->PCCTL.bit.CHPEN = CHP_DISABLE;
    regs->TZCTL.bit.TZA = TZ_FORCE_LO;
    regs->TZCTL.bit.TZB = TZ_FORCE_LO;
    regs->TZEINT.all = 0U;
    regs->ETSEL.bit.INTEN = 0U;
    regs->ETSEL.bit.SOCAEN = 0U;
    regs->ETSEL.bit.SOCBEN = 0U;
    EDIS;
}

static BoardTest_U16 BoardPwm_ReadForceLowReadback(
    volatile struct EPWM_REGS *regs)
{
    return (BoardTest_U16)
        ((regs->AQCSFRC.bit.CSFA & 0x0003U) |
         ((regs->AQCSFRC.bit.CSFB & 0x0003U) << 2U) |
         ((regs->TZCTL.bit.TZA & 0x0003U) << 4U) |
         ((regs->TZCTL.bit.TZB & 0x0003U) << 6U));
}

static BoardTest_U16 BoardPwm_CheckSafeState(volatile struct EPWM_REGS *regs)
{
    BoardTest_U16 statusMask;

    statusMask = 0U;

    if(regs->TBCTL.bit.CTRMODE == TB_FREEZE)
    {
        statusMask |= BOARD_PWM_SAFE_COUNTERS_FROZEN;
    }

    if((regs->TBPRD == 0U) &&
       (regs->CMPA.all == 0UL) &&
       (regs->CMPB.all == 0UL) &&
       (regs->CMPC == 0U) &&
       (regs->CMPD == 0U))
    {
        statusMask |= BOARD_PWM_SAFE_COMPARES_ZERO;
    }

    if(BoardPwm_ReadForceLowReadback(regs) ==
       BOARD_PWM_FORCE_LOW_EXPECTED_READBACK)
    {
        statusMask |= BOARD_PWM_SAFE_FORCE_LOW;
    }

    if((regs->DBCTL.bit.OUT_MODE == DB_DISABLE) &&
       (regs->DBRED == 0U) &&
       (regs->DBFED == 0U) &&
       (regs->PCCTL.bit.CHPEN == CHP_DISABLE))
    {
        statusMask |= BOARD_PWM_SAFE_DEADBAND_DISABLED;
    }

    if((regs->ETSEL.bit.INTEN == 0U) &&
       (regs->ETSEL.bit.SOCAEN == 0U) &&
       (regs->ETSEL.bit.SOCBEN == 0U) &&
       (regs->TZEINT.all == 0U))
    {
        statusMask |= BOARD_PWM_SAFE_TRIGGERS_DISABLED;
    }

    return statusMask;
}

BoardTest_Result BoardPwm_RunSafetyConfigTest(BoardTest_Record *record)
{
    volatile struct EPWM_REGS *regs;
    BoardTest_U16 channel;
    BoardTest_U16 channelMask;
    BoardTest_U16 commonMask;
    BoardTest_U16 aggregateMask;
    BoardTest_U16 failedChannel;
    BoardTest_U16 firstFailureMask;
    BoardTest_U16 forceLowReadback;
    BoardTest_U16 checkedPairs;

    BoardPwm_SelectCpu1AndEnableClocks();

    commonMask = 0U;
    if(BoardPwm_AreClocksEnabled() != 0U)
    {
        commonMask |= BOARD_PWM_SAFE_CLOCKS_ENABLED;
    }

    if(BoardPwm_IsCpu1Selected() != 0U)
    {
        commonMask |= BOARD_PWM_SAFE_CPU1_SELECTED;
    }

    if(CpuSysRegs.PCLKCR0.bit.TBCLKSYNC == 0U)
    {
        commonMask |= BOARD_PWM_SAFE_TBCLK_STOPPED;
    }

    aggregateMask = BOARD_PWM_SAFE_REQUIRED_MASK;
    failedChannel = 0U;
    firstFailureMask = 0U;
    forceLowReadback = BOARD_PWM_FORCE_LOW_READBACK_INVALID;
    checkedPairs = 0U;

    for(channel = 1U; channel <= BOARD_EPWM_PAIR_COUNT; channel++)
    {
        regs = BoardPwm_GetRegs(channel);
        if(regs == 0)
        {
            if(failedChannel == 0U)
            {
                failedChannel = channel;
                firstFailureMask = BOARD_PWM_SAFE_REQUIRED_MASK;
            }
            continue;
        }

        BoardPwm_ConfigureSafeState(regs);
        channelMask = BoardPwm_CheckSafeState(regs);
        aggregateMask &= (BoardTest_U16)(commonMask | channelMask);
        checkedPairs++;

        if(((commonMask | channelMask) & BOARD_PWM_SAFE_REQUIRED_MASK) !=
           BOARD_PWM_SAFE_REQUIRED_MASK)
        {
            if(failedChannel == 0U)
            {
                failedChannel = channel;
                firstFailureMask =
                    (BoardTest_U16)(BOARD_PWM_SAFE_REQUIRED_MASK &
                                    ~(commonMask | channelMask));
                if((firstFailureMask & BOARD_PWM_SAFE_FORCE_LOW) != 0U)
                {
                    forceLowReadback = BoardPwm_ReadForceLowReadback(regs);
                }
            }
        }
    }

    gBoardPwmSafetySnapshot.statusMask = aggregateMask;
    gBoardPwmSafetySnapshot.checkedPairs = checkedPairs;
    gBoardPwmSafetySnapshot.failedChannel = failedChannel;
    gBoardPwmSafetySnapshot.firstFailureMask = firstFailureMask;
    if(forceLowReadback == BOARD_PWM_FORCE_LOW_READBACK_INVALID)
    {
        gBoardPwmSafetySnapshot.forceLowReadback =
            BOARD_PWM_FORCE_LOW_EXPECTED_READBACK;
    }
    else
    {
        gBoardPwmSafetySnapshot.forceLowReadback = forceLowReadback;
    }

    return BoardPwm_EvaluateSafetyStatus(aggregateMask,
                                         checkedPairs,
                                         failedChannel,
                                         record);
}

static BoardTest_Result BoardPwm_UpdateDidoRunningRecord(
    BoardTest_Record *record,
    const BoardProfile_PinMap *pins)
{
    BoardTest_U16 activeBit;
    BoardTest_U16 liveOutputMask;

    liveOutputMask = BoardPwm_ReadDidoMask(
        pins->pwmDidoOutputs,
        BOARD_PROFILE_PWM_DIDO_OUTPUT_COUNT);
    activeBit = (BoardTest_U16)(
        1U << BoardPwm_DidoRun.currentOutputIndex);

    gBoardPwmDidoSnapshot.statusMask = BoardPwm_DidoRun.statusMask;
    gBoardPwmDidoSnapshot.failCode = BoardPwm_DidoRun.failCode;
    gBoardPwmDidoSnapshot.phase = BoardPwm_DidoRun.phase;
    gBoardPwmDidoSnapshot.elapsedMilliseconds = (BoardTest_U16)(
        BoardPwm_DidoRun.elapsedUs / 1000UL);
    gBoardPwmDidoSnapshot.liveOutputReadbackMask = liveOutputMask;

    record->rawValue =
        (((BoardTest_U32)liveOutputMask & 0x0FFFUL) << 16U) |
        ((BoardTest_U32)BoardPwm_DidoRun.statusMask & 0xFFFFUL);
    record->measuredValue = ((liveOutputMask & activeBit) != 0U) ?
        3.3F : 0.0F;
    record->expectedMin = (float)BoardPwm_DidoRun.outputMask;
    record->expectedMax = (float)gBoardPwmDidoSnapshot.elapsedMilliseconds;
    record->errorCode = BOARD_TEST_ERROR_NONE;
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardPwm_FinishDidoRun(
    BoardTest_Record *record,
    const BoardProfile_PinMap *pins)
{
    BoardTest_U16 outputMask;
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 toggledOutputMask;

    outputMask = BoardPwm_DidoRun.outputMask;
    statusMask = BoardPwm_DidoRun.statusMask;
    failCode = BoardPwm_DidoRun.failCode;
    toggledOutputMask = gBoardPwmDidoSnapshot.toggledOutputMask;

    if((pins != 0) && (BoardPwm_DidoPinsAreValid(pins) != 0U))
    {
        BoardPwm_ForceDidoPinsLow(pins);
        gBoardPwmDidoSnapshot.finalOutputMask =
            BoardPwm_ReadDidoMask(pins->pwmDidoOutputs,
                                  BOARD_PROFILE_PWM_DIDO_OUTPUT_COUNT);
        gBoardPwmDidoSnapshot.enableReadbackMask =
            BoardPwm_ReadDidoMask(pins->pwmDidoEnables,
                                  BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT);
        gBoardPwmDidoSnapshot.powerEnableReadbackMask =
            BoardPwm_ReadDidoMask(pins->pwmDidoPowerEnables,
                                  BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT);
        if((gBoardPwmDidoSnapshot.finalOutputMask == 0U) &&
           (gBoardPwmDidoSnapshot.enableReadbackMask == 0U) &&
           (gBoardPwmDidoSnapshot.powerEnableReadbackMask == 0U))
        {
            statusMask |= BOARD_PWM_DIDO_STATUS_OUTPUTS_SAFE;
        }
        else if(failCode == BOARD_PWM_DIDO_FAIL_NONE)
        {
            failCode = BOARD_PWM_DIDO_FAIL_OUTPUT_UNSAFE;
        }
    }

    BoardPwm_RestoreDidoTimer();

    BoardPwm_DidoRun.active = 0U;
    BoardPwm_DidoRun.phase = BOARD_PWM_DIDO_PHASE_IDLE;
    BoardPwm_DidoRun.elapsedUs = 0UL;
    gBoardPwmDidoSnapshot.activeOutputIndex = 0U;
    gBoardPwmDidoSnapshot.liveOutputReadbackMask = 0U;
    gBoardPwmDidoSnapshot.phase = BOARD_PWM_DIDO_PHASE_IDLE;
    gBoardPwmDidoSnapshot.elapsedMilliseconds = 0U;
    gBoardPwmDidoSnapshot.statusMask = statusMask;
    gBoardPwmDidoSnapshot.failCode = failCode;

    return BoardPwm_EvaluateDidoStatus(statusMask,
                                       failCode,
                                       toggledOutputMask,
                                       outputMask,
                                       record);
}

static BoardTest_Result BoardPwm_StartDidoOutput(
    BoardTest_Record *record,
    const BoardProfile_PinMap *pins,
    BoardTest_U16 outputIndex)
{
    BoardTest_U16 enableIndex;
    BoardTest_U16 expectedEnableMask;
    BoardTest_U16 activeBit;

    BoardPwm_ForceDidoPinsLow(pins);
    if((BoardPwm_ReadDidoMask(pins->pwmDidoEnables,
                              BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT) != 0U) ||
       (BoardPwm_ReadDidoMask(pins->pwmDidoPowerEnables,
                              BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT) != 0U))
    {
        BoardPwm_DidoRun.failCode = BOARD_PWM_DIDO_FAIL_ENABLE_UNSAFE;
        return BoardPwm_FinishDidoRun(record, pins);
    }

    enableIndex = BoardPwm_DidoEnableBaseForOutput(outputIndex);
    expectedEnableMask = (BoardTest_U16)(0x0003U << enableIndex);
    GPIO_WritePin(pins->pwmDidoEnables[enableIndex], 1U);
    GPIO_WritePin(pins->pwmDidoEnables[enableIndex + 1U], 1U);
    gBoardPwmDidoSnapshot.enableReadbackMask =
        BoardPwm_ReadDidoMask(pins->pwmDidoEnables,
                              BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT);
    gBoardPwmDidoSnapshot.activeEnableReadbackMask =
        gBoardPwmDidoSnapshot.enableReadbackMask;
    gBoardPwmDidoSnapshot.powerEnableReadbackMask =
        BoardPwm_ReadDidoMask(pins->pwmDidoPowerEnables,
                              BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT);
    if((gBoardPwmDidoSnapshot.enableReadbackMask != expectedEnableMask) ||
       (gBoardPwmDidoSnapshot.powerEnableReadbackMask != 0U))
    {
        BoardPwm_DidoRun.failCode = BOARD_PWM_DIDO_FAIL_ENABLE_UNSAFE;
        return BoardPwm_FinishDidoRun(record, pins);
    }

    BoardPwm_DidoRun.currentOutputIndex = outputIndex;
    BoardPwm_DidoRun.currentEnableIndex = enableIndex;
    BoardPwm_BeginDidoPhase(BOARD_PWM_DIDO_PHASE_HIGH);
    gBoardPwmDidoSnapshot.activeOutputIndex =
        (BoardTest_U16)(outputIndex + 1U);
    GPIO_WritePin(pins->pwmDidoOutputs[outputIndex], 1U);
    DELAY_US(BOARD_PWM_DIDO_READBACK_SETTLE_US);
    activeBit = (BoardTest_U16)(1U << outputIndex);
    if(GPIO_ReadPin(pins->pwmDidoOutputs[outputIndex]) == 0U)
    {
        BoardPwm_DidoRun.failCode = BOARD_PWM_DIDO_FAIL_OUTPUT_HIGH;
        return BoardPwm_FinishDidoRun(record, pins);
    }
    gBoardPwmDidoSnapshot.toggledOutputMask |= activeBit;
    return BoardPwm_UpdateDidoRunningRecord(record, pins);
}

static BoardTest_Result BoardPwm_ServiceDidoRun(
    BoardTest_Record *record,
    const BoardProfile_PinMap *pins)
{
    BoardTest_U16 index;

    if((pins == 0) || (BoardPwm_DidoPinsAreValid(pins) == 0U))
    {
        BoardPwm_DidoRun.failCode = BOARD_PWM_DIDO_FAIL_PROFILE;
        return BoardPwm_FinishDidoRun(record, pins);
    }

    BoardPwm_UpdateDidoElapsed();

    if(BoardPwm_DidoRun.phase == BOARD_PWM_DIDO_PHASE_HIGH)
    {
        if(GPIO_ReadPin(
               pins->pwmDidoOutputs[BoardPwm_DidoRun.currentOutputIndex]) == 0U)
        {
            BoardPwm_DidoRun.failCode = BOARD_PWM_DIDO_FAIL_OUTPUT_HIGH;
            return BoardPwm_FinishDidoRun(record, pins);
        }
        if(BoardPwm_DidoRun.elapsedUs < BOARD_PWM_DIDO_HIGH_HOLD_US)
        {
            return BoardPwm_UpdateDidoRunningRecord(record, pins);
        }

        GPIO_WritePin(
            pins->pwmDidoOutputs[BoardPwm_DidoRun.currentOutputIndex], 0U);
        BoardPwm_BeginDidoPhase(BOARD_PWM_DIDO_PHASE_LOW);
        return BoardPwm_UpdateDidoRunningRecord(record, pins);
    }

    if(GPIO_ReadPin(
           pins->pwmDidoOutputs[BoardPwm_DidoRun.currentOutputIndex]) != 0U)
    {
        BoardPwm_DidoRun.failCode = BOARD_PWM_DIDO_FAIL_OUTPUT_UNSAFE;
        return BoardPwm_FinishDidoRun(record, pins);
    }
    if(BoardPwm_DidoRun.elapsedUs < BOARD_PWM_DIDO_LOW_HOLD_US)
    {
        return BoardPwm_UpdateDidoRunningRecord(record, pins);
    }

    GPIO_WritePin(pins->pwmDidoEnables[BoardPwm_DidoRun.currentEnableIndex],
                  0U);
    GPIO_WritePin(
        pins->pwmDidoEnables[BoardPwm_DidoRun.currentEnableIndex + 1U], 0U);
    if((BoardPwm_ReadDidoMask(pins->pwmDidoEnables,
                              BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT) != 0U) ||
       (BoardPwm_ReadDidoMask(pins->pwmDidoPowerEnables,
                              BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT) != 0U))
    {
        BoardPwm_DidoRun.failCode = BOARD_PWM_DIDO_FAIL_ENABLE_UNSAFE;
        return BoardPwm_FinishDidoRun(record, pins);
    }

    for(index = (BoardTest_U16)(BoardPwm_DidoRun.currentOutputIndex + 1U);
        index < BOARD_PROFILE_PWM_DIDO_OUTPUT_COUNT;
        index++)
    {
        if((BoardPwm_DidoRun.outputMask & (BoardTest_U16)(1U << index)) != 0U)
        {
            return BoardPwm_StartDidoOutput(record, pins, index);
        }
    }

    if(gBoardPwmDidoSnapshot.toggledOutputMask == BoardPwm_DidoRun.outputMask)
    {
        BoardPwm_DidoRun.statusMask |= BOARD_PWM_DIDO_STATUS_SEQUENCE_DONE;
    }
    else
    {
        BoardPwm_DidoRun.failCode = BOARD_PWM_DIDO_FAIL_OUTPUT_HIGH;
    }
    return BoardPwm_FinishDidoRun(record, pins);
}

BoardTest_Result BoardPwm_RunDidoExternalTest(BoardTest_Record *record,
                                              BoardTest_U16 armKey,
                                              BoardTest_U16 outputMask)
{
    const BoardProfile_HardwareDescriptor *hardware;
    const BoardProfile_PinMap *pins;
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 toggledOutputMask;
    BoardTest_U16 index;
    BoardTest_U16 enableIndex;
    BoardTest_U16 expectedEnableMask;

    hardware = BoardProfile_GetCurrentHardware();
    pins = (hardware != 0) ? &hardware->pins : 0;
    if(BoardPwm_DidoRun.active != 0U)
    {
        return BoardPwm_ServiceDidoRun(record, pins);
    }
    statusMask = 0U;
    failCode = BOARD_PWM_DIDO_FAIL_NONE;
    toggledOutputMask = 0U;

    gBoardPwmDidoSnapshot.statusMask = 0U;
    gBoardPwmDidoSnapshot.failCode = BOARD_PWM_DIDO_FAIL_NONE;
    gBoardPwmDidoSnapshot.requestedOutputMask = outputMask;
    gBoardPwmDidoSnapshot.toggledOutputMask = 0U;
    gBoardPwmDidoSnapshot.activeOutputIndex = 0U;
    gBoardPwmDidoSnapshot.activeEnableReadbackMask = 0U;
    gBoardPwmDidoSnapshot.enableReadbackMask = 0U;
    gBoardPwmDidoSnapshot.powerEnableReadbackMask = 0U;
    gBoardPwmDidoSnapshot.finalOutputMask = 0U;
    gBoardPwmDidoSnapshot.phase = BOARD_PWM_DIDO_PHASE_IDLE;
    gBoardPwmDidoSnapshot.elapsedMilliseconds = 0U;
    gBoardPwmDidoSnapshot.liveOutputReadbackMask = 0U;

    if(armKey != BOARD_PWM_DIDO_TEST_ARM_KEY)
    {
        failCode = BOARD_PWM_DIDO_FAIL_NOT_ARMED;
    }
    else if((outputMask == 0U) ||
            ((outputMask & BOARD_PWM_DIDO_OUTPUT_MASK) != outputMask))
    {
        failCode = BOARD_PWM_DIDO_FAIL_SELECTION;
    }
    else if((pins == 0) ||
            (BoardPwm_DidoPinsAreValid(pins) == 0U))
    {
        failCode = BOARD_PWM_DIDO_FAIL_PROFILE;
    }
    else if(BoardPwm_PrepareDidoPins(pins) == 0U)
    {
        failCode = BOARD_PWM_DIDO_FAIL_CONFIG;
    }
    else
    {
        statusMask |= BOARD_PWM_DIDO_STATUS_CONFIGURED;
        gBoardPwmDidoSnapshot.enableReadbackMask =
            BoardPwm_ReadDidoMask(pins->pwmDidoEnables,
                                  BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT);
        gBoardPwmDidoSnapshot.powerEnableReadbackMask =
            BoardPwm_ReadDidoMask(pins->pwmDidoPowerEnables,
                                  BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT);
        if((gBoardPwmDidoSnapshot.enableReadbackMask == 0U) &&
           (gBoardPwmDidoSnapshot.powerEnableReadbackMask == 0U))
        {
            statusMask |= BOARD_PWM_DIDO_STATUS_ENABLES_SAFE;
        }
        else
        {
            failCode = BOARD_PWM_DIDO_FAIL_ENABLE_UNSAFE;
        }
    }

    if(failCode == BOARD_PWM_DIDO_FAIL_NONE)
    {
        for(index = 0U; index < BOARD_PROFILE_PWM_DIDO_OUTPUT_COUNT; index++)
        {
            if((outputMask & (BoardTest_U16)(1U << index)) == 0U)
            {
                continue;
            }

            BoardPwm_ForceDidoPinsLow(pins);
            if((BoardPwm_ReadDidoMask(pins->pwmDidoEnables,
                                      BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT) !=
                0U) ||
               (BoardPwm_ReadDidoMask(
                    pins->pwmDidoPowerEnables,
                    BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT) != 0U))
            {
                failCode = BOARD_PWM_DIDO_FAIL_ENABLE_UNSAFE;
                break;
            }

            enableIndex = BoardPwm_DidoEnableBaseForOutput(index);
            expectedEnableMask = (BoardTest_U16)(0x0003U << enableIndex);
            GPIO_WritePin(pins->pwmDidoEnables[enableIndex], 1U);
            GPIO_WritePin(pins->pwmDidoEnables[enableIndex + 1U], 1U);
            gBoardPwmDidoSnapshot.enableReadbackMask =
                BoardPwm_ReadDidoMask(pins->pwmDidoEnables,
                                      BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT);
            gBoardPwmDidoSnapshot.activeEnableReadbackMask =
                gBoardPwmDidoSnapshot.enableReadbackMask;
            gBoardPwmDidoSnapshot.powerEnableReadbackMask =
                BoardPwm_ReadDidoMask(
                    pins->pwmDidoPowerEnables,
                    BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT);
            if((gBoardPwmDidoSnapshot.enableReadbackMask !=
                expectedEnableMask) ||
               (gBoardPwmDidoSnapshot.powerEnableReadbackMask != 0U))
            {
                failCode = BOARD_PWM_DIDO_FAIL_ENABLE_UNSAFE;
                break;
            }

            gBoardPwmDidoSnapshot.activeOutputIndex =
                (BoardTest_U16)(index + 1U);
            GPIO_WritePin(pins->pwmDidoOutputs[index], 1U);
            DELAY_US(BOARD_PWM_DIDO_READBACK_SETTLE_US);
            if(GPIO_ReadPin(pins->pwmDidoOutputs[index]) == 0U)
            {
                failCode = BOARD_PWM_DIDO_FAIL_OUTPUT_HIGH;
                break;
            }
            toggledOutputMask |= (BoardTest_U16)(1U << index);
            gBoardPwmDidoSnapshot.toggledOutputMask = toggledOutputMask;
            BoardPwm_DidoRun.active = 1U;
            BoardPwm_DidoRun.phase = BOARD_PWM_DIDO_PHASE_HIGH;
            BoardPwm_DidoRun.outputMask = outputMask;
            BoardPwm_DidoRun.statusMask = statusMask;
            BoardPwm_DidoRun.failCode = failCode;
            BoardPwm_DidoRun.currentOutputIndex = index;
            BoardPwm_DidoRun.currentEnableIndex = enableIndex;
            BoardPwm_DidoRun.timerConfigured = 0U;
            BoardPwm_ConfigureDidoTimer();
            BoardPwm_BeginDidoPhase(BOARD_PWM_DIDO_PHASE_HIGH);
            return BoardPwm_UpdateDidoRunningRecord(record, pins);
        }

        if((failCode == BOARD_PWM_DIDO_FAIL_NONE) &&
           (toggledOutputMask == outputMask))
        {
            statusMask |= BOARD_PWM_DIDO_STATUS_SEQUENCE_DONE;
        }
    }

    if(pins != 0)
    {
        BoardPwm_ForceDidoPinsLow(pins);
        gBoardPwmDidoSnapshot.finalOutputMask =
            BoardPwm_ReadDidoMask(pins->pwmDidoOutputs,
                                  BOARD_PROFILE_PWM_DIDO_OUTPUT_COUNT);
        gBoardPwmDidoSnapshot.enableReadbackMask =
            BoardPwm_ReadDidoMask(pins->pwmDidoEnables,
                                  BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT);
        gBoardPwmDidoSnapshot.powerEnableReadbackMask =
            BoardPwm_ReadDidoMask(pins->pwmDidoPowerEnables,
                                  BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT);
        if((gBoardPwmDidoSnapshot.finalOutputMask == 0U) &&
           (gBoardPwmDidoSnapshot.enableReadbackMask == 0U) &&
           (gBoardPwmDidoSnapshot.powerEnableReadbackMask == 0U))
        {
            statusMask |= BOARD_PWM_DIDO_STATUS_OUTPUTS_SAFE;
        }
        else if(failCode == BOARD_PWM_DIDO_FAIL_NONE)
        {
            failCode = BOARD_PWM_DIDO_FAIL_OUTPUT_UNSAFE;
        }
    }

    gBoardPwmDidoSnapshot.activeOutputIndex = 0U;
    gBoardPwmDidoSnapshot.statusMask = statusMask;
    gBoardPwmDidoSnapshot.failCode = failCode;
    return BoardPwm_EvaluateDidoStatus(statusMask,
                                       failCode,
                                       toggledOutputMask,
                                       outputMask,
                                       record);
}
#endif
