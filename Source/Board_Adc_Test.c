#include "Board_Adc_Test.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

volatile BoardAdc_BasicSnapshot gBoardAdcBasicSnapshot =
{
    0U,
    0U,
    0U,
    0.0F
};

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

#ifndef BOARD_TEST_HOST

#define BOARD_ADC_TIMEOUT       200000UL
#define BOARD_ADC_ACQPS_12BIT   14U

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

BoardTest_Result BoardAdc_RunBasicSampleTest(BoardTest_Record *record)
{
    BoardTest_U16 rawValue;
    BoardTest_U16 statusMask;

    statusMask = BoardAdc_RunAdcaSoc0(&rawValue);

    gBoardAdcBasicSnapshot.statusMask = statusMask;
    gBoardAdcBasicSnapshot.rawValue = rawValue;
    gBoardAdcBasicSnapshot.channel = BOARD_ADC_BASIC_ADCA_CHANNEL;
    gBoardAdcBasicSnapshot.inputVoltage = BoardAdc_RawToVoltage(rawValue);

    return BoardAdc_EvaluateBasicSample(statusMask, rawValue, record);
}
#endif
