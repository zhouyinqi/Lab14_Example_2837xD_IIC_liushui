#include "Board_Adc_Test.h"
#include "Board_Profile.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

volatile BoardAdc_BasicSnapshot gBoardAdcBasicSnapshot =
{
    0U
};

static const BoardAdc_ChannelConfig BoardAdc_LowAltitudeChannels[] =
{
    {BOARD_ADC_SIGNAL_IU1, BOARD_ADC_MODULE_D, 0U},
    {BOARD_ADC_SIGNAL_IV1, BOARD_ADC_MODULE_C, 3U},
    {BOARD_ADC_SIGNAL_IW1, BOARD_ADC_MODULE_B, 3U},
    {BOARD_ADC_SIGNAL_IU2, BOARD_ADC_MODULE_A, 15U},
    {BOARD_ADC_SIGNAL_IV2, BOARD_ADC_MODULE_B, 2U},
    {BOARD_ADC_SIGNAL_IW2, BOARD_ADC_MODULE_C, 2U},
    {BOARD_ADC_SIGNAL_UDC1, BOARD_ADC_MODULE_A, 14U},
    {BOARD_ADC_SIGNAL_UDC2, BOARD_ADC_MODULE_D, 1U},
    {BOARD_ADC_SIGNAL_MOTOR_TEMPERATURE, BOARD_ADC_MODULE_A, 3U},
    {BOARD_ADC_SIGNAL_FAULT_1, BOARD_ADC_MODULE_D, 2U},
    {BOARD_ADC_SIGNAL_FAULT_2, BOARD_ADC_MODULE_D, 3U},
    {BOARD_ADC_SIGNAL_D5V, BOARD_ADC_MODULE_B, 1U},
    {BOARD_ADC_SIGNAL_A5V, BOARD_ADC_MODULE_A, 2U},
    {BOARD_ADC_SIGNAL_INVERTER_TEMPERATURE_1, BOARD_ADC_MODULE_A, 4U},
    {BOARD_ADC_SIGNAL_INVERTER_TEMPERATURE_2, BOARD_ADC_MODULE_A, 5U},
    {BOARD_ADC_SIGNAL_UUV1, BOARD_ADC_MODULE_D, 4U},
    {BOARD_ADC_SIGNAL_UVW1, BOARD_ADC_MODULE_C, 4U},
    {BOARD_ADC_SIGNAL_UUV2, BOARD_ADC_MODULE_B, 0U},
    {BOARD_ADC_SIGNAL_UVW2, BOARD_ADC_MODULE_A, 0U}
};

#define BOARD_ADC_LOW_ALTITUDE_CHANNEL_COUNT \
    ((BoardTest_U16)(sizeof(BoardAdc_LowAltitudeChannels) / \
                     sizeof(BoardAdc_LowAltitudeChannels[0])))

static float BoardAdc_ClampVoltage(float inputVoltage)
{
    if(inputVoltage < 0.0F)
    {
        return 0.0F;
    }

    if(inputVoltage > BOARD_ADC_REFERENCE_VOLTAGE_V)
    {
        return BOARD_ADC_REFERENCE_VOLTAGE_V;
    }

    return inputVoltage;
}

BoardTest_U16 BoardAdc_VoltageToRaw(float inputVoltage)
{
    float clampedVoltage;
    float scaledRaw;

    clampedVoltage = BoardAdc_ClampVoltage(inputVoltage);
    scaledRaw = (clampedVoltage * (float)BOARD_ADC_RAW_MAX) /
                BOARD_ADC_REFERENCE_VOLTAGE_V;
    scaledRaw += 0.5F;

    if(scaledRaw > (float)BOARD_ADC_RAW_MAX)
    {
        return BOARD_ADC_RAW_MAX;
    }

    return (BoardTest_U16)scaledRaw;
}

float BoardAdc_RawToVoltage(BoardTest_U16 rawValue)
{
    BoardTest_U16 clippedRaw;

    clippedRaw = rawValue;
    if(clippedRaw > BOARD_ADC_RAW_MAX)
    {
        clippedRaw = BOARD_ADC_RAW_MAX;
    }

    return ((float)clippedRaw * BOARD_ADC_REFERENCE_VOLTAGE_V) /
           (float)BOARD_ADC_RAW_MAX;
}

