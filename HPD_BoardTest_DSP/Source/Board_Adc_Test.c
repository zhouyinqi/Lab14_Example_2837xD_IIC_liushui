#include "Board_Adc_Test.h"
#include "Board_Di_Test.h"
#include "Board_Profile.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

volatile BoardAdc_BasicSnapshot gBoardAdcBasicSnapshot =
{
    0U
};

volatile BoardAdc_ExternalInjectionConfig gBoardAdcExternalInjectionConfig =
{
    0U,
    BOARD_ADC_SIGNAL_IU1,
    1000U,
    100U
};

volatile BoardAdc_ExternalInjectionSnapshot gBoardAdcExternalInjectionSnapshot =
{
    0U
};

volatile BoardAdc_ExternalGroupSnapshot
    gBoardAdcExternalGroupSnapshots[BOARD_ADC_EXTERNAL_GROUP_COUNT] =
{
    {0U},
    {0U}
};

volatile BoardAdc_MuxInjectionSnapshot gBoardAdcMuxInjectionSnapshot =
{
    0U
};

volatile BoardAdc_ProtectionSnapshot gBoardAdcProtectionSnapshot =
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
    {BOARD_ADC_SIGNAL_INVERTER_TEMPERATURE_1, BOARD_ADC_MODULE_A, 4U},
    {BOARD_ADC_SIGNAL_INVERTER_TEMPERATURE_2, BOARD_ADC_MODULE_A, 5U},
    {BOARD_ADC_SIGNAL_UUV1, BOARD_ADC_MODULE_D, 4U},
    {BOARD_ADC_SIGNAL_UVW1, BOARD_ADC_MODULE_C, 4U},
    {BOARD_ADC_SIGNAL_UUV2, BOARD_ADC_MODULE_B, 0U},
    {BOARD_ADC_SIGNAL_UVW2, BOARD_ADC_MODULE_A, 0U}
};

static const BoardTest_U16 BoardAdc_ExternalGroup1Signals[] =
{
    BOARD_ADC_SIGNAL_IU1,
    BOARD_ADC_SIGNAL_IV1,
    BOARD_ADC_SIGNAL_IW1,
    BOARD_ADC_SIGNAL_UDC1,
    BOARD_ADC_SIGNAL_INVERTER_TEMPERATURE_1,
    BOARD_ADC_SIGNAL_UUV1,
    BOARD_ADC_SIGNAL_UVW1
};

static const BoardTest_U16 BoardAdc_ExternalGroup2Signals[] =
{
    BOARD_ADC_SIGNAL_IU2,
    BOARD_ADC_SIGNAL_IV2,
    BOARD_ADC_SIGNAL_IW2,
    BOARD_ADC_SIGNAL_UDC2,
    BOARD_ADC_SIGNAL_INVERTER_TEMPERATURE_2,
    BOARD_ADC_SIGNAL_UUV2,
    BOARD_ADC_SIGNAL_UVW2
};

#define BOARD_ADC_LOW_ALTITUDE_CHANNEL_COUNT \
    ((BoardTest_U16)(sizeof(BoardAdc_LowAltitudeChannels) / \
                     sizeof(BoardAdc_LowAltitudeChannels[0])))

BoardTest_U16 BoardAdc_IsProtectionSelectionSupported(
    BoardTest_U16 selectionId)
{
    return (selectionId < BOARD_ADC_PROTECTION_SELECTION_COUNT) ? 1U : 0U;
}

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

float BoardAdc_RawToExternalTerminalVoltage(BoardTest_U16 rawValue)
{
    BoardTest_U16 clippedRaw;

    clippedRaw = rawValue;
    if(clippedRaw > BOARD_ADC_RAW_MAX)
    {
        clippedRaw = BOARD_ADC_RAW_MAX;
    }

    return ((float)clippedRaw * BOARD_ADC_EXTERNAL_TERMINAL_FULL_SCALE_V) /
           (float)BOARD_ADC_RAW_MAX;
}

BoardTest_U16 BoardAdc_IsExternalInjectionSignalSupported(
    BoardTest_U16 signalId)
{
    switch(signalId)
    {
        case BOARD_ADC_SIGNAL_IU1:
        case BOARD_ADC_SIGNAL_IV1:
        case BOARD_ADC_SIGNAL_IW1:
        case BOARD_ADC_SIGNAL_IU2:
        case BOARD_ADC_SIGNAL_IV2:
        case BOARD_ADC_SIGNAL_IW2:
        case BOARD_ADC_SIGNAL_UDC1:
        case BOARD_ADC_SIGNAL_UDC2:
        case BOARD_ADC_SIGNAL_INVERTER_TEMPERATURE_1:
        case BOARD_ADC_SIGNAL_INVERTER_TEMPERATURE_2:
        case BOARD_ADC_SIGNAL_UUV1:
        case BOARD_ADC_SIGNAL_UVW1:
        case BOARD_ADC_SIGNAL_UUV2:
        case BOARD_ADC_SIGNAL_UVW2:
            return 1U;

        default:
            return 0U;
    }
}

BoardTest_U16 BoardAdc_IsExternalInjectionSelectionSupported(
    BoardTest_U16 selectionId)
{
    if(BoardAdc_IsExternalInjectionSignalSupported(selectionId) != 0U)
    {
        return 1U;
    }

    if((selectionId == BOARD_ADC_EXTERNAL_SELECTION_GROUP_1) ||
       (selectionId == BOARD_ADC_EXTERNAL_SELECTION_GROUP_2))
    {
        return 1U;
    }

    if((selectionId >= BOARD_ADC_EXTERNAL_SELECTION_PT100_BASE) &&
       (selectionId < (BOARD_ADC_EXTERNAL_SELECTION_PT100_BASE +
                       BOARD_ADC_EXTERNAL_SELECTION_PT100_COUNT)))
    {
        return 1U;
    }

    if((selectionId >= BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_1_BASE) &&
       (selectionId < (BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_1_BASE +
                       BOARD_ADC_EXTERNAL_SELECTION_FAULT_COUNT)))
    {
        return 1U;
    }

    return ((selectionId >= BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_2_BASE) &&
            (selectionId <
             (BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_2_BASE +
              BOARD_ADC_EXTERNAL_SELECTION_FAULT_COUNT))) ? 1U : 0U;
}

BoardTest_U16 BoardAdc_GetExternalInjectionGroupSignal(
    BoardTest_U16 selectionId,
    BoardTest_U16 channelIndex,
    BoardTest_U16 *signalId)
{
    if((signalId == 0) ||
       (channelIndex >= BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT))
    {
        return 0U;
    }

    if(selectionId == BOARD_ADC_EXTERNAL_SELECTION_GROUP_1)
    {
        *signalId = BoardAdc_ExternalGroup1Signals[channelIndex];
        return 1U;
    }

    if(selectionId == BOARD_ADC_EXTERNAL_SELECTION_GROUP_2)
    {
        *signalId = BoardAdc_ExternalGroup2Signals[channelIndex];
        return 1U;
    }

    return 0U;
}

BoardTest_U16 BoardAdc_ConfigureExternalInjection(
    BoardTest_U16 signalId,
    BoardTest_U16 expectedMillivolts,
    BoardTest_U16 toleranceMillivolts)
{
    if((BoardAdc_IsExternalInjectionSelectionSupported(signalId) == 0U) ||
       (expectedMillivolts > BOARD_ADC_EXTERNAL_MAX_MILLIVOLTS) ||
       (toleranceMillivolts == 0U) ||
       (toleranceMillivolts >
        BOARD_ADC_EXTERNAL_MAX_TOLERANCE_MILLIVOLTS))
    {
        return 0U;
    }

    gBoardAdcExternalInjectionConfig.configured = 0U;
    gBoardAdcExternalInjectionConfig.signalId = signalId;
    gBoardAdcExternalInjectionConfig.expectedMillivolts =
        expectedMillivolts;
    gBoardAdcExternalInjectionConfig.toleranceMillivolts =
        toleranceMillivolts;
    gBoardAdcExternalInjectionConfig.configured = 1U;
    return 1U;
}

BoardTest_U16 BoardAdc_IsMuxSelectionSupported(
    BoardTest_U16 testId,
    BoardTest_U16 selectionId)
{
    if(testId == BOARD_TEST_ID_ADC_PT100_MUX_EXTERNAL)
    {
        return (selectionId ==
                BOARD_ADC_EXTERNAL_SELECTION_PT100_TEST_PT4) ? 1U : 0U;
    }

    if(testId == BOARD_TEST_ID_ADC_FAULT_MUX_EXTERNAL)
    {
        return 0U;
    }

    return 0U;
}

BoardTest_U16 BoardAdc_GetMuxSelectorCode(
    BoardTest_U16 selectionId,
    BoardTest_U16 *selectorCode)
{
    if(selectorCode == 0)
    {
        return 0U;
    }

    if((selectionId >= BOARD_ADC_EXTERNAL_SELECTION_PT100_BASE) &&
       (selectionId < (BOARD_ADC_EXTERNAL_SELECTION_PT100_BASE +
                       BOARD_ADC_EXTERNAL_SELECTION_PT100_COUNT)))
    {
        *selectorCode = selectionId - BOARD_ADC_EXTERNAL_SELECTION_PT100_BASE;
        return 1U;
    }

    if((selectionId >= BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_1_BASE) &&
       (selectionId < (BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_1_BASE +
                       BOARD_ADC_EXTERNAL_SELECTION_FAULT_COUNT)))
    {
        *selectorCode =
            selectionId - BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_1_BASE;
        return 1U;
    }

    if((selectionId >= BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_2_BASE) &&
       (selectionId < (BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_2_BASE +
                       BOARD_ADC_EXTERNAL_SELECTION_FAULT_COUNT)))
    {
        *selectorCode =
            selectionId - BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_2_BASE;
        return 1U;
    }

    return 0U;
}

float BoardAdc_Pt100VoltageToResistance(float inputVoltage)
{
    float ratio;

    ratio = (180.0F / 2180.0F) - (inputVoltage / 65.0F);
    if((ratio <= 0.0F) || (ratio >= 1.0F))
    {
        return -1.0F;
    }

    return (2000.0F * ratio) / (1.0F - ratio);
}

