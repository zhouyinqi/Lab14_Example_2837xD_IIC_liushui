#ifndef BOARD_ADC_TEST_H
#define BOARD_ADC_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_ADC_RESOLUTION_BITS        12U
#define BOARD_ADC_RAW_MAX                4095U
#define BOARD_ADC_REFERENCE_VOLTAGE_V    3.3F
#define BOARD_ADC_EXTERNAL_TERMINAL_FULL_SCALE_V 5.0F
#define BOARD_ADC_EXTERNAL_SAMPLE_COUNT  64U
#define BOARD_ADC_EXTERNAL_MAX_MILLIVOLTS 5000U
#define BOARD_ADC_EXTERNAL_MAX_TOLERANCE_MILLIVOLTS 500U

#define BOARD_ADC_BASIC_CLOCK_ENABLED    0x0001U
#define BOARD_ADC_BASIC_POWERED          0x0002U
#define BOARD_ADC_BASIC_SOC_CONFIGURED   0x0004U
#define BOARD_ADC_BASIC_CONVERSION_DONE  0x0008U
#define BOARD_ADC_BASIC_RAW_IN_RANGE     0x0010U

#define BOARD_ADC_BASIC_STATUS_REQUIRED_MASK \
    (BOARD_ADC_BASIC_CLOCK_ENABLED |         \
     BOARD_ADC_BASIC_POWERED |               \
     BOARD_ADC_BASIC_SOC_CONFIGURED |        \
     BOARD_ADC_BASIC_CONVERSION_DONE |       \
     BOARD_ADC_BASIC_RAW_IN_RANGE)

#define BOARD_ADC_BASIC_STATUS_DIAGNOSTIC_MASK \
    BOARD_ADC_BASIC_STATUS_REQUIRED_MASK

#define BOARD_ADC_BASIC_ADCA_CHANNEL      0U
#define BOARD_ADC_MAX_CHANNEL_COUNT       17U
#define BOARD_ADC_NO_FAILED_CHANNEL       0xFFFFU
#define BOARD_ADC_EXTERNAL_TERMINAL_SIGNAL_COUNT 14U
#define BOARD_ADC_EXTERNAL_GROUP_COUNT    2U
#define BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT 7U
#define BOARD_ADC_EXTERNAL_GROUP_REQUIRED_MASK 0x007FU
#define BOARD_ADC_EXTERNAL_SELECTION_GROUP_1 0x0080U
#define BOARD_ADC_EXTERNAL_SELECTION_GROUP_2 0x0081U
#define BOARD_ADC_EXTERNAL_SELECTION_PT100_BASE 0x0090U
#define BOARD_ADC_EXTERNAL_SELECTION_PT100_COUNT 4U
#define BOARD_ADC_EXTERNAL_SELECTION_PT100_TEST_PT4 \
    (BOARD_ADC_EXTERNAL_SELECTION_PT100_BASE + 3U)
#define BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_1_BASE 0x00B0U
#define BOARD_ADC_EXTERNAL_SELECTION_FAULT_GROUP_2_BASE 0x00C0U
#define BOARD_ADC_EXTERNAL_SELECTION_FAULT_COUNT 6U
#define BOARD_ADC_MUX_CHANNEL_COUNT       2U
#define BOARD_ADC_MUX_SETTLE_TIME_US      20U

#define BOARD_ADC_PROTECTION_SELECTION_OC_HIGH_BASE 0U
#define BOARD_ADC_PROTECTION_SELECTION_OC_LOW_BASE  6U
#define BOARD_ADC_PROTECTION_SELECTION_UOV_BASE     12U
#define BOARD_ADC_PROTECTION_SELECTION_COUNT        14U

#define BOARD_ADC_PROTECTION_MODE_OC_HIGH 0U
#define BOARD_ADC_PROTECTION_MODE_OC_LOW  1U
#define BOARD_ADC_PROTECTION_MODE_UOV_HIGH 2U

#define BOARD_ADC_PROTECTION_OC_LOW_THRESHOLD_V  0.25F
#define BOARD_ADC_PROTECTION_OC_HIGH_THRESHOLD_V 4.75F
#define BOARD_ADC_PROTECTION_UOV_THRESHOLD_V     3.80F
#define BOARD_ADC_PROTECTION_TERMINAL_MAX_V      5.00F