BoardTest_U16 BoardAdc_GetChannelCount(BoardTest_U16 boardId,
                                       BoardTest_U16 hardwareRevision)
{
    if((boardId == BOARD_PROFILE_ID_LOW_ALTITUDE_UNIFIED) &&
       (hardwareRevision == BOARD_PROFILE_HARDWARE_REVISION_LOW_ALTITUDE_V01))
    {
        return BOARD_ADC_LOW_ALTITUDE_CHANNEL_COUNT;
    }

    return 0U;
}

BoardTest_U16 BoardAdc_GetChannelConfig(BoardTest_U16 boardId,
                                        BoardTest_U16 hardwareRevision,
                                        BoardTest_U16 channelIndex,
                                        BoardAdc_ChannelConfig *config)
{
    BoardTest_U16 channelCount;

    if(config == 0)
    {
        return 0U;
    }

    channelCount = BoardAdc_GetChannelCount(boardId, hardwareRevision);
    if(channelIndex >= channelCount)
    {
        return 0U;
    }

    *config = BoardAdc_LowAltitudeChannels[channelIndex];
    return 1U;
}

BoardTest_Result BoardAdc_EvaluateBasicSample(BoardTest_U16 statusMask,
                                              BoardTest_U16 rawValue,
                                              BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)rawValue & 0xFFFFUL);
    record->measuredValue = BoardAdc_RawToVoltage(rawValue);
    record->expectedMin = 0.0F;
    record->expectedMax = BOARD_ADC_REFERENCE_VOLTAGE_V;

    if(((statusMask & BOARD_ADC_BASIC_STATUS_REQUIRED_MASK) ==
        BOARD_ADC_BASIC_STATUS_REQUIRED_MASK) &&
       (rawValue <= BOARD_ADC_RAW_MAX))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_ADC_SAMPLE;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardAdc_EvaluateChannelScan(
    BoardTest_U16 statusMask,
    BoardTest_U16 sampledChannelCount,
    BoardTest_U16 expectedChannelCount,
    BoardTest_U16 failedChannelIndex,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)failedChannelIndex & 0xFFFFUL);
    record->measuredValue = (float)sampledChannelCount;
    record->expectedMin = (float)expectedChannelCount;
    record->expectedMax = (float)expectedChannelCount;

    if(((statusMask & BOARD_ADC_BASIC_STATUS_REQUIRED_MASK) ==
        BOARD_ADC_BASIC_STATUS_REQUIRED_MASK) &&
       (expectedChannelCount != 0U) &&
       (sampledChannelCount == expectedChannelCount) &&
       (failedChannelIndex == BOARD_ADC_NO_FAILED_CHANNEL))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_ADC_SAMPLE;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST

#define BOARD_ADC_TIMEOUT       200000UL
#define BOARD_ADC_ACQPS_12BIT   14U

static void BoardAdc_InitModule(volatile struct ADC_REGS *adcRegs)
{
    adcRegs->ADCCTL2.bit.PRESCALE = 6U;
    adcRegs->ADCCTL2.bit.RESOLUTION = 0U;
    adcRegs->ADCCTL2.bit.SIGNALMODE = 0U;
    adcRegs->ADCCTL1.bit.INTPULSEPOS = 1U;
    adcRegs->ADCCTL1.bit.ADCPWDNZ = 1U;
}

static BoardTest_U16 BoardAdc_GetModuleRegisters(
    BoardTest_U16 module,
    volatile struct ADC_REGS **adcRegs,
    volatile struct ADC_RESULT_REGS **resultRegs)
{
    switch(module)
    {
        case BOARD_ADC_MODULE_A:
            *adcRegs = &AdcaRegs;
            *resultRegs = &AdcaResultRegs;
            break;

        case BOARD_ADC_MODULE_B:
            *adcRegs = &AdcbRegs;
            *resultRegs = &AdcbResultRegs;
            break;

        case BOARD_ADC_MODULE_C:
            *adcRegs = &AdccRegs;
            *resultRegs = &AdccResultRegs;
            break;

        case BOARD_ADC_MODULE_D:
            *adcRegs = &AdcdRegs;
            *resultRegs = &AdcdResultRegs;
            break;

        default:
            *adcRegs = 0;
            *resultRegs = 0;
            return 0U;
    }

    return 1U;
}