float BoardAdc_FaultVoltageToDutyPercent(float inputVoltage)
{
    if(inputVoltage <= 0.0F)
    {
        return 0.0F;
    }
    if(inputVoltage >= BOARD_ADC_EXTERNAL_TERMINAL_FULL_SCALE_V)
    {
        return 100.0F;
    }

    return (inputVoltage * 100.0F) /
           BOARD_ADC_EXTERNAL_TERMINAL_FULL_SCALE_V;
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

BoardTest_Result BoardAdc_EvaluateExternalInjection(
    BoardTest_U16 statusMask,
    BoardTest_U16 averageRaw,
    float expectedVoltage,
    float toleranceVoltage,
    BoardTest_Record *record)
{
    float minimumVoltage;
    float maximumVoltage;

    minimumVoltage = expectedVoltage - toleranceVoltage;
    if(minimumVoltage < 0.0F)
    {
        minimumVoltage = 0.0F;
    }
    maximumVoltage = expectedVoltage + toleranceVoltage;
    if(maximumVoltage > BOARD_ADC_EXTERNAL_TERMINAL_FULL_SCALE_V)
    {
        maximumVoltage = BOARD_ADC_EXTERNAL_TERMINAL_FULL_SCALE_V;
    }

    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)averageRaw & 0xFFFFUL);
    record->measuredValue =
        BoardAdc_RawToExternalTerminalVoltage(averageRaw);
    record->expectedMin = minimumVoltage;
    record->expectedMax = maximumVoltage;

    if(((statusMask & BOARD_ADC_BASIC_STATUS_REQUIRED_MASK) ==
        BOARD_ADC_BASIC_STATUS_REQUIRED_MASK) &&
       (averageRaw <= BOARD_ADC_RAW_MAX) &&
       (record->measuredValue >= minimumVoltage) &&
       (record->measuredValue <= maximumVoltage))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_ADC_EXTERNAL_INJECTION;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardAdc_EvaluatePt100Injection(
    BoardTest_U16 statusMask,
    BoardTest_U16 averageRaw,
    float expectedVoltage,
    float toleranceVoltage,
    BoardTest_Record *record)
{
    float minimumVoltage;
    float maximumVoltage;

    minimumVoltage = expectedVoltage - toleranceVoltage;
    if(minimumVoltage < 0.0F)
    {
        minimumVoltage = 0.0F;
    }
    maximumVoltage = expectedVoltage + toleranceVoltage;
    if(maximumVoltage > BOARD_ADC_REFERENCE_VOLTAGE_V)
    {
        maximumVoltage = BOARD_ADC_REFERENCE_VOLTAGE_V;
    }

    record->rawValue = (BoardTest_U32)averageRaw;
    record->measuredValue = BoardAdc_RawToVoltage(averageRaw);
    record->expectedMin = minimumVoltage;
    record->expectedMax = maximumVoltage;

    if(((statusMask & BOARD_ADC_BASIC_STATUS_REQUIRED_MASK) ==
        BOARD_ADC_BASIC_STATUS_REQUIRED_MASK) &&
       (averageRaw <= BOARD_ADC_RAW_MAX) &&
       (record->measuredValue >= minimumVoltage) &&
       (record->measuredValue <= maximumVoltage))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_ADC_MUX_INJECTION;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardAdc_EvaluateExternalInjectionGroup(
    BoardTest_U16 selectionId,
    BoardTest_U16 passedMask,
    BoardTest_U16 failedMask,
    BoardTest_U16 channelCount,
    BoardTest_Record *record)
{
    BoardTest_U16 bitIndex;
    BoardTest_U16 passedCount;

    passedCount = 0U;
    for(bitIndex = 0U;
        bitIndex < BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT;
        bitIndex++)
    {
        if((passedMask & ((BoardTest_U16)1U << bitIndex)) != 0U)
        {
            passedCount++;
        }
    }

    record->rawValue = ((BoardTest_U32)passedMask << 16U) |
                       (BoardTest_U32)failedMask;
    record->measuredValue = (float)passedCount;
    record->expectedMin = (float)BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT;
    record->expectedMax = (float)BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT;

    if(((selectionId == BOARD_ADC_EXTERNAL_SELECTION_GROUP_1) ||
        (selectionId == BOARD_ADC_EXTERNAL_SELECTION_GROUP_2)) &&
       (channelCount == BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT) &&
       (passedMask == BOARD_ADC_EXTERNAL_GROUP_REQUIRED_MASK) &&
       (failedMask == 0U))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_ADC_EXTERNAL_INJECTION;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardAdc_EvaluateMuxInjection(
    BoardTest_U16 selectionId,
    BoardTest_U16 passedMask,
    BoardTest_U16 failedMask,
    BoardTest_U16 channelCount,
    BoardTest_Record *record)
{
    BoardTest_U16 bitIndex;
    BoardTest_U16 passedCount;
    BoardTest_U16 requiredMask;
    BoardTest_U16 maximumCount;

    maximumCount = (channelCount < BOARD_ADC_MUX_CHANNEL_COUNT) ?
                   channelCount : BOARD_ADC_MUX_CHANNEL_COUNT;
    requiredMask = (maximumCount == 1U) ? 0x0001U : 0x0003U;
    passedCount = 0U;
    for(bitIndex = 0U; bitIndex < maximumCount; bitIndex++)
    {
        if((passedMask & ((BoardTest_U16)1U << bitIndex)) != 0U)
        {
            passedCount++;
        }
    }

    record->rawValue = (((BoardTest_U32)selectionId & 0x00FFUL) << 24U) |
                       (((BoardTest_U32)passedMask & 0x00FFUL) << 16U) |
                       (((BoardTest_U32)failedMask & 0x00FFUL) << 8U) |
                       ((BoardTest_U32)channelCount & 0x00FFUL);
    record->measuredValue = (float)passedCount;
    record->expectedMin = (float)channelCount;
    record->expectedMax = (float)channelCount;

    if((BoardAdc_IsExternalInjectionSelectionSupported(selectionId) != 0U) &&
       (channelCount != 0U) &&
       (channelCount <= BOARD_ADC_MUX_CHANNEL_COUNT) &&
       (passedMask == requiredMask) &&
       (failedMask == 0U))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_ADC_MUX_INJECTION;
    return BOARD_TEST_RESULT_FAIL;
}

static void BoardAdc_GetProtectionExpectedRange(
    BoardTest_U16 selectionId,
    float *minimumVoltage,
    float *maximumVoltage)
{
    if(selectionId < BOARD_ADC_PROTECTION_SELECTION_OC_LOW_BASE)
    {
        *minimumVoltage = BOARD_ADC_PROTECTION_OC_HIGH_THRESHOLD_V;
        *maximumVoltage = BOARD_ADC_PROTECTION_TERMINAL_MAX_V;
    }
    else if(selectionId < BOARD_ADC_PROTECTION_SELECTION_UOV_BASE)
    {
        *minimumVoltage = 0.0F;
        *maximumVoltage = BOARD_ADC_PROTECTION_OC_LOW_THRESHOLD_V;
    }
    else
    {
        *minimumVoltage = BOARD_ADC_PROTECTION_UOV_THRESHOLD_V;
        *maximumVoltage = BOARD_ADC_PROTECTION_TERMINAL_MAX_V;
    }
}

BoardTest_Result BoardAdc_EvaluateProtectionThreshold(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 selectionId,
    BoardTest_U16 triggeredFaultMask,
    BoardTest_U16 finalNormalMask,
    float faultVoltage,
    BoardTest_Record *record)
{
    float minimumVoltage;
    float maximumVoltage;

    BoardAdc_GetProtectionExpectedRange(selectionId,
                                         &minimumVoltage,
                                         &maximumVoltage);
    record->rawValue =
        (((BoardTest_U32)selectionId & 0x000FUL) << 28U) |
        BOARD_ADC_PROTECTION_RECORD_FORMAT_V2 |
        (((BoardTest_U32)statusMask & 0x01FFUL) << 16U) |
        (((BoardTest_U32)triggeredFaultMask & 0x000FUL) << 8U) |
        ((BoardTest_U32)finalNormalMask & 0x000FUL);
    record->measuredValue = faultVoltage;
    record->expectedMin = minimumVoltage;
    record->expectedMax = maximumVoltage;

    if((BoardAdc_IsProtectionSelectionSupported(selectionId) != 0U) &&
       ((statusMask & BOARD_ADC_PROTECTION_REQUIRED_STATUS_MASK) ==
        BOARD_ADC_PROTECTION_REQUIRED_STATUS_MASK) &&
       (failCode == BOARD_ADC_PROTECTION_FAIL_NONE) &&
       (faultVoltage >= minimumVoltage) &&
       (faultVoltage <= maximumVoltage))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_PROTECTION_FAULT_DI_EXTERNAL;
    return (failCode == BOARD_ADC_PROTECTION_FAIL_TIMEOUT) ?
           BOARD_TEST_RESULT_TIMEOUT : BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST

#define BOARD_ADC_TIMEOUT       200000UL
#define BOARD_ADC_ACQPS_12BIT   14U
#define BOARD_ADC_PROTECTION_INITIAL_TIMEOUT_US 30000000UL
#define BOARD_ADC_PROTECTION_INITIAL_STABLE_US  200000UL
#define BOARD_ADC_PROTECTION_PHASE_TIMEOUT_US   30000000UL
#define BOARD_ADC_PROTECTION_RESPONSE_TIMEOUT_US 10000000UL
#define BOARD_ADC_PROTECTION_TIMER_DIVIDER      199U
#define BOARD_ADC_PROTECTION_SAMPLE_INTERVAL_US 1000UL
#define BOARD_ADC_PROTECTION_CLEAR_PULSE_US     10000UL
#define BOARD_ADC_PROTECTION_BASELINE_CLEAR_RETRY_US 1000000UL

static volatile BoardTest_U16 BoardAdc_ProtectionXintCount = 0U;

typedef struct
{
    BoardTest_U16 active;
    BoardTest_U16 selectionId;
    BoardTest_U16 signalId;
    BoardTest_U16 faultBit;
    BoardTest_U16 mode;
    BoardTest_U16 phase;
    BoardTest_U16 waitSampleCount;
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 timerConfigured;
    BoardTest_U16 normalStableActive;
    BoardTest_U16 faultVoltageObserved;
    BoardTest_U16 xintConfigured;
    BoardTest_U32 phaseStartCounter;
    BoardTest_U32 normalStableStartCounter;
    BoardTest_U32 faultVoltageStartCounter;
    BoardTest_U32 baselineClearStartCounter;
    BoardTest_U32 previousTimerPeriod;
    BoardTest_U32 previousTimerCounter;
    BoardTest_U16 previousTimerTcr;
    BoardTest_U16 previousTimerTpr;
    BoardTest_U16 previousTimerTprh;
    BoardTest_U16 previousXintControl;
    BoardTest_U16 previousPieEnable;
    BoardTest_U16 previousIer;
    BoardTest_U16 previousInputSelect;
    PINT previousXintVector;
    BoardAdc_ChannelConfig config;
} BoardAdc_ProtectionRunState;

static BoardAdc_ProtectionRunState BoardAdc_ProtectionState =
{
    0U
};

interrupt void BoardAdc_ProtectionXint1Isr(void)
{
    BoardAdc_ProtectionXintCount++;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

static void BoardAdc_InitModule(volatile struct ADC_REGS *adcRegs)
{
    adcRegs->ADCCTL2.bit.PRESCALE = 6U;
    adcRegs->ADCCTL2.bit.RESOLUTION = 0U;
    adcRegs->ADCCTL2.bit.SIGNALMODE = 0U;
    adcRegs->ADCCTL1.bit.INTPULSEPOS = 1U;
    adcRegs->ADCCTL1.bit.ADCPWDNZ = 1U;
}

static BoardTest_U16 BoardAdc_InitAllModules(void)
{
    BoardTest_U16 statusMask;

    statusMask = 0U;
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

    return statusMask;
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

    statusMask = BoardAdc_InitAllModules();

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

static BoardTest_U16 BoardAdc_FindExternalChannelConfig(
    const BoardProfile_HardwareDescriptor *hardware,
    BoardTest_U16 signalId,
    BoardAdc_ChannelConfig *config)
{
    BoardTest_U16 channelIndex;

    for(channelIndex = 0U;
        channelIndex < BOARD_ADC_LOW_ALTITUDE_CHANNEL_COUNT;
        channelIndex++)
    {
        if((BoardAdc_GetChannelConfig(hardware->boardId,
                                      hardware->hardwareRevision,
                                      channelIndex,
                                      config) != 0U) &&
           (config->signalId == signalId))
        {
            return 1U;
        }
    }

    return 0U;
}

BoardTest_Result BoardAdc_RunExternalInjectionTest(BoardTest_Record *record)
{
    const BoardProfile_HardwareDescriptor *hardware;
    BoardAdc_ChannelConfig config;
    BoardTest_U32 rawSum;
    BoardTest_U16 averageRaw;
    BoardTest_U16 minimumRaw;
    BoardTest_U16 maximumRaw;
    BoardTest_U16 rawValue;
    BoardTest_U16 sampleIndex;
    BoardTest_U16 statusMask;
    BoardTest_Result result;
    float expectedVoltage;
    float toleranceVoltage;

    gBoardAdcExternalInjectionSnapshot.statusMask = 0U;
    gBoardAdcExternalInjectionSnapshot.failCode = 0U;
    gBoardAdcExternalInjectionSnapshot.sampleCount = 0U;
    gBoardAdcExternalInjectionSnapshot.averageRaw = 0U;
    gBoardAdcExternalInjectionSnapshot.minimumRaw = BOARD_ADC_RAW_MAX;
    gBoardAdcExternalInjectionSnapshot.maximumRaw = 0U;

    hardware = BoardProfile_GetCurrentHardware();
    if((hardware == 0) ||
       (hardware->boardId != BOARD_PROFILE_ID_LOW_ALTITUDE_UNIFIED) ||
       (hardware->hardwareRevision !=
        BOARD_PROFILE_HARDWARE_REVISION_LOW_ALTITUDE_V01) ||
       (gBoardAdcExternalInjectionConfig.configured == 0U))
    {
        gBoardAdcExternalInjectionSnapshot.failCode = 1U;
        record->errorCode = BOARD_TEST_ERROR_ADC_EXTERNAL_INJECTION;
        return BOARD_TEST_RESULT_FAIL;
    }

    if((BoardAdc_FindExternalChannelConfig(
            hardware,
            gBoardAdcExternalInjectionConfig.signalId,
            &config) == 0U) ||
       (BoardAdc_IsExternalInjectionSignalSupported(config.signalId) == 0U))
    {
        gBoardAdcExternalInjectionSnapshot.failCode = 2U;
        record->errorCode = BOARD_TEST_ERROR_ADC_EXTERNAL_INJECTION;
        return BOARD_TEST_RESULT_FAIL;
    }

    statusMask = BoardAdc_InitAllModules();
    statusMask |= BOARD_ADC_BASIC_SOC_CONFIGURED;
    rawSum = 0UL;
    minimumRaw = BOARD_ADC_RAW_MAX;
    maximumRaw = 0U;
    for(sampleIndex = 0U;
        sampleIndex < BOARD_ADC_EXTERNAL_SAMPLE_COUNT;
        sampleIndex++)
    {
        if(BoardAdc_RunConfiguredChannel(&config, &rawValue) == 0U)
        {
            gBoardAdcExternalInjectionSnapshot.failCode = 3U;
            break;
        }
        rawSum += rawValue;
        if(rawValue < minimumRaw)
        {
            minimumRaw = rawValue;
        }
        if(rawValue > maximumRaw)
        {
            maximumRaw = rawValue;
        }
        gBoardAdcExternalInjectionSnapshot.sampleCount++;
    }

    if(gBoardAdcExternalInjectionSnapshot.sampleCount ==
       BOARD_ADC_EXTERNAL_SAMPLE_COUNT)
    {
        averageRaw = (BoardTest_U16)
            ((rawSum + (BOARD_ADC_EXTERNAL_SAMPLE_COUNT / 2U)) /
             BOARD_ADC_EXTERNAL_SAMPLE_COUNT);
        statusMask |= BOARD_ADC_BASIC_CONVERSION_DONE;
        statusMask |= BOARD_ADC_BASIC_RAW_IN_RANGE;
    }
    else
    {
        averageRaw = 0U;
    }

    expectedVoltage =
        (float)gBoardAdcExternalInjectionConfig.expectedMillivolts /
        1000.0F;
    toleranceVoltage =
        (float)gBoardAdcExternalInjectionConfig.toleranceMillivolts /
        1000.0F;
    gBoardAdcExternalInjectionSnapshot.statusMask = statusMask;
    gBoardAdcExternalInjectionSnapshot.signalId = config.signalId;
    gBoardAdcExternalInjectionSnapshot.module = config.module;
    gBoardAdcExternalInjectionSnapshot.channel = config.channel;
    gBoardAdcExternalInjectionSnapshot.averageRaw = averageRaw;
    gBoardAdcExternalInjectionSnapshot.minimumRaw = minimumRaw;
    gBoardAdcExternalInjectionSnapshot.maximumRaw = maximumRaw;
    gBoardAdcExternalInjectionSnapshot.expectedMillivolts =
        gBoardAdcExternalInjectionConfig.expectedMillivolts;
    gBoardAdcExternalInjectionSnapshot.toleranceMillivolts =
        gBoardAdcExternalInjectionConfig.toleranceMillivolts;
    gBoardAdcExternalInjectionSnapshot.adcPinVoltage =
        BoardAdc_RawToVoltage(averageRaw);
    gBoardAdcExternalInjectionSnapshot.terminalVoltage =
        BoardAdc_RawToExternalTerminalVoltage(averageRaw);

    result = BoardAdc_EvaluateExternalInjection(statusMask,
                                                averageRaw,
                                                expectedVoltage,
                                                toleranceVoltage,
                                                record);
    if((result != BOARD_TEST_RESULT_PASS) &&
       (gBoardAdcExternalInjectionSnapshot.failCode == 0U))
    {
        gBoardAdcExternalInjectionSnapshot.failCode = 4U;
    }
    return result;
}

BoardTest_Result BoardAdc_RunExternalInjectionGroupTest(
    BoardTest_U16 selectionId,
    BoardTest_Record *record)
{
    const BoardProfile_HardwareDescriptor *hardware;
    volatile BoardAdc_ExternalGroupSnapshot *snapshot;
    BoardAdc_ChannelConfig config;
    BoardTest_Record channelRecord;
    BoardTest_U32 rawSum;
    BoardTest_U16 rawValue;
    BoardTest_U16 averageRaw;
    BoardTest_U16 minimumRaw;
    BoardTest_U16 maximumRaw;
    BoardTest_U16 sampleCount;
    BoardTest_U16 sampleIndex;
    BoardTest_U16 channelIndex;
    BoardTest_U16 signalId;
    BoardTest_U16 statusMask;
    BoardTest_U16 passedMask;
    BoardTest_U16 failedMask;
    BoardTest_U16 snapshotIndex;
    BoardTest_Result channelResult;
    float expectedVoltage;
    float toleranceVoltage;

    if(selectionId == BOARD_ADC_EXTERNAL_SELECTION_GROUP_1)
    {
        snapshotIndex = 0U;
    }
    else if(selectionId == BOARD_ADC_EXTERNAL_SELECTION_GROUP_2)
    {
        snapshotIndex = 1U;
    }
    else
    {
        record->errorCode = BOARD_TEST_ERROR_ADC_EXTERNAL_INJECTION;
        return BOARD_TEST_RESULT_FAIL;
    }

    snapshot = &gBoardAdcExternalGroupSnapshots[snapshotIndex];
    snapshot->statusMask = 0U;
    snapshot->failCode = 0U;
    snapshot->selectionId = selectionId;
    snapshot->channelCount = BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT;
    snapshot->passedMask = 0U;
    snapshot->failedMask = 0U;
    for(channelIndex = 0U;
        channelIndex < BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT;
        channelIndex++)
    {
        snapshot->signalIds[channelIndex] = 0U;
        snapshot->modules[channelIndex] = 0U;
        snapshot->channels[channelIndex] = 0U;
        snapshot->averageRaw[channelIndex] = 0U;
        snapshot->minimumRaw[channelIndex] = BOARD_ADC_RAW_MAX;
        snapshot->maximumRaw[channelIndex] = 0U;
        snapshot->terminalVoltages[channelIndex] = 0.0F;
        snapshot->results[channelIndex] = BOARD_TEST_RESULT_NOT_RUN;
    }

    hardware = BoardProfile_GetCurrentHardware();
    if((hardware == 0) ||
       (hardware->boardId != BOARD_PROFILE_ID_LOW_ALTITUDE_UNIFIED) ||
       (hardware->hardwareRevision !=
        BOARD_PROFILE_HARDWARE_REVISION_LOW_ALTITUDE_V01) ||
       (gBoardAdcExternalInjectionConfig.configured == 0U) ||
       (gBoardAdcExternalInjectionConfig.signalId != selectionId))
    {
        snapshot->failCode = 0x0100U;
        record->errorCode = BOARD_TEST_ERROR_ADC_EXTERNAL_INJECTION;
        return BOARD_TEST_RESULT_FAIL;
    }

    statusMask = BoardAdc_InitAllModules();
    statusMask |= BOARD_ADC_BASIC_SOC_CONFIGURED;
    expectedVoltage =
        (float)gBoardAdcExternalInjectionConfig.expectedMillivolts / 1000.0F;
    toleranceVoltage =
        (float)gBoardAdcExternalInjectionConfig.toleranceMillivolts / 1000.0F;
    passedMask = 0U;
    failedMask = 0U;

    for(channelIndex = 0U;
        channelIndex < BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT;
        channelIndex++)
    {
        if((BoardAdc_GetExternalInjectionGroupSignal(selectionId,
                                                     channelIndex,
                                                     &signalId) == 0U) ||
           (BoardAdc_FindExternalChannelConfig(hardware,
                                               signalId,
                                               &config) == 0U))
        {
            failedMask |= ((BoardTest_U16)1U << channelIndex);
            snapshot->results[channelIndex] = BOARD_TEST_RESULT_FAIL;
            if(snapshot->failCode == 0U)
            {
                snapshot->failCode = (BoardTest_U16)(channelIndex + 1U);
            }
            continue;
        }

        snapshot->signalIds[channelIndex] = config.signalId;
        snapshot->modules[channelIndex] = config.module;
        snapshot->channels[channelIndex] = config.channel;
        rawSum = 0UL;
        minimumRaw = BOARD_ADC_RAW_MAX;
        maximumRaw = 0U;
        sampleCount = 0U;
        for(sampleIndex = 0U;
            sampleIndex < BOARD_ADC_EXTERNAL_SAMPLE_COUNT;
            sampleIndex++)
        {
            if(BoardAdc_RunConfiguredChannel(&config, &rawValue) == 0U)
            {
                break;
            }
            rawSum += rawValue;
            if(rawValue < minimumRaw)
            {
                minimumRaw = rawValue;
            }
            if(rawValue > maximumRaw)
            {
                maximumRaw = rawValue;
            }
            sampleCount++;
        }

        if(sampleCount == BOARD_ADC_EXTERNAL_SAMPLE_COUNT)
        {
            averageRaw = (BoardTest_U16)
                ((rawSum + (BOARD_ADC_EXTERNAL_SAMPLE_COUNT / 2U)) /
                 BOARD_ADC_EXTERNAL_SAMPLE_COUNT);
            channelResult = BoardAdc_EvaluateExternalInjection(
                (BoardTest_U16)(statusMask |
                                BOARD_ADC_BASIC_CONVERSION_DONE |
                                BOARD_ADC_BASIC_RAW_IN_RANGE),
                averageRaw,
                expectedVoltage,
                toleranceVoltage,
                &channelRecord);
        }
        else
        {
            averageRaw = 0U;
            channelResult = BoardAdc_EvaluateExternalInjection(
                statusMask,
                averageRaw,
                expectedVoltage,
                toleranceVoltage,
                &channelRecord);
        }

        snapshot->averageRaw[channelIndex] = averageRaw;
        snapshot->minimumRaw[channelIndex] = minimumRaw;
        snapshot->maximumRaw[channelIndex] = maximumRaw;
        snapshot->terminalVoltages[channelIndex] =
            BoardAdc_RawToExternalTerminalVoltage(averageRaw);
        snapshot->results[channelIndex] = channelResult;
        if(channelResult == BOARD_TEST_RESULT_PASS)
        {
            passedMask |= ((BoardTest_U16)1U << channelIndex);
        }
        else
        {
            failedMask |= ((BoardTest_U16)1U << channelIndex);
            if(snapshot->failCode == 0U)
            {
                snapshot->failCode = (BoardTest_U16)(channelIndex + 1U);
            }
        }
    }

    snapshot->passedMask = passedMask;
    snapshot->failedMask = failedMask;
    if((passedMask == BOARD_ADC_EXTERNAL_GROUP_REQUIRED_MASK) &&
       (failedMask == 0U))
    {
        statusMask |= BOARD_ADC_BASIC_CONVERSION_DONE;
        statusMask |= BOARD_ADC_BASIC_RAW_IN_RANGE;
    }
    snapshot->statusMask = statusMask;

    return BoardAdc_EvaluateExternalInjectionGroup(
        selectionId,
        passedMask,
        failedMask,
        BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT,
        record);
}

static BoardTest_U16 BoardAdc_AreSelectorPinsValid(
    const BoardProfile_HardwareDescriptor *hardware,
    BoardTest_U16 testId)
{
    if((hardware == 0) ||
       (hardware->boardId != BOARD_PROFILE_ID_LOW_ALTITUDE_UNIFIED) ||
       (hardware->hardwareRevision !=
        BOARD_PROFILE_HARDWARE_REVISION_LOW_ALTITUDE_V01))
    {
        return 0U;
    }

    if(testId == BOARD_TEST_ID_ADC_PT100_MUX_EXTERNAL)
    {
        return ((hardware->pins.pt100Select0 != BOARD_PROFILE_PIN_UNUSED) &&
                (hardware->pins.pt100Select1 != BOARD_PROFILE_PIN_UNUSED)) ?
               1U : 0U;
    }

    return ((hardware->pins.ntcFaultSelect0 != BOARD_PROFILE_PIN_UNUSED) &&
            (hardware->pins.ntcFaultSelect1 != BOARD_PROFILE_PIN_UNUSED) &&
            (hardware->pins.ntcFaultSelect2 != BOARD_PROFILE_PIN_UNUSED)) ?
           1U : 0U;
}

static void BoardAdc_WriteSelector(
    const BoardProfile_HardwareDescriptor *hardware,
    BoardTest_U16 testId,
    BoardTest_U16 selectorCode)
{
    BoardTest_U16 select0;
    BoardTest_U16 select1;
    BoardTest_U16 select2;

    if(testId == BOARD_TEST_ID_ADC_PT100_MUX_EXTERNAL)
    {
        select0 = hardware->pins.pt100Select0;
        select1 = hardware->pins.pt100Select1;
        GPIO_SetupPinMux(select0, GPIO_MUX_CPU1, 0U);
        GPIO_SetupPinMux(select1, GPIO_MUX_CPU1, 0U);
        GPIO_WritePin(select0, selectorCode & 0x0001U);
        GPIO_WritePin(select1, (selectorCode >> 1U) & 0x0001U);
        GPIO_SetupPinOptions(select0, GPIO_OUTPUT, 0U);
        GPIO_SetupPinOptions(select1, GPIO_OUTPUT, 0U);
        return;
    }

    select0 = hardware->pins.ntcFaultSelect0;
    select1 = hardware->pins.ntcFaultSelect1;
    select2 = hardware->pins.ntcFaultSelect2;
    GPIO_SetupPinMux(select0, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinMux(select1, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinMux(select2, GPIO_MUX_CPU1, 0U);
    GPIO_WritePin(select0, selectorCode & 0x0001U);
    GPIO_WritePin(select1, (selectorCode >> 1U) & 0x0001U);
    GPIO_WritePin(select2, (selectorCode >> 2U) & 0x0001U);
    GPIO_SetupPinOptions(select0, GPIO_OUTPUT, 0U);
    GPIO_SetupPinOptions(select1, GPIO_OUTPUT, 0U);
    GPIO_SetupPinOptions(select2, GPIO_OUTPUT, 0U);
}

static BoardTest_U16 BoardAdc_GetMuxSignal(
    BoardTest_U16 testId,
    BoardTest_U16 selectionId,
    BoardTest_U16 channelIndex,
    BoardTest_U16 *signalId)
{
    if(signalId == 0)
    {
        return 0U;
    }

    if(testId == BOARD_TEST_ID_ADC_PT100_MUX_EXTERNAL)
    {
        if(channelIndex != 0U)
        {
            return 0U;
        }
        *signalId = BOARD_ADC_SIGNAL_MOTOR_TEMPERATURE;
        return 1U;
    }

    if((testId == BOARD_TEST_ID_ADC_FAULT_MUX_EXTERNAL) &&
       (channelIndex == 0U))
    {
        if((selectionId >=
            BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_1_BASE) &&
           (selectionId <
            (BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_1_BASE +
             BOARD_ADC_EXTERNAL_SELECTION_FAULT_COUNT)))
        {
            *signalId = BOARD_ADC_SIGNAL_FAULT_1;
            return 1U;
        }

        if((selectionId >=
            BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_2_BASE) &&
           (selectionId <
            (BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_2_BASE +
             BOARD_ADC_EXTERNAL_SELECTION_FAULT_COUNT)))
        {
            *signalId = BOARD_ADC_SIGNAL_FAULT_2;
            return 1U;
        }
    }

    return 0U;
}

BoardTest_Result BoardAdc_RunMuxInjectionTest(
    BoardTest_U16 testId,
    BoardTest_Record *record)
{
    const BoardProfile_HardwareDescriptor *hardware;
    BoardAdc_ChannelConfig config;
    BoardTest_Record channelRecord;
    BoardTest_U32 rawSum;
    BoardTest_U16 averageRaw;
    BoardTest_U16 rawValue;
    BoardTest_U16 sampleIndex;
    BoardTest_U16 sampleCount;
    BoardTest_U16 channelIndex;
    BoardTest_U16 channelCount;
    BoardTest_U16 signalId;
    BoardTest_U16 selectorCode;
    BoardTest_U16 statusMask;
    BoardTest_U16 passedMask;
    BoardTest_U16 failedMask;
    BoardTest_Result channelResult;
    float expectedVoltage;
    float toleranceVoltage;
    float terminalVoltage;

    gBoardAdcMuxInjectionSnapshot.statusMask = 0U;
    gBoardAdcMuxInjectionSnapshot.failCode = 0U;
    gBoardAdcMuxInjectionSnapshot.selectionId =
        gBoardAdcExternalInjectionConfig.signalId;
    gBoardAdcMuxInjectionSnapshot.selectorCode = 0U;
    gBoardAdcMuxInjectionSnapshot.channelCount = 0U;
    gBoardAdcMuxInjectionSnapshot.passedMask = 0U;
    gBoardAdcMuxInjectionSnapshot.failedMask = 0U;

    hardware = BoardProfile_GetCurrentHardware();
    if((record == 0) ||
       (gBoardAdcExternalInjectionConfig.configured == 0U) ||
       (BoardAdc_IsMuxSelectionSupported(
            testId,
            gBoardAdcExternalInjectionConfig.signalId) == 0U))
    {
        gBoardAdcMuxInjectionSnapshot.failCode = 1U;
        if(record != 0)
        {
            record->errorCode = BOARD_TEST_ERROR_ADC_MUX_INJECTION;
        }
        return BOARD_TEST_RESULT_FAIL;
    }

    if((BoardAdc_AreSelectorPinsValid(hardware, testId) == 0U) ||
       (BoardAdc_GetMuxSelectorCode(
            gBoardAdcExternalInjectionConfig.signalId,
            &selectorCode) == 0U))
    {
        gBoardAdcMuxInjectionSnapshot.failCode = 2U;
        record->errorCode = BOARD_TEST_ERROR_ADC_MUX_INJECTION;
        return BOARD_TEST_RESULT_FAIL;
    }

    channelCount = 1U;
    gBoardAdcMuxInjectionSnapshot.selectorCode = selectorCode;
    gBoardAdcMuxInjectionSnapshot.channelCount = channelCount;
    statusMask = BoardAdc_InitAllModules();
    statusMask |= BOARD_ADC_BASIC_SOC_CONFIGURED;
    BoardAdc_WriteSelector(hardware, testId, selectorCode);
    DELAY_US(BOARD_ADC_MUX_SETTLE_TIME_US);

    expectedVoltage =
        (float)gBoardAdcExternalInjectionConfig.expectedMillivolts / 1000.0F;
    toleranceVoltage =
        (float)gBoardAdcExternalInjectionConfig.toleranceMillivolts / 1000.0F;
    passedMask = 0U;
    failedMask = 0U;

    for(channelIndex = 0U; channelIndex < channelCount; channelIndex++)
    {
        averageRaw = 0U;
        rawSum = 0UL;
        sampleCount = 0U;
        signalId = BOARD_ADC_SIGNAL_IU1;
        if((BoardAdc_GetMuxSignal(
                testId,
                gBoardAdcExternalInjectionConfig.signalId,
                channelIndex,
                &signalId) == 0U) ||
           (BoardAdc_FindExternalChannelConfig(
                hardware, signalId, &config) == 0U))
        {
            gBoardAdcMuxInjectionSnapshot.failCode = 3U;
            failedMask |= ((BoardTest_U16)1U << channelIndex);
            continue;
        }

        gBoardAdcMuxInjectionSnapshot.signalIds[channelIndex] = signalId;
        gBoardAdcMuxInjectionSnapshot.modules[channelIndex] = config.module;
        gBoardAdcMuxInjectionSnapshot.channels[channelIndex] = config.channel;
        for(sampleIndex = 0U;
            sampleIndex < BOARD_ADC_EXTERNAL_SAMPLE_COUNT;
            sampleIndex++)
        {
            if(BoardAdc_RunConfiguredChannel(&config, &rawValue) == 0U)
            {
                break;
            }
            rawSum += rawValue;
            sampleCount++;
        }

        if(sampleCount == BOARD_ADC_EXTERNAL_SAMPLE_COUNT)
        {
            averageRaw = (BoardTest_U16)
                ((rawSum + (BOARD_ADC_EXTERNAL_SAMPLE_COUNT / 2U)) /
                 BOARD_ADC_EXTERNAL_SAMPLE_COUNT);
            if(testId == BOARD_TEST_ID_ADC_PT100_MUX_EXTERNAL)
            {
                channelResult = BoardAdc_EvaluatePt100Injection(
                    (BoardTest_U16)(statusMask |
                                    BOARD_ADC_BASIC_CONVERSION_DONE |
                                    BOARD_ADC_BASIC_RAW_IN_RANGE),
                    averageRaw,
                    expectedVoltage,
                    toleranceVoltage,
                    &channelRecord);
            }
            else
            {
                channelResult = BoardAdc_EvaluateExternalInjection(
                    (BoardTest_U16)(statusMask |
                                    BOARD_ADC_BASIC_CONVERSION_DONE |
                                    BOARD_ADC_BASIC_RAW_IN_RANGE),
                    averageRaw,
                    expectedVoltage,
                    toleranceVoltage,
                    &channelRecord);
            }
        }
        else
        {
            if(testId == BOARD_TEST_ID_ADC_PT100_MUX_EXTERNAL)
            {
                channelResult = BoardAdc_EvaluatePt100Injection(
                    statusMask,
                    0U,
                    expectedVoltage,
                    toleranceVoltage,
                    &channelRecord);
            }
            else
            {
                channelResult = BoardAdc_EvaluateExternalInjection(
                    statusMask,
                    0U,
                    expectedVoltage,
                    toleranceVoltage,
                    &channelRecord);
            }
        }

        terminalVoltage =
            (testId == BOARD_TEST_ID_ADC_PT100_MUX_EXTERNAL) ?
            BoardAdc_RawToVoltage(averageRaw) :
            BoardAdc_RawToExternalTerminalVoltage(averageRaw);
        gBoardAdcMuxInjectionSnapshot.averageRaw[channelIndex] = averageRaw;
        gBoardAdcMuxInjectionSnapshot.terminalVoltages[channelIndex] =
            terminalVoltage;
        if(testId == BOARD_TEST_ID_ADC_PT100_MUX_EXTERNAL)
        {
            gBoardAdcMuxInjectionSnapshot.convertedValues[channelIndex] =
                BoardAdc_Pt100VoltageToResistance(terminalVoltage);
        }
        else
        {
            gBoardAdcMuxInjectionSnapshot.convertedValues[channelIndex] =
                BoardAdc_FaultVoltageToDutyPercent(terminalVoltage);
        }
        gBoardAdcMuxInjectionSnapshot.results[channelIndex] = channelResult;
        if(channelResult == BOARD_TEST_RESULT_PASS)
        {
            passedMask |= ((BoardTest_U16)1U << channelIndex);
        }
        else
        {
            failedMask |= ((BoardTest_U16)1U << channelIndex);
            if(gBoardAdcMuxInjectionSnapshot.failCode == 0U)
            {
                gBoardAdcMuxInjectionSnapshot.failCode =
                    (BoardTest_U16)(channelIndex + 4U);
            }
        }
    }

    if(testId == BOARD_TEST_ID_ADC_PT100_MUX_EXTERNAL)
    {
        BoardAdc_WriteSelector(hardware, testId, 3U);
    }
    else
    {
        BoardAdc_WriteSelector(hardware, testId, 7U);
    }
    gBoardAdcMuxInjectionSnapshot.statusMask = statusMask;
    gBoardAdcMuxInjectionSnapshot.passedMask = passedMask;
    gBoardAdcMuxInjectionSnapshot.failedMask = failedMask;

    channelResult = BoardAdc_EvaluateMuxInjection(
        gBoardAdcExternalInjectionConfig.signalId,
        passedMask,
        failedMask,
        channelCount,
        record);

    if((testId == BOARD_TEST_ID_ADC_PT100_MUX_EXTERNAL) &&
       (channelCount == 1U))
    {
        record->rawValue =
            (((BoardTest_U32)gBoardAdcExternalInjectionConfig.signalId &
              0x00FFUL) << 24U) |
            (((BoardTest_U32)gBoardAdcMuxInjectionSnapshot.signalIds[0] &
              0x00FFUL) << 16U) |
            ((BoardTest_U32)gBoardAdcMuxInjectionSnapshot.averageRaw[0] &
             0xFFFFUL);
        record->measuredValue =
            gBoardAdcMuxInjectionSnapshot.terminalVoltages[0];
        record->expectedMin = expectedVoltage - toleranceVoltage;
        if(record->expectedMin < 0.0F)
        {
            record->expectedMin = 0.0F;
        }
        record->expectedMax = expectedVoltage + toleranceVoltage;
        if(record->expectedMax > BOARD_ADC_REFERENCE_VOLTAGE_V)
        {
            record->expectedMax = BOARD_ADC_REFERENCE_VOLTAGE_V;
        }
    }
    else if((testId == BOARD_TEST_ID_ADC_FAULT_MUX_EXTERNAL) &&
       (channelCount == 1U))
    {
        record->rawValue =
            (((BoardTest_U32)gBoardAdcExternalInjectionConfig.signalId &
              0x00FFUL) << 24U) |
            (((BoardTest_U32)gBoardAdcMuxInjectionSnapshot.signalIds[0] &
              0x00FFUL) << 16U) |
            ((BoardTest_U32)gBoardAdcMuxInjectionSnapshot.averageRaw[0] &
             0xFFFFUL);
        record->measuredValue =
            gBoardAdcMuxInjectionSnapshot.terminalVoltages[0];
        record->expectedMin = expectedVoltage - toleranceVoltage;
        if(record->expectedMin < 0.0F)
        {
            record->expectedMin = 0.0F;
        }
        record->expectedMax = expectedVoltage + toleranceVoltage;
        if(record->expectedMax > BOARD_ADC_EXTERNAL_TERMINAL_FULL_SCALE_V)
        {
            record->expectedMax = BOARD_ADC_EXTERNAL_TERMINAL_FULL_SCALE_V;
        }
    }

    return channelResult;
}

static BoardTest_U16 BoardAdc_GetProtectionSelection(
    BoardTest_U16 selectionId,
    BoardTest_U16 *signalId,
    BoardTest_U16 *faultBit,
    BoardTest_U16 *mode)
{
    BoardTest_U16 currentSignal;

    if(BoardAdc_IsProtectionSelectionSupported(selectionId) == 0U)
    {
        return 0U;
    }

    if(selectionId < BOARD_ADC_PROTECTION_SELECTION_OC_LOW_BASE)
    {
        currentSignal = selectionId;
        *mode = BOARD_ADC_PROTECTION_MODE_OC_HIGH;
    }
    else if(selectionId < BOARD_ADC_PROTECTION_SELECTION_UOV_BASE)
    {
        currentSignal =
            selectionId - BOARD_ADC_PROTECTION_SELECTION_OC_LOW_BASE;
        *mode = BOARD_ADC_PROTECTION_MODE_OC_LOW;
    }
    else
    {
        currentSignal = BOARD_ADC_SIGNAL_UDC1 +
            (selectionId - BOARD_ADC_PROTECTION_SELECTION_UOV_BASE);
        *mode = BOARD_ADC_PROTECTION_MODE_UOV_HIGH;
    }

    *signalId = currentSignal;
    if(currentSignal <= BOARD_ADC_SIGNAL_IW1)
    {
        *faultBit = BOARD_DI_PROTECTION_OC1_BIT;
    }
    else if(currentSignal <= BOARD_ADC_SIGNAL_IW2)
    {
        *faultBit = BOARD_DI_PROTECTION_OC2_BIT;
    }
    else if(currentSignal == BOARD_ADC_SIGNAL_UDC1)
    {
        *faultBit = BOARD_DI_PROTECTION_UOV1_BIT;
    }
    else
    {
        *faultBit = BOARD_DI_PROTECTION_UOV2_BIT;
    }
    return 1U;
}

static BoardTest_U16 BoardAdc_IsProtectionVoltageNormal(
    BoardTest_U16 mode,
    float voltage)
{
    if((mode == BOARD_ADC_PROTECTION_MODE_OC_HIGH) ||
       (mode == BOARD_ADC_PROTECTION_MODE_OC_LOW))
    {
        return ((voltage > BOARD_ADC_PROTECTION_OC_LOW_THRESHOLD_V) &&
                (voltage < BOARD_ADC_PROTECTION_OC_HIGH_THRESHOLD_V)) ?
               1U : 0U;
    }

    return (voltage < BOARD_ADC_PROTECTION_UOV_THRESHOLD_V) ? 1U : 0U;
}

static BoardTest_U16 BoardAdc_IsProtectionVoltageFault(
    BoardTest_U16 mode,
    float voltage)
{
    if(mode == BOARD_ADC_PROTECTION_MODE_OC_HIGH)
    {
        return ((voltage >= BOARD_ADC_PROTECTION_OC_HIGH_THRESHOLD_V) &&
                (voltage <= BOARD_ADC_PROTECTION_TERMINAL_MAX_V)) ? 1U : 0U;
    }
    if(mode == BOARD_ADC_PROTECTION_MODE_OC_LOW)
    {
        return (voltage <= BOARD_ADC_PROTECTION_OC_LOW_THRESHOLD_V) ? 1U : 0U;
    }
    return ((voltage >= BOARD_ADC_PROTECTION_UOV_THRESHOLD_V) &&
            (voltage <= BOARD_ADC_PROTECTION_TERMINAL_MAX_V)) ? 1U : 0U;
}

static BoardTest_U16 BoardAdc_ReadProtectionFaultMask(
    const BoardProfile_PinMap *pins)
{
    BoardTest_U16 index;
    BoardTest_U16 mask;

    mask = 0U;
    for(index = 0U; index < BOARD_PROFILE_PROTECTION_FAULT_DI_COUNT; index++)
    {
        if((pins->protectionFaultInputs[index] != BOARD_PROFILE_PIN_UNUSED) &&
           (GPIO_ReadPin(pins->protectionFaultInputs[index]) == 0U))
        {
            mask |= ((BoardTest_U16)1U << index);
        }
    }
    return mask;
}

static void BoardAdc_PulseFaultClear(const BoardProfile_PinMap *pins)
{
    GPIO_WritePin(pins->faultClearOutput, 0U);
    DELAY_US(BOARD_ADC_PROTECTION_CLEAR_PULSE_US);
    GPIO_WritePin(pins->faultClearOutput, 1U);
    DELAY_US(BOARD_ADC_PROTECTION_CLEAR_PULSE_US);
    gBoardAdcProtectionSnapshot.clearPulseCount++;
}

static void BoardAdc_RestoreProtectionXint(void)
{
    if(BoardAdc_ProtectionState.xintConfigured == 0U)
    {
        return;
    }

    XintRegs.XINT1CR.bit.ENABLE = 0U;
    PieCtrlRegs.PIEIER1.all = BoardAdc_ProtectionState.previousPieEnable;
    IER = BoardAdc_ProtectionState.previousIer;
    EALLOW;
    TrigRegs.INPUT4SELECT = BoardAdc_ProtectionState.previousInputSelect;
    PieVectTable.XINT1_INT = BoardAdc_ProtectionState.previousXintVector;
    EDIS;
    XintRegs.XINT1CR.all = BoardAdc_ProtectionState.previousXintControl;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    BoardAdc_ProtectionState.xintConfigured = 0U;
}

static BoardTest_U32 BoardAdc_ProtectionElapsedUs(
    BoardTest_U32 startCounter)
{
    return startCounter - CpuTimer1Regs.TIM.all;
}

static void BoardAdc_ConfigureProtectionTimer(void)
{
    BoardAdc_ProtectionState.previousTimerPeriod = CpuTimer1Regs.PRD.all;
    BoardAdc_ProtectionState.previousTimerCounter = CpuTimer1Regs.TIM.all;
    BoardAdc_ProtectionState.previousTimerTcr = CpuTimer1Regs.TCR.all;
    BoardAdc_ProtectionState.previousTimerTpr = CpuTimer1Regs.TPR.all;
    BoardAdc_ProtectionState.previousTimerTprh = CpuTimer1Regs.TPRH.all;

    CpuTimer1Regs.TCR.bit.TSS = 1U;
    CpuTimer1Regs.PRD.all = 0xFFFFFFFFUL;
    CpuTimer1Regs.TPR.all = BOARD_ADC_PROTECTION_TIMER_DIVIDER;
    CpuTimer1Regs.TPRH.all = 0U;
    CpuTimer1Regs.TCR.bit.TIE = 0U;
    CpuTimer1Regs.TCR.bit.SOFT = 1U;
    CpuTimer1Regs.TCR.bit.FREE = 1U;
    CpuTimer1Regs.TCR.bit.TRB = 1U;
    CpuTimer1Regs.TCR.bit.TSS = 0U;

    BoardAdc_ProtectionState.phaseStartCounter = CpuTimer1Regs.TIM.all;
    BoardAdc_ProtectionState.timerConfigured = 1U;
}

static void BoardAdc_RestoreProtectionTimer(void)
{
    if(BoardAdc_ProtectionState.timerConfigured == 0U)
    {
        return;
    }

    CpuTimer1Regs.TCR.bit.TSS = 1U;
    CpuTimer1Regs.PRD.all = BoardAdc_ProtectionState.previousTimerPeriod;
    CpuTimer1Regs.TIM.all = BoardAdc_ProtectionState.previousTimerCounter;
    CpuTimer1Regs.TPR.all = BoardAdc_ProtectionState.previousTimerTpr;
    CpuTimer1Regs.TPRH.all = BoardAdc_ProtectionState.previousTimerTprh;
    CpuTimer1Regs.TCR.all = BoardAdc_ProtectionState.previousTimerTcr;
    BoardAdc_ProtectionState.timerConfigured = 0U;
}

static void BoardAdc_BeginProtectionPhase(BoardTest_U16 phase)
{
    BoardAdc_ProtectionState.phase = phase;
    BoardAdc_ProtectionState.phaseStartCounter = CpuTimer1Regs.TIM.all;
    BoardAdc_ProtectionState.waitSampleCount = 0U;
}

static BoardTest_U32 BoardAdc_UpdateProtectionElapsedTime(void)
{
    BoardTest_U32 elapsedUs;
    BoardTest_U32 elapsedMs;

    elapsedUs = BoardAdc_ProtectionElapsedUs(
        BoardAdc_ProtectionState.phaseStartCounter);
    elapsedMs = elapsedUs / 1000UL;
    BoardAdc_ProtectionState.waitSampleCount =
        (elapsedMs > 0xFFFFUL) ? 0xFFFFU : (BoardTest_U16)elapsedMs;
    return elapsedUs;
}

void BoardAdc_AbortProtectionThresholdTest(void)
{
    const BoardProfile_HardwareDescriptor *hardware;

    hardware = BoardProfile_GetCurrentHardware();
    if((hardware != 0) &&
       (hardware->pins.faultClearOutput != BOARD_PROFILE_PIN_UNUSED))
    {
        GPIO_WritePin(hardware->pins.faultClearOutput, 1U);
    }
    BoardAdc_RestoreProtectionXint();
    BoardAdc_RestoreProtectionTimer();
    BoardAdc_ProtectionState.active = 0U;
    BoardAdc_ProtectionState.phase = BOARD_ADC_PROTECTION_PHASE_IDLE;
    BoardAdc_ProtectionState.waitSampleCount = 0U;
    BoardAdc_ProtectionState.normalStableActive = 0U;
    BoardAdc_ProtectionState.faultVoltageObserved = 0U;
    gBoardAdcProtectionSnapshot.phase = BOARD_ADC_PROTECTION_PHASE_IDLE;
    gBoardAdcProtectionSnapshot.waitSampleCount = 0U;
}

static void BoardAdc_ConfigureProtectionXint(
    const BoardProfile_PinMap *pins)
{
    BoardAdc_ProtectionState.previousXintControl = XintRegs.XINT1CR.all;
    BoardAdc_ProtectionState.previousPieEnable = PieCtrlRegs.PIEIER1.all;
    BoardAdc_ProtectionState.previousIer = IER;
    BoardAdc_ProtectionState.previousInputSelect = TrigRegs.INPUT4SELECT;
    BoardAdc_ProtectionState.previousXintVector = PieVectTable.XINT1_INT;

    XintRegs.XINT1CR.bit.ENABLE = 0U;
    GPIO_SetupXINT1Gpio(pins->protectionBreakInput);
    EALLOW;
    PieVectTable.XINT1_INT = &BoardAdc_ProtectionXint1Isr;
    EDIS;
    BoardAdc_ProtectionXintCount = 0U;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1U;
    IER |= M_INT1;
    XintRegs.XINT1CR.bit.POLARITY = 1U;
    XintRegs.XINT1CR.bit.ENABLE = 1U;
    BoardAdc_ProtectionState.xintConfigured = 1U;
}

static void BoardAdc_UpdateProtectionRunningRecord(
    BoardTest_Record *record,
    float voltage,
    BoardTest_U16 faultMask,
    BoardTest_U16 breakLevel,
    BoardTest_U16 clearLevel)
{
    float phaseLimitSeconds;

    phaseLimitSeconds = 30.0F;
    record->errorCode = BOARD_TEST_ERROR_NONE;
    record->rawValue =
        (((BoardTest_U32)BoardAdc_ProtectionState.phase & 0x000FUL) << 28U) |
        BOARD_ADC_PROTECTION_RECORD_FORMAT_V2 |
        (((BoardTest_U32)BoardAdc_ProtectionState.statusMask & 0x01FFUL) <<
         16U) |
        (((BoardTest_U32)BoardAdc_ProtectionState.faultBit & 0x000FUL) <<
         12U) |
        (((BoardTest_U32)gBoardAdcProtectionSnapshot.triggeredFaultMask &
          0x000FUL) << 8U) |
        (((BoardTest_U32)gBoardAdcProtectionSnapshot.initialBreakLevel &
          0x0001UL) << 5U) |
        (((BoardTest_U32)clearLevel & 0x0001UL) << 6U) |
        ((gBoardAdcProtectionSnapshot.clearPulseCount != 0U) ?
            0x00000080UL : 0UL) |
        (((BoardTest_U32)breakLevel & 0x0001UL) << 4U) |
        ((BoardTest_U32)faultMask & 0x000FUL);
    record->measuredValue = voltage;
    if((BoardAdc_ProtectionState.phase ==
        BOARD_ADC_PROTECTION_PHASE_WAIT_FAULT) &&
       (BoardAdc_ProtectionState.faultVoltageObserved != 0U))
    {
        record->expectedMin =
            (float)BoardAdc_ProtectionElapsedUs(
                BoardAdc_ProtectionState.faultVoltageStartCounter) /
            1000000.0F;
        record->expectedMax =
            (float)BOARD_ADC_PROTECTION_RESPONSE_TIMEOUT_US / 1000000.0F;
    }
    else
    {
        record->expectedMin =
            (float)BoardAdc_ProtectionState.waitSampleCount / 1000.0F;
        record->expectedMax = phaseLimitSeconds;
    }
}

static BoardTest_Result BoardAdc_FinishProtectionThresholdTest(
    const BoardProfile_PinMap *pins,
    BoardTest_Record *record)
{
    BoardTest_Result result;
    float diagnosticVoltage;

    if((pins != 0) &&
       (pins->faultClearOutput != BOARD_PROFILE_PIN_UNUSED))
    {
        GPIO_WritePin(pins->faultClearOutput, 1U);
    }
    BoardAdc_RestoreProtectionXint();
    BoardAdc_RestoreProtectionTimer();

    gBoardAdcProtectionSnapshot.statusMask =
        BoardAdc_ProtectionState.statusMask;
    gBoardAdcProtectionSnapshot.failCode =
        BoardAdc_ProtectionState.failCode;
    gBoardAdcProtectionSnapshot.phase =
        BOARD_ADC_PROTECTION_PHASE_COMPLETE;
    gBoardAdcProtectionSnapshot.waitSampleCount =
        BoardAdc_ProtectionState.waitSampleCount;

    diagnosticVoltage =
        ((BoardAdc_ProtectionState.statusMask &
          BOARD_ADC_PROTECTION_STATUS_INITIAL_NORMAL) != 0U) ?
        gBoardAdcProtectionSnapshot.faultVoltage :
        gBoardAdcProtectionSnapshot.initialVoltage;
    result = BoardAdc_EvaluateProtectionThreshold(
        BoardAdc_ProtectionState.statusMask,
        BoardAdc_ProtectionState.failCode,
        BoardAdc_ProtectionState.selectionId,
        gBoardAdcProtectionSnapshot.triggeredFaultMask,
        gBoardAdcProtectionSnapshot.finalNormalMask,
        diagnosticVoltage,
        record);
    record->rawValue |=
        (((BoardTest_U32)BoardAdc_ProtectionState.faultBit & 0x000FUL) <<
         12U) |
        ((gBoardAdcProtectionSnapshot.clearPulseCount != 0U) ?
            0x00000080UL : 0UL) |
        (((pins != 0) &&
          (pins->faultClearOutput != BOARD_PROFILE_PIN_UNUSED) &&
          (GPIO_ReadPin(pins->faultClearOutput) != 0U)) ?
            0x00000040UL : 0UL) |
        (((BoardTest_U32)gBoardAdcProtectionSnapshot.initialBreakLevel &
          0x0001UL) << 5U) |
        (((BoardTest_U32)gBoardAdcProtectionSnapshot.finalBreakLevel &
          0x0001UL) << 4U);
    BoardAdc_ProtectionState.active = 0U;
    BoardAdc_ProtectionState.phase = BOARD_ADC_PROTECTION_PHASE_COMPLETE;
    return result;
}

BoardTest_Result BoardAdc_RunProtectionThresholdTest(
    BoardTest_U16 selectionId,
    BoardTest_Record *record)
{
    const BoardProfile_HardwareDescriptor *hardware;
    const BoardProfile_PinMap *pins;
    BoardTest_U16 index;
    BoardTest_U16 rawValue;
    BoardTest_U16 faultMask;
    BoardTest_U16 breakLevel;
    BoardTest_U16 clearLevel;
    BoardTest_U16 normalNow;
    BoardTest_U16 faultStatusMask;
    BoardTest_U32 elapsedUs;
    float voltage;

    hardware = BoardProfile_GetCurrentHardware();
    pins = (hardware != 0) ? &hardware->pins : 0;

    if((BoardAdc_ProtectionState.active == 0U) ||
       (BoardAdc_ProtectionState.selectionId != selectionId))
    {
        BoardAdc_AbortProtectionThresholdTest();
        gBoardAdcProtectionSnapshot.statusMask = 0U;
        gBoardAdcProtectionSnapshot.failCode =
            BOARD_ADC_PROTECTION_FAIL_NONE;
        gBoardAdcProtectionSnapshot.phase =
            BOARD_ADC_PROTECTION_PHASE_WAIT_NORMAL;
        gBoardAdcProtectionSnapshot.waitSampleCount = 0U;
        gBoardAdcProtectionSnapshot.selectionId = selectionId;
        gBoardAdcProtectionSnapshot.signalId = 0U;
        gBoardAdcProtectionSnapshot.mode = 0U;
        gBoardAdcProtectionSnapshot.faultBit = 0U;
        gBoardAdcProtectionSnapshot.initialFaultMask = 0U;
        gBoardAdcProtectionSnapshot.initialBreakLevel = 0U;
        gBoardAdcProtectionSnapshot.currentFaultMask = 0U;
        gBoardAdcProtectionSnapshot.currentBreakLevel = 0U;
        gBoardAdcProtectionSnapshot.triggeredFaultMask = 0U;
        gBoardAdcProtectionSnapshot.finalNormalMask = 0U;
        gBoardAdcProtectionSnapshot.finalBreakLevel = 0U;
        gBoardAdcProtectionSnapshot.xintCount = 0U;
        gBoardAdcProtectionSnapshot.clearPulseCount = 0U;
        gBoardAdcProtectionSnapshot.faultRaw = 0U;
        gBoardAdcProtectionSnapshot.initialVoltage = 0.0F;
        gBoardAdcProtectionSnapshot.faultVoltage = 0.0F;
        gBoardAdcProtectionSnapshot.finalVoltage = 0.0F;

        BoardAdc_ProtectionState.active = 1U;
        BoardAdc_ProtectionState.selectionId = selectionId;
        BoardAdc_ProtectionState.signalId = 0U;
        BoardAdc_ProtectionState.faultBit = 0U;
        BoardAdc_ProtectionState.mode = 0U;
        BoardAdc_ProtectionState.phase =
            BOARD_ADC_PROTECTION_PHASE_WAIT_NORMAL;
        BoardAdc_ProtectionState.waitSampleCount = 0U;
        BoardAdc_ProtectionState.statusMask = 0U;
        BoardAdc_ProtectionState.failCode =
            BOARD_ADC_PROTECTION_FAIL_NONE;
        BoardAdc_ProtectionState.timerConfigured = 0U;
        BoardAdc_ProtectionState.normalStableActive = 0U;
        BoardAdc_ProtectionState.faultVoltageObserved = 0U;
        BoardAdc_ProtectionState.xintConfigured = 0U;

        if(BoardAdc_GetProtectionSelection(
               selectionId,
               &BoardAdc_ProtectionState.signalId,
               &BoardAdc_ProtectionState.faultBit,
               &BoardAdc_ProtectionState.mode) == 0U)
        {
            BoardAdc_ProtectionState.failCode =
                BOARD_ADC_PROTECTION_FAIL_SELECTION;
        }
        else if((pins == 0) ||
                (pins->protectionBreakInput == BOARD_PROFILE_PIN_UNUSED) ||
                (pins->faultClearOutput == BOARD_PROFILE_PIN_UNUSED) ||
                (BoardAdc_FindExternalChannelConfig(
                     hardware,
                     BoardAdc_ProtectionState.signalId,
                     &BoardAdc_ProtectionState.config) == 0U))
        {
            BoardAdc_ProtectionState.failCode =
                BOARD_ADC_PROTECTION_FAIL_CONFIG;
        }

        gBoardAdcProtectionSnapshot.signalId =
            BoardAdc_ProtectionState.signalId;
        gBoardAdcProtectionSnapshot.mode = BoardAdc_ProtectionState.mode;
        gBoardAdcProtectionSnapshot.faultBit =
            BoardAdc_ProtectionState.faultBit;

        if(BoardAdc_ProtectionState.failCode ==
           BOARD_ADC_PROTECTION_FAIL_NONE)
        {
            for(index = 0U;
                index < BOARD_PROFILE_PROTECTION_FAULT_DI_COUNT;
                index++)
            {
                if(pins->protectionFaultInputs[index] ==
                   BOARD_PROFILE_PIN_UNUSED)
                {
                    BoardAdc_ProtectionState.failCode =
                        BOARD_ADC_PROTECTION_FAIL_CONFIG;
                    break;
                }
                GPIO_SetupPinMux(pins->protectionFaultInputs[index],
                                 GPIO_MUX_CPU1,
                                 0U);
                GPIO_SetupPinOptions(pins->protectionFaultInputs[index],
                                     GPIO_INPUT,
                                     GPIO_ASYNC);
            }
        }

        if(BoardAdc_ProtectionState.failCode ==
           BOARD_ADC_PROTECTION_FAIL_NONE)
        {
            GPIO_SetupPinMux(pins->protectionBreakInput,
                             GPIO_MUX_CPU1,
                             0U);
            GPIO_SetupPinOptions(pins->protectionBreakInput,
                                 GPIO_INPUT,
                                 GPIO_ASYNC);
            GPIO_WritePin(pins->faultClearOutput, 1U);
            GPIO_SetupPinMux(pins->faultClearOutput, GPIO_MUX_CPU1, 0U);
            GPIO_SetupPinOptions(pins->faultClearOutput,
                                 GPIO_OUTPUT,
                                 GPIO_ASYNC);
            GPIO_WritePin(pins->faultClearOutput, 1U);

            if((GPIO_ReadPin(pins->faultClearOutput) == 0U) ||
               ((BoardAdc_InitAllModules() &
                 (BOARD_ADC_BASIC_CLOCK_ENABLED |
                  BOARD_ADC_BASIC_POWERED)) !=
                (BOARD_ADC_BASIC_CLOCK_ENABLED |
                 BOARD_ADC_BASIC_POWERED)))
            {
                BoardAdc_ProtectionState.failCode =
                    BOARD_ADC_PROTECTION_FAIL_CONFIG;
            }
            else
            {
                BoardAdc_ProtectionState.statusMask |=
                    BOARD_ADC_PROTECTION_STATUS_CONFIGURED;
                BoardAdc_PulseFaultClear(pins);
                BoardAdc_ConfigureProtectionTimer();
                BoardAdc_ProtectionState.baselineClearStartCounter =
                    CpuTimer1Regs.TIM.all;
            }
        }

        if(BoardAdc_ProtectionState.failCode !=
           BOARD_ADC_PROTECTION_FAIL_NONE)
        {
            return BoardAdc_FinishProtectionThresholdTest(pins, record);
        }
        faultMask = BoardAdc_ReadProtectionFaultMask(pins);
        breakLevel = GPIO_ReadPin(pins->protectionBreakInput);
        clearLevel = GPIO_ReadPin(pins->faultClearOutput);
        gBoardAdcProtectionSnapshot.currentFaultMask = faultMask;
        gBoardAdcProtectionSnapshot.currentBreakLevel = breakLevel;
        gBoardAdcProtectionSnapshot.finalNormalMask =
            (BoardTest_U16)(~faultMask) &
            BOARD_DI_PROTECTION_FAULT_INPUT_MASK;
        gBoardAdcProtectionSnapshot.finalBreakLevel = breakLevel;
        BoardAdc_UpdateProtectionRunningRecord(record,
                                               0.0F,
                                               faultMask,
                                               breakLevel,
                                               clearLevel);
        return BOARD_TEST_RESULT_RUNNING;
    }

    if(BoardAdc_RunConfiguredChannel(&BoardAdc_ProtectionState.config,
                                     &rawValue) == 0U)
    {
        BoardAdc_ProtectionState.failCode = BOARD_ADC_PROTECTION_FAIL_ADC;
        return BoardAdc_FinishProtectionThresholdTest(pins, record);
    }

    voltage = BoardAdc_RawToExternalTerminalVoltage(rawValue);
    faultMask = BoardAdc_ReadProtectionFaultMask(pins);
    breakLevel = GPIO_ReadPin(pins->protectionBreakInput);
    clearLevel = GPIO_ReadPin(pins->faultClearOutput);
    gBoardAdcProtectionSnapshot.currentFaultMask = faultMask;
    gBoardAdcProtectionSnapshot.currentBreakLevel = breakLevel;
    gBoardAdcProtectionSnapshot.finalNormalMask =
        (BoardTest_U16)(~faultMask) &
        BOARD_DI_PROTECTION_FAULT_INPUT_MASK;
    gBoardAdcProtectionSnapshot.finalBreakLevel = breakLevel;
    elapsedUs = BoardAdc_UpdateProtectionElapsedTime();

    if(BoardAdc_ProtectionState.phase ==
       BOARD_ADC_PROTECTION_PHASE_WAIT_NORMAL)
    {
        if((BoardAdc_IsProtectionVoltageNormal(
                BoardAdc_ProtectionState.mode,
                voltage) != 0U) &&
           ((((faultMask & BoardAdc_ProtectionState.faultBit) != 0U) ||
             (breakLevel != 0U))) &&
           (BoardAdc_ProtectionElapsedUs(
                BoardAdc_ProtectionState.baselineClearStartCounter) >=
            BOARD_ADC_PROTECTION_BASELINE_CLEAR_RETRY_US))
        {
            BoardAdc_PulseFaultClear(pins);
            BoardAdc_ProtectionState.baselineClearStartCounter =
                CpuTimer1Regs.TIM.all;
            faultMask = BoardAdc_ReadProtectionFaultMask(pins);
            breakLevel = GPIO_ReadPin(pins->protectionBreakInput);
            gBoardAdcProtectionSnapshot.currentFaultMask = faultMask;
            gBoardAdcProtectionSnapshot.currentBreakLevel = breakLevel;
            gBoardAdcProtectionSnapshot.finalNormalMask =
                (BoardTest_U16)(~faultMask) &
                BOARD_DI_PROTECTION_FAULT_INPUT_MASK;
            gBoardAdcProtectionSnapshot.finalBreakLevel = breakLevel;
        }
        normalNow =
            ((BoardAdc_IsProtectionVoltageNormal(
                  BoardAdc_ProtectionState.mode,
                  voltage) != 0U) &&
             ((faultMask & BoardAdc_ProtectionState.faultBit) == 0U)) ?
            1U : 0U;
        gBoardAdcProtectionSnapshot.initialVoltage = voltage;
        gBoardAdcProtectionSnapshot.initialFaultMask = faultMask;
        gBoardAdcProtectionSnapshot.initialBreakLevel = breakLevel;
        if(normalNow != 0U)
        {
            if(BoardAdc_ProtectionState.normalStableActive == 0U)
            {
                BoardAdc_ProtectionState.normalStableActive = 1U;
                BoardAdc_ProtectionState.normalStableStartCounter =
                    CpuTimer1Regs.TIM.all;
            }
        }
        else
        {
            BoardAdc_ProtectionState.normalStableActive = 0U;
        }

        if((BoardAdc_ProtectionState.normalStableActive != 0U) &&
           (BoardAdc_ProtectionElapsedUs(
                BoardAdc_ProtectionState.normalStableStartCounter) >=
            BOARD_ADC_PROTECTION_INITIAL_STABLE_US))
        {
            BoardAdc_ProtectionState.statusMask |=
                BOARD_ADC_PROTECTION_STATUS_INITIAL_NORMAL;
            BoardAdc_ConfigureProtectionXint(pins);
            BoardAdc_BeginProtectionPhase(
                BOARD_ADC_PROTECTION_PHASE_WAIT_FAULT);
        }
        else if(elapsedUs >= BOARD_ADC_PROTECTION_INITIAL_TIMEOUT_US)
        {
            BoardAdc_ProtectionState.failCode =
                BOARD_ADC_PROTECTION_FAIL_INITIAL;
            return BoardAdc_FinishProtectionThresholdTest(pins, record);
        }
    }
    else if(BoardAdc_ProtectionState.phase ==
            BOARD_ADC_PROTECTION_PHASE_WAIT_FAULT)
    {
        gBoardAdcProtectionSnapshot.faultVoltage = voltage;
        gBoardAdcProtectionSnapshot.faultRaw = rawValue;
        if(BoardAdc_IsProtectionVoltageFault(
               BoardAdc_ProtectionState.mode,
               voltage) != 0U)
        {
            BoardAdc_ProtectionState.statusMask |=
                BOARD_ADC_PROTECTION_STATUS_FAULT_VOLTAGE;
            if(BoardAdc_ProtectionState.faultVoltageObserved == 0U)
            {
                BoardAdc_ProtectionState.faultVoltageObserved = 1U;
                BoardAdc_ProtectionState.faultVoltageStartCounter =
                    CpuTimer1Regs.TIM.all;
            }
        }
        if((faultMask & BoardAdc_ProtectionState.faultBit) != 0U)
        {
            BoardAdc_ProtectionState.statusMask |=
                BOARD_ADC_PROTECTION_STATUS_FAULT_ASSERTED;
            gBoardAdcProtectionSnapshot.triggeredFaultMask |=
                BoardAdc_ProtectionState.faultBit;
        }
        if((gBoardAdcProtectionSnapshot.initialBreakLevel == 0U) &&
           (breakLevel != 0U))
        {
            BoardAdc_ProtectionState.statusMask |=
                BOARD_ADC_PROTECTION_STATUS_BREAK_ASSERTED;
        }
        if(BoardAdc_ProtectionXintCount != 0U)
        {
            BoardAdc_ProtectionState.statusMask |=
                BOARD_ADC_PROTECTION_STATUS_XINT_TRIGGERED;
            gBoardAdcProtectionSnapshot.xintCount =
                BoardAdc_ProtectionXintCount;
        }

        faultStatusMask =
            BOARD_ADC_PROTECTION_STATUS_FAULT_VOLTAGE |
            BOARD_ADC_PROTECTION_STATUS_FAULT_ASSERTED;
        if(gBoardAdcProtectionSnapshot.initialBreakLevel == 0U)
        {
            faultStatusMask |=
                BOARD_ADC_PROTECTION_STATUS_BREAK_ASSERTED |
                BOARD_ADC_PROTECTION_STATUS_XINT_TRIGGERED;
        }
        if((BoardAdc_ProtectionState.statusMask & faultStatusMask) ==
           faultStatusMask)
        {
            BoardAdc_BeginProtectionPhase(
                BOARD_ADC_PROTECTION_PHASE_WAIT_RELEASE);
        }
        else if((BoardAdc_ProtectionState.faultVoltageObserved != 0U) &&
                (BoardAdc_ProtectionElapsedUs(
                     BoardAdc_ProtectionState.faultVoltageStartCounter) >=
                 BOARD_ADC_PROTECTION_RESPONSE_TIMEOUT_US))
        {
            BoardAdc_ProtectionState.failCode =
                BOARD_ADC_PROTECTION_FAIL_RESPONSE;
            return BoardAdc_FinishProtectionThresholdTest(pins, record);
        }
        else if((BoardAdc_ProtectionState.faultVoltageObserved == 0U) &&
                (elapsedUs >= BOARD_ADC_PROTECTION_PHASE_TIMEOUT_US))
        {
            BoardAdc_ProtectionState.failCode =
                BOARD_ADC_PROTECTION_FAIL_TIMEOUT;
            return BoardAdc_FinishProtectionThresholdTest(pins, record);
        }
    }
    else if(BoardAdc_ProtectionState.phase ==
            BOARD_ADC_PROTECTION_PHASE_WAIT_RELEASE)
    {
        if(BoardAdc_IsProtectionVoltageNormal(
               BoardAdc_ProtectionState.mode,
               voltage) != 0U)
        {
            BoardAdc_PulseFaultClear(pins);
            BoardAdc_ProtectionState.statusMask |=
                BOARD_ADC_PROTECTION_STATUS_CLEAR_PULSED;
            faultMask = BoardAdc_ReadProtectionFaultMask(pins);
            breakLevel = GPIO_ReadPin(pins->protectionBreakInput);
            gBoardAdcProtectionSnapshot.finalVoltage = voltage;
            gBoardAdcProtectionSnapshot.finalNormalMask =
                (BoardTest_U16)(~faultMask) &
                BOARD_DI_PROTECTION_FAULT_INPUT_MASK;
            gBoardAdcProtectionSnapshot.finalBreakLevel = breakLevel;
            if(((faultMask & BoardAdc_ProtectionState.faultBit) == 0U) &&
               (breakLevel == 0U))
            {
                BoardAdc_ProtectionState.statusMask |=
                    BOARD_ADC_PROTECTION_STATUS_FAULT_CLEARED;
                BoardAdc_ProtectionState.statusMask |=
                    BOARD_ADC_PROTECTION_STATUS_SAFE_FINAL;
            }
            else
            {
                BoardAdc_ProtectionState.failCode =
                    BOARD_ADC_PROTECTION_FAIL_CLEAR;
            }
            return BoardAdc_FinishProtectionThresholdTest(pins, record);
        }

        if(elapsedUs >= BOARD_ADC_PROTECTION_PHASE_TIMEOUT_US)
        {
            BoardAdc_ProtectionState.failCode =
                BOARD_ADC_PROTECTION_FAIL_TIMEOUT;
            return BoardAdc_FinishProtectionThresholdTest(pins, record);
        }
    }
    else
    {
        BoardAdc_ProtectionState.failCode =
            BOARD_ADC_PROTECTION_FAIL_CONFIG;
        return BoardAdc_FinishProtectionThresholdTest(pins, record);
    }

    gBoardAdcProtectionSnapshot.statusMask =
        BoardAdc_ProtectionState.statusMask;
    gBoardAdcProtectionSnapshot.failCode =
        BoardAdc_ProtectionState.failCode;
    gBoardAdcProtectionSnapshot.phase = BoardAdc_ProtectionState.phase;
    gBoardAdcProtectionSnapshot.waitSampleCount =
        BoardAdc_ProtectionState.waitSampleCount;
    BoardAdc_UpdateProtectionRunningRecord(record,
                                           voltage,
                                           faultMask,
                                           breakLevel,
                                           clearLevel);
    DELAY_US(BOARD_ADC_PROTECTION_SAMPLE_INTERVAL_US);
    return BOARD_TEST_RESULT_RUNNING;
}
#endif