#define BOARD_ADC_PROTECTION_STATUS_CONFIGURED       0x0001U
#define BOARD_ADC_PROTECTION_STATUS_INITIAL_NORMAL   0x0002U
#define BOARD_ADC_PROTECTION_STATUS_FAULT_VOLTAGE    0x0004U
#define BOARD_ADC_PROTECTION_STATUS_FAULT_ASSERTED   0x0008U
#define BOARD_ADC_PROTECTION_STATUS_BREAK_ASSERTED   0x0010U
#define BOARD_ADC_PROTECTION_STATUS_XINT_TRIGGERED   0x0020U
#define BOARD_ADC_PROTECTION_STATUS_CLEAR_PULSED     0x0040U
#define BOARD_ADC_PROTECTION_STATUS_FAULT_CLEARED    0x0080U
#define BOARD_ADC_PROTECTION_STATUS_SAFE_FINAL       0x0100U
#define BOARD_ADC_PROTECTION_REQUIRED_STATUS_MASK    0x01FFU

#define BOARD_ADC_PROTECTION_FAIL_NONE          0U
#define BOARD_ADC_PROTECTION_FAIL_SELECTION     1U
#define BOARD_ADC_PROTECTION_FAIL_CONFIG        2U
#define BOARD_ADC_PROTECTION_FAIL_INITIAL       3U
#define BOARD_ADC_PROTECTION_FAIL_ADC           4U
#define BOARD_ADC_PROTECTION_FAIL_TIMEOUT       5U
#define BOARD_ADC_PROTECTION_FAIL_CLEAR         6U
#define BOARD_ADC_PROTECTION_FAIL_RESPONSE      7U

#define BOARD_ADC_PROTECTION_PHASE_IDLE          0U
#define BOARD_ADC_PROTECTION_PHASE_WAIT_NORMAL   1U
#define BOARD_ADC_PROTECTION_PHASE_WAIT_FAULT    2U
#define BOARD_ADC_PROTECTION_PHASE_WAIT_RELEASE  3U
#define BOARD_ADC_PROTECTION_PHASE_COMPLETE      4U

#define BOARD_ADC_PROTECTION_RECORD_FORMAT_MASK 0x0E000000UL
#define BOARD_ADC_PROTECTION_RECORD_FORMAT_V2   0x0A000000UL

#define BOARD_ADC_MODULE_A                0U
#define BOARD_ADC_MODULE_B                1U
#define BOARD_ADC_MODULE_C                2U
#define BOARD_ADC_MODULE_D                3U

typedef enum
{
    BOARD_ADC_SIGNAL_IU1 = 0,
    BOARD_ADC_SIGNAL_IV1,
    BOARD_ADC_SIGNAL_IW1,
    BOARD_ADC_SIGNAL_IU2,
    BOARD_ADC_SIGNAL_IV2,
    BOARD_ADC_SIGNAL_IW2,
    BOARD_ADC_SIGNAL_UDC1,
    BOARD_ADC_SIGNAL_UDC2,
    BOARD_ADC_SIGNAL_MOTOR_TEMPERATURE,
    BOARD_ADC_SIGNAL_FAULT_1,
    BOARD_ADC_SIGNAL_FAULT_2,
    BOARD_ADC_SIGNAL_D5V,
    BOARD_ADC_SIGNAL_A5V,
    BOARD_ADC_SIGNAL_INVERTER_TEMPERATURE_1,
    BOARD_ADC_SIGNAL_INVERTER_TEMPERATURE_2,
    BOARD_ADC_SIGNAL_UUV1,
    BOARD_ADC_SIGNAL_UVW1,
    BOARD_ADC_SIGNAL_UUV2,
    BOARD_ADC_SIGNAL_UVW2
} BoardAdc_SignalId;

typedef struct
{
    BoardTest_U16 signalId;
    BoardTest_U16 module;
    BoardTest_U16 channel;
} BoardAdc_ChannelConfig;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 rawValue;
    volatile BoardTest_U16 channel;
    volatile float inputVoltage;
    volatile BoardTest_U16 boardId;
    volatile BoardTest_U16 configuredChannelCount;
    volatile BoardTest_U16 sampledChannelCount;
    volatile BoardTest_U16 failedChannelIndex;
    volatile BoardTest_U16 signalIds[BOARD_ADC_MAX_CHANNEL_COUNT];
    volatile BoardTest_U16 modules[BOARD_ADC_MAX_CHANNEL_COUNT];
    volatile BoardTest_U16 channels[BOARD_ADC_MAX_CHANNEL_COUNT];
    volatile BoardTest_U16 rawValues[BOARD_ADC_MAX_CHANNEL_COUNT];
    volatile float inputVoltages[BOARD_ADC_MAX_CHANNEL_COUNT];
} BoardAdc_BasicSnapshot;