static void BoardAdc_InitAdcaSoc0(void)
{
    EALLOW;
    DevCfgRegs.CPUSEL11.bit.ADC_A = 0U;
    CpuSysRegs.PCLKCR13.bit.ADC_A = 1U;
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6U;
    AdcaRegs.ADCCTL2.bit.RESOLUTION = 0U;
    AdcaRegs.ADCCTL2.bit.SIGNALMODE = 0U;
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1U;
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1U;
    DELAY_US(1000UL);

    AdcaRegs.ADCSOC0CTL.bit.CHSEL = BOARD_ADC_BASIC_ADCA_CHANNEL;
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = BOARD_ADC_ACQPS_12BIT;
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 0U;
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0U;
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1U;
    AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 0U;
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1U;
    AdcaRegs.ADCINTOVFCLR.bit.ADCINT1 = 1U;
    EDIS;
}

static BoardTest_U16 BoardAdc_RunAdcaSoc0(BoardTest_U16 *rawValue)
{
    BoardTest_U32 timeout;
    BoardTest_U16 statusMask;

    statusMask = 0U;
    *rawValue = 0U;

    BoardAdc_InitAdcaSoc0();

    if(CpuSysRegs.PCLKCR13.bit.ADC_A == 1U)
    {
        statusMask |= BOARD_ADC_BASIC_CLOCK_ENABLED;
    }

    if(AdcaRegs.ADCCTL1.bit.ADCPWDNZ == 1U)
    {
        statusMask |= BOARD_ADC_BASIC_POWERED;
    }

    if((AdcaRegs.ADCSOC0CTL.bit.CHSEL == BOARD_ADC_BASIC_ADCA_CHANNEL) &&
       (AdcaRegs.ADCINTSEL1N2.bit.INT1SEL == 0U) &&
       (AdcaRegs.ADCINTSEL1N2.bit.INT1E == 1U))
    {
        statusMask |= BOARD_ADC_BASIC_SOC_CONFIGURED;
    }

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1U;
    AdcaRegs.ADCSOCFRC1.bit.SOC0 = 1U;

    timeout = BOARD_ADC_TIMEOUT;
    while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0U)
    {
        if(timeout-- == 0UL)
        {
            return statusMask;
        }
    }

    statusMask |= BOARD_ADC_BASIC_CONVERSION_DONE;
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1U;
    *rawValue = AdcaResultRegs.ADCRESULT0;

    if(*rawValue <= BOARD_ADC_RAW_MAX)
    {
        statusMask |= BOARD_ADC_BASIC_RAW_IN_RANGE;
    }

    return statusMask;
}

static void BoardAdc_ResetChannelSnapshot(BoardTest_U16 boardId,
                                          BoardTest_U16 channelCount)
{
    BoardTest_U16 index;

    gBoardAdcBasicSnapshot.statusMask = 0U;
    gBoardAdcBasicSnapshot.rawValue = 0U;
    gBoardAdcBasicSnapshot.channel = 0U;
    gBoardAdcBasicSnapshot.inputVoltage = 0.0F;
    gBoardAdcBasicSnapshot.boardId = boardId;
    gBoardAdcBasicSnapshot.configuredChannelCount = channelCount;
    gBoardAdcBasicSnapshot.sampledChannelCount = 0U;
    gBoardAdcBasicSnapshot.failedChannelIndex = BOARD_ADC_NO_FAILED_CHANNEL;

    for(index = 0U; index < BOARD_ADC_MAX_CHANNEL_COUNT; index++)
    {
        gBoardAdcBasicSnapshot.signalIds[index] = 0U;
        gBoardAdcBasicSnapshot.modules[index] = 0U;
        gBoardAdcBasicSnapshot.channels[index] = 0U;
        gBoardAdcBasicSnapshot.rawValues[index] = 0U;
        gBoardAdcBasicSnapshot.inputVoltages[index] = 0.0F;
    }
}

