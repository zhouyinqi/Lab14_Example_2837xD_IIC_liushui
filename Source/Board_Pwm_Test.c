#include "Board_Pwm_Test.h"
#include "Board_Pinmap.h"

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

#ifndef BOARD_TEST_HOST

#define BOARD_PWM_FORCE_LOW_READBACK_INVALID 0xFFFFU
#define BOARD_PWM_FORCE_LOW_EXPECTED_READBACK \
    ((BoardTest_U16)AQ_CLEAR |                \
     ((BoardTest_U16)AQ_CLEAR << 2U) |        \
     ((BoardTest_U16)TZ_FORCE_LO << 4U) |     \
     ((BoardTest_U16)TZ_FORCE_LO << 6U))

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
#endif