typedef struct
{
    volatile BoardTest_U16 configured;
    volatile BoardTest_U16 signalId;
    volatile BoardTest_U16 expectedMillivolts;
    volatile BoardTest_U16 toleranceMillivolts;
} BoardAdc_ExternalInjectionConfig;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 signalId;
    volatile BoardTest_U16 module;
    volatile BoardTest_U16 channel;
    volatile BoardTest_U16 sampleCount;
    volatile BoardTest_U16 averageRaw;
    volatile BoardTest_U16 minimumRaw;
    volatile BoardTest_U16 maximumRaw;
    volatile BoardTest_U16 expectedMillivolts;
    volatile BoardTest_U16 toleranceMillivolts;
    volatile float adcPinVoltage;
    volatile float terminalVoltage;
} BoardAdc_ExternalInjectionSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 selectionId;
    volatile BoardTest_U16 channelCount;
    volatile BoardTest_U16 passedMask;
    volatile BoardTest_U16 failedMask;
    volatile BoardTest_U16 signalIds[BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT];
    volatile BoardTest_U16 modules[BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT];
    volatile BoardTest_U16 channels[BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT];
    volatile BoardTest_U16 averageRaw[BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT];
    volatile BoardTest_U16 minimumRaw[BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT];
    volatile BoardTest_U16 maximumRaw[BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT];
    volatile float terminalVoltages[BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT];
    volatile BoardTest_U16 results[BOARD_ADC_EXTERNAL_GROUP_CHANNEL_COUNT];
} BoardAdc_ExternalGroupSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 selectionId;
    volatile BoardTest_U16 selectorCode;
    volatile BoardTest_U16 channelCount;
    volatile BoardTest_U16 passedMask;
    volatile BoardTest_U16 failedMask;
    volatile BoardTest_U16 signalIds[BOARD_ADC_MUX_CHANNEL_COUNT];
    volatile BoardTest_U16 modules[BOARD_ADC_MUX_CHANNEL_COUNT];
    volatile BoardTest_U16 channels[BOARD_ADC_MUX_CHANNEL_COUNT];
    volatile BoardTest_U16 averageRaw[BOARD_ADC_MUX_CHANNEL_COUNT];
    volatile float terminalVoltages[BOARD_ADC_MUX_CHANNEL_COUNT];
    volatile float convertedValues[BOARD_ADC_MUX_CHANNEL_COUNT];
    volatile BoardTest_U16 results[BOARD_ADC_MUX_CHANNEL_COUNT];
} BoardAdc_MuxInjectionSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 phase;
    volatile BoardTest_U16 waitSampleCount;
    volatile BoardTest_U16 selectionId;
    volatile BoardTest_U16 signalId;
    volatile BoardTest_U16 mode;
    volatile BoardTest_U16 faultBit;
    volatile BoardTest_U16 initialFaultMask;
    volatile BoardTest_U16 initialBreakLevel;
    volatile BoardTest_U16 currentFaultMask;
    volatile BoardTest_U16 currentBreakLevel;
    volatile BoardTest_U16 triggeredFaultMask;
    volatile BoardTest_U16 finalNormalMask;
    volatile BoardTest_U16 finalBreakLevel;
    volatile BoardTest_U16 xintCount;
    volatile BoardTest_U16 clearPulseCount;
    volatile BoardTest_U16 faultRaw;
    volatile float initialVoltage;
    volatile float faultVoltage;
    volatile float finalVoltage;
} BoardAdc_ProtectionSnapshot;

extern volatile BoardAdc_BasicSnapshot gBoardAdcBasicSnapshot;
extern volatile BoardAdc_ExternalInjectionConfig
    gBoardAdcExternalInjectionConfig;
extern volatile BoardAdc_ExternalInjectionSnapshot
    gBoardAdcExternalInjectionSnapshot;
extern volatile BoardAdc_ExternalGroupSnapshot
    gBoardAdcExternalGroupSnapshots[BOARD_ADC_EXTERNAL_GROUP_COUNT];
extern volatile BoardAdc_MuxInjectionSnapshot gBoardAdcMuxInjectionSnapshot;
extern volatile BoardAdc_ProtectionSnapshot gBoardAdcProtectionSnapshot;