static BoardTest_U16 BoardAdc_RunConfiguredChannel(
    const BoardAdc_ChannelConfig *config,
    BoardTest_U16 *rawValue)
{
    volatile struct ADC_REGS *adcRegs;
    volatile struct ADC_RESULT_REGS *resultRegs;
    BoardTest_U32 timeout;

    *rawValue = 0U;
    if(BoardAdc_GetModuleRegisters(config->module,
                                   &adcRegs,
                                   &resultRegs) == 0U)
    {
        return 0U;
    }

    EALLOW;
    adcRegs->ADCSOC0CTL.bit.CHSEL = config->channel;
    adcRegs->ADCSOC0CTL.bit.ACQPS = BOARD_ADC_ACQPS_12BIT;
    adcRegs->ADCSOC0CTL.bit.TRIGSEL = 0U;
    adcRegs->ADCINTSEL1N2.bit.INT1SEL = 0U;
    adcRegs->ADCINTSEL1N2.bit.INT1E = 1U;
    adcRegs->ADCINTSEL1N2.bit.INT1CONT = 0U;
    adcRegs->ADCINTFLGCLR.bit.ADCINT1 = 1U;
    adcRegs->ADCINTOVFCLR.bit.ADCINT1 = 1U;
    EDIS;

    if((adcRegs->ADCSOC0CTL.bit.CHSEL != config->channel) ||
       (adcRegs->ADCINTSEL1N2.bit.INT1SEL != 0U) ||
       (adcRegs->ADCINTSEL1N2.bit.INT1E != 1U))
    {
        return 0U;
    }

    adcRegs->ADCINTFLGCLR.bit.ADCINT1 = 1U;
    adcRegs->ADCSOCFRC1.bit.SOC0 = 1U;

    timeout = BOARD_ADC_TIMEOUT;
    while(adcRegs->ADCINTFLG.bit.ADCINT1 == 0U)
    {
        if(timeout-- == 0UL)
        {
            return 0U;
        }
    }

    adcRegs->ADCINTFLGCLR.bit.ADCINT1 = 1U;
    *rawValue = resultRegs->ADCRESULT0;
    return (*rawValue <= BOARD_ADC_RAW_MAX) ? 1U : 0U;
}

static BoardTest_Result BoardAdc_RunLowAltitudeChannelScan(
    const BoardProfile_HardwareDescriptor *hardware,
    BoardTest_Record *record)
{
    BoardAdc_ChannelConfig config;
    BoardTest_U16 index;
    BoardTest_U16 rawValue;
    BoardTest_U16 statusMask;
    BoardTest_U16 channelCount;
    BoardTest_U16 allSamplesPassed;

    channelCount = BoardAdc_GetChannelCount(hardware->boardId,
                                            hardware->hardwareRevision);
    BoardAdc_ResetChannelSnapshot(hardware->boardId, channelCount);
    statusMask = 0U;
    allSamplesPassed = 1U;

    EALLOW;
    DevCfgRegs.CPUSEL11.bit.ADC_A = 0U;
    DevCfgRegs.CPUSEL11.bit.ADC_B = 0U;
    DevCfgRegs.CPUSEL11.bit.ADC_C = 0U;
    DevCfgRegs.CPUSEL11.bit.ADC_D = 0U;
    CpuSysRegs.PCLKCR13.bit.ADC_A = 1U;
    CpuSysRegs.PCLKCR13.bit.ADC_B = 1U;
    CpuSysRegs.PCLKCR13.bit.ADC_C = 1U;
    CpuSysRegs.PCLKCR13.bit.ADC_D = 1U;
    BoardAdc_InitModule(&AdcaRegs);
    BoardAdc_InitModule(&AdcbRegs);
    BoardAdc_InitModule(&AdccRegs);
    BoardAdc_InitModule(&AdcdRegs);
    EDIS;
    DELAY_US(1000UL);

    if((CpuSysRegs.PCLKCR13.bit.ADC_A == 1U) &&
       (CpuSysRegs.PCLKCR13.bit.ADC_B == 1U) &&
       (CpuSysRegs.PCLKCR13.bit.ADC_C == 1U) &&
       (CpuSysRegs.PCLKCR13.bit.ADC_D == 1U))
    {
        statusMask |= BOARD_ADC_BASIC_CLOCK_ENABLED;
    }

    if((AdcaRegs.ADCCTL1.bit.ADCPWDNZ == 1U) &&
       (AdcbRegs.ADCCTL1.bit.ADCPWDNZ == 1U) &&
       (AdccRegs.ADCCTL1.bit.ADCPWDNZ == 1U) &&
       (AdcdRegs.ADCCTL1.bit.ADCPWDNZ == 1U))
    {
        statusMask |= BOARD_ADC_BASIC_POWERED;
    }

    if((channelCount != 0U) &&
       (channelCount <= BOARD_ADC_MAX_CHANNEL_COUNT))
    {
        statusMask |= BOARD_ADC_BASIC_SOC_CONFIGURED;
    }

    for(index = 0U; index < channelCount; index++)
    {
        if(BoardAdc_GetChannelConfig(hardware->boardId,
                                     hardware->hardwareRevision,
                                     index,
                                     &config) == 0U)
        {
            allSamplesPassed = 0U;
            if(gBoardAdcBasicSnapshot.failedChannelIndex ==
               BOARD_ADC_NO_FAILED_CHANNEL)
            {
                gBoardAdcBasicSnapshot.failedChannelIndex = index;
            }
            continue;
        }

        gBoardAdcBasicSnapshot.signalIds[index] = config.signalId;
        gBoardAdcBasicSnapshot.modules[index] = config.module;
        gBoardAdcBasicSnapshot.channels[index] = config.channel;

        if(BoardAdc_RunConfiguredChannel(&config, &rawValue) == 0U)
        {
            allSamplesPassed = 0U;
            if(gBoardAdcBasicSnapshot.failedChannelIndex ==
               BOARD_ADC_NO_FAILED_CHANNEL)
            {
                gBoardAdcBasicSnapshot.failedChannelIndex = index;
            }
            continue;
        }

        gBoardAdcBasicSnapshot.rawValues[index] = rawValue;
        gBoardAdcBasicSnapshot.inputVoltages[index] =
            BoardAdc_RawToVoltage(rawValue);
        gBoardAdcBasicSnapshot.sampledChannelCount++;
    }

    if(allSamplesPassed != 0U)
    {
        statusMask |= BOARD_ADC_BASIC_CONVERSION_DONE;
        statusMask |= BOARD_ADC_BASIC_RAW_IN_RANGE;
    }

    if(channelCount != 0U)
    {
        gBoardAdcBasicSnapshot.rawValue =
            gBoardAdcBasicSnapshot.rawValues[0];
        gBoardAdcBasicSnapshot.channel =
            gBoardAdcBasicSnapshot.channels[0];
        gBoardAdcBasicSnapshot.inputVoltage =
            gBoardAdcBasicSnapshot.inputVoltages[0];
    }
    gBoardAdcBasicSnapshot.statusMask = statusMask;

    return BoardAdc_EvaluateChannelScan(
        statusMask,
        gBoardAdcBasicSnapshot.sampledChannelCount,
        channelCount,
        gBoardAdcBasicSnapshot.failedChannelIndex,
        record);
}

