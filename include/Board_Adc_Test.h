#ifndef BOARD_ADC_TEST_H
#define BOARD_ADC_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_ADC_RESOLUTION_BITS        12U
#define BOARD_ADC_RAW_MAX                4095U
#define BOARD_ADC_REFERENCE_VOLTAGE_V    3.3F

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
#define BOARD_ADC_MAX_CHANNEL_COUNT       19U
#define BOARD_ADC_NO_FAILED_CHANNEL       0xFFFFU

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

extern volatile BoardAdc_BasicSnapshot gBoardAdcBasicSnapshot;

BoardTest_U16 BoardAdc_VoltageToRaw(float inputVoltage);
float BoardAdc_RawToVoltage(BoardTest_U16 rawValue);
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

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardAdc_RunBasicSampleTest(BoardTest_Record *record);
#endif

#ifdef __cplusplus
}
#endif

#endif