BoardTest_U16 BoardAdc_VoltageToRaw(float inputVoltage);
float BoardAdc_RawToVoltage(BoardTest_U16 rawValue);
float BoardAdc_RawToExternalTerminalVoltage(BoardTest_U16 rawValue);
BoardTest_U16 BoardAdc_IsExternalInjectionSignalSupported(
    BoardTest_U16 signalId);
BoardTest_U16 BoardAdc_IsExternalInjectionSelectionSupported(
    BoardTest_U16 selectionId);
BoardTest_U16 BoardAdc_GetExternalInjectionGroupSignal(
    BoardTest_U16 selectionId,
    BoardTest_U16 channelIndex,
    BoardTest_U16 *signalId);
BoardTest_U16 BoardAdc_ConfigureExternalInjection(
    BoardTest_U16 signalId,
    BoardTest_U16 expectedMillivolts,
    BoardTest_U16 toleranceMillivolts);
BoardTest_U16 BoardAdc_IsMuxSelectionSupported(
    BoardTest_U16 testId,
    BoardTest_U16 selectionId);
BoardTest_U16 BoardAdc_GetMuxSelectorCode(
    BoardTest_U16 selectionId,
    BoardTest_U16 *selectorCode);
BoardTest_U16 BoardAdc_IsProtectionSelectionSupported(
    BoardTest_U16 selectionId);
float BoardAdc_Pt100VoltageToResistance(float inputVoltage);
float BoardAdc_FaultVoltageToDutyPercent(float inputVoltage);
BoardTest_U16 BoardAdc_GetChannelCount(BoardTest_U16 boardId,
                                       BoardTest_U16 hardwareRevision);
BoardTest_U16 BoardAdc_GetChannelConfig(BoardTest_U16 boardId,
                                        BoardTest_U16 hardwareRevision,
                                        BoardTest_U16 channelIndex,
                                        BoardAdc_ChannelConfig *config);
BoardTest_Result BoardAdc_EvaluateBasicSample(BoardTest_U16 statusMask,
                                              BoardTest_U16 rawValue,
                                              BoardTest_Record *record);
BoardTest_Result BoardAdc_EvaluateChannelScan(
    BoardTest_U16 statusMask,
    BoardTest_U16 sampledChannelCount,
    BoardTest_U16 expectedChannelCount,
    BoardTest_U16 failedChannelIndex,
    BoardTest_Record *record);
BoardTest_Result BoardAdc_EvaluateExternalInjection(
    BoardTest_U16 statusMask,
    BoardTest_U16 averageRaw,
    float expectedVoltage,
    float toleranceVoltage,
    BoardTest_Record *record);
BoardTest_Result BoardAdc_EvaluatePt100Injection(
    BoardTest_U16 statusMask,
    BoardTest_U16 averageRaw,
    float expectedVoltage,
    float toleranceVoltage,
    BoardTest_Record *record);
BoardTest_Result BoardAdc_EvaluateExternalInjectionGroup(
    BoardTest_U16 selectionId,
    BoardTest_U16 passedMask,
    BoardTest_U16 failedMask,
    BoardTest_U16 channelCount,
    BoardTest_Record *record);
BoardTest_Result BoardAdc_EvaluateMuxInjection(
    BoardTest_U16 selectionId,
    BoardTest_U16 passedMask,
    BoardTest_U16 failedMask,
    BoardTest_U16 channelCount,
    BoardTest_Record *record);
BoardTest_Result BoardAdc_EvaluateProtectionThreshold(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 selectionId,
    BoardTest_U16 triggeredFaultMask,
    BoardTest_U16 finalNormalMask,
    float faultVoltage,
    BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardAdc_RunBasicSampleTest(BoardTest_Record *record);
BoardTest_Result BoardAdc_RunExternalInjectionTest(BoardTest_Record *record);
BoardTest_Result BoardAdc_RunExternalInjectionGroupTest(
    BoardTest_U16 selectionId,
    BoardTest_Record *record);
BoardTest_Result BoardAdc_RunMuxInjectionTest(
    BoardTest_U16 testId,
    BoardTest_Record *record);
BoardTest_Result BoardAdc_RunProtectionThresholdTest(
    BoardTest_U16 selectionId,
    BoardTest_Record *record);
void BoardAdc_AbortProtectionThresholdTest(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