BoardTest_Result BoardAdc_RunBasicSampleTest(BoardTest_Record *record)
{
    const BoardProfile_HardwareDescriptor *hardware;
    BoardTest_U16 rawValue;
    BoardTest_U16 statusMask;

    hardware = BoardProfile_GetCurrentHardware();
    if((hardware != 0) &&
       (hardware->boardId == BOARD_PROFILE_ID_LOW_ALTITUDE_UNIFIED) &&
       (hardware->hardwareRevision ==
        BOARD_PROFILE_HARDWARE_REVISION_LOW_ALTITUDE_V01))
    {
        return BoardAdc_RunLowAltitudeChannelScan(hardware, record);
    }

    statusMask = BoardAdc_RunAdcaSoc0(&rawValue);

    gBoardAdcBasicSnapshot.statusMask = statusMask;
    gBoardAdcBasicSnapshot.rawValue = rawValue;
    gBoardAdcBasicSnapshot.channel = BOARD_ADC_BASIC_ADCA_CHANNEL;
    gBoardAdcBasicSnapshot.inputVoltage = BoardAdc_RawToVoltage(rawValue);
    gBoardAdcBasicSnapshot.boardId =
        (hardware != 0) ? hardware->boardId : BOARD_PROFILE_ID_NONE;
    gBoardAdcBasicSnapshot.configuredChannelCount = 1U;
    gBoardAdcBasicSnapshot.sampledChannelCount =
        ((statusMask & BOARD_ADC_BASIC_CONVERSION_DONE) != 0U) ? 1U : 0U;
    gBoardAdcBasicSnapshot.failedChannelIndex =
        ((statusMask & BOARD_ADC_BASIC_CONVERSION_DONE) != 0U) ?
        BOARD_ADC_NO_FAILED_CHANNEL : 0U;

    return BoardAdc_EvaluateBasicSample(statusMask, rawValue, record);
}
#endif
